//-------------------
//Community generator
//-------------------

/*Creates the community as a collection of building footprint objects from a csv*/

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
	//Name of the original community database with its extension
        char *origin_cmmt_name = new char[250];

	//Network to load
	char *ntwk_name = new char[250];

	//Name
	char *cmmt_name = new char[250];

	//Year of the analyzed status community
	unsigned cmmt_year;
	
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
								c = sprintf(origin_cmmt_name, "%s", token_buffer);
							}
							else if(i == 1)
							{
								c = sprintf(cmmt_name, "%s", token_buffer);
							}
							else if(i == 2)
							{
								c = sprintf(ntwk_name, "%s", token_buffer);
							}
							else if(i == 3)
							{
								cmmt_year = atoi(token_buffer);
							}
					}

					++arg_counter;
					token_buffer = strtok(NULL, "=");
				}

				delete [] token_buffer;
			}

			infile.close();

			//Create community based on the corresponding network and with user-defined parameters
			createCommunity(origin_cmmt_name, ntwk_name, cmmt_name, cmmt_year);
		}
	}


	//Delete names
	delete [] origin_cmmt_name;
	delete [] cmmt_name;
	delete [] ntwk_name;
	delete [] config_file;


	printf("Press enter to exit the program");

	getchar();

	return 0;


}


