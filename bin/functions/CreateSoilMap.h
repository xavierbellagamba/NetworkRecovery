/*Create soil map*/


/*###############################################################################################################################

getSoilInput : Get the Soil input from csv file

###############################################################################################################################*/
std::vector <float> getSoilInput(char *origin_map_path)
{
	//Declare base variable
	int i, j;
	int row_count = Row_counter(origin_map_path);

	
	//Create the IM_pt structures
	std::vector<float> input(row_count, 0.0);
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
		while(arg_counter <= 3)
		{
			switch(arg_counter)
			{
				case 3 : 
					input[i] = atof(token_buffer); 
					break;
			}
			++arg_counter;
			token_buffer = strtok(NULL, ";, ");
		}
	}


	//Close data stream
	data_stream.close();


	//Return results
	return input;
}



/*###############################################################################################################################

fromSuscept2CRR : Transform susceptibility into CRR

###############################################################################################################################*/
std::vector <float> fromSuscept2CRR(std::vector<float> input)
{
	//Declare basic variables
	int i, j;
	int n = input.size();

	//Declare CRR vector
	std::vector<float> CRR(n, 0.0);

	//Transform
	for(i=0 ; i<n ; i++)
	{
		CRR[i] = exp(-input[i]);
	}

	return CRR;
}


/*###############################################################################################################################

fromLRI2CRR : Transform LRI into CRR

###############################################################################################################################*/
std::vector <float> fromLRI2CRR(std::vector<float> input)
{
	//Declare basic variables
	int i, j;
	int n = input.size();

	//Declare CRR vector
	std::vector<float> CRR(n, 0.0);

	//Transform
	for(i=0 ; i<n ; i++)
	{
		switch(int(input[i]))
		{
			case 0:
				CRR[i] = 0.032;
				break;

			case 1:
				CRR[i] = 0.065;
				break;

			case 2:
				CRR[i] = 0.13;
				break;

			case 3:
				CRR[i] = 0.195;
				break;

			case 4:
				CRR[i] = 0.26;
				break;

			case -1:
				CRR[i] = 0.265;
				break;

			case -2:
				CRR[i] = -1.0;
				break;

			default:
				CRR[i] = -2.0;
				break;
		}
	}

	return CRR;
}



/*###############################################################################################################################

createSoilMap : Create the CRR map based on a given input

###############################################################################################################################*/
void createSoilMap(char *origin_map_name, char *map_name, char *inputName)
{
	//Create result folder
	char *map_folder = new char[50];
	int ptr_1 = sprintf(map_folder, "./gen/CRR/%s", map_name);
	mkdir("./gen", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./gen/CRR", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(map_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	//Path of the files
	//To load
	char *origin_map_path = new char[50];
	ptr_1 = sprintf(origin_map_path, "./data/soils/%s.csv", origin_map_name);
	//To save
	char *map_path = new char[50];
	ptr_1 = sprintf(map_path, "./gen/CRR/%s/Soil.dat", map_name);


	//Initialize empty GM map object
	soilMap map;


	//Get the raster coordinates
	printf("\x1b[36mImport raster coordinate and ground motion intensity data from: %s.csv\x1b[0m\n", origin_map_name);
	std::vector< std::vector <double> > raster = getRasterCoordinate(origin_map_path);

	//Get the input
	std::vector<float> soilInput = getSoilInput(origin_map_path);

	//If required (not CRR), transform the input into CRR
	std::vector<float> CRR = soilInput;
	if(strcmp(inputName, "CRR") != 0)
	{
		//Transform given input
		if(strcmp(inputName, "Susceptibility") == 0)
		{
			CRR = fromSuscept2CRR(soilInput);
		}
		else if(strcmp(inputName, "LRI") == 0)
		{
			CRR = fromLRI2CRR(soilInput);
		}
	}

	//Assemble the map object
	std::vector< std::vector<int> > pseudo_matrix(2, std::vector<int>(2, -1));
	map.name(map_name);
	map.inputName(inputName);
	map.map_ID(0);
	map.n_rasters(raster.size());
	map.raster(raster);
	map.spacing(sqrt(pow(raster[0][0]-raster[1][0], 2) + pow(raster[0][1]-raster[1][1], 2)));
	map.CRR(CRR);
	map.soilInput(soilInput);
	map.pipe_belong(pseudo_matrix);
	map.building_belong(pseudo_matrix);


	//Save the object
	printf("\x1b[32mSaving the ground motion intensity map as %s/GM.egm\x1b[0m\n", map_name);
	ofstream outfile;
	map.SAVE_Map(map_folder, outfile);


	//Delete the object
	delete [] map_folder, map_path, origin_map_path;
}


