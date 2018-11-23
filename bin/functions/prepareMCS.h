/*###############################################################################################################################

assignGMRasterID2Pipe : assign raster from the GM map to the pipelines

###############################################################################################################################*/
std::vector<pipeSegment> assignGMRasterID2Pipe(std::vector<pipeSegment> networkSegment, std::vector< std::vector<double> > raster)
{
	//Initialize basic variables
	int i, j;
	int n_raster = raster.size();
	int n_pipe = networkSegment.size();
	float dist;
	float d_max = (raster[1][0] - raster[0][0])/2;

	//5. Look for the closest raster center to each pipeline geometric center
	for(i=0 ; i<n_pipe ; i++)
	{
		//Initialize variables
		float closest_dist = 10000000000.0;//Initialize large so will be updated on the first iteration
		dist = 0.0;
		int ID_closest = -1;

		for(j=0 ; j<n_raster ; j++)
		{
			//Compute distance between raster j and pipeline i
			dist = sqrt(pow(networkSegment[i].center_loc()[0] - raster[j][0], 2) + 
				pow(networkSegment[i].center_loc()[1] - raster[j][1], 2));

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

		//Assign raster to pipeline
		networkSegment[i].GMraster_ID(ID_closest);
	}


	//Return updated pipe segments
	return networkSegment;
}


/*###############################################################################################################################

prepareNetwork4MCS : get GM raster ID for each pipeline, save the updated network in the MCS root folder and return leel of prepareness

###############################################################################################################################*/
void prepareNetwork4MCS(char *ntwk_name, char *gm_name)
{
	//Declare base variables
	int i, j;
	ifstream infile;


	//Load network and raster from gm map
	//Network
	char *ntwk_path = new char[100];
	int ptr_1 = sprintf(ntwk_path, "./gen/networks/%s/Network.dat", ntwk_name);
	pipeNetwork network;
	network.READ_Network(ntwk_path, infile);
	delete [] ntwk_path;

	//GM map
	std::vector< std::vector<double> > raster_gm;
	//Type of GM map
	int GMM_type = GM_map_type(gm_name);
	if(GMM_type == 0)
	{
		char *map_path = new char[100];
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.egm", gm_name);
		singleScenarioEmpiricalGM map_gm;
		map_gm.READ_Map(map_path, infile);
		raster_gm = map_gm.raster();
		delete [] map_path;
	}
	else if(GMM_type == 1)
	{
		char *map_path = new char[100];
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.pgm", gm_name);
		singleScenarioPhysicsGM map_gm;
		map_gm.READ_Map(map_path, infile);
		raster_gm = map_gm.raster();
		delete [] map_path;
	}


	//Assign
	std::vector<pipeSegment> pipe_update = assignGMRasterID2Pipe(network.pipe_collection(), raster_gm);
	network.pipe_collection(pipe_update);


	//Save the updated object
	ofstream outfile;
	char *ntwk_folder = new char[200];
	char *gm_folder = new char[200];
	j = sprintf(ntwk_folder, "./gen/MCS/predictions/%s/", ntwk_name);
	j = sprintf(gm_folder, "./gen/MCS/predictions/%s/%s", ntwk_name, gm_name);
	mkdir("./gen/MCS", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./gen/MCS/predictions", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(ntwk_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(gm_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	network.SAVE_Network(gm_folder, outfile);
	delete [] ntwk_folder, gm_folder;
}


/*###############################################################################################################################

assignGMRasterID2Building : assign raster from the GM map to the buildings

###############################################################################################################################*/
std::vector<buildingFootprint> assignGMRasterID2Building(std::vector<buildingFootprint> building, std::vector< std::vector<double> > raster)
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
		building[i].GMraster_ID(ID_closest);
	}


	//Return updated objects
	return building;
}


/*###############################################################################################################################

prepareCommunity4MCS : get GM raster ID for each building and save the updated community in the MCS root folder

###############################################################################################################################*/
void prepareCommunity4MCS(char *ntwk_name, char *cmmt_name, char *gm_name)
{
	//Declare base variables
	int i, j;
	ifstream infile;


	//Load network and raster from gm map
	//Network
	char *cmmt_path = new char[100];
	int ptr_1 = sprintf(cmmt_path, "./gen/communities/%s/Community.dat", cmmt_name);
	buildingCommunity community;
	community.READ_Community(cmmt_path, infile);
	delete [] cmmt_path;

	//GM map
	std::vector< std::vector<double> > raster_gm;
	//Type of GM map
	int GMM_type = GM_map_type(gm_name);
	if(GMM_type == 0)
	{
		char *map_path = new char[100];
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.egm", gm_name);
		singleScenarioEmpiricalGM map_gm;
		map_gm.READ_Map(map_path, infile);
		raster_gm = map_gm.raster();
		delete [] map_path;
	}
	else if(GMM_type == 1)
	{
		char *map_path = new char[100];
		ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.pgm", gm_name);
		singleScenarioPhysicsGM map_gm;
		map_gm.READ_Map(map_path, infile);
		raster_gm = map_gm.raster();
		delete [] map_path;
	}


	//Assign
	std::vector<buildingFootprint> building_update = assignGMRasterID2Building(community.building_collection(), raster_gm);
	community.building_collection(building_update);


	//Save the updated object
	ofstream outfile;
	char *gm_folder = new char[200];
	j = sprintf(gm_folder, "./gen/MCS/predictions/%s/%s", ntwk_name, gm_name);
	community.SAVE_Community(gm_folder, outfile);
	delete [] gm_folder;
}


/*###############################################################################################################################

prepareMCS : prepare the network and community objects for simulations

###############################################################################################################################*/
void prepareMCS(char *ntwk_name, char *cmmt_name, char *gm_name, int analysis_lvl)
{
	//Prepare the network
	printf("\x1b[36mImport and prepare the network %s for MCS with %s\x1b[0m\n", ntwk_name, gm_name);
	prepareNetwork4MCS(ntwk_name, gm_name);	
	printf("\x1b[32mSaving the prepared network as MCS/predictions/%s/%s/Network.dat\x1b[0m\n", ntwk_name, gm_name);


	//If necessary prepare the community
	if(analysis_lvl > 1)
	{
		printf("\x1b[36mImport and prepare the community %s for MCS with %s\x1b[0m\n", cmmt_name, gm_name);
		prepareCommunity4MCS(ntwk_name, cmmt_name, gm_name);
		printf("\x1b[32mSaving the prepared community as MCS/predictions/%s/%s/Community.dat\x1b[0m\n", ntwk_name, gm_name);
	}
}




