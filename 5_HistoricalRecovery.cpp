//------------------------------------------
//Study of historical cases
//------------------------------------------

//Estimate the impact of a given damage map on the community

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
	//Name of the base network file
	char* network_name = new char[250];

	//Name of the community file
	char* community_name =  new char[250];

	//Name of the pipe failure file
	char* pipe_failure_name =  new char[250];

	//Number of simulations
	int n_sim;

	//Year of the event
	int year;

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

			for(i=0 ; i<4 ; i++)
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
								c = sprintf(network_name, "%s", token_buffer);
							}
							else if(i == 1)
							{
								c = sprintf(community_name, "%s", token_buffer);
							}
							else if(i == 2)
							{
								c = sprintf(pipe_failure_name, "%s", token_buffer);
							}
							else if(i == 3)
							{
								n_sim = atoi(token_buffer);
							}
					}

					++arg_counter;
					token_buffer = strtok(NULL, "=");
				}

				delete [] token_buffer;
			}

			infile.close();

			//Conduct the recovey analysis
			historicRecoveryAnalysis(pipe_failure_name, network_name, community_name, n_sim);
		}
	}


	//Delete names
	delete [] config_file;
	delete [] pipe_failure_name;
	delete [] network_name;
	delete [] community_name;


	//Exit
	printf("Press enter to exit the program");
	getchar();

	return 0;
}
