//**************************
//Damage_csv : export the discovery and repair dates of failures
//**************************
void Damage_csv(damageCollection damage, char* sim_folder)
{
	//Initialize basic variables
	int i;

	//Variable declaration
	char *str_csv_file = new char[300];
	int buff;
	ofstream outfile;

	//Name of the file
	buff = sprintf(str_csv_file, "%s/_post_damage.csv", sim_folder);

	//Open ofstream
	outfile.open(str_csv_file, ios::out);

	//Write the file		
	for(i=0 ; i<damage.n_failures() ; i++)
	{
		int str_ptr;
		char *str_fail = new char[50];

		//Discovery and Repair dates
		str_ptr = sprintf(str_fail, "%d ; %d\n", damage.failure(i)->discoveryDate(), damage.failure(i)->repairDate());
		outfile.write(str_fail, strlen(str_fail));

		delete [] str_fail;
	}

	outfile.close();

	//Clear file string
	delete [] str_csv_file;	
}


//**************************
//Import failure collection
//**************************
damageCollection ImportFailureCollection(char* collection_name)
{
	//Initialize basic variables
	int i, j, k, row_count;

	//Create path of the collection file
	char* collection_path = new char[150];
	i = sprintf(collection_path, "./data/failure_collection/%s.csv", collection_name);

	//Row count
	row_count = Row_counter(collection_path);

	//Initialize the objects
	damageCollection coll;
	std::vector<pipeFailure> failure(row_count);

	//Create the networPoints structure
	std::string line_buffer;
	int arg_counter;
	char *tk_bf;

	ifstream data_stream;
	data_stream.open(collection_path);

	//Import each point of the discretized network as a structure
	for(i=0 ; i<row_count ; i++)
	{
		arg_counter = 0;
		getline(data_stream, line_buffer);
		std::vector<char> v(line_buffer.length() + 1);
		strcpy(&v[0], line_buffer.c_str());
		char *line_buffer_char = &v[0];
		char *token_buffer = strtok(line_buffer_char, ";,");
		++arg_counter;
		std::vector<double> loc;
		while(arg_counter <= 5)
		{
			switch(arg_counter)
			{
				case 1 :
					loc.push_back(atof(token_buffer));
					break;

				case 2 : 
					loc.push_back(atof(token_buffer));
					break;

				case 3 : 
					failure[i].pipeID(atoi(token_buffer));
					break;

				case 4 : 
                                        failure[i].discoveryDate(atoi(token_buffer));
                                        break;

				case 5 : 
                                        failure[i].priority(atoi(token_buffer));
                                        break;
			}
			++arg_counter;
			token_buffer = strtok(NULL, ";,");
		}
		failure[i].failureID(i+1);
		failure[i].pipeUnikID(-1);
		failure[i].location(loc);
		failure[i].repairDate(-1);
	}

	//Close data_steam	
	data_stream.close();

	//Populate failure collection
	coll.failureCollection(failure);
	coll.n_failures(row_count);

	//Delete array
	delete [] collection_path;

	//Return structure
	return coll;
}


//*****************************************************************
//Function to assign pipe failures to pipe using the vector breaks 
//(warning: 0 cannot be a failure indice, use +1)
//*****************************************************************
damageCollection AssignFailure(char* collection_name, char* network_name, char* cmmt_name)
{
	//Declare basic variables
	int i, j, k, l, v;
	double dist;
	ifstream infile;
	ofstream outfile;

	//Create the collection of failure
	damageCollection damage = ImportFailureCollection(collection_name);
	int n_f = damage.n_failures();

	//Load the network
	char* network_path = new char[200];
	i = sprintf(network_path, "./gen/networks/%s/Network.dat", network_name);
	pipeNetwork network;
	network.READ_Network(network_path, infile);
	int n_pipes = network.n_pipes();

	//Create the folder for historic case study
	char* MCS_folder = new char[200];
	i = sprintf(MCS_folder, "./gen/MCS");
	mkdir(MCS_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	char* root_hist_folder = new char[200];
	i = sprintf(root_hist_folder, "./gen/MCS/historical");
	mkdir(root_hist_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	char* ntwk_hist_folder = new char[200];
	i = sprintf(ntwk_hist_folder, "./gen/MCS/historical/%s", network_name);
	mkdir(ntwk_hist_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	char* hist_folder = new char[200];
	i = sprintf(hist_folder, "./gen/MCS/historical/%s/%s", network_name, collection_name);
	mkdir(hist_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	delete [] MCS_folder, root_hist_folder, ntwk_hist_folder;

	//Initialize vector of failure
	std::vector< std::vector<int> > failure_list(n_pipes, std::vector<int>(1, -1));

	//For each failure
	for(i=0 ; i<n_f ; i++)
	{
		//Variable used to escape useless loops
		bool treated = false;
		int last_corresp_ind = -1;

		//For each pipeline
		for(j=0 ; j<n_pipes ; j++)
		{
			//If pipeline ID == failure_collection(pipeID)
			if(network.pipe(j)->db_ID() == damage.failure(i)->pipeID())
			{
				//Update last corresponding index
				last_corresp_ind = j;
				//For each pair of points
				for(k=0 ; k<network.pipe(j)->n_pts()-1 ; k++)
				{
					//Get the max and min of both X, Y coordinates
					double x_max = max(network.pipe(j)->points()[0][k], network.pipe(j)->points()[0][k+1]);
					double x_min = min(network.pipe(j)->points()[0][k], network.pipe(j)->points()[0][k+1]);
					double y_max = max(network.pipe(j)->points()[1][k], network.pipe(j)->points()[1][k+1]);
					double y_min = min(network.pipe(j)->points()[1][k], network.pipe(j)->points()[1][k+1]);

					//Check if break is inside x_k, x_k+1 ; y_k, y_k+1
					if(damage.failure(i)->location()[0] <= x_max &&
						damage.failure(i)->location()[0] >= x_min &&
						damage.failure(i)->location()[1] <= y_max &&
						damage.failure(i)->location()[1] >= y_min)
					{
						//If yes: add failure ID to pipe breaks list
						treated = true;

						if(failure_list[j][0] < 0)
						{
							failure_list[j][0] = damage.failure(i)->failureID();

							//Update failure collection
							damage.failure(i)->pipeUnikID(network.pipe(j)->unik_ID());

							break;
						}
						else
						{
							failure_list[j].push_back(damage.failure(i)->failureID());
							
							//Update failure collection
							damage.failure(i)->pipeUnikID(network.pipe(j)->unik_ID());

							break;
						}
						
					}
				}
				if(treated)
				{
					break;
				}
			}
		}
	}

	//For non-assigned failure, find most probable pipe
	int n_raster = network.raster().size();
	float spacing = sqrt(pow(network.raster()[0][0]-network.raster()[1][0], 2) + pow(network.raster()[0][1]-network.raster()[1][1], 2));
	for(i=0 ; i<n_f ; i++)
	{
		//Check if assigned
		if(damage.failure(i)->pipeUnikID() == -1)
		{
			double closest_dist = 100000000.0;
			int ID_closest = -1;

			double loc_x = damage.failure(i)->location()[0];
			double loc_y = damage.failure(i)->location()[1];

			int rasterID;

			//Determine raster ID
			for(j=0 ; j<n_raster ; j++)
			{
				double x_min = network.raster()[j][0] - spacing/2;
				double x_max = network.raster()[j][0] + spacing/2;
				double y_min = network.raster()[j][1] - spacing/2;
				double y_max = network.raster()[j][1] + spacing/2;

				if((loc_x <= x_max) && (loc_x > x_min) && (loc_y <= y_max) && (loc_y > y_min))
				{
					memcpy(&rasterID, &j, sizeof(int));
					break;
				}
			}

			//Potential rasters in which pipe could lay
			std::vector<int> neighbour = network.nearest_neighbour()[rasterID];

			//For each neighbouring raster
			for(j=0 ; j<neighbour.size() ; j++)
			{
				//For each pipe in neighbouring rasters
				for(k=0 ; k<network.pipe_belong()[neighbour[j]].size() ; k++)
				{
					//Compute orthogonal distance
					int pipeID = network.pipe_belong()[neighbour[j]][k];
					pipeSegment segment = network.pipe_collection()[pipeID];

					//For each pipe segment
					for(l=0 ; l<segment.n_pts()-1 ; l++)
					{
						//Pipe standardized vector
						double norm = sqrt(pow(segment.points()[0][l] - segment.points()[0][l+1], 2) + 
							pow(segment.points()[1][l] - segment.points()[1][l+1], 2));
						if(norm > 0.001)
						{
							std::vector<double> vec(2, -1.0);
							vec[0] = (segment.points()[0][l+1] - segment.points()[0][l]) / norm;
							vec[1] = (segment.points()[1][l+1] - segment.points()[1][l]) / norm;

							double loc_t_x;
							double loc_t_y;

							for(v=0 ; v<101 ; v++)
							{
								loc_t_x = segment.points()[0][l] + v*vec[0]*norm/100.0;
								loc_t_y = segment.points()[1][l] + v*vec[1]*norm/100.0;

								dist = sqrt(pow(loc_t_x-loc_x, 2) + pow(loc_t_y-loc_y, 2));

								if(dist < closest_dist)
								{
									//Update the distance to the closest raster
									memcpy(&closest_dist, &dist, sizeof(double));

									//Update the ID of the closest raster
									memcpy(&ID_closest, &pipeID, sizeof(int));
								}
							}
						}
					}
				}
			}

			if(ID_closest > -1)
			{
				if(failure_list[ID_closest][0] < 0)
				{
					failure_list[ID_closest][0] = damage.failure(i)->failureID();
					damage.failure(i)->pipeUnikID(ID_closest);
				}
				else
				{
					failure_list[ID_closest].push_back(damage.failure(i)->failureID());
					damage.failure(i)->pipeUnikID(ID_closest);
				}
			}
		}
	}

	//Update pipe objects
	for(i=0 ; i<n_pipes ; i++)
	{
		//Replace remaining -1 by 0
		if(failure_list[i][0] == -1)
		{
			failure_list[i][0] = 0;
		}
		
		//Update
		network.pipe(i)->n_breaks(failure_list[i]);
	}

	//Save updated network
	network.SAVE_Network(hist_folder, outfile);

	//Copy-Paste the community file into ./hist/
	char* community_path = new char[200];
	i = sprintf(community_path, "./gen/communities/%s/Community.dat", cmmt_name);
	buildingCommunity community;
	community.READ_Community(community_path, infile);
	community.SAVE_Community(hist_folder, outfile);

	//Delete char tables
	delete [] network_path;
	delete [] hist_folder;
	delete [] community_path;

	//Return results
	return damage;
}


//*****************************************************************************************************************************

//removeNonAssignedFailure : remove failure with pipeUnikID == -1

//*****************************************************************************************************************************
damageCollection removeNonAssignedFailure(damageCollection damage)
{
	//Initialize base variables
	int i, j; 
	int n_f = damage.n_failures();

	//Copy vector of failures
	std::vector<pipeFailure> damage_update = damage.failureCollection();

	for(i=n_f-1 ; i>=0 ; i--)
	{
		if(damage.failure(i)->pipeUnikID() == -1)
		{
			damage_update.erase(damage_update.begin() + i);
		}
	}

	damage.failureCollection(damage_update);
	damage.n_failures(damage_update.size());

	//Return updated DB
	return damage;
}


//*************************
//Failure removal function
//*************************
pipeNetwork FailureRemoval(int day, pipeNetwork network, damageCollection damage)
{
	//Declare basic variables
	int i, j, k;
	int n_f = damage.n_failures();

	//For each failure
	for(i=0 ; i<n_f ; i++)
	{
		//Check if repaired on analysed day
		if(day == damage.failure(i)->repairDate())
		{
			//Analysed pipe ID
			int pipeID = damage.failure(i)->pipeUnikID();

			//If length of break_list == 1, update the value
			if(network.pipe(pipeID)->n_breaks().size() == 1)
			{
				std::vector<int> repaired (1, 0);
				network.pipe(pipeID)->n_breaks(repaired);
			}
			//Else, remove corresponding element
			else
			{
				//Indice to be removed
				int ind2rm;

				//Look for the element's indice to be removed
				for(j=0 ; j<network.pipe(pipeID)->n_breaks().size() ; j++)
				{
					if(network.pipe(pipeID)->n_breaks()[j] == damage.failure(i)->failureID())
					{
						ind2rm = j;
						break;
					}
				}

				//Remove repaired failure
				std::vector<int> failures_update = network.pipe(pipeID)->n_breaks();
				failures_update.erase (failures_update.begin()+ind2rm);

				//Update pipe failure list
				network.pipe(pipeID)->n_breaks(failures_update);
			}
		}
	}

	//Return updated network
	return network;

}


//**************************************
//Iteration process during the recovery 
//**************************************
void HistoricRecovery(char* network_name, char* reco_name, damageCollection damage, int n_reco_day, int sim)
{
	//Declare basic variables
	int i, j, k;
	ifstream infile;
	ofstream outfile;

	//Load the original damaged network
	char *ntwk_path = new char[200];
	i = sprintf(ntwk_path, "./gen/MCS/historical/%s/%s/Network.dat", network_name, reco_name);
	pipeNetwork network;
	network.READ_Network(ntwk_path, infile);

	//Load the original impacted community
	char *comm_path = new char[200];
	i = sprintf(comm_path, "./gen/MCS/historical/%s/%s/Community.dat", network_name, reco_name);
	buildingCommunity community;
	community.READ_Community(comm_path, infile);


	//For each recovery day
	for(i=0 ; i<n_reco_day+1 ; i++) //+1 for last day repairs
	{
		//0. Create result folder
		char* reco_folder = new char[200];
		j = sprintf(reco_folder, "./gen/MCS/historical/%s/%s/simulations/sim_%d/day_%d", network_name, reco_name, sim, i);
		mkdir(reco_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		//1. Pipe connectivity from MCS.h
		network = PipeConnectivity(network, false);

		//2. Building connectivity from MCS.h
		community = BuildingConnectivity(community, network);

		//3. Save the results from MCS.h
		//Network
		PipeConnection_csv(network, reco_folder);
		network.SAVE_Network(reco_folder, outfile);

		//Community
		BuildingConnection_csv(community, reco_folder);
		community.SAVE_Community(reco_folder, outfile);

		//4. Remove repaired breaks
		network = FailureRemoval(i, network, damage);

		//5. Delete tables
		delete [] reco_folder;
	}

	//Delete names
	delete [] comm_path, ntwk_path;
}


//**************
//MCS recovery
//**************
void MCS_Recovery(char* network_name, char* reco_name, damageCollection damage, int n_sim)
{
	//Declare basic variables
	int i, j, k;

	//Number of pipe failures
	int n_f = damage.n_failures();

	//Initialize RN generator
	VSLStreamStatePtr rng_stream;

	//Create folder for simulations
	char* root_reco_folder = new char[200];
	j = sprintf(root_reco_folder, "./gen/MCS/historical/%s/%s/simulations", network_name, reco_name);
	mkdir(root_reco_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	delete [] root_reco_folder;

	//For each simulation
	float sim_count = 1;
	for(i=0 ; i<n_sim ; i++)
	{
		//Create file for recovery simulation i
		char *sim_folder = new char[200];
		j = sprintf(sim_folder, "./gen/MCS/historical/%s/%s/simulations/sim_%d", network_name, reco_name, i);
		mkdir(sim_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		//Initialize date of last repair
		int date_last = 0;

		//Generate RN
		int clock_val = rand();
		vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, clock_val);
		for(j=0 ; j<n_f ; j++)
		{
			int repair_delay;
			viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, rng_stream, 1, &repair_delay, 0, damage.failure(j)->priority());
			int repair_date = repair_delay + damage.failure(j)->discoveryDate();
			damage.failure(j)->repairDate(repair_date);

			//Update last repair date
			if(repair_date > date_last)
			{
				date_last = repair_date;
			}
		}
		vslDeleteStream(&rng_stream);

		//Save repair list as csv
		Damage_csv(damage, sim_folder);
		delete [] sim_folder; 

		//Network recovery
		HistoricRecovery(network_name, reco_name, damage, date_last, i);

		//status update
		if(((float)i/(float)n_sim)*10 >= sim_count)
		{
			printf("%.0f%%\n", 10*sim_count);
			sim_count = sim_count + 1;
		}
	}
	printf("100%%\n");
}


//********************
//Initial performance
//********************
void HistoricOriginalPerformance(char *ntwk_name, char *reco_name)
{
	//Initiliaze varaibles
	int i, j;
	ifstream infile;
	ofstream outfile;

	//Create the paths of objects to be loaded
	char *ntwk_path = new char[250];
	char *cmmt_path = new char[250];
	i = sprintf(ntwk_path, "./gen/MCS/historical/%s/%s/Network.dat", ntwk_name, reco_name);
	i = sprintf(cmmt_path, "./gen/MCS/historical/%s/%s/Community.dat", ntwk_name, reco_name);

	//Create the paths of objects to be saved
	char *root_reco_folder = new char[250];
	i = sprintf(root_reco_folder, "./gen/MCS/historical/%s/%s/simulations", ntwk_name, reco_name);
	mkdir(root_reco_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//Load network
	pipeNetwork damagedNetwork;
	damagedNetwork.READ_Network(ntwk_path, infile);

	//Load community
	buildingCommunity impactedCommunity;
	impactedCommunity.READ_Community(cmmt_path, infile);

	//Delete path of loaded objects
	delete [] ntwk_path, cmmt_path;

	//Perform pipeconnectivity assessment and save network
	damagedNetwork = PipeConnectivity(damagedNetwork, false);
	damagedNetwork.SAVE_Network(root_reco_folder, outfile);

	//Perform building connectivity assessment and save community
	impactedCommunity = BuildingConnectivity(impactedCommunity, damagedNetwork);
	impactedCommunity.SAVE_Community(root_reco_folder, outfile);

	//Delete path of saved objects
	delete [] root_reco_folder;
}


//********************
//historicalRecoveryAnalysis
//********************
void historicRecoveryAnalysis(char *pipe_failure_name, char *network_name, char *community_name, int n_sim)
{
	srand(time(NULL));
	//1. Assignment of pipe failures (return a structure containing all pipe failure)
	printf("\x1b[36mImport historical recovery %s.csv and networks/%s/Network.dat\x1b[0m\n", pipe_failure_name, network_name);
	damageCollection damage = AssignFailure(pipe_failure_name, network_name, community_name);
	damage = removeNonAssignedFailure(damage);
	printf("%d pipe failures assigned to the network\n", damage.n_failures());

	//2. Original performance
	printf("Estimate original performance of the network\n");
	HistoricOriginalPerformance(network_name, pipe_failure_name);
	printf("\x1b[32mSaving network and community with orginal performance in MCS/historical/%s/%s\x1b[0m\n", network_name,
		pipe_failure_name);

	//3. MCS of the repair
	printf("%d recovery simulations\n", n_sim);
	MCS_Recovery(network_name, pipe_failure_name, damage, n_sim);
	printf("\x1b[32m%d MC recovery simulations realized and saved in MCS/historical/%s/%s/simulations\x1b[0m\n", n_sim, network_name,
		pipe_failure_name);
}





