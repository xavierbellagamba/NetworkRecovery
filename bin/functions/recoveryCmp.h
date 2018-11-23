/*************************************************************************************************************

histGARecovery : evaluate the recovery of one scenario (multiple realizations) using a master-slave approach

**************************************************************************************************************/
void histGARecovery(std::vector<int> inspection_list, int n_Iteam, float Iteam_capacity, int n_Rteam, int Rteam_capacity, char *map_name,
	char *ntwk_name, char *cmmt_name, char *failure_name, std::vector<float> w, int n_sim, int n_gen_min, int n_gen_max, int n_chromosome_max, int n_GAreal, int n_elite, 
	int tournament_size, float P_c, float P_m, int recovery_type, int rank, int rootRank, int size)
{
	//Declare basic variables
	int i, j, sp, job_ID;
	MPI_Request requ;
	MPI_Status status;

	status.MPI_SOURCE = -1;
	status.MPI_TAG = -1;
	status.MPI_ERROR = 0;

	//Construct GA-based recovery path
	char *folder_str = new char[300];
	if(recovery_type == 1)
	{
		sp = sprintf(folder_str, "./gen/MCS/historical/%s/%s/GA/", ntwk_name, failure_name);
	}
	else
	{
		sp = sprintf(folder_str, "./gen/MCS/historical/%s/%s/random/", ntwk_name, failure_name);
	}
	mkdir(folder_str, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//Prepare simulations (create 1 folder per simulation and copy-paste damage network and impacted community)
	if(rank == rootRank)
	{
		char *original_network = new char[250];
		char *original_community = new char[250];

		sp = sprintf(original_network, "./gen/MCS/historical/%s/%s/simulations/Network.dat", ntwk_name, failure_name);
		sp = sprintf(original_community, "./gen/MCS/historical/%s/%s/simulations/Community.dat", ntwk_name, failure_name);

		//Transform discover dates in n_breaks (pipeSegment) into number of breaks per segment
		ifstream infile;
		ofstream outfile;
		pipeNetwork network;
		network.READ_Network(original_network, infile);

		for(i=0 ; i<network.n_pipes() ; i++)
		{
			int f_count = 0;
			for(j=0 ; j<network.pipe(i)->n_breaks().size() ; j++)
			{
				if(network.pipe(i)->n_breaks()[j] > 0)
				{
					f_count++;
				}
			}

			std::vector<int> n_f_new(1, f_count);
			network.pipe(i)->n_breaks(n_f_new);
		}


		for(i=0 ; i<n_sim ; i++)
		{
			char *sim_folder_GA = new char[300];
			sp = sprintf(sim_folder_GA, "%ssim_%d", folder_str, i);
			mkdir(sim_folder_GA, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

			char *community_dest = new char[300];

			sp = sprintf(community_dest, "%s/Community.dat", sim_folder_GA);


			network.SAVE_Network(sim_folder_GA, outfile);

			ifstream src_2(original_community, std::ios::binary);
			ofstream dst_2(community_dest, std::ios::binary);
			dst_2 << src_2.rdbuf();

			delete [] sim_folder_GA;
			delete [] community_dest;
		}

		delete [] original_network;
		delete [] original_community;
	}

	//Get the folder names of prepared simulations
	MPI_Barrier(MPI_COMM_WORLD);
	std::vector<std::string> sim_folder = fileDir(folder_str, "sim_");

	//For each simulation
	for(i=0 ; i<n_sim ; i++)
	{
		//Get name of the simulation
		char *full_name = new char[350];
		if(recovery_type == 1)
		{
			sp = sprintf(full_name, "./gen/MCS/historical/%s/%s/GA/%s", ntwk_name, failure_name, 
				sim_folder[i].c_str());
		}
		else
		{
			sp = sprintf(full_name, "./gen/MCS/historical/%s/%s/random/%s", ntwk_name, failure_name, 
				sim_folder[i].c_str());
		}

		//Execute recovery
		singleSimulationRecovery(inspection_list, n_Iteam, Iteam_capacity, n_Rteam, Rteam_capacity, full_name, ntwk_name, cmmt_name, w, n_gen_min, n_gen_max, n_chromosome_max, n_GAreal, 
			n_elite, tournament_size, P_c, P_m, recovery_type, rank, rootRank, size);
	}
}


