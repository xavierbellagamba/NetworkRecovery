/*Create Community

Contains 2 functions: 
	- CreateCommunityEmpiricalGM
	- CreateCommunityPhysicsGM

Both functions require the same input: the original name of the network (without extension), the name of map (without its extension), the name of the network to be created, the max length of a pipe segment and the year of the analyzed network

*/


/*###############################################################################################################################

OriginalCommunity_csv : Save the network geometry for post-processing purposes as csv file

###############################################################################################################################*/
void OriginalCommunity_csv(buildingCommunity originalCommunity, char *network_folder)
{
	//Variable declaration
	char *str_csv_file = new char[200];
	int buff, i;
	ofstream outfile;

	//Name of the file
	buff = sprintf(str_csv_file, "%s/_post_community_geometry.csv", network_folder);

	//Open ofstream
	outfile.open(str_csv_file, ios::out);

	//Write the file		
	for(i=0 ; i<originalCommunity.n_buildings() ; i++)
	{
		int str_ptr;
		char *str_pipe = new char[200];

		//Building ID, size, location E-N
		str_ptr = sprintf(str_pipe, "%d ; %f ; %f ; %s ; %f ; %f ; %f", originalCommunity.building(i)->unik_ID(), 
			originalCommunity.building(i)->area(), originalCommunity.building(i)->pop(), 
			originalCommunity.building(i)->build_type().c_str(), originalCommunity.building(i)->importance(), 
			originalCommunity.building(i)->center_loc()[0], originalCommunity.building(i)->center_loc()[1]);

		outfile.write(str_pipe, strlen(str_pipe));
			
		outfile.write("\n", strlen("\n"));

		delete [] str_pipe;
	}
	outfile.close();

	//Clear char*
	delete [] str_csv_file;
}


/*###############################################################################################################################

importCommunityData: Import data points from csv file

###############################################################################################################################*/
std::vector<buildingFootprint> importCommunityData(char *origin_cmmt_path, int cmmt_year)
{
	//Declare basic variables
	int i, j;
	int row_count = Row_counter(origin_cmmt_path);


	//Create the building footprints structure
	buildingFP *buildFP = new buildingFP[row_count];
	string line_buffer;
	int arg_counter;
	char *tk_bf;


	//Initialize data stream
	ifstream data_stream;
	data_stream.open(origin_cmmt_path);


	//Import each building footprint as a structure
	for(i=0 ; i<row_count ; i++)
	{
		arg_counter = 0;
		getline(data_stream, line_buffer);
		std::vector<char> v(line_buffer.length() + 1);
		strcpy(&v[0], line_buffer.c_str());
		char *line_buffer_char = &v[0];
		char *token_buffer = strtok(line_buffer_char, ";,");
		++arg_counter;
		while(arg_counter <= 9)
		{
			switch(arg_counter)
			{
				case 1 :
					buildFP[i].center_loc.push_back(atof(token_buffer));
					break;

				case 2 : 
					buildFP[i].center_loc.push_back(atof(token_buffer));
					break;

				case 3 : 
					buildFP[i].db_ID = atoi(token_buffer); 
					break;

				case 4 : 
					buildFP[i].mb_ID = atoi(token_buffer); 
					break;

				case 5 : 
                                        buildFP[i].area = atof(token_buffer); 
                                        break;

				case 6 : 
                                        buildFP[i].pop = atof(token_buffer); 
                                        break;

				case 7 :
					buildFP[i].crr = atof(token_buffer);
					break;

				case 8 :
					buildFP[i].buildType = token_buffer;
					break;

				case 9 : 
					buildFP[i].importance = atof(token_buffer);
					break;

			}
			++arg_counter;
			token_buffer = strtok(NULL, ";,");
		}
	}

	//Close data stream
	data_stream.close();

	
	//Creation of the objects
	std::vector<buildingFootprint> building(row_count);
	std::vector<buildingFootprint*> ref_building(row_count);

	//Initialize pseudo-list of fragility functions
	std::vector< std::vector<int> > frag_fx {{-1}};

	//Pseudo connection
	std::vector<int> pseudo_connection;

	//Estimate the population, area and assign center
	for(i=0 ; i<row_count ; i++)
	{
		//Referencing object i
		ref_building[i] = &building[i];

		//Database ID
		building[i].db_ID(buildFP[i].db_ID);

		//Meshblock ID
		building[i].mb_ID(buildFP[i].mb_ID);

		//Population
		building[i].pop(buildFP[i].pop);

		//Area
		building[i].area(buildFP[i].area);

		//Center location
		building[i].center_loc(buildFP[i].center_loc);

		//CRR
		building[i].crr(buildFP[i].crr);

		//Building type
		building[i].build_type(buildFP[i].buildType);

		//Structure type
		building[i].struct_type("Unknown");

		//Foundation type
		building[i].found_type("Unknown");

		//Number of storey
		building[i].n_storey(-1);

		//Is connected
		building[i].is_connect(pseudo_connection);

		//Year construction
		building[i].year_constr(-1);

		//Unik ID
		building[i].unik_ID(i);

		//List fragility fx
		building[i].list_fragFx(frag_fx);

		//GM raster ID
		building[i].GMraster_ID(-1);

		//Pipe ID conenct
		building[i].pipeID_connect(-1);

		//Importance
		building[i].importance(buildFP[i].importance);
	}

	delete [] buildFP;

	//Return vector of objects
	return building;
}


/*###############################################################################################################################

createCommunityMap : create a blank raster map for the community as a base for the nearest neighbour algorithm (based on network map)

###############################################################################################################################*/
std::vector< std::vector<double> > createCommunityMap(pipeNetwork network)
{
	//Create a copy of the map
	std::vector< std::vector<double> > map_copy = network.raster();

	//Return the copy
	return map_copy;
}


/*###############################################################################################################################

getNeighbourRasterFromNetwork : get the nearest neighbour rasters from the network map

###############################################################################################################################*/
std::vector< std::vector<int> > getNeighbourRasterFromNetwork(pipeNetwork network)
{
	//Create a copy of the map
	std::vector< std::vector<int> > neighbour_copy = network.nearest_neighbour();

	//Return the copy
	return neighbour_copy;
}


/*###############################################################################################################################

assignRasterID2Building: assign raster ID to building

###############################################################################################################################*/
std::vector<buildingFootprint> assignRasterID2Building(std::vector<buildingFootprint> building, std::vector< std::vector<double> > raster)
{
	//Declae base variables
	int i, j;
	float d_max = sqrt(pow((raster[1][0]-raster[0][0])/2, 2)*2);
	int n_build = building.size();
	int n_raster = raster.size();


	//Assign raster to buildings
	for(i=0 ; i<n_build ; i++)
	{
		//Initialize variables
		float closest_dist = 10000000000.0;//Initialize large so will be updated on the first iteration
		float dist = 0.0;
		int ID_closest = -1;

		for(j=0 ; j<n_raster ; j++)
		{
			//Compute distance between raster j and pipeline i
			dist = sqrt(pow(building[i].center_loc()[0] - raster[j][0], 2) + 
				pow(building[i].center_loc()[1] - raster[j][1], 2));

			//Update closest raster ID and closest distance is needed
			if(dist < closest_dist)
			{
				//Update the distance to the closest raster
				memcpy(&closest_dist, &dist, sizeof(float));

				//Update the ID of the closest raster
				memcpy(&ID_closest, &j, sizeof(int));
			}

			//Quit loop if closest dist is smaller than sqrt(c²)
			if(closest_dist<d_max)
			{
				break;
			}
		}


		//Assign raster to building
		building[i].raster_ID(ID_closest);
	}


	//Return updated objects
	return building;
}


/*###############################################################################################################################

assignBuildingID2Raster: assign raster ID to building

###############################################################################################################################*/
std::vector< std::vector<int> > assignBuildingID2Raster(std::vector<buildingFootprint> building, std::vector< std::vector<double> > raster)
{
	//Declare base variables
	int i;
	int n_build = building.size();
	int n_raster = raster.size();

	
	//Assign buildings to rasters
	//Initialize vector
	std::vector< std::vector <int> > belong_building(n_raster);

	for(i=0 ; i<n_build ; i++)
	{
		//Push-back ID of the pipe
		belong_building[building[i].raster_ID()].push_back(i);
	}

	//Return result
	return belong_building;
}


/*###############################################################################################################################

getBuildingConnection : determine which submain pipeline is serving each building based on distance (shortest->serve)

###############################################################################################################################*/
std::vector<buildingFootprint> getBuildingConnection(std::vector<buildingFootprint> building, pipeNetwork network)
{
	//Declare base variables
	int i, j, k, l;
	int n_build = building.size();
	
	//Assign serving pipeline (assumption: closest submain is serving)
	for(i=0 ; i<n_build ; i++)
	{
		//Initialize variables
		float closest_dist = 10000000000.0;//Initialize large so will be updated on the first iteration
		float dist = 0.0;
		int ID_closest = -1;

		//Look into neighbour rasters
		for(j=0 ; j<network.nearest_neighbour()[building[i].raster_ID()].size() ; j++)
		{
			int ind_near_raster = network.nearest_neighbour()[building[i].raster_ID()][j];
			//For each pipeline in neighbour raster j
			for(k=0 ; k<network.submain_belong()[ind_near_raster].size() ; k++)
			{
				int ind_submain = network.submain_belong()[ind_near_raster][k];
				//For each point of pipeline k
				for(l=0 ; l<network.pipe(ind_submain)->n_pts() ; l++)
				{
					//Compute distance between point k of pipeline j and building i
					dist = sqrt(pow(building[i].center_loc()[0] - 
						network.pipe(ind_submain)->points()[0][l], 2) + 
						pow(building[i].center_loc()[1] - 
						network.pipe(ind_submain)->points()[1][l], 2));

					//Update closest raster ID and closest distance is needed
					if(dist < closest_dist)
					{
						//Update the distance to the closest raster
						memcpy(&closest_dist, &dist, sizeof(float));

						//Update the ID of the closest raster
						int buff = network.pipe(ind_submain)->unik_ID();
						memcpy(&ID_closest, &buff, sizeof(int));
					}

					if(closest_dist<10.0)
					{
						break;
					}
				}

				if(closest_dist<10.0)
				{
					break;
				}
			}

			if(closest_dist<10.0)
			{
				break;
			}
		}

		//Assign pipeline to building
		building[i].pipeID_connect(ID_closest);
	}


	//Number of connected buildings
	int n_build_connect = 0;

	for(i=0 ; i<n_build ; i++)
	{
		if(building[i].pipeID_connect() != -1)
		{
			n_build_connect++;
		}
	}

	//Remove building if building not connected to any pipeline
	std::vector<buildingFootprint> newBuilding(n_build_connect);
	int count = 0;

	for(i=0 ; i<n_build ; i++)
	{
		if(building[i].pipeID_connect() != -1)
		{
			newBuilding[count] = building[i];
			newBuilding[count].unik_ID(count);
			count++;
		}
	}
	

	//Return updated building footprints
	return building;
}


/*###############################################################################################################################

evaluateImportance: evaluate the importance of each building for the recovery

###############################################################################################################################*/
std::vector<buildingFootprint> evaluateImportance(std::vector<buildingFootprint> building)
{
	//Declare base variables
	int i;
	int n_build = building.size();


	for(i=0 ; i<n_build ; i++)
	{
		if(strncmp("Unknown", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(building[i].pop());
		}
		else if(strncmp("Hospital", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(10000.0);
		}
		else if(strncmp("Marae", building[i].build_type().c_str(), 4) == 0)
		{
			building[i].importance(250.0);
		}
		else if(strncmp("Schools", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(100.0);
		}
		else if(strncmp("Police station", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(100.0);
		}
		else if(strncmp("Fire station", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(100.0);
		}
		else if(strncmp("Administration", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(250.0);
		}
		else if(strncmp("Medical center", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(1000.0);
		}
		else if(strncmp("Commercial", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(building[i].area()/1000.0);
		}
		else if(strncmp("Industrial", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(building[i].area()/1000.0);
		}
		else if(strncmp("Residential", building[i].build_type().c_str(), 6) == 0)
		{
			building[i].importance(building[i].pop());
		}
		else
		{
			building[i].importance(building[i].pop());
		}
	}


	//Return updated building collection
	return building;
}


/*###############################################################################################################################

removeNonConnectedBuildings : remove buildings that are not linked to any submain pipe (too far away from network)

###############################################################################################################################*/
std::vector<buildingFootprint> removeNonConnectedBuildings(std::vector<buildingFootprint> building)
{
	//Declare base variables
	int i;
	int n_build = building.size();

	//Remove non-connected
	for(i=n_build-1 ; i>=0 ; i--)
	{
		if(building[i].pipeID_connect() == -1)
		{
			building.erase(building.begin()+i);
		}
	}

	//Update ID
	n_build = building.size();
	for(i=0 ; i<n_build ; i++)
	{
		building[i].unik_ID(i);
	}

	//Return updated collection
	return building;
}


/*###############################################################################################################################

createCommunity : Create community object containing building footprint objects from csv files

###############################################################################################################################*/
void createCommunity(char *origin_cmmt_name, char *ntwk_name, char *cmmt_name, unsigned cmmt_year)
{
	//Initialize base variables
	ifstream data_stream;


	//Create result folder
	char *cmmt_folder = new char[100];
	int ptr_1 = sprintf(cmmt_folder, "./gen/communities/%s", cmmt_name);
	mkdir("./gen", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./gen/communities", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(cmmt_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	//ID number of the community
	unsigned cmmt_ID = 0;


	//Path of the files to load
	//CSV
	char *origin_cmmt_path = new char[50];
	ptr_1 = sprintf(origin_cmmt_path, "./data/communities/%s.csv", origin_cmmt_name);

	//Network
	char *ntwk_path = new char[100];
	ptr_1 = sprintf(ntwk_path, "./gen/networks/%s/Network.dat", ntwk_name);


	//Load the network
	pipeNetwork network;
	network.READ_Network(ntwk_path, data_stream);


	if(network.prep_lvl() > 1)
	{
		//Initialize empty community object
		buildingCommunity community;


		//Create the building footprint onjects
		printf("\x1b[36mImport community data from: %s.csv\x1b[0m\n", origin_cmmt_name);
		std::vector<buildingFootprint> building = importCommunityData(origin_cmmt_path, cmmt_year);
		

		//Create community map
		printf("%d building footprints imported\n", building.size());
		printf("Create a grid to reference the footprints\n");
		std::vector< std::vector<double> > raster = createCommunityMap(network);


		//Determine nearest neighbour raster
		std::vector< std::vector<int> > nearest_neighbour = getNeighbourRasterFromNetwork(network);


		//Assign raster ID to building footprint
		printf("Created map contains %d rasters\n", raster.size());
		printf("Assign raster ID to building footprints\n");
		building = assignRasterID2Building(building, raster);


		//Assign building footprint ID to raster
		std::vector< std::vector<int> > building_belong = assignBuildingID2Raster(building, raster);


		//Get buidling connection
		printf("Raster ID assigned to buildings\n");
		printf("Determine building connectivity\n");
		building = getBuildingConnection(building, network);

		//Remove non-connected buildings
		printf("Connectivity evaluated\n");
		printf("Remove non-connected buildings\n");
		building = removeNonConnectedBuildings(building);


		//Evaluate buildings importance
		printf("Non-connected buildings removed\n");
		//printf("Evaluate importance of buildings\n");
		//building = evaluateImportance(building);


		//Assemble the community object
		community.n_buildings(building.size());
		community.status_year(cmmt_year);
		community.building_collection(building);
		community.community_name(cmmt_name);
		community.raster(raster);
		community.nearest_neighbour(nearest_neighbour);
		community.building_belong(building_belong);
	

		//Save the network object and create of the .csv file used in the post-processing
		//printf("Importance evaluated\n");
		printf("\x1b[32mExporting geometry for post-processing saving the community as %s/Community.dat\x1b[0m\n", cmmt_name);
		ofstream outfile;
		community.SAVE_Community(cmmt_folder, outfile);
		OriginalCommunity_csv(community, cmmt_folder);


		//Delete the char*
		delete [] origin_cmmt_path, ntwk_path, cmmt_folder;
	}
	else
	{
		printf("Preparation level of the network insufficient...\nPlease, re-run the network creation script with a preaparation level index of 2.\n");
	}
}




