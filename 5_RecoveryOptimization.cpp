//------------------------------------------
//Study of historical cases
//------------------------------------------

//Optimize recovery based on discovered pipe failures

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


	//Declare parametes
	//Name of the base map file
	char *map_name = new char[250];

	//Name of the base network file
	char *network_name = new char[250];

	//Name of the community file
	char *community_name = new char[250];

	//Objective name
	char *obj_str = new char[50];

	//Number of inspection teams
	int n_Iteams;

	//Max length teams can inspect per day
	float l_max; //[m]

	//Number of repair team
	int n_Rteams;

	//Max number of repair per day
	int n_max;

	//Recovery type (0 = "random" or 1 = "GA")
	int recovery_type = 1;

	//Extract GA-related variables
	int n_generation;
	int n_chromosome;
	int n_elite;
	int tournament_size;
	float P_crossover;
	float P_mutation;

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

			for(i=0 ; i<14 ; i++)
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
								c = sprintf(network_name, "%s", token_buffer);
							}
							else if(i == 2)
							{
								c = sprintf(community_name, "%s", token_buffer);
							}
							else if(i == 3)
							{
								n_Iteams = atoi(token_buffer);
							}
							else if(i == 4)
							{
								l_max = atof(token_buffer);
							}
							else if(i == 5)
							{
								n_Rteams = atoi(token_buffer);
							}
							else if(i == 6)
							{
								n_max = atoi(token_buffer);
							}
							else if(i == 7)
							{
								c = sprintf(obj_str, "%s", token_buffer);
							}
							else if(i == 8)
							{
								n_generation = atoi(token_buffer);
								if(n_generation == -1)
								{
									recovery_type = 0;
								}
							}
							else if(i == 9)
							{
								n_chromosome = atoi(token_buffer);
							}
							else if(i == 10)
							{
								n_elite = atoi(token_buffer);
							}
							else if(i == 11)
							{
								tournament_size = atoi(token_buffer);
							}
							else if(i == 12)
							{
								P_crossover = atof(token_buffer);
							}
							else if(i == 13)
							{
								P_mutation = atof(token_buffer);
							}
					}

					++arg_counter;
					token_buffer = strtok(NULL, "=");
				}

				delete [] token_buffer;
			}

			infile.close();

			//1. Getting inspection list
			std::vector<int> inspectionList;
			int n_pipe;
			if(rank == rootRank)
			{
				inspectionList = getInspectionList(map_name, network_name);
				n_pipe = inspectionList.size();
				printf("\x1b[32mInspection list saved!\x1b[0m\n");
			}
			//Broadcast length of inpsection list
			MPI_Bcast(&n_pipe, 1, MPI_INT, rootRank, MPI_COMM_WORLD);

			//Broadcast list of inspection to all processors
			if(rank != rootRank)
			{
				inspectionList.resize(n_pipe);
			}
			MPI_Bcast(&inspectionList[0], n_pipe, MPI_INT, rootRank, MPI_COMM_WORLD);


			//Make sure they all receive the list (barrier)
			MPI_Barrier(MPI_COMM_WORLD);

			//2. Condut recovery for the analyzed scenario
			scenarioRecovery(inspectionList, n_Iteams, l_max, n_Rteams, n_max, map_name, network_name, community_name, obj_str,
				n_generation, n_chromosome, n_elite, tournament_size, P_crossover, P_mutation, recovery_type, rank, rootRank,
				size);

		}
	}


	//Delete names
	delete [] config_file;
	delete [] obj_str;
	delete [] map_name;
	delete [] network_name;
	delete [] community_name;

	//End parallel code
	if(rank == rootRank)
	{
		printf("Press enter to exit the program");
	}
	rc = MPI_Finalize();

	getchar();

	return 0;
}
