

/*********************
Number of simulations
*********************/
int NumberPipelines(char *root_folder)
{
	//Initialize base variables
	int i;
	ifstream infile;

	//Path of the network
	char *ntwk_path = new char[200];
	i = sprintf(ntwk_path, "%sNetwork.dat", root_folder);

	//Load network
	pipeNetwork network;
	network.READ_Network(ntwk_path, infile);

	//number of pipelines
	int n = network.n_pipes();

	//Delete char
	delete [] ntwk_path;
	
	//Return
	return n;
}


/**********************
Number of fragility fx
**********************/
int NumberFragFx(char *root_folder)
{
	//Initialize base variables
	int i;
	ifstream infile;

	//Path of the network
	char *ntwk_path = new char[250];
	i = sprintf(ntwk_path, "%sNetwork.dat", root_folder);

	//Load network
	pipeNetwork network;
	network.READ_Network(ntwk_path, infile);

	//number of pipelines
	int n = network.pipe_frag_fx().size();

	//Delete char
	delete [] ntwk_path;
	
	//Return
	return n;
}


/*************************************************************************************************************

numberFailure : determine the number of pipe failures in a particular damage network

**************************************************************************************************************/
std::vector<int> numberFailure(pipeNetwork network)
{
	//Initialize base variables
	int i, j;
	int n_pipe = network.n_pipes();
	int n_fx =network.pipe(0)->n_breaks().size();

	//Initialize vector of n_failure (1 per fragility fx)
	std::vector<int> n_failure(n_fx, 0);

	//Count number of failures
	for(i=0 ; i<n_pipe ; i++)
	{
		for(j=0 ; j<n_fx ; j++)
		{
			n_failure[j] = n_failure[j] + network.pipe(i)->n_breaks()[j];
		}
	}

	//Return results
	return n_failure;
}


/************************
Compute inspection score
************************/
float ComputeInspectionScore(float P_f, int diameter, std::string function)
{
	//Transform function into float
	float cst_function;

	if(strcmp(function.c_str(), "Trunk main") == 0)
	{
		cst_function = 10.0;
	}
	else if(strcmp(function.c_str(), "Main") == 0)
	{
		cst_function = 5.0;
	}
	else if(strcmp(function.c_str(), "Crossover") == 0)
	{
		cst_function = 2.0;
	}
	else if(strcmp(function.c_str(), "Submain") == 0)
	{
		cst_function = 1.0;
	}
	else
	{
		cst_function = 1.5;
	}

	//Transform diameter into float
	float cst_diameter = diameter/100.0;

	//Compute the score
	float score = (cst_diameter + cst_function)*pow(100.0, P_f);;

	return score;
}


/****************
Scoring function
****************/
std::vector<float> InspectionScore(char *map_name, char *ntwk_name)
{
	//Initialize base variables
	int i, j, k;
	ifstream infile;
	ofstream outfile;

	//Create path to simulation root folder
	char *sim_root_folder = new char[250];
	char *root_folder = new char[250];
	i = sprintf(sim_root_folder, "./gen/MCS/predictions/%s/%s/simulations/", ntwk_name, map_name);
	i = sprintf(root_folder, "./gen/MCS/predictions/%s/%s/", ntwk_name, map_name);

	//Determine number of simulations
	std::vector<std::string> sim_name;
	sim_name = fileDir(sim_root_folder, "sim_");
	int n_sim = sim_name.size();

	//Load all damaged networks and determine if constitutive pipelines failed
	//Get number of pipelines in network
	int n_pipes = NumberPipelines(root_folder);

	//Get the number of used fragiltiy functions
	i = sprintf(root_folder, "./gen/MCS/predictions/%s/%s/simulations/%s/", ntwk_name, map_name, sim_name[0].c_str());
	int n_fx = NumberFragFx(root_folder);

	//Initialize info vectors
	std::vector<float>P_f(n_pipes, 0);
	std::vector<float>P_disc(n_pipes, 0);
	std::vector<int> fx;
	std::vector<float> fx_trust;
	
	//From last loaded network extract: Pipe diameter, functionality
	for(i=0 ; i<n_sim ; i++)
	{
		//Load simulation i
		char *ntwk_path = new char[200];
		k = sprintf(ntwk_path, "%s%s/Network.dat", sim_root_folder, sim_name[i].c_str());
		pipeNetwork network;
		network.READ_Network(ntwk_path, infile);
		delete [] ntwk_path;

		//If 1st simulation, get used fx
		if(i==0)
		{
			fx = network.pipe_frag_fx();
			fx_trust = network.frag_fx_trust();
		}

		//Extract pipe info
		for(j=0 ; j<n_pipes ; j++)
		{
			for(k=0 ; k< n_fx ; k++)
			{
				//Check if pipeline failed
				if(network.pipe(j)->n_breaks()[k]>0)
				{
					//Increase P_f if failed
					P_f[j] = P_f[j] + fx_trust[k]/n_sim;
				}

				//Add disconnection probability
				if(network.pipe(j)->connected2pump()[k]<1)
				{
					P_disc[j] = P_disc[j] + fx_trust[k]/n_sim;
				}
			}
		}
	}

	//Compute score
	std::vector<float> score(n_pipes, -1);
	for(i=0 ; i<n_pipes ; i++)
	{
		score[i] = (1-P_disc[i]+P_f[i])/((1-P_f[i])*(1-P_f[i])+0.0001);
	}

	//Delete chars
	delete [] sim_root_folder;
	delete [] root_folder;

	//Return score
	return score;
}


/*************************************************************************************************************

generateInspectionList : given number of team and their capacity, return 2 vectors of booleans (1 for pipe and 1 for failure)

**************************************************************************************************************/
std::vector<int> generateInspectionList(std::vector<float> score)
{
	//Initialize base variables
	int i, j;
	int n_pipe = score.size();

	//Initialize inspection list, inspected list and max score
	std::vector<int> inspectionList(n_pipe, -1);
	std::vector<bool> inspected(n_pipe, 0);
	float max_score = *std::max_element(score.begin(), score.end());

	for(i=0 ; i<n_pipe ; i++)
	{
		//Initialize index of next max value and temporary max score
		int ind_score_max = -1;
		float temp_score = -1.0;

		//Look for the next highest score
		for(j=0 ; j<n_pipe ; j++)
		{
			if((score[j] > temp_score) && (score[j] <= max_score) && !(inspected[j]))
			{
				memcpy(&temp_score, &score[j], sizeof(float));
				memcpy(&ind_score_max, &j, sizeof(int));
			}
		}

		//Update inspected list, max_score and the inspection list
		memcpy(&max_score, &temp_score, sizeof(float));
		inspectionList[i] = ind_score_max;
		inspected[ind_score_max] = 1;
	}

	//Return the list
	return inspectionList;
}


/*************************************************************************************************************

InspectionList_csv : save the inspection list as csv containing the priority of inspection and the score along pipe IDs and length

**************************************************************************************************************/
void InspectionList_csv(std::vector<int> inspectionList, std::vector<float> score, char *map_name, char *ntwk_name)
{
	//Initialize base variables
	int i, j;
	ofstream outfile;
	ifstream infile;

	//Load the network
	char *ntwk_path = new char[200];
	i = sprintf(ntwk_path, "./gen/networks/%s/Network.dat", ntwk_name);
	pipeNetwork network;
	network.READ_Network(ntwk_path, infile);

	//Name of the file to save
	char *list_path = new char[300];
	i = sprintf(list_path, "./gen/MCS/predictions/%s/%s/inspection_list.csv", ntwk_name, map_name);

	//Open ofstream
	outfile.open(list_path, ios::out);

	//Write the header
	char *header = new char[150];
	i = sprintf(header, "Loc X ; Loc Y ; DB ID ; ID ; Length [m] ; Score\n");
	outfile.write(header, strlen(header));

	//Write lines
	for(i=0 ; i<inspectionList.size() ; i++)
	{
		int ind = inspectionList[i];
		char *line = new char[250];
		j = sprintf(line, "%.1f ; %.1f ; %d ; %d ; %.2f ; %.2f\n", 
			network.pipe(ind)->center_loc()[0],
			network.pipe(ind)->center_loc()[1],
			network.pipe(ind)->db_ID(),
			network.pipe(ind)->unik_ID(),
			network.pipe(ind)->length(), 
			score[ind]);
		outfile.write(line, strlen(line));
		delete [] line;
	}

	//Delete names and close stream
	delete [] ntwk_path;
	delete [] list_path;
	delete [] header;
	outfile.close();
}


/*************************************************************************************************************

getInspectionList : generate the inspection list and save it as a csv file in the scenario root folder

**************************************************************************************************************/
std::vector<int> getInspectionList(char *map_name, char *ntwk_name)
{
	//Initialize base variables
	int i, j;

	//Generate the inspection score
	std::vector<float> score = InspectionScore(map_name, ntwk_name);

	//Generate list
	std::vector<int> inspectionList = generateInspectionList(score);

	//Save the inspection lsit as csv
	InspectionList_csv(inspectionList, score, map_name, ntwk_name);

	//Return the inspection list
	return inspectionList;
}


/*************************************************************************************************************

inspectNetwork : given number of team and capacity, return one vectror of booleans representing the inspected (1) - non-inspected (0) pipes

**************************************************************************************************************/
std::vector<bool> inspectNetwork(pipeNetwork network, std::vector<int> inspection_list, std::vector<bool> pipe_stat, int n_team, 
	float team_capacity)
{
	//Initialize base variables
	int i, j;
	int n_pipe = network.n_pipes();
	int n_fx =network.pipe(0)->n_breaks().size();

	//Inspectable pipe length
	float inspect_length_max = n_team * team_capacity;

	//Initialize inspected length
	float inspect_length = 0.0;

	//As long as inspected pipe length < max inspected pipe length
	i = 0;
	while(inspect_length < inspect_length_max)
	{
		//Inspect pipe if previously not inpsected
		if(!(pipe_stat[inspection_list[i]]))
		{
			inspect_length = inspect_length + network.pipe(i)->length();
			pipe_stat[inspection_list[i]] = 1;
		}

		i++;
	}

	//Return inspected status
	return pipe_stat;
}


/*************************************************************************************************************

addDetectedFailure : add detected failures to the failure collection

**************************************************************************************************************/
std::vector<pipeFailure> addDetectedFailure(pipeNetwork network, std::vector<pipeFailure> failure, std::vector<bool> pipe_stat, 
	std::vector<bool> pipe_stat_old, int ind_fail_detect, int date)
{
	//Initialize base variables
	int i, j;
	int n_pipe = network.n_pipes();

	for(i=0 ; i<n_pipe ; i++)
	{
		//If newly inspected and damaged
		if((pipe_stat[i] == 1) && (pipe_stat_old[i] == 0) && (network.pipe(i)->n_breaks()[0] > 0))
		{
			//Create the pipe failures
			for(j=0 ; j<network.pipe(i)->n_breaks()[0] ; j++)
			{
				ind_fail_detect++;
				failure[ind_fail_detect].failureID(ind_fail_detect);
				failure[ind_fail_detect].pipeID(network.pipe(i)->db_ID());
				failure[ind_fail_detect].discoveryDate(date);
				failure[ind_fail_detect].pipeUnikID(i);
				failure[ind_fail_detect].repairDate(-1);
				failure[ind_fail_detect].repaired(0);
			}
		}
	}

	//Return updated damage collection
	return failure;
}


/*************************************************************************************************************

updateFailureIndex : update the number of detected failure

**************************************************************************************************************/
int updateFailureIndex(std::vector<pipeFailure> failure, int ind_fail_detect)
{
	for(int i=ind_fail_detect+1 ; i<failure.size() ; i++)
	{
		if(failure[i].failureID() == -1)
		{
			break;
		}
		ind_fail_detect++;
	}

	//Return result
	return ind_fail_detect;
}


/*************************************************************************************************************

generateUnrepairedFailureList : generate a list of unrepaired but known pipe failures

**************************************************************************************************************/
std::vector<pipeFailure> generateUnrepairedFailureList(std::vector<pipeFailure> failure)
{
	//Initialize base variables
	int i, j, n_unrepaired = 0;
	int n_f = failure.size();

	//Number of known and unrepaired failure
	for(i=0 ; i<n_f ; i++)
	{
		if((failure[i].failureID() != -1) && (failure[i].repaired() == 0))
		{
			n_unrepaired++;
		}
	}

	//Initialize vector of known and unrepaired failure
	std::vector<pipeFailure> unrepairedFailure(n_unrepaired);

	//Create the vector of unrepaired failures
	j = 0;
	for(i=0 ; i<n_f ; i++)
	{
		if((failure[i].failureID() != -1) && (failure[i].repaired() == 0))
		{
			memcpy(&unrepairedFailure[j], &failure[i], sizeof(pipeFailure));
			j++;
		}
	}

	//Return vector of unrepaired pipe failure
	return unrepairedFailure;
}


/*************************************************************************************************************

evaluateSearchSpaceSize : evaluate the search space size using binomial coefficient (reccursive function)

**************************************************************************************************************/
unsigned long long binomialCoeff(int n, int k)
{
	if(k>n)
	{
		return 1;
	}
	if(k>0)
	{
		return n * binomialCoeff(n-1, k-1) / k;
	}
	else
	{
		return 1;
	}
}


/*************************************************************************************************************

evaluateSearchSpaceSize : evaluate the search space size using binomial coefficient

**************************************************************************************************************/
unsigned long long evaluateSearchSpaceSize(int n_Rteam, int Rteam_capacity, int n_f)
{
	//Number of potential repair
	int k = n_Rteam * Rteam_capacity;

	//Compute the number of potential solutions
	unsigned long long C = binomialCoeff(n_f, k);

	//If C too large, return 100000
	if(C>1000000)
	{
		return 1000000;
	}
	else
	{
		return C;
	}
}


/*************************************************************************************************************

generateAllSolution : Reccursively compute all possible solutions (using impera and divide paradigm)

**************************************************************************************************************/
std::vector< std::vector<int> > generateAllSolutions(int l, int k, std::vector<int> base, std::vector< std::vector<int> > solution)
{
	if(l > 0)
	{
		std::vector<int> base0 = base;
		base0.push_back(0);
		solution = generateAllSolutions(l-1, k, base0, solution);
		std::vector<int> base1 = base;
		base1.push_back(1);
		solution = generateAllSolutions(l-1, k, base1, solution);
	}
	else
	{
		//Sum all bits to be sure that all capacity is used
		int sum = 0;
		for(int i=0 ; i<base.size() ; i++)
		{
			if(base[i])
			{
				sum++;
			}
		}
		//If all capacity used, add to solution collection
		if(sum == k)
		{
			solution.push_back(base);
		}
	}

	//Return vector of solution
	return solution;
}


/*************************************************************************************************************

optimalSolution : find the optimal solution for repairs (objectie function: numbe rof people, assumption: WYSIATI)

**************************************************************************************************************/
std::vector<int> optimalSolution(char *ntwk_name, char *cmmt_name, std::vector<float> w, std::vector<pipeFailure> failure, int n_Rteam, int Rteam_capacity, int day, int rank, int rootRank, int size)
{
	//Initialize base variables
	int i;

	//Declare MPI variables
	int sp, job_ID;
	MPI_Request requ;
	MPI_Status status;

	status.MPI_SOURCE = -1;
	status.MPI_TAG = -1;
	status.MPI_ERROR = 0;

	//Generate the first generation of potential solutions (chromosomes) and broadcast to all processors
	int n_s = binomialCoeff(failure.size(), n_Rteam*Rteam_capacity);
	if(n_s == 0)
	{
		n_s = 1;
	}
	std::vector< std::vector<int> > solution(n_s, std::vector<int>(failure.size(), -1));
	if(rank == rootRank)
	{
		std::vector<int> base;
		solution.clear();
		if(failure.size() > n_Rteam*Rteam_capacity)
		{
			solution = generateAllSolutions(failure.size(), n_Rteam*Rteam_capacity , base, solution);
		}
		else
		{
			std::vector<int> solution_(failure.size(), 1);
			solution.push_back(solution_);
		}
	}
	std::vector<float> fitness(solution.size(), -1.0);
	MPI_Barrier(MPI_COMM_WORLD);
	for(i=0 ; i<n_s ; i++)
	{
		MPI_Bcast(&solution[i][0], failure.size(), MPI_INT, rootRank, MPI_COMM_WORLD);
	}

	//Build paths for original network and community
	char *ntwk_path = new char[200];
	char *cmmt_path = new char[200];
	i = sprintf(ntwk_path, "./gen/networks/%s/Network.dat", ntwk_name);
	i = sprintf(cmmt_path, "./gen/communities/%s/Community.dat", cmmt_name);

	//GA for n_gen generations
	MPI_Barrier(MPI_COMM_WORLD);
	//Parallel evaluation of all solutions (Master-slave approach: Require more than 1 core)
	//Root process do all the communication tasks
	if(rank == rootRank)
	{
		printf("Day %d:\t %d failures, optimal solution\n", day, failure.size());
		int h = 10;
		int count = 0;
		int free_worker = -1;
		int done = -1;
		std::vector<int> v_done(size-1, 0);

		//Initial jobs
		for(i=1 ; i<size ; i++)
		{
			if(count < n_s)
			{
				MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				count++;
			}
			else
			{
				MPI_Send(&done, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				v_done[i-1] = 1;
			}
		}

		//Do the rest
		while(true)
		{
			int job_ID_buff = -1;
			float fit_buff = -1.0;
			//Receive the rank of free worker
			MPI_Recv(&free_worker, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			//Receive results from workers
			MPI_Recv(&job_ID_buff, 1, MPI_INT, free_worker, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&fit_buff, 1, MPI_FLOAT, free_worker, 0, MPI_COMM_WORLD, &status);
			if(job_ID_buff != -1)
			{
				fitness[job_ID_buff] = fit_buff;
			}

			//Check if all jobs completed, if yes send end signal to free worker
			if(count == n_s)
			{
				MPI_Send(&done, 1, MPI_INT, free_worker, 0, MPI_COMM_WORLD);
				v_done[free_worker-1] = 1;
			}
			//If not, send next job
			else
			{
				MPI_Send(&count, 1, MPI_INT, free_worker, 0, MPI_COMM_WORLD);
				count++;
			}

			//Status update
			if(((float)count/(float)n_s)*100 >= h)
			{
				h = ((float)count/(float)n_s)*100;
				printf("\tCompletion: %d%%\n", h);
			}

			//If all workers are free, end the simulations
			int sum_done = 0;
			for(i=1 ; i<size ; i++)
			{
				sum_done = sum_done + v_done[i-1];
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

		//If initial job has ID -1, tell free and tell receive no job
		if(job_ID == -1)
		{
			float fit_i = -1.0;
			//Tell free
			MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&job_ID, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&fit_i, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
		}
		while(job_ID != -1)
		{
			//Evaluate solution[job_ID]
			float fit_i = -1.0;
			fit_i = testSingleSolution(ntwk_path, cmmt_path, w, failure, solution[job_ID]);

			//Tell free
			MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			//Send job_ID back and fitness
			MPI_Send(&job_ID, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&fit_i, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);

			//Receive
			MPI_Recv(&job_ID, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
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

	//Delete names
	delete [] ntwk_path;
	delete [] cmmt_path;
	
	//Return best solution
	return solution[best_ind];
}


/*************************************************************************************************************

randomSolution : generate a random solution for repairs (objectie function: numbe rof people, assumption: WYSIATI)

**************************************************************************************************************/
std::vector<int> randomSolution(int n_f, int n_Rteam, int Rteam_capacity)
{
	std::vector< std::vector<int> > random_sol_vec = generateRandomSolutions(n_f, n_Rteam*Rteam_capacity, 1);
	return random_sol_vec[0];
}


/*************************************************************************************************************

updatePumpStationLoS : update the level of service of pump stations based on their restoration date

**************************************************************************************************************/
pipeNetwork updatePumpStationLoS(pipeNetwork network, int day)
{
	//Initialize base variables
	int i, j;
	int n_pump = network.n_pumps();

	//Full LoS
	std::vector<float> los_ud (1, 1.0);

	//Update for each pump station
	for(i=0 ; i< n_pump ; i++)
	{
		if(network.pump(i)->restoration_date() > day && network.pump(i)->los()[0] < 0.99)
		{
			network.pump(i)->los(los_ud);
		}
	}

	//Return updated network
	return network;
}


/*************************************************************************************************************

removeRepairFailure : remove selected repairs

**************************************************************************************************************/
pipeNetwork removeRepairFailure(pipeNetwork network, std::vector<pipeFailure> failure, std::vector<int> solution)
{
	//Initialize base variables
	int i;

	//Remove repaired failures 
	for(i=0 ; i<solution.size() ; i++)
	{
		if(solution[i] == 1)
		{
			int pipeID = failure[i].pipeUnikID();
			std::vector<int> update_failure = network.pipe(pipeID)->n_breaks();
			update_failure[0] = update_failure[0] - 1;
			network.pipe(pipeID)->n_breaks(update_failure);
		}
	}

	//Return network
	return network;
}


/*************************************************************************************************************

updateRepairedFailureStatus : update the status of repaired failures

**************************************************************************************************************/
std::vector<pipeFailure> updateRepairedFailureStatus(std::vector<pipeFailure> failure, std::vector<pipeFailure> unrepairedFailure,
	std::vector<int> repairedFailure)
{
	for(int i=0 ; i<repairedFailure.size() ; i++)
	{
		if(repairedFailure[i] == 1)
		{
			int ind2repair = unrepairedFailure[i].failureID();
			failure[ind2repair].repaired(1);
		}
	}

	//Return updated failure list
	return failure;
}


/*************************************************************************************************************

singleSimulationRecovery : evaluate the recovery of one simulation based on 1 scenario analysis (multiple realizations)

**************************************************************************************************************/
void singleSimulationRecovery(std::vector<int> inspection_list, int n_Iteam, float Iteam_capacity, int n_Rteam, int Rteam_capacity, char *sim_folder, char *ntwk_name, char *cmmt_name, 
	std::vector<float> w, int n_gen_min, int n_gen_max, int n_chromosome_max, int n_GAreal, int n_elite, int tournament_size, float P_c, float P_m, int recovery_type, int rank, int rootRank, int size)
{
	//Initialize base variables
	srand(time(NULL));
	int i;
	int n_thres = -1;
	ifstream infile;
	ofstream outfile;

	//Create recovery path 
	char *reco_path = new char[300];
	i = sprintf(reco_path, "%s/recovery/", sim_folder);
	mkdir(reco_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//Determine number of pipe failures
	std::vector<int> n_fail(1, 1);

	//Initialize empty pipe failure collection
	std::vector<pipeFailure> failure;

	//Initialize vectors of booleans for inspected pipe and discovered failure
	std::vector<bool> pipe_stat;
	std::vector<bool> fail_stat;

	//Initialize repair, discovered and day counter
	int n_repair = 0;
	int n_disc = 0;
	int n_day = 0;
	int ind_fail_detect = -1;

	//Varaible GA parameters
	int n_gen = -1;
	int n_chromosome = -1;

	//As long as not all pipe failures are repaired
	while(n_repair < n_fail[0])
	{
		//Initilaize names
		char *ntwk_path = new char[300];
		char *cmmt_path = new char[300];

		char *day_save = new char[300];
		i = sprintf(day_save, "%sday_%d/", reco_path, n_day);
		if(rank == rootRank)
		{
			mkdir(day_save, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}

		//Initialize network and community
		pipeNetwork network;
		buildingCommunity community;

		//If n_day == 0 => load the root network and community
		if(n_day == 0)
		{
			//Build the names
			i = sprintf(ntwk_path, "%s/Network.dat", sim_folder);
			i = sprintf(cmmt_path, "%s/Community.dat", sim_folder);

			//Load network and community
			network.READ_Network(ntwk_path, infile);
			community.READ_Community(cmmt_path, infile);

			//Get number of failure
			n_fail = numberFailure(network);

			//Initialize empty pipe failure collection
			failure.resize(n_fail[0]);
			for(i=0 ; i<n_fail[0] ; i++)
			{
				failure[i].failureID(-1);
			}

			//Initialize vectors of booleans for inspected pipe and discovered failure
			pipe_stat.resize(network.n_pipes(), 0);
			fail_stat.resize(n_fail[0]);
		}
		//If n_day > 0 => load the network and community from n_day-1
		else
		{
			//Build the names
			i = sprintf(ntwk_path, "%s/recovery/day_%d/Network.dat", sim_folder, n_day-1);
			i = sprintf(cmmt_path, "%s/recovery/day_%d/Community.dat", sim_folder, n_day-1);

			//Load network and community
			network.READ_Network(ntwk_path, infile);
			community.READ_Community(cmmt_path, infile);
		}

		//Delete names
		delete [] ntwk_path;
		delete [] cmmt_path;

		//Initialize daily repair counter
		int n_repair_day = 0;

		//Initialize vector of repaired failure
		std::vector<int> repairedFailure;
		std::vector<pipeFailure> unrepairedFailure;

		MPI_Barrier(MPI_COMM_WORLD);

		//Repair if unrepaired failure collection not empty
		if(n_disc > n_repair)
		{
			//Update pump station serviceability
			network = updatePumpStationLoS(network, n_day);

			//Generate list of unrepaired but discovered pipe failure
			unrepairedFailure = generateUnrepairedFailureList(failure);

			//GA-based recovery
			if(recovery_type == 1)
			{
				//Evaluate search space size
				unsigned long long C = evaluateSearchSpaceSize(n_Rteam, Rteam_capacity, unrepairedFailure.size());

				//If search space relatively small (<n_gen*n_chromosome)
				if(C <= n_thres)
				{
					//Optimal solution (assumption: WYSIATI)
					repairedFailure = optimalSolution(ntwk_name, cmmt_name, w, unrepairedFailure, n_Rteam,
						Rteam_capacity, n_day, rank, rootRank, size);
				}
				else
				{
					//GA (assumption: WYSIATI)
					repairedFailure = GASolution(ntwk_name, cmmt_name, sim_folder, n_day, w, unrepairedFailure, n_Rteam, Rteam_capacity, 						n_gen, n_chromosome, n_elite, tournament_size, P_c, P_m, rank, rootRank, size);
				}
			}
			else if(recovery_type == 0)
			{
				if(unrepairedFailure.size() > n_Rteam*Rteam_capacity)
				{
					repairedFailure = randomSolution(unrepairedFailure.size(), n_Rteam, Rteam_capacity);
				}
				else
				{
					std::vector<int> final_random_repair(unrepairedFailure.size(), 1);
					repairedFailure = final_random_repair;
				}
			}

			MPI_Barrier(MPI_COMM_WORLD);

			//Update number of repair
			for(i=0 ; i<repairedFailure.size() ; i++)
			{
				if(repairedFailure[i])
				{
					n_repair++;
					n_repair_day++;
				}
			}

			//Update status of reapired failure
			failure = updateRepairedFailureStatus(failure, unrepairedFailure, repairedFailure);

			//Remove failure from network
			network = removeRepairFailure(network, unrepairedFailure, repairedFailure);

			//Update network and community
			network = PipeConnectivity(network, false);
			community = BuildingConnectivity(community, network);
		}

		//Search for pipe failure as long as n_disc < n_fail
		if(n_disc < n_fail[0])
		{
			//Inspect non-inspected pipe segments
			std::vector<bool> pipe_stat_old = pipe_stat;
			pipe_stat = inspectNetwork(network, inspection_list, pipe_stat, n_Iteam, Iteam_capacity);

			//If new failures discovered, add them to the collection
			failure = addDetectedFailure(network, failure, pipe_stat, pipe_stat_old, ind_fail_detect, n_day);

			//Update index of detected failures
			ind_fail_detect = updateFailureIndex(failure, ind_fail_detect);
			n_disc = ind_fail_detect + 1;

			//Update number of chromosomes and generations
			n_chromosome = (n_disc - n_repair)*10;
			if(n_chromosome > n_chromosome_max)
			{
				n_chromosome = n_chromosome_max;
			}
			n_gen = n_gen_max;
			if(n_GAreal<n_gen*n_chromosome)
			{
				n_gen = n_GAreal/n_chromosome;
				if(n_gen<n_gen_min)
				{
					n_gen = n_gen_min;
				}
			}
			
			//Update n_thres for the search of optimal solution using brute force approach
			n_thres = n_gen * n_chromosome/10;
		}

		if(rank == rootRank)
		{
			//Save network and community in a dedicated folder
			network.SAVE_Network(day_save, outfile);
			community.SAVE_Community(day_save, outfile);
			PipeConnection_csv(network, day_save);
			BuildingConnection_csv(community, day_save);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		delete [] day_save;

		//Update number of days after event
		n_day++;
	}

	//Delete names
	delete [] reco_path;
}
