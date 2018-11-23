//----------------------
//MCS framework
//----------------------

/*
Executes the MC simulations with the user parameters.
*/

//Included libraries
#include "./bin/include.ll"


using namespace std;


//Main
int main(int argc,char *argv[])
{
	//Start parellel code here
	//Declaration of MPI related variables
	int rc = 0, rank = 0, size = 0, rootRank = 0;

	//Initialize MPI code
	rc = MPI_Init(&argc, &argv);
	
	//Initialize MPI status
	MPI_Status status;
	status.MPI_SOURCE = -1;
	status.MPI_TAG = -1;
	status.MPI_ERROR = 0;

	//Initialize communicator
	rc = MPI_Comm_size(MPI_COMM_WORLD, &size);
	rc = MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//Declare basic varaibles
	int i, c;
	ifstream infile;
	char *str_n = new char[50];


	//Declare parametes
	//Name of the original network
        char *ntwk_name = new char[250];

	//Name of the map to load
	char *map_name = new char[250];

	//Simulation parameters
        //ntwk_lvl: Network level of knowledge. 1=Nothing known, 2=Material ductility, 3=Material, 4=Diameter
        //soil_lvl: Soil condition knowledge. 0:Unknown, 1=Known
        int ntwk_lvl;
        int soil_lvl;

	//Number of simulations
        int n_sim;

	//Used functions sets
	//0: Bellagamba and Bradley 2017
        std::vector<int> fx_set;

	//Confidence in each function (the sum must be equal to 1)
	std::vector<float> fx_trust;

	//Analysis level
	//0: pipe failure, 1: network connectivity, 2: building connectivity
	int analysis_lvl;

	//Recovery base 
	//1: true: considers all pump stations as operational
	//0: false: considers damage to pump stations
	int recoB;
	bool recoBase;

	//Config file
	char *config_file = new char[250];


	//Assign the parameters
	if(argc < 2)
	{
		//End parallel code
		if(rank == rootRank)
		{
			printf("\x1b[31mProvide configuaration file name!\n\x1b[0m\n");
		}
		rc = MPI_Finalize();
		return 1;
	}
	else if(argc > 2)
	{
		//End parallel code
		if(rank == rootRank)
		{
			printf("\x1b[31mToo many parameters, please check your command!\n\x1b[0m\n");
		}
		rc = MPI_Finalize();
		return 1;
	}
	else
	{
		//Open config file
		c = sprintf(config_file, "./config/%s.cfg", argv[1]);
		infile.open(config_file);
		

		if(infile.fail())
		{
			//End parallel code
			if(rank == rootRank)
			{
				printf("\x1b[31mConfig file %s doesn't exist!\n\x1b[0m\n", config_file);
			}
			rc = MPI_Finalize();
			return 1;
		}
		else
		{
			//Loading configuration
			string line_buffer;
			int arg_counter;

			for(i=0 ; i<9 ; i++)
			{
				arg_counter = 0;
				getline(infile, line_buffer);
				std::vector<char> v(line_buffer.length() + 1);
				strcpy(&v[0], line_buffer.c_str());
				char *line_buffer_char = &v[0];
				char *token_buffer = new char[200];
				token_buffer = strtok(line_buffer_char, "=");
				++arg_counter;

				while(arg_counter <= 2)
				{
					switch(arg_counter)
					{
						case 2 :
							if(i == 0)
							{
								c = sprintf(map_name, "%s", token_buffer);
							}
							else if(i == 1)
							{
								c = sprintf(ntwk_name, "%s", token_buffer);
							}
							else if(i == 2)
							{
								n_sim = atoi(token_buffer);
							}
							else if(i == 3)
							{
								analysis_lvl = atoi(token_buffer);
							}
							else if(i == 4)
							{
								ntwk_lvl = atoi(token_buffer);
							}
							else if(i == 5)
							{
								soil_lvl = atoi(token_buffer);
							}
							else if(i == 6)
							{
								str_n = strpbrk(token_buffer, ",");
								if(str_n != NULL)
								{
									fx_set.push_back(atoi(str_n-1));

									while(str_n != NULL)
									{
										str_n = strpbrk(str_n+1, ",");
										if(str_n != NULL)
										{
											fx_set.push_back(atoi(str_n-1));
										}
									}

									char *last_c = strrchr(token_buffer, ',');
									str_n = strtok(last_c, ",");
									fx_set.push_back(atoi(str_n));
								}
								else
								{
									fx_set.push_back(atoi(token_buffer));
								}

								fx_set.shrink_to_fit();
							}
							else if(i == 7)
							{
								str_n = strpbrk(token_buffer, ",");
								if(str_n != NULL)
								{
									fx_trust.push_back(atof(str_n-4));

									while(str_n != NULL)
									{
										str_n = strpbrk(str_n+1, ",");
										if(str_n != NULL)
										{
											fx_trust.push_back(atof(str_n-4));
										}
									}

									char *last_c = strrchr(token_buffer, ',');
									str_n = strtok(last_c, ",");
									fx_trust.push_back(atof(str_n));
								}
								else
								{
									fx_trust.push_back(atof(token_buffer));
								}

								fx_trust.shrink_to_fit();
							}
							else if(i == 8)
							{
								recoB = atoi(token_buffer);
								if(recoB == 0)
								{
									recoBase == false;
								}
								else
								{
									recoBase == true;
								}
							}
					}					

					++arg_counter;
					token_buffer = strtok(NULL, "=");
				}

				delete [] token_buffer;
			}

			infile.close();

			//Realize the MCS
			MPI_Barrier(MPI_COMM_WORLD);
			MCS(map_name, ntwk_name, ntwk_lvl, soil_lvl, n_sim, fx_set, fx_trust, analysis_lvl, recoBase, rank, rootRank, size);
		}
	}


	//Delete names
	delete [] config_file;
	delete [] map_name;
	delete [] ntwk_name;


	//End parallel code
	if(rank == rootRank)
	{
		printf("Exit the program");
	}
	rc = MPI_Finalize();

	return 0;
}


