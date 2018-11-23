/*Create Ground Motion maps

Contains 2 functions: 
	- CreateEmpiricalGMMap
	- CreatePhysicsGMMap

Both functions require the same input: the original name of the map (without its extension), the name of map to be created (without its extension)

CreatePhysicsGMMap requires the number of simulated GM maps.

*/


/*###############################################################################################################################

getRasterCoordinate : Get the raster coordinates from the csv file

###############################################################################################################################*/
std::vector< std::vector<double> > getRasterCoordinate(char *origin_map_path)
{
	//Declare base variable
	int i, j;
	int row_count = Row_counter(origin_map_path);


	//Initialize importation varaibles
	std::vector< std::vector<double> > rast(row_count, std::vector<double>(2));
	string line_buffer;
	int arg_counter;
	char *tk_bf;

	//Open the file
	ifstream data_stream;
	data_stream.open(origin_map_path);


	//Import each point coordinates of the analyzed area
	for(i=0 ; i<row_count ; i++)
	{
		arg_counter = 0;
		getline(data_stream, line_buffer);
		std::vector<char> v(line_buffer.length() + 1);
		strcpy(&v[0], line_buffer.c_str());
		char *line_buffer_char = &v[0];
		char *token_buffer = strtok(line_buffer_char, ";, ");
		++arg_counter;
		while(arg_counter <= 4)
		{
			switch(arg_counter)
			{
				case 1 :
					rast[i][0] = atof(token_buffer);
					break;

				case 2 : 
					rast[i][1] = atof(token_buffer); 
					break;
			}
			++arg_counter;
			token_buffer = strtok(NULL, ";, ");
		}
	}

	//Close data stream
	data_stream.close();


	//Return results
	return rast;
}


/*###############################################################################################################################

getEmpiricalGM : Get the PGV from csv file, takes the log of it and the corresponding log std deviation

###############################################################################################################################*/
std::vector< std::vector <float> > getEmpiricalGM(char *origin_map_path)
{
	//Declare base variable
	int i, j;
	int row_count = Row_counter(origin_map_path);

	
	//Create the IM_pt structures
	std::vector< std::vector<float> > gm(row_count, std::vector<float>(2));
	string line_buffer;
	int arg_counter;
	char *tk_bf;


	//Open the file
	ifstream data_stream;
	data_stream.open(origin_map_path);


	//Import each point GM intensity of the analyzed area
	for(i=0 ; i<row_count ; i++)
	{
		arg_counter = 0;
		getline(data_stream, line_buffer);
		std::vector<char> v(line_buffer.length() + 1);
		strcpy(&v[0], line_buffer.c_str());
		char *line_buffer_char = &v[0];
		char *token_buffer = strtok(line_buffer_char, ";, ");
		++arg_counter;
		while(arg_counter <= 4)
		{
			switch(arg_counter)
			{
				case 3 : 
					gm[i][0] = log(atof(token_buffer)); 
					break;

				case 4 : 
					gm[i][1] = atof(token_buffer); 
					break;
			}
			++arg_counter;
			token_buffer = strtok(NULL, ";, ");
		}
	}


	//Close data stream
	data_stream.close();


	//Return results
	return gm;
}


/*###############################################################################################################################

covarianceCholeskyL : compute the covariance matrix of the GM and decomposes it into a packed L Cholesky vector

###############################################################################################################################*/
std::vector<float> covarianceCholeskyL(std::vector< std::vector <double> > raster, std::vector< std::vector <float> > gm)
{
	//Declare base variable
	int i, j;
	int n_raster = raster.size();


	//Initialize covariance related variables
	std::vector< std::vector<float> > cov(n_raster, std::vector<float>(n_raster));//Covariance matrix
	double h;//Inter-site distance
	double r = 43000;//Range of the semi-variogram between 43000 and 55000 m for PGV (Jayaram and Baker, 2009)


	//Compute the cavariance matrix
	for(i=0 ; i<n_raster ; i++)
	{
		for(j=i+1 ; j<n_raster ; j++)
		{
			//Computation of the inter-site distance h in m
			h = sqrt(pow(raster[i][0]-raster[j][0], 2) + pow(raster[i][1]-raster[j][1], 2));

			//Covariance
			cov[i][j] = gm[i][1] * gm[j][1] * exp(-3*h/r);
			cov[j][i] = cov[i][j];
		}
		//Diagonal
		cov[i][i] = gm[i][1] * gm[i][1];
	}


	//Cholesky decomposition of the covariance matrix
	std::vector<float> packed_cov = Cholesky_sym_L_packed(cov);


	//Retrun packed L-Cholesky covariance matrix
	return packed_cov;
}


/*###############################################################################################################################

getPhysicsGM : Get the PGV from csv file, takes the log of it and the corresponding log std deviation

###############################################################################################################################*/
std::vector< std::vector <float> > getPhysicsGM(char *origin_map_path)
{
	//Initialize base varaibles
	int i, j, i_sim, ptr_1;


	//Get number of raster
	int row_count = Row_counter(origin_map_path);
	int col_count = Col_counter(origin_map_path);


	//Initialization of the PGV collector matrix
	std::vector< std::vector<float> > gm(col_count-2, std::vector<float>(row_count));

	//Open the file
	ifstream data_stream;
	data_stream.open(origin_map_path);

	for(int i=0 ; i<row_count ; i++)
	{
		//Initialize importation related variables
		j = 0;
		string line_buffer;
		int arg_counter;
		char *tk_bf;

		//Import each point of the analyzed area as a structure
		arg_counter = 0;
		getline(data_stream, line_buffer);
		std::vector<char> v(line_buffer.length() + 1);
		strcpy(&v[0], line_buffer.c_str());
		char *line_buffer_char = &v[0];
		char *token_buffer = strtok(line_buffer_char, ";, ");
		++arg_counter;

		while(arg_counter <= col_count)
		{
			if(arg_counter > 2)
			{
				gm[j][i] = atof(token_buffer);
				++j;
			}

			++arg_counter;
			token_buffer = strtok(NULL, ";, ");
		}
	}

	data_stream.close();

	//Return results
	return gm;
}


/*###############################################################################################################################

createSingleScenarioEmpirical : Create the map of a empirically-generated GM from a csv file

###############################################################################################################################*/
void createSingleScenarioEmpirical(char* origin_map_name, char* map_name)
{
	//Create result folder
	char *map_folder = new char[50];
	int ptr_1 = sprintf(map_folder, "./gen/GM/%s", map_name);
	mkdir("./gen", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./gen/GM", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(map_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	//Path of the files
	//To load
	char *origin_map_path = new char[50];
	ptr_1 = sprintf(origin_map_path, "./data/maps/%s.csv", origin_map_name);
	//To save
	char *map_path = new char[50];
	ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.egm", map_name);


	//Initialize empty GM map object
	singleScenarioEmpiricalGM map;


	//Get the raster coordinates
	printf("\x1b[36mImport raster coordinate and ground motion intensity data from: %s.csv\x1b[0m\n", origin_map_name);
	std::vector< std::vector <double> > raster = getRasterCoordinate(origin_map_path);


	//Get the GM intensity
	std::vector< std::vector <float> > gm = getEmpiricalGM(origin_map_path);


	//Evaluate covariance matrix (stored as packed L-decomposed Cholesky matrix)
	printf("%d raster data imported\n", raster.size());
	printf("Evaluate the Cholesky-L decomposed covariance matrix\n");
	std::vector<float> packed_cov = covarianceCholeskyL(raster, gm);


	//Assemble the map object
	std::vector< std::vector<int> > pseudo_matrix(2, std::vector<int>(2, -1));
	map.name(map_name);
	map.map_ID(0);
	map.lambda(-1.0);
	map.source_ID(-1);
	map.n_rasters(raster.size());
	map.raster(raster);
	map.spacing(sqrt(pow(raster[0][0]-raster[1][0], 2) + pow(raster[0][1]-raster[1][1], 2)));
	map.packed_cov(packed_cov);
	map.pipe_belong(pseudo_matrix);
	map.building_belong(pseudo_matrix);
	map.lnPGV(gm);


	//Save the object
	printf("\x1b[32mSaving the ground motion intensity map as %s/GM.egm\x1b[0m\n", map_name);
	ofstream outfile;
	map.SAVE_Map(map_folder, outfile);


	//Delete the object
	delete [] map_folder, map_path, origin_map_path;
}


/*###############################################################################################################################

createSingleScenarioPhysics : Create the map of a physics-based GM from a csv file

###############################################################################################################################*/
void createSingleScenarioPhysics(char* origin_map_name, char* map_name)
{
	//Create result folder
	char *map_folder = new char[50];
	int ptr_1 = sprintf(map_folder, "./gen/GM/%s", map_name);
	mkdir("./gen", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./gen/GM", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(map_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	//Path of the files
	//To load
	char *origin_map_path = new char[50];
	ptr_1 = sprintf(origin_map_path, "./data/maps/%s.csv", origin_map_name);
	//To save
	char *map_path = new char[50];
	ptr_1 = sprintf(map_path, "./gen/GM/%s/GM.pgm", map_name);


	//Initialize empty GM map object
	singleScenarioPhysicsGM map;


	//Number of generated GMs
	int n_sim = Col_counter(origin_map_path);


	//Get the raster coordinates
	printf("\x1b[36mImport raster coordinate and ground motion intensity data from: %s\x1b[0m\n", origin_map_name);
	std::vector< std::vector <double> > raster = getRasterCoordinate(origin_map_path);


	//Gather GM data
	printf("%d raster data imported\n", raster.size());
	std::vector< std::vector<float> > gm = getPhysicsGM(origin_map_path);
int i, j;
for(i=0 ; i<gm[0].size() ; i++)
{
	for(j=0 ; j<gm.size() ; j++)
	{
		printf("%.2f\t", gm[j][i]);
	}
	printf("\n");
}
	//Assemble the object
	std::vector< std::vector<int> > pseudo_matrix(2, std::vector<int>(2, -1));
	map.name(map_name);
	map.map_ID(0);
	map.lambda(-1.0);
	map.source_ID(-1);
	map.n_rasters(raster.size());
	map.raster(raster);
	map.spacing(sqrt(pow(raster[0][0]-raster[1][0], 2) + pow(raster[0][1]-raster[1][1], 2)));
	map.pipe_belong(pseudo_matrix);
	map.building_belong(pseudo_matrix);
	map.PGV(gm);
	

	//Save the object
	printf("%d ground motion realizations imported\n", n_sim);
	printf("\x1b[32mSaving the ground motion intensity map as %s/GM.pgm\x1b[0m\n", map_name);
	ofstream outfile;
	map.SAVE_Map(map_folder, outfile);


	//Remove char*
	delete [] map_folder, map_path;
}


/*###############################################################################################################################

createMap: function calling the appropriate subfunction to create a GM map

###############################################################################################################################*/
void createGMMap(char *origin_map_name, char *map_name, int GMM_type)
{
	//Create selected map type
	if(GMM_type == 0)
	{
		createSingleScenarioEmpirical(origin_map_name, map_name);
	}
	else if(GMM_type == 1)
	{
		createSingleScenarioPhysics(origin_map_name, map_name);
	}
	
}





