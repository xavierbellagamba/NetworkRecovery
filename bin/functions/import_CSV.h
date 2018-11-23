//Function to import csv table

//Requires string, cstring, array, vector, iostd, fstream

//Function to count the number of rows
int Row_counter(std::string file_name)
{
	//Variable declaration
	int counter = 0;
	std::string buffer;
	std::ifstream data_stream;

	//Open file
	data_stream.open(file_name.c_str());

	//Counting lines in for loop
	while(getline(data_stream, buffer, '\n'))
	{
		//getline(data_stream, buffer);
		++counter;
	}

	//Close the stream
	data_stream.close();

	//Returning
	return counter;
}


//Function to count the number of columns
int Col_counter(std::string input_csv)
{
	//Creation of the data steam
        std::ifstream data_stream;
        data_stream.open(input_csv);

	//Initialize variables
	int n_col = 0;
	std::string line_buffer;

	//Get first line
        getline(data_stream, line_buffer);
        std::vector<char> v(line_buffer.length() + 1);
        strcpy(&v[0], line_buffer.c_str());
        char *line_buffer_char = &v[0];
        char *token_buffer = strtok(line_buffer_char, ";, \n");

	//Counts elements in the first line
        while(token_buffer != NULL)
        {
		//Update column position
                ++n_col;

                //Take next element from csv
                token_buffer = strtok(NULL, ";, \n");
	}

	//Close data stream
        data_stream.close();

        //Return result
        return n_col;
}


//Import regular tables of floats
float** import_float_table(std::string input_csv)
{
	//Creation of the data steam
	std::ifstream data_stream;
	data_stream.open(input_csv);

	//Count number of rows
	int row_count = Row_counter(input_csv);

	//Count number of columns
	int col_count = Col_counter(input_csv);

	//Declaration of variables
	int i, j;
	float **import_table = new float*[row_count];
	std::string line_buffer;

	//Check if input table is a vector (only 1 column)
	if(col_count == 1) //Is a vector
	{
		//Import each line
                for(i=0 ; i<row_count ; i++)
                {
                        //Initialize variables
                        import_table[i] = new float[1];
                        getline(data_stream, line_buffer);
                        std::vector<char> v(line_buffer.length() + 1);
                        strcpy(&v[0], line_buffer.c_str());
                        char *line_buffer_char = &v[0];

                        //Assign to cell i, arg_counter
                        import_table[i][0] = atof(line_buffer_char);
                }
	}
	else //Is a matrix
	{
		//Import each line
		for(i=0 ; i<row_count ; i++)
		{
			//Initialize variables
			import_table[i] = new float[col_count];
			j = 0;
			getline(data_stream, line_buffer);
			std::vector<char> v(line_buffer.length() + 1);
			strcpy(&v[0], line_buffer.c_str());
			char *line_buffer_char = &v[0];
			char *token_buffer = strtok(line_buffer_char, ";, ");

			while(token_buffer != NULL)
			{
				//Assign to cell i, arg_counter
				import_table[i][j] = atof(token_buffer);

				//Update column position
				++j;

				//Take next element from csv
				token_buffer = strtok(NULL, ";,");
			}
		}
	}

	//Close data stream
	data_stream.close();

	//Return result
	return import_table;
}

void dealloc_float_table(float **float_table)
{
	//Declare variables
	int i, n, m;

	//Size of a column
	m = sizeof(float_table[0]);

	//Number of vectors of pointers
	n = sizeof(float_table)/m;

	//Deallocate memory
	for(i=0 ; i<n ; i++)
	{
		delete [] float_table[i];
	}
}











