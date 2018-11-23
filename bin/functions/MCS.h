/*###############################################################################################################################

DamageGeneration : compute the number of failures per pipeline for all selected fragility models

###############################################################################################################################*/
pipeNetwork DamageGeneration(pipeNetwork originalNetwork, std::vector<float> GM_intensity, int ntwk_lvl, int soil_lvl, struct BR_tables BR_t,
	int seed, int i_sim)
{
	//Variable declaration
	//int i, j, k;
	std::vector<int> fx_set = originalNetwork.pipe_frag_fx();
	int n_fx_set = fx_set.size();

	//Number of pipes
	int n = originalNetwork.pipe_collection().size();

	//Creation of the object damagedNetwork
	pipeNetwork damagedNetwork = originalNetwork;
	damagedNetwork.network_ID(i_sim);

	//Generate random numbers to be compared with the probabilities of failure (1 number per pipe segment)
	std::vector<float> P_compare(n);
	VSLStreamStatePtr rng_stream;
	vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed);
        vsRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n, &P_compare[0], 0.0, 1.0);
        vslDeleteStream(&rng_stream);

	//Computation of the break rate for each pipe segment for each set of functions chosen
	damagedNetwork = BR_fx(damagedNetwork, P_compare, GM_intensity, fx_set, ntwk_lvl, soil_lvl, BR_t);

	//Return damaged network
	return damagedNetwork;
}


/*###############################################################################################################################

PipeConnectivity : determine whether or not a pipe remains connected given the generated damage

###############################################################################################################################*/
pipeNetwork PipeConnectivity(pipeNetwork damagedNetwork, bool recoBase)
{
	int i, j, i_fx;
	int n_pumps = damagedNetwork.n_pumps();
	int n_frag_fx = damagedNetwork.pipe(0)->n_breaks().size();
	std::vector< std::vector<int> >connections(n_frag_fx, std::vector<int>(damagedNetwork.n_pipes(), -1));

	//For each fragility function used to assess the pipe vulnerability
	for(i_fx=0 ; i_fx<n_frag_fx ; i_fx++)
	{
		//Initiliaze one empty list of connected pipes
		std::vector<int> pipeList_pump (damagedNetwork.n_pipes(), 0);

		//For each pump
		for(i=0 ; i<n_pumps ; i++)
		{
			//Check if pump i LoS equal to 1
			if(damagedNetwork.pump(i)->los()[0]>0.99 || recoBase == true)
			{
				//For each pipeline connected to the pump, run connectivity algorithm
				for(j=0 ; j<damagedNetwork.pump(i)->pipe_out_ID().size() ; j++)
				{
					pipe_connect_new(pipeList_pump, &damagedNetwork, damagedNetwork.pump(i)->pipe_out_ID()[j], i_fx);
				}
			}
		}

		//Update connectivity for frag_fx i
		connections[i_fx] = pipeList_pump;
	}

	//Modify the objects
	for(i=0 ; i<damagedNetwork.n_pipes() ; i++)
	{
		//Initialize pipe connectivity
		std::vector<int> conn_pipe(n_frag_fx, -1);

		for(j=0 ; j<n_frag_fx ; j++)
		{
			//If not connected => -1
			if(connections[j][i] != 0)
			{
				conn_pipe[j] = connections[j][i];
			}
		}

		//Update pipe segmenet
		damagedNetwork.pipe(i)->connected2pump(conn_pipe);
	}

	//Return updated network
	return damagedNetwork;		
}


/*###############################################################################################################################

BuildingConnectivity : evaluate the buiding connectivity given the pipe damage

###############################################################################################################################*/
buildingCommunity BuildingConnectivity(buildingCommunity originalCommunity, pipeNetwork damagedNetwork)
{
	//Declare variables
	int i, j;
	buildingCommunity impactedCommunity(originalCommunity);
	int n_fx = damagedNetwork.pipe(0)->n_breaks().size();

	//For each building, check if surving pipe is connected
	for(i=0 ; i<originalCommunity.n_buildings() ; i++)
	{
		//Initialize connection vector
		std::vector<int> connections(n_fx, -1);

		//Get ID of serving pipe
		int pipeID = impactedCommunity.building(i)->pipeID_connect();

		//For each pipe fragility function used
		if(pipeID >= 0)
			{
			for(j=0 ; j<n_fx ; j++)
			{
				//Determine if serving if connected
				if(damagedNetwork.pipe(pipeID)->connected2pump()[j] >= 0)//Still connected
				{
					connections[j] = 1;
				}
				else//Disconnected
				{
					connections[j] = 0;
				}
			}

			//Modify the objects
			impactedCommunity.building(i)->is_connect(connections);
		}
	}

	//Return results
	return impactedCommunity;
}


//Save pump los
void PumpLoS_csv()
{
	
}


/*###############################################################################################################################

PipeFailure_csv : save the pipe damage into a csv file

###############################################################################################################################*/
void PipeFailure_csv(pipeNetwork damagedNetwork, char *sim_folder)
{
	//Variable declaration
	char *str_csv_file = new char[200];
	int buff;
	ofstream outfile;

	//Name of the file
	buff = sprintf(str_csv_file, "%s/_post_network_pipefailure.csv", sim_folder);

	//Open ofstream
	outfile.open(str_csv_file, ios::out);

	//Write the file		
	for(int i=0 ; i<damagedNetwork.n_pipes() ; i++)
	{
		int str_ptr;
		char *str_pipe = new char[100];

		//Pipe ID
		str_ptr = sprintf(str_pipe, "%d", damagedNetwork.pipe(i)->unik_ID());
		outfile.write(str_pipe, strlen(str_pipe));
			
		//Failures
		for(int j=0 ; j<damagedNetwork.pipe(i)->n_breaks().size() ; j++)
		{
			char *str_loc = new char[30];
			str_ptr = sprintf(str_loc, "; %d", damagedNetwork.pipe(i)->n_breaks()[j]);
			outfile.write(str_loc, strlen(str_loc));
			delete [] str_loc;
                }
		outfile.write("\n", strlen("\n"));

		delete [] str_pipe;
	}
	outfile.close();

	//Clear file string
	delete [] str_csv_file;	
}


/*###############################################################################################################################

PipeConnection_csv: save the pipe connectivity results into a csv file

###############################################################################################################################*/
void PipeConnection_csv(pipeNetwork damagedNetwork, char *sim_folder)
{
	//Variable declaration
	char *str_csv_file = new char[200];
	int buff;
	ofstream outfile;

	//Name of the file
	buff = sprintf(str_csv_file, "%s/_post_network_pipeconnection.csv", sim_folder);

	//Open ofstream
	outfile.open(str_csv_file, ios::out);

	//Write the file		
	for(int i=0 ; i<damagedNetwork.n_pipes() ; i++)
	{
		int str_ptr;
		char *str_pipe = new char[100];

		//Pipe ID
		str_ptr = sprintf(str_pipe, "%d", damagedNetwork.pipe(i)->unik_ID());
		outfile.write(str_pipe, strlen(str_pipe));

		//For each analysed fragility function
		for(int j=0 ; j<damagedNetwork.pipe(i)->n_breaks().size() ; j++)
		{
			char *str_loc = new char[30];
			str_ptr = sprintf(str_loc, "; %d", damagedNetwork.pipe(i)->connected2pump()[j]);
			outfile.write(str_loc, strlen(str_loc));
			delete [] str_loc;
                }
		outfile.write("\n", strlen("\n"));

		delete [] str_pipe;
	}
	outfile.close();

	//Clear file string
	delete [] str_csv_file;	
}


/*###############################################################################################################################

BuildingConnectivition_csv : save the building connectivity results into a csv file

###############################################################################################################################*/
void BuildingConnection_csv(buildingCommunity impactedCommunity, char *sim_folder)
{
	//Variable declaration
	char *str_csv_file = new char[200];
	int buff;
	ofstream outfile;
	int n_fx = impactedCommunity.building(0)->is_connect().size();

	//Name of the file
	buff = sprintf(str_csv_file, "%s/_post_community_buildingconnection.csv", sim_folder);

	//Open ofstream
	outfile.open(str_csv_file, ios::out);

	//Write the file		
	for(int i=0 ; i<impactedCommunity.n_buildings() ; i++)
	{
		int str_ptr;
		char *str_building = new char[10];

		//Building ID
		str_ptr = sprintf(str_building, "%d ", impactedCommunity.building(i)->unik_ID());
		outfile.write(str_building, strlen(str_building));

		//For each used pipe fragility function
		for(int j=0 ; j<n_fx ; j++)
		{
			int s = impactedCommunity.building(i)->is_connect().size();
			if(s > 0)
			{
				str_ptr = sprintf(str_building, " ; %d", impactedCommunity.building(i)->is_connect()[j]);
				outfile.write(str_building, strlen(str_building));
			}
			else
			{
				str_ptr = sprintf(str_building, " ; -1");
				outfile.write(str_building, strlen(str_building));
			}
		}
		outfile.write("\n", strlen("\n"));

		delete [] str_building;
	}
	outfile.close();

	//Clear file string
	delete [] str_csv_file;	
}


/*###############################################################################################################################

computeEmpiricalGM : compute the empirically-derived GM intensities for a single MCS realization

###############################################################################################################################*/
std::vector<float> computeEmpiricalGM(int seed_ID_core, singleScenarioEmpiricalGM map_IM_emp)
{
	//Declare basic variables
	int i;

	//Extract relevant characteristics
	int gm_dim = map_IM_emp.n_rasters();
	//packed_cov = map_IM_emp.packed_cov();

	//Null vector of dimension gm_dim
	float *vec_0 = new float[gm_dim];
	for(i=0 ; i<gm_dim ; i++)
	{
		vec_0[i] = 0.0;
	}

	//Generate GM intensities
	//Initialize array of spatial residuals and GM intensities
	float *spatial_residual = new float[gm_dim];
	std::vector<float> GM_intensity(gm_dim, 0.0);

	//Generation of random numbers
	VSLStreamStatePtr rng_stream_sim;
	vslNewStream(&rng_stream_sim, VSL_BRNG_SFMT19937, seed_ID_core);
	vsRngGaussianMV(VSL_RNG_METHOD_GAUSSIAN_BOXMULLER, rng_stream_sim, 1, spatial_residual, gm_dim, VSL_MATRIX_STORAGE_PACKED, vec_0, 
		map_IM_emp.packed_cov().data());
	vslDeleteStream(&rng_stream_sim);

	//Computation and assignment of the GM intensity
	for(i=0 ; i<gm_dim ; i++)
	{
		GM_intensity[i] = exp(map_IM_emp.lnPGV()[i][0] + spatial_residual[i]);
	}

	delete [] vec_0;
	delete [] spatial_residual;

	//Return GM intensities
	return GM_intensity;
}


/*###############################################################################################################################

computePhysicsGM : compute the empirically-derived GM intensities for a single MCS realization

###############################################################################################################################*/
std::vector<float> computePhysicsGM(int seed_ID_core, singleScenarioPhysicsGM map_IM)
{
	//Extract the GM intensities
	std::vector<float> GM_intensity = map_IM.PGV()[seed_ID_core];

	//Return GM intensities
	return GM_intensity;
}



/*###############################################################################################################################

realizeSingleMCS : realize MCS simulations based on empirically generated GM (MPI) using data-paralell architecture

###############################################################################################################################*/
void realizeSingleMCS(char *map_name, char *ntwk_name, int analysis_lvl, int n_sim, std::vector<int> fx_set, std::vector<float> fx_trust, 
	int ntwk_lvl, int soil_lvl, int rank, int i_sim, int seed2_ID_core, std::vector<float> GM_intensity, pipeNetwork originalNetwork, 
	struct BR_tables BR_t, buildingCommunity originalCommunity, bool recoBase)
{
	//Declare basic variables
	int i;
	ofstream outfile;

	//Create result folder
	char *result_folder = new char[200];
	int ptr_sim = sprintf(result_folder, "./gen/MCS/predictions/%s/%s/simulations/sim_%d_%d", ntwk_name, map_name, rank, i_sim);
	mkdir(result_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	//Generate damage
	pipeNetwork damagedNetwork = DamageGeneration(originalNetwork, GM_intensity, ntwk_lvl, soil_lvl, BR_t, seed2_ID_core, i_sim);
	//Save pipe failures
	PipeFailure_csv(damagedNetwork, result_folder);


	//Check pipe connectivity
	if(analysis_lvl > 0.1)
	{
		damagedNetwork = PipeConnectivity(damagedNetwork, recoBase);
		PipeConnection_csv(damagedNetwork, result_folder);
	}

	//Save results for the network
	damagedNetwork.SAVE_Network(result_folder, outfile);


	//Check building connectivity
	if(analysis_lvl > 1.1)
	{
		buildingCommunity impactedCommunity = BuildingConnectivity(originalCommunity, damagedNetwork);
		BuildingConnection_csv(impactedCommunity, result_folder);
		//Save results for the community
		impactedCommunity.SAVE_Community(result_folder, outfile);
	}

	//Delete single simulation related variables
	delete [] result_folder;
	//delete [] spatial_residual;
}


/*###############################################################################################################################

MCS_DP : realize MCS simulations using data-parallel architecture

###############################################################################################################################*/
void MCS_DP(char *map_name, char *ntwk_name, int analysis_lvl, int n_sim, std::vector<int> fx_set, std::vector<float> fx_trust, int ntwk_lvl,
	int soil_lvl, bool recoBase, int rank, int rootRank, int size)
{
	//Declaration of basic variables
	int i, j, k, l, progress_status, i_sim, rc, ptr_sim, ptr_1;
	float progress;
	ifstream infile;
	ofstream outfile;

	if(rank == rootRank)
	{
		printf("Prepare multicore processing for %d MC simulations\n", n_sim);
		printf("Data-parallel architecture applied (number of cores used < 5)\n");
	}

	//Files to load
	//GM map
	char *map_path = new char[100];
	//Network
	char *ntwk_path = new char[200];
	ptr_1 = sprintf(ntwk_path, "./gen/MCS/predictions/%s/%s/Network.dat", ntwk_name, map_name);
	//Community
	char *comm_path = new char[200];
	ptr_1 = sprintf(comm_path, "./gen/MCS/predictions/%s/%s/Community.dat", ntwk_name, map_name);


	//Create folder
	if(rank == rootRank)
	{
		//Create root result folder
		char *root_result_folder = new char[200];
		ptr_sim = sprintf(root_result_folder, "./gen/MCS/predictions/%s/%s/simulations", ntwk_name, map_name);
		mkdir(root_result_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		delete [] root_result_folder;

		//Load the GM map
		printf("\x1b[36mImport ground motion %s/GM.egm\x1b[0m\n", map_name);
	}


	//Load the map and relevant characteristics
	int GMM_type = GM_map_type(map_name);
	singleScenarioEmpiricalGM map_IM_emp;
	singleScenarioPhysicsGM map_IM_ph;
	int n_realization;

	//Empirical
	if(GMM_type == 0)
	{
		//Load map
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.egm", map_name);
		map_IM_emp.READ_Map(map_path, infile);
	}
	//Physics
	else
	{
		//Load map
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.pgm", map_name);
		map_IM_ph.READ_Map(map_path, infile);

		//Extract the number of realizations
		n_realization = map_IM_ph.PGV().size();
	}
	

	//Make sure all processors have loaded the map
	rc = MPI_Barrier(MPI_COMM_WORLD);


	//Load network
	if(rank == rootRank)
	{
		printf("\x1b[36mImport network MCS/predictions/%s/%s/Network.dat\x1b[0m\n", ntwk_name, map_name);
	}
	pipeNetwork originalNetwork;
	originalNetwork.READ_Network(ntwk_path, infile);

	//Load community
	buildingCommunity originalCommunity;
	if(analysis_lvl == 2)
	{
		if(rank == rootRank)
		{
			printf("\x1b[36mImport community MCS/predictions/%s/%s/Community.dat\x1b[0m\n", ntwk_name, map_name);
		}
		originalCommunity.READ_Community(comm_path, infile);
	}

	//Load the BR coefficient tables
	struct BR_tables BR_t = load_BR_tables();

	//Update fragility functions and fragility fx trust and size of n_break in pipe objects
	originalNetwork.pipe_frag_fx(fx_set);
	originalNetwork.frag_fx_trust(fx_trust);
	std::vector<int> n_f_0(fx_set.size(), -1);

	for(i=0 ; i<originalNetwork.n_pipes() ; i++)
	{
		originalNetwork.pipe(i)->n_breaks(n_f_0);
	}


	//Pre-process - preparing data to be send over
	//Number of simulation per process
	int n_sim_core = n_sim/size;
	//Add the remainder to the last process
	if(rank == size-1)
	{
		n_sim_core += n_sim%size;
	}

	//Initialize variables
	//Initialize receiving varaibles
	int *seed_ID_core = new int[n_sim_core];
	int *seed2_ID_core = new int[n_sim_core];

	//Initialize data arrays
	int *seed_ID = new int[n_sim];
	int *seed2_ID = new int[n_sim];

	//Initialize displacement and count array for scatterv
	int s_counts[size], s_displ[size];

	if(rank == rootRank)
	{
		//Randomly generate the seed IDs
		VSLStreamStatePtr rng_stream;
		//First array of seeds
		int clock_val = getClock();
		vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
		if(GMM_type == 0)
		{
			viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_sim, seed_ID, 1, 100*n_sim);
		}
		else
		{
			viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_sim, seed_ID, 0, n_realization);
		}
		vslDeleteStream(&rng_stream);

		//Second array of seeds
		clock_val = getClock();
		vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
		viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_sim, seed2_ID, 1, 100*n_sim);
		vslDeleteStream(&rng_stream);

		//Compute the send counts and send displacements
		int sum = 0;
		for(i=0 ; i<size ; i++)
		{
			s_counts[i] = n_sim/size;
			if(i == size-1)
			{
				s_counts[i] += n_sim%size;
			}
			s_displ[i] = sum;
			sum += s_counts[i];
		}
	}

	//Make sure all processes have received their data before starting MCS
	rc = MPI_Barrier(MPI_COMM_WORLD);

	//Send the data
	rc = MPI_Scatterv(&seed_ID[0], s_counts, s_displ, MPI_INT, &seed_ID_core[0], n_sim_core, MPI_INT, rootRank, MPI_COMM_WORLD);
	rc = MPI_Scatterv(&seed2_ID[0], s_counts, s_displ, MPI_INT, &seed2_ID_core[0], n_sim_core, MPI_INT, rootRank, MPI_COMM_WORLD);

	//Clear unnecessary data
	delete [] seed_ID;
	delete [] seed2_ID;


	//MCS
	if(rank == rootRank)
	{
		printf("Preparation ready\n");
	}
	printf("%d simulations for core %d\n", n_sim_core, rank);
	float sim_count = 1;

	for(i_sim=0 ; i_sim<n_sim_core ; i_sim++)
	{
		//Create result folder
		char *result_folder = new char[200];
		int ptr_sim = sprintf(result_folder, "./gen/MCS/predictions/%s/%s/simulations/sim_%d_%d", ntwk_name, map_name, rank, i_sim);
		mkdir(result_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


		//Get GM intenisty fort simulation i depending on the generation method
		std::vector<float> GM_intensity;
		//Empirical
		if(GMM_type == 0)
		{
			GM_intensity = computeEmpiricalGM(seed_ID_core[i_sim], map_IM_emp);
		}
		//Physics
		else
		{
			GM_intensity = computePhysicsGM(seed_ID_core[i_sim], map_IM_ph);
		}

		//Realize the MCS
		realizeSingleMCS(map_name, ntwk_name, analysis_lvl, n_sim, fx_set, fx_trust, ntwk_lvl, soil_lvl, rank, i_sim, 
			seed2_ID_core[i_sim], GM_intensity, originalNetwork, BR_t, originalCommunity, recoBase);


		//status update
		if(((float)i_sim/(float)n_sim_core)*10 >= sim_count)
		{
			printf("Core %d:\t%.0f%%\n", rank, 10*sim_count);
			sim_count = sim_count + 1;
		}
	}

	printf("Core %d:\t100%%\n", rank);

	delete [] seed_ID_core;
	delete [] seed2_ID_core;
	delete [] map_path;
	delete [] ntwk_path;
	delete [] comm_path;
	//delete [] vec_0;

	//Wait all the processes
	rc = MPI_Barrier(MPI_COMM_WORLD);


	if(rank == rootRank)
	{
		printf("\x1b[32m%d MC simulations realized and saved in MCS/predictions/%s/%s/simulations\x1b[0m\n", n_sim, 
			ntwk_name, map_name);
	}
}



/*###############################################################################################################################

MCS_MS : realize MCS simulations using master-slave architecture

###############################################################################################################################*/
void MCS_MS(char *map_name, char *ntwk_name, int analysis_lvl, int n_sim, std::vector<int> fx_set, std::vector<float> fx_trust, int ntwk_lvl,
	int soil_lvl, bool recoBase, int rank, int rootRank, int size)
{
	//Declare basic variables
	int i, j, k, l, progress_status, i_sim, rc, ptr_sim, ptr_1;
	int *job_ID = new int[3];
	float progress;
	ifstream infile;
	ofstream outfile;
	MPI_Request requ;
	MPI_Status status;

	status.MPI_SOURCE = -1;
	status.MPI_TAG = -1;
	status.MPI_ERROR = 0;

	if(rank == rootRank)
	{
		printf("Prepare multicore processing for %d MC simulations\n", n_sim);
		printf("Master-slave architecture applied (number of cores used > 4)\n");
		printf("\tMaster rank: %d\n", rootRank);
		printf("\tNumber of slaves: %d\n", size-1);
	}

	//Load the map
	char *map_path = new char[100];
	int GMM_type = GM_map_type(map_name);
	singleScenarioEmpiricalGM map_IM_emp;
	singleScenarioPhysicsGM map_IM_ph;
	int n_realization, gm_dim;

	//Empirical
	if(GMM_type == 0)
	{
		//Load map
		if(rank == rootRank)
		{
			printf("\x1b[36mImport ground motion %s/GM.egm\x1b[0m\n", map_name);
		}
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.egm", map_name);
		map_IM_emp.READ_Map(map_path, infile);
		gm_dim = map_IM_emp.n_rasters();
	}
	//Physics
	else
	{
		//Load map
		if(rank == rootRank)
		{
			printf("\x1b[36mImport ground motion %s/GM.pgm\x1b[0m\n", map_name);
		}
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.pgm", map_name);
		map_IM_ph.READ_Map(map_path, infile);

		//Extract info
		n_realization = map_IM_ph.PGV().size();
		gm_dim = map_IM_ph.n_rasters();
	}


	//Root process do all the communication tasks
	if(rank == rootRank)
	{
		//Initialize master variables
		int h = 10;
		int count = 0;
		int free_worker = -1;
		int done = -1;
		std::vector<int> v_done(size-1, 0);


		//Create root result folder
		char *root_result_folder = new char[200];
		ptr_sim = sprintf(root_result_folder, "./gen/MCS/predictions/%s/%s/simulations", ntwk_name, map_name);
		mkdir(root_result_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		delete [] root_result_folder;


		//Generate the required seeds
		//Initialize data arrays
		int *seed_ID = new int[n_sim];
		int *seed2_ID = new int[n_sim];

		//Randomly generate the seed IDs
		VSLStreamStatePtr rng_stream;
		//First array of seeds
		int clock_val = rand()%32000;
		vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
		if(GMM_type == 0)
		{
			viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_sim, seed_ID, 1, 100*n_sim);
		}
		else
		{
			viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_sim, seed_ID, 0, n_realization);
		}
		vslDeleteStream(&rng_stream);

		//Second array of seeds
		clock_val = rand();
		vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val+1);
		viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, n_sim, seed2_ID, 1, 100*n_sim);
		vslDeleteStream(&rng_stream);


		//Send initial jobs
		for(i=1 ; i<size ; i++)
		{
			job_ID[0] = count; 
			job_ID[1] = seed_ID[count];
			job_ID[2] = seed2_ID[count];
			MPI_Send(&job_ID[0], 3, MPI_INT, i, 0, MPI_COMM_WORLD);
			count++;
		}


		//Do the rest
		while(true)
		{
			//Receive the rank of free worker
			MPI_Recv(&free_worker, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

			//Check if all jobs completed, if yes send end signal to free worker
			if(count == n_sim)
			{
				job_ID[0] = -1; 
				job_ID[1] = -1;
				job_ID[2] = -1;
				MPI_Send(&job_ID[0], 3, MPI_INT, free_worker, 0, MPI_COMM_WORLD);
				v_done[free_worker-1] = 1;
			}
			//If not, send next job
			else
			{
				
				job_ID[0] = count; 
				job_ID[1] = seed_ID[count];
				job_ID[2] = seed2_ID[count];
				MPI_Send(&job_ID[0], 3, MPI_INT, free_worker, 0, MPI_COMM_WORLD);
				count++;
			}

			//Status update
			if(((float)count/(float)n_sim)*100 >= h)
			{
				printf("Completion: %d%%\n", h);
				h = h + 10;
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

		delete [] seed_ID;
		delete [] seed2_ID;
	}
	//Slaves do the job
	else
	{
		//Files to load
		//Network
		char *ntwk_path = new char[200];
		ptr_1 = sprintf(ntwk_path, "./gen/MCS/predictions/%s/%s/Network.dat", ntwk_name, map_name);
		//Community
		char *comm_path = new char[200];
		ptr_1 = sprintf(comm_path, "./gen/MCS/predictions/%s/%s/Community.dat", ntwk_name, map_name);


		//Load network
		pipeNetwork originalNetwork;
		originalNetwork.READ_Network(ntwk_path, infile);
		if(rank == rootRank+1)
		{
			printf("\x1b[36mImport community MCS/predictions/%s/%s/Network.dat\x1b[0m\n", ntwk_name, map_name);
		}

		//Load community
		buildingCommunity originalCommunity;
		if(analysis_lvl == 2)
		{
			originalCommunity.READ_Community(comm_path, infile);
			if(rank == rootRank+1)
			{
				printf("\x1b[36mImport community MCS/predictions/%s/%s/Community.dat\x1b[0m\n", ntwk_name, map_name);
			}
		}

		//Load the BR coefficient tables
		struct BR_tables BR_t = load_BR_tables();

		//Update fragility functions and fragility fx trust
		originalNetwork.pipe_frag_fx(fx_set);
		originalNetwork.frag_fx_trust(fx_trust);
		std::vector<int> n_f_0(fx_set.size(), -1);
		for(i=0 ; i<originalNetwork.n_pipes() ; i++)
		{
			originalNetwork.pipe(i)->n_breaks(n_f_0);
		}

	
		//Receive initial seed IDs
		MPI_Recv(&job_ID[0], 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		//Do the rest
		while(job_ID[0] != -1)
		{
			//Do the job
			//Create result folder
			char *result_folder = new char[200];
			int ptr_sim = sprintf(result_folder, "./gen/MCS/predictions/%s/%s/simulations/sim_%d_%d", ntwk_name, map_name, rank,
				job_ID[0]);
			mkdir(result_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


			//Get GM intenisty fort simulation i depending on the generation method
			std::vector<float> GM_intensity(gm_dim);
			//Empirical
			if(GMM_type == 0)
			{
				GM_intensity = computeEmpiricalGM(job_ID[1], map_IM_emp);
			}
			//Physics
			else
			{
				GM_intensity = computePhysicsGM(job_ID[1], map_IM_ph);
			}

			//Realize the MCS
			realizeSingleMCS(map_name, ntwk_name, analysis_lvl, n_sim, fx_set, fx_trust, ntwk_lvl, soil_lvl, rank, job_ID[0], 
				job_ID[2], GM_intensity, originalNetwork, BR_t, originalCommunity, recoBase);


			//Tell free
			MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

			//Receive
			MPI_Recv(&job_ID[0], 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

			delete [] result_folder;
		}

		delete [] ntwk_path;
		delete [] comm_path;

	}

	//Wait all the processes
	if(rank == rootRank)
	{
		printf("\x1b[32m%d MC simulations realized and saved in MCS/predictions/%s/%s/simulations\x1b[0m\n", n_sim, 
			ntwk_name, map_name);
	}

	delete [] job_ID;
	delete [] map_path;

	MPI_Barrier(MPI_COMM_WORLD);
}



/*###############################################################################################################################

Main function

###############################################################################################################################*/
void MCS(char *map_name, char *ntwk_name, int ntwk_lvl, int soil_lvl, int n_sim, std::vector<int> fx_set, std::vector<float> fx_trust, 
	int analysis_lvl, bool recoBase, int rank, int rootRank, int size)
{
	srand(time(NULL));
	//Use data-parallel architecture
	if(size<5)
	{
		MCS_DP(map_name, ntwk_name, analysis_lvl, n_sim, fx_set, fx_trust, ntwk_lvl, soil_lvl, recoBase, rank, rootRank, size);
	}
	//Use master-slave architecture
	else
	{
		MCS_MS(map_name, ntwk_name, analysis_lvl, n_sim, fx_set, fx_trust, ntwk_lvl, soil_lvl, recoBase, rank, rootRank, size);
	}
}

