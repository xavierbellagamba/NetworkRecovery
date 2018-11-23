/*************************************************************************************************************

loadGAParameters : import the GA-related parameter and returns a structure

**************************************************************************************************************/
/*************************************************************************************************************

generateRandomSolutions : generate N random solutions

**************************************************************************************************************/
std::vector< std::vector<int> > generateRandomSolutions(int l, int k, int n)
{
	//Initialize base variables
	int i, j, i_l;

	//Initialize vectors of solutions
	std::vector< std::vector<int> > solution(n, std::vector<int>(l, 0));

	//Initialize random generator
	VSLStreamStatePtr rng_stream;

	//Generate random seeds
	int *seed = new int[n];
	int clock_val = rand();
	vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
	viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n, seed, 1, 30000);
	vslDeleteStream(&rng_stream);

	//Generate the solution
	for(i=0 ; i<n ; i++)
	{
		bool isUnique = false;
		int U = 0;
		while(!isUnique && U < 10)
		{
			//Change the loci values
			for(j=0 ; j<k ; j++)
			{
				//Generate the loci
				int locus;
				int count = rand() % 32000;

				vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed[i]+count);
				viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, 1, &locus, 0, l);
				vslDeleteStream(&rng_stream);

				while(solution[i][locus] == 1)
				{
					count++;
					vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed[i]+count);
					viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, 1, &locus, 0, l);
					vslDeleteStream(&rng_stream);
				}

				solution[i][locus] = 1;
			}
			//Ensure uniqueness
			U++;
			int count_diff = 0;
			if(i != 0)
			{
				for(j=0 ; j<i ; j++)
				{
					bool isIdentical = true;
					for(i_l=0 ; i_l<l ; i_l++)
					{
						if(solution[j][i_l] != solution[i][i_l])
						{
							isIdentical = false;
							break;
						}
					}
					if(!isIdentical)
					{
						count_diff++;
					}
				}
				if(count_diff == i)
				{
					isUnique = true;
				}
			}
			else
			{
				isUnique = true;
			}
		}
	}

	delete [] seed;

	//Return the generated solutions
	return solution;
}


/*************************************************************************************************************

computeRawFitness : compute the number of people disconnected after repairs (assumption: WYSIATI)

**************************************************************************************************************/
float computeRawFitness(buildingCommunity community, std::vector<float> w)
{
	//Initialize base variables
	int i;
	int n_build = community.n_buildings();
	float fitness = 0.0;
	float fitness_pop = 0.0;
	float fitness_build = 0.0;
	float fitness_util = 0.0;
	float max_pop = 0.0;
	float max_util = 0.0;

	for(i=0 ; i<n_build ; i++)
	{
		max_pop = max_pop + community.building(i)->pop();
		max_util = max_util + community.building(i)->importance();

		if(community.building(i)->is_connect().size() > 0)
		{
			if(community.building(i)->is_connect()[0] == 0)
			{
				fitness_pop = fitness_pop + community.building(i)->pop();
				fitness_build = fitness_build + 1.0;
				fitness_util = fitness_util + community.building(i)->importance();
			}
		}
	}

	fitness = w[0]*(max_pop - fitness_pop) + w[1]*(float(n_build) - fitness_build) + w[2]*(max_util - fitness_util);

	//Return fitness
	return fitness;
}


/*************************************************************************************************************

assignFailure_recovery : assign the failures to the network 

**************************************************************************************************************/
pipeNetwork assignFailure_recovery(pipeNetwork network, std::vector<pipeFailure> failure, std::vector<int> repair)
{
	//Initialize base variables
	int i, j;
	int n_pipes = network.n_pipes();
	int n_f = failure.size();

	//For each failure
	for(i=0 ; i<n_f ; i++)
	{
		if(repair[i] == 0)
		{
			int ID = failure[i].pipeUnikID();
			std::vector<int> failurePipeID = network.pipe(ID)->n_breaks();
			if(failurePipeID[0] != -1)
			{
				failurePipeID[0] = failurePipeID[0] + 1;
			}
			else
			{
				failurePipeID[0] = failurePipeID[0] + 2;
			}
			network.pipe(ID)->n_breaks(failurePipeID);
		}
	}

	//Return updated network
	return network;
}


/*************************************************************************************************************

testSingleSoliution : return the raw fitness of a single possible solution

**************************************************************************************************************/
float testSingleSolution(char *ntwk_path, char *cmmt_path, std::vector<float> w, std::vector<pipeFailure> failure, std::vector<int> singleSolution)
{
	//Initialize base variables
	int i;
	float fitness;
	ifstream infile;

	//Load original network and community
	pipeNetwork network;
	network.READ_Network(ntwk_path, infile);
	buildingCommunity community;
	community.READ_Community(cmmt_path, infile);

	//Assign pipe failure to network	
	network = assignFailure_recovery(network, failure, singleSolution);

	//Evaluate serviceability of the network with the proposed repairs
	network = PipeConnectivity(network, false);
	community = BuildingConnectivity(community, network);

	//Compute raw fitness (number of disconnected people after repair)
	fitness = computeRawFitness(community, w);

	//Return raw fitness
	return fitness;
}


/*************************************************************************************************************

evaluateAllSolutionsRawFitness : return a vector containing the raw chromosome scores

**************************************************************************************************************/
std::vector<float> evaluateAllSolutionsRawFitness(char *ntwk_path, char *cmmt_path, std::vector<float> w, std::vector<pipeFailure> failure, 
	std::vector< std::vector<int> > solution)
{
	//Initialize base variables
	int i; 

	//Initialize vector of raw fitness
	std::vector<float> fitness(solution.size());

	//Evaluate the fitness of each solution
	for(i=0 ; i<solution.size() ; i++)
	{
		fitness[i] = testSingleSolution(ntwk_path, cmmt_path, w, failure, solution[i]);
	}

	//Return raw fitness
	return fitness;
}


/*************************************************************************************************************

computeMeanFitness : return the mean fitness of the population

**************************************************************************************************************/
float computeMeanFitness(std::vector<float> fitness)
{
	//Initialize base variables
	int i;

	float sum = 0.0;
	float mean;
	for(i=0 ; i<fitness.size() ; i++)
	{
		sum = sum + fitness[i];
	}

	mean = sum/(float)fitness.size();

	//Return the mean
	return mean;
}


/*************************************************************************************************************

computeStdFitness : return the std fitness of the population

**************************************************************************************************************/
float computeStdFitness(std::vector<float> fitness)
{
	//Initialize base variables
	int i;

	//Compute the mean
	float mean = computeMeanFitness(fitness);

	//Create the X-mean vector
	std::vector<float> x_mean(fitness.size(), -1);
	for(i=0 ; i<fitness.size() ; i++)
	{
		x_mean[i] = pow(fitness[i]-mean, 2);
	}

	float std = computeMeanFitness(x_mean);
	std = sqrt(std);

	//Return std
	return std;
}


/*************************************************************************************************************

sigmaScaleFitness : return a vector containing the sigma scale chromosome fitness

**************************************************************************************************************/
std::vector<float> sigmaScaleFitness(std::vector<float> rawFitness)
{
	//Initialize base variables
	int i;

	//Compute the mean
	float mean = computeMeanFitness(rawFitness);

	//Compute the std
	float std = computeStdFitness(rawFitness);

	//Compute the scaled fitness
	std::vector<float> scaledFitness(rawFitness.size(), -1);
	for(i=0 ; rawFitness.size() ; i++)
	{
		if(std > 0.001)
		{
			scaledFitness[i] = 1 + (rawFitness[i] - mean)/std;
		}
		else
		{
			scaledFitness[i] = 1 + rawFitness[i] - mean;
		}
		if(scaledFitness[i] < 0)
		{
			if(scaledFitness[i] > -1)
			{
				scaledFitness[i] = -0.1*(1-scaledFitness[i]);
			}
			else
			{
				scaledFitness[i] = -0.1/scaledFitness[i];
			}
		}
	}

	//Return scaled fitness
	return scaledFitness;
}


/*************************************************************************************************************

GetBestFitness : returns the best fitness in the population

**************************************************************************************************************/
float getBestFitness(std::vector<float> fitness)
{
	int i;
	float current_max_fitness = -1.0;

	for(i=0 ; i<fitness.size() ; i++)
	{
		if(fitness[i] > current_max_fitness)
		{
			memcpy(&current_max_fitness, &fitness[i], sizeof(float));
		}
	}

	return current_max_fitness;
}


/*************************************************************************************************************

rankIndex : returns the vector indices in decreasing order

**************************************************************************************************************/
std::vector<int> rankIndex(std::vector<float> fitness, std::vector<int> ind_elite, int count)
{
	//If size of fitness = 0, end of the reccursion
	if(fitness.size() == count)
	{
		return ind_elite;
	}
	else //Complete the list
	{
		//Initialize base variables
		int i, j;
		int ind_max = -1;
		float call_max, current_max;

		//Set ceiling
		if(count > 0)
		{
			call_max = fitness[ind_elite[count-1]];
		}
		else
		{
			call_max = 10000000.0;
		}

		//Initiliaze floor
		current_max = -10000000.0;

		for(i=0 ; i<fitness.size() ; i++)
		{
			if(fitness[i] > current_max && fitness[i] < call_max)
			{
				memcpy(&ind_max, &i, sizeof(int));
				memcpy(&current_max, &fitness[i], sizeof(float));
			}
			//If multiple times the same value
			else if(fitness[i] == call_max)
			{
				bool different_from_all = 1;
				for(j=0 ; j<count ; j++)
				{
					if(i == ind_elite[j])
					{
						different_from_all = 0;
						break;
					}
				}

				if(different_from_all)
				{
					memcpy(&ind_max, &i, sizeof(int));
					memcpy(&current_max, &fitness[i], sizeof(float));
				}
			}
		}

		//Update count and fitness
		ind_elite[count] = ind_max;
		count++;

		//Reccursive call
		return rankIndex(fitness, ind_elite, count);
	}
}


/*************************************************************************************************************

selectEliteChromosomes : return the n_elite best solutions

**************************************************************************************************************/
std::vector< std::vector<int> > selectEliteChromosomes(std::vector< std::vector<int> > solution, std::vector<float> fitness, int n_elite)
{
	//Initialize base variables
	int i;

	//Get the index of the n_elite best solutions
	std::vector<int> ind_elite(solution.size());
	ind_elite = rankIndex(fitness, ind_elite, 0);

	//Initialize vector of elite solutions
	std::vector< std::vector<int> > eliteSolution(n_elite, std::vector<int>(solution[0].size(), -1));

	//Populate
	for(i=0 ; i<n_elite ; i++)
	{
		eliteSolution[i] = solution[ind_elite[i]];
	}

	//Return elite solutions
	return eliteSolution;
}


/*************************************************************************************************************

checkValidatiyOffspring : returns an int representing the difference of repairs between offspring (=0: valid, <0: 0 has too much, >0: 1 has)

**************************************************************************************************************/
int checkValidityOffspring(std::vector<int> chrom_0, std::vector<int> chrom_1)
{
	//Initialize base variables
	int i, diff;
	int n_allele = chrom_0.size();

	//Sum of both
	int sum_0 = 0;
	int sum_1 = 0;
	for(i=0 ; i<n_allele ; i++)
	{
		sum_0 = sum_0 + chrom_0[i];
		sum_1 = sum_1 + chrom_1[i];
	}

	diff = sum_1 - sum_0;

	//Return difference
	return diff;
}


/*************************************************************************************************************

executeCrossover : return new offspring from selected chromosome pairs (Unifrom parametrized crossover)

**************************************************************************************************************/
std::vector< std::vector<int> > executeCrossover(std::vector< std::vector<int> > solution, std::vector< std::vector<int> > pair, float P_c, 
	int capacity)
{
	//Initialize base variables
	int i, j, k;
	int n_new = pair.size();
	int n_allele = solution[0].size();

	//Initialize random generator
	VSLStreamStatePtr rng_stream;

	//Generate random seeds
	int *seed = new int[n_new];
	int clock_val = rand()%31000;
	vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
	viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_new, seed, 1, 100000);
	vslDeleteStream(&rng_stream);

	//Initialize vector of new chromosomes
	std::vector< std::vector<int> > new_gen(n_new*2, std::vector<int>(n_allele, -1));

	for(i=0 ; i<n_new ; i++)
	{
		//Isolate selected chromosomes
		std::vector<int> chrom_0 = solution[pair[i][0]];
		std::vector<int> chrom_1 = solution[pair[i][1]];

		//Generate n_allele random numbers unformly dist. between 0 and 1
		int *p_c = new int[n_allele];
		vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed[i]);
		viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_allele, p_c, 0.0, 1.0);
		vslDeleteStream(&rng_stream);

		//Exchange alleles
		for(j=0 ; j<n_allele ; j++)
		{
			if(p_c[j] < P_c)//Swap locus j
			{
				int allele_0, allele_1;
				memcpy(&allele_0, &chrom_0[j], sizeof(int));
				memcpy(&allele_1, &chrom_1[j], sizeof(int));

				memcpy(&chrom_1[j], &allele_0, sizeof(int));
				memcpy(&chrom_0[j], &allele_1, sizeof(int));
			}
		}

		delete [] p_c;

		//Check validity of offspring (valid if difference of their sum == 0)
		int offspring_valid = checkValidityOffspring(chrom_0, chrom_1);

		//If not valid, randomly select 'valid' loci to exchange
		if(!(offspring_valid == 0))
		{
			//Generate the random loci to be exchanged
			int n_ex = abs(offspring_valid);
			int *loci = new int[n_ex];
			int range = 0; // = capacity + n_ex;

			//Compute the RN range for loci to change
			for(j=0 ; j<n_allele ; j++)
			{
				if((chrom_0[j] - chrom_1[j]) != 0)
				{
					range++;
				}
			}

			//Generate loci to change
			vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed[n_new - i - 1]);
			viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_ex, loci, 0, range);
			vslDeleteStream(&rng_stream);

			//Select chromosome
			int ind_chrom;
			if(offspring_valid < 0)
			{
				int count = 0;
				for(j=0 ; j<n_allele ; j++)
				{
					if(chrom_0[j] == 1 && chrom_1[j] == 0)
					{
						for(k=0 ; k<n_ex ; k++)
						{
							if(count == loci[k])
							{
								int allele_0, allele_1;
								memcpy(&allele_0, &chrom_0[j], sizeof(int));
								memcpy(&allele_1, &chrom_1[j], sizeof(int));

								memcpy(&chrom_1[j], &allele_0, sizeof(int));
								memcpy(&chrom_0[j], &allele_1, sizeof(int));
							}
						}

						count++;
					}
				}
			}
			else
			{
				int count = 0;
				for(j=0 ; j<n_allele ; j++)
				{
					if(chrom_1[j] == 1 && chrom_0[j] == 0)
					{
						for(k=0 ; k<n_ex ; k++)
						{
							if(count == loci[k])
							{
								int allele_0, allele_1;
								memcpy(&allele_0, &chrom_0[j], sizeof(int));
								memcpy(&allele_1, &chrom_1[j], sizeof(int));

								memcpy(&chrom_1[j], &allele_0, sizeof(int));
								memcpy(&chrom_0[j], &allele_1, sizeof(int));
							}
						}

						count++;
					}
				}
			}

			delete [] loci;
		}

		//Add offspring to new generation
		new_gen[i] = chrom_0;
		new_gen[i+n_new] = chrom_1;
	}

	delete [] seed;

	//Return new offspring
	return new_gen;
}


/*************************************************************************************************************

executeTournamentSelection : retrun pairs of parents

**************************************************************************************************************/
std::vector< std::vector<int> > executeTournamentSelection(std::vector<float> fitness, int tournament_size, int n_pair)
{
	//Initialize base variables
	int i, j, k, l;
	int pop_size = fitness.size();

	//Initialize random generator
	VSLStreamStatePtr rng_stream;

	//Generate random seeds
	int *seed = new int[n_pair*2];
	int clock_val = rand()%31000;
	vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
	viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_pair*2, seed, 1, 100000);
	vslDeleteStream(&rng_stream);

	//Initialize vector of pairs
	std::vector< std::vector<int> > pair(n_pair, std::vector<int>(2, -1));

	//Select parents (full pair)
	for(i=0 ; i<n_pair ; i++)
	{
		//Select single parent (pair member)
		for(j=0 ; j<2 ; j++)
		{
			//Initilialize selected chromosome array
			std::vector<int> selected_chromosome(tournament_size, -1);

			for(k=0 ; k<tournament_size ; k++)
			{
				//Select individuals for tournament
				int chrom;
				bool valid = false;
				int count = 0;
				while(!valid)
				{
					vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed[i+j*n_pair]+count);
					viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, 1, &chrom, 0, pop_size);
					vslDeleteStream(&rng_stream);
					count++;

					//Check if already part of the tournament
					for(l=0 ; l<tournament_size ; l++)
					{
						if(chrom == selected_chromosome[l])
						{
							valid = false;
							break;
						}
						valid = true;
					}
				}
				memcpy(&selected_chromosome[k], &chrom, sizeof(int));
			}

			//Pick best individual
			int best_ind = -1;
			float current_best_fitness = -1;
			for(k=0 ; k<tournament_size ; k++)
			{
				if(best_ind == -1 || fitness[selected_chromosome[k]] > current_best_fitness)
				{
					memcpy(&best_ind, &selected_chromosome[k], sizeof(int));
					memcpy(&current_best_fitness, &fitness[selected_chromosome[k]], sizeof(float));
				}
			}

			//Assign parent for crossover
			pair[i][j] = best_ind;
		}
	}

	delete [] seed;

	//Return pairs of parents
	return pair;

}


/*************************************************************************************************************

executeMutation : return mutated new offspring (swap mutation)

**************************************************************************************************************/
std::vector< std::vector<int> > executeMutation(std::vector< std::vector<int> > new_gen, float P_m)
{
	//Initialize base variables
	int i;
	int n_chrom = new_gen.size();
	int n_allele = new_gen[0].size();

	//Initialize random generator
	VSLStreamStatePtr rng_stream;

	//Generate proba of mutation
	float *p_m = new float[n_chrom];
	int clock_val = rand();
	vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
	vsRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_chrom, p_m, 0.0, 1.0);
	vslDeleteStream(&rng_stream);

	//Check if mutation occurs
	for(i=0 ; i<n_chrom ; i++)
	{
		if(p_m[i] < P_m)//Mutate
		{
			//Randomly select two loci to be swapped
			int *loci = new int[2];
			vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
			viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, 2, loci, 0, n_allele);
			vslDeleteStream(&rng_stream);

			//Swap values
			int allele_0, allele_1;
			memcpy(&allele_0, &new_gen[i][loci[0]], sizeof(int));
			memcpy(&allele_1, &new_gen[i][loci[1]], sizeof(int));

			memcpy(&new_gen[i][loci[1]], &allele_0, sizeof(int));
			memcpy(&new_gen[i][loci[0]], &allele_1, sizeof(int));

			delete [] loci;
		}
	}

	delete [] p_m;

	//Return mutated new generation
	return new_gen;
}


/*************************************************************************************************************

renewGeneration : return the new generation (elites + new chromosomes)

**************************************************************************************************************/
std::vector< std::vector<int> > renewGeneration(std::vector< std::vector<int> > eliteChrom, std::vector< std::vector<int> > newChrom)
{
	//Initialize base variables
	int i;
	int n_elite = eliteChrom.size();
	int n_new_pop = n_elite + newChrom.size();
	int n_allele = newChrom[0].size();

	//Initialize new population
	std::vector< std::vector<int> > new_pop(n_new_pop, std::vector<int>(n_allele, -1));

	//Add elites
	for(i=0 ; i<n_elite ; i++)
	{
		new_pop[i] = eliteChrom[i];
	}


	//Add new chromosomes
	for(i=n_elite ; i<n_new_pop ; i++)
	{
		new_pop[i] = newChrom[i-n_elite];
	}


	//Return new generation
	return new_pop;
}


/*************************************************************************************************************

GASolution : determine a 'good' solution via GA

**************************************************************************************************************/
std::vector<int> GASolution(char *ntwk_name, char *cmmt_name, char *folder, int day, std::vector<float> w, std::vector<pipeFailure> failure, int n_Rteam, int Rteam_capacity, int n_gen, int n_chromosome, int n_elite, int tournament_size, float P_c, float P_m, int rank, int rootRank, int size)
{
	//Initialize base variables
	int i, j;
	int n_pair = (n_chromosome - n_elite)/2;

	//Declare MPI variables
	int sp, job_ID;
	MPI_Request requ;
	MPI_Status status;

	status.MPI_SOURCE = -1;
	status.MPI_TAG = -1;
	status.MPI_ERROR = 0;

	//Generate the first generation of potential solutions (chromosomes) and broadcast to all processors
	std::vector< std::vector<int> > solution(n_chromosome, std::vector<int>(failure.size(), -1));

	if(rank == rootRank)
	{
		solution = generateRandomSolutions(failure.size(), n_Rteam*Rteam_capacity, n_chromosome);
		printf("Day %d: %d chromosomes and %d generations\n", day, n_chromosome, n_gen);
	}
	std::vector<float> fitness(solution.size(), -1.0);
	MPI_Barrier(MPI_COMM_WORLD);
	for(i=0 ; i<n_chromosome ; i++)
	{
		MPI_Bcast(&solution[i][0], failure.size(), MPI_INT, rootRank, MPI_COMM_WORLD);
	}

	//Build paths for original network and community
	char *ntwk_path = new char[300];
	char *cmmt_path = new char[300];
	i = sprintf(ntwk_path, "./gen/networks/%s/Network.dat", ntwk_name);
	i = sprintf(cmmt_path, "./gen/communities/%s/Community.dat", cmmt_name);
	//Build path for GA metrics
	char *metrics_path = new char[300];
	i = sprintf(metrics_path, "%s/recovery/day_%d/GAMonitoring.csv", folder, day);

	//GA for n_gen generations
	MPI_Barrier(MPI_COMM_WORLD);
	for(i=0 ; i<n_gen ; i++)
	{
		//Parallel evaluation of all solutions (Master-slave approach: Require more than 1 core)
		//Root process do all the communication tasks
		if(rank == rootRank)
		{
			int h = 10;
			int count = 0;
			int free_worker = -1;
			int done = -1;
			std::vector<int> v_done(size, 0);

			//Initial jobs
			for(j=1 ; j<size ; j++)
			{
				MPI_Send(&count, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
				count++;
			}

			//Do the rest
			while(true)
			{
				//Receive results from workers
				int job_ID_buff = -1;
				float fit_buff = -1.0;

				MPI_Recv(&job_ID_buff, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&fit_buff, 1, MPI_FLOAT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
				fitness[job_ID_buff] = fit_buff;

				//Receive the rank of free worker
				MPI_Recv(&free_worker, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

				//Check if all jobs completed, if yes send end signal to free worker
				if(count == n_chromosome)
				{
					MPI_Send(&done, 1, MPI_INT, free_worker, 0, MPI_COMM_WORLD);
					v_done[free_worker] = 1;
				}
				//If not, send next job
				else
				{
					MPI_Send(&count, 1, MPI_INT, free_worker, 0, MPI_COMM_WORLD);
					count++;
				}

				//If all workers are free, end the simulations
				int sum_done = 0;
				for(j=0 ; j<size ; j++)
				{
					sum_done = sum_done + v_done[j];
				}

				if(sum_done == size-1)
				{
					break;
				}
			}
		}
		//Other processes do the simulations
		else
		{
			//Job ID receiver
			MPI_Recv(&job_ID, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

			while(job_ID != -1)
			{
				//Evaluate solution[job_ID]
				float fit_i = testSingleSolution(ntwk_path, cmmt_path, w, failure, solution[job_ID]);

				//Send job_ID back and fitness
				MPI_Send(&job_ID, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
				MPI_Send(&fit_i, 1, MPI_FLOAT, 0, 2, MPI_COMM_WORLD);

				//Tell free
				MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

				//Receive
				MPI_Recv(&job_ID, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			}
		}

		//Save metrics of population and create new generation
		float std_fitness = -1.0;
		if(rank == rootRank)
		{
			//Write population metrics
			std_fitness = computeStdFitness(fitness);
			float mean_fitness = computeMeanFitness(fitness);
			float max_fitness = getBestFitness(fitness);
		
			ofstream outfile;

			char *dayMetric = new char[60];
			outfile.open(metrics_path, ios_base::app);
			int l = sprintf(dayMetric, "%d ; %d ; %.2f ; %.2f ; %.2f\n", failure.size(), n_chromosome, std_fitness, mean_fitness, max_fitness);

			outfile.write(dayMetric, strlen(dayMetric));

			//Close file
			outfile.close();
			printf("\tGeneration %d:\t max = %.1f\tmean = %.1f\tstd = %.1f\n", i, max_fitness, mean_fitness, std_fitness);
			delete [] dayMetric;
		}

		MPI_Bcast(&std_fitness, 1, MPI_FLOAT, rootRank, MPI_COMM_WORLD);

		//If std of fitness is 0, means the 'best' solution has been found
		if(std_fitness < 0.01)
		{
			//Broadcast new solutions to all cores
			for(j=0 ; j<n_chromosome ; j++)
			{
				MPI_Bcast(&solution[j][0], failure.size(), MPI_INT, rootRank, MPI_COMM_WORLD);
			}
			break;
		}
		else
		{
			if(rank == rootRank)
			{
				//Initilialize new generation
				std::vector< std::vector<int> > new_gen(n_chromosome, std::vector<int>(failure.size(), -1));

				//Elitism
				std::vector< std::vector<int> > eliteChromosome = selectEliteChromosomes(solution, fitness, n_elite);

				//Tournament selection (vectors of pairs)
				std::vector< std::vector<int> > reproductiveChromosome = executeTournamentSelection(fitness, tournament_size, n_pair);

				//Crossover
				new_gen = executeCrossover(solution, reproductiveChromosome, P_c, n_Rteam*Rteam_capacity);

				//Mutation (swap mutation)
				new_gen = executeMutation(new_gen, P_m);

				//Replace old generation by the new one
				solution = renewGeneration(eliteChromosome, new_gen);
			}
		}

		//Broadcast new solutions to all cores
		for(j=0 ; j<n_chromosome ; j++)
		{
			MPI_Bcast(&solution[j][0], failure.size(), MPI_INT, rootRank, MPI_COMM_WORLD);
		}
	}

	//Delete names
	delete [] ntwk_path;
	delete [] cmmt_path;
	delete [] metrics_path;

	int best_ind = -1;
	if(rank == rootRank)
	{
		//Select the best solution from the last generation
		float current_max_fitness = -1.0;

		for(i=0 ; i<solution.size() ; i++)
		{
			if(fitness[i] > current_max_fitness)
			{
				memcpy(&current_max_fitness, &fitness[i], sizeof(float));
				memcpy(&best_ind, &i, sizeof(int));
			}
		}
	}

	//Broadcast best_ind
	MPI_Bcast(&best_ind, 1, MPI_INT, rootRank, MPI_COMM_WORLD);

	//Return best solution
	return solution[best_ind];
}










