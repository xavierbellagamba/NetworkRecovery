//-------------------
//Network generator
//-------------------

/*Takes the discretized network (collection of points) as input and generate pipeline objects*/

//Included libraries
#include "./bin/include.ll"

using namespace std;

//Main
int main(int argc,char *argv[])
{
	//Declare basic varaibles
	int i, c;
	ifstream infile;


	//Declare parametes
	//Name of the original network with its extension
        char *origin_ntwk_name = new char[250];

	//Name of the network to save
	char *ntwk_name = new char[250];

	//Name of the soil map
	char *soil_name = new char[250];

	//Max pipe length
	float max_pipe_length; //m

	//Year of the analyzed network state
	unsigned ntwk_year;

	//Preparation level: 1 for pipe damage only, 2 for connectivity analysis
	int prep_lvl;

	//Config file
	char *config_file = new char[250];


	//Assign the parameters
	if(argc < 2)
	{
		printf("\x1b[31mProvide configuaration file name!\n\x1b[0m\n");
		return 1;
	}
	else if(argc > 2)
	{
		printf("\x1b[31mToo many parameters, please check your command!\n\x1b[0m\n");
		return 1;
	}
	else
	{
		//Open config file
		c = sprintf(config_file, "./config/%s.cfg", argv[1]);
		infile.open(config_file);
		

		if(infile.fail())
		{
			printf("\x1b[31mConfig file %s doesn't exist!\n\x1b[0m\n", config_file);
		}
		else
		{
			//Loading configuration
			string line_buffer;
			int arg_counter;

			for(i=0 ; i<6 ; i++)
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
								c = sprintf(origin_ntwk_name, "%s", token_buffer);
							}
							else if(i == 1)
							{
								c = sprintf(ntwk_name, "%s", token_buffer);
							}
							else if(i == 2)
							{
								c = sprintf(soil_name, "%s", token_buffer);
							}
							else if(i == 3)
							{
								max_pipe_length = atof(token_buffer);
							}
							else if(i == 4)
							{
								ntwk_year = atoi(token_buffer);
							}
							else if(i == 5)
							{
								prep_lvl = atoi(token_buffer);
							}
					}

					++arg_counter;
					token_buffer = strtok(NULL, "=");
				}
	
				delete [] token_buffer;
			}

			infile.close();
		
			//Create network with user-defined parameters
			createNetwork(origin_ntwk_name, ntwk_name, soil_name, max_pipe_length, ntwk_year, prep_lvl);
		}
	}


	//Delete names
	delete [] config_file;
	delete [] origin_ntwk_name;
	delete [] ntwk_name;


	//Pause the code before returning
	printf("Press enter to exit the program");
	getchar();

	return 0;
}


