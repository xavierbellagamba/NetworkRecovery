/*--------------------------------------*/
// Binary writing and reading functions //
/*--------------------------------------*/

//Needs stdio.h, fstream, vector, cstring, ctype.h

/*Covered types:
	- double
	- float
	- integer
	- char*
	- vector of doubles
	- vector of ints
	- vector of floats
	- matrix of doubles (vector of vector)
	- matrix of integers
	- matrix of floats
	- irregular table of ints
	- irregular table of floats
	- irregular table of doubles
*/

using namespace std;

//Integers
//Write function
void WriteInt(ofstream &out_file, int &n_out)
{
	out_file.write(reinterpret_cast<char *>(&n_out), sizeof(int));
}

//Read function
int ReadInt(ifstream &in_file)
{
	int n_in;
	in_file.read(reinterpret_cast<char *>(&n_in), sizeof(int));
	return n_in;
}


//Doubles
//Write function
void WriteDouble(ofstream &out_file, double &x_out)
{
        out_file.write(reinterpret_cast<char *>(&x_out), sizeof(double));
}

//Read function
double ReadDouble(ifstream &in_file)
{
        double x_in;
        in_file.read(reinterpret_cast<char *>(&x_in), sizeof(double));
        return x_in;
}


//Floats
//Write function
void WriteFloat(ofstream &out_file, float &x_out)
{
        out_file.write(reinterpret_cast<char *>(&x_out), sizeof(float));
}

//Read function
float ReadFloat(ifstream &in_file)
{
        float x_in;
        in_file.read(reinterpret_cast<char *>(&x_in), sizeof(float));
        return x_in;
}


//String
//Write function
void WriteString(ofstream &out_file, std::string str_out)
{
	//Size of the string
	int n = str_out.size();

	//Write length of the string
	out_file.write(reinterpret_cast<char *>(&n), sizeof(int));
	
	//Write the string
	out_file.write(&str_out[0], n*sizeof(char));
}

//Read function
std::string ReadString(ifstream &in_file)
{
	//Create the string
	std::string str_in;

	//Read the length of the string
	int n;
	in_file.read(reinterpret_cast<char *>(&n), sizeof(int));

	//Read the char
	str_in.resize(n);
	in_file.read(&str_in[0], n*sizeof(char));

	//Enforce \0
	/*char* c_str = new char[n+1];
	int ptr = sprintf(c_str, "%s", str_in.c_str());
	delete [] c_str;*/

	//Return results
	return str_in;
}



//Char *
//Write function
void WriteChar(ofstream &out_file, char *c_out)
{
	//Add the \0 at the end of the chain
	int i, n, char_length, char_length_0;
	char_length = strlen(c_out);

	//Check if last char of the array returns 0
	if(c_out[char_length] == 0)
	{
		//Add 1 to the length to take into account the 0 char
		char_length += 1;

		//Write length of the char
		out_file.write(reinterpret_cast<char *>(&char_length), sizeof(char_length));

                //Write the char
                out_file.write(c_out, char_length);
	}
	else
	{
		//If doesn't contain the 0 char, add it at the end of the array
		char *c_out_0 = new char[char_length+1];
		n = sprintf(c_out_0, "%s", c_out);
		c_out_0[char_length+1] = 0;

		//To be able to work with char of different length, store the length of the table as well
		char_length_0 = strlen(c_out_0);
		out_file.write(reinterpret_cast<char *>(&char_length_0), sizeof(char_length_0));

		//Write the char
		out_file.write(c_out, char_length_0);

		delete [] c_out_0;
	}


}

//Read function
char* ReadChar(ifstream &in_file)
{
	//Read the length of char
	int i, char_length;
	in_file.read(reinterpret_cast<char *>(&char_length), sizeof(int));

	//Read the char
	char *c_in = new char[char_length];
	in_file.read(c_in, char_length);

	//Check if 0 char at the end of the array
	if(c_in[char_length] == 0)
	{
		return c_in;
	}
	else //Add the 0 char at the end of the read array
	{
		char *c_in_0 = new char[char_length+1];
		int n;
		n = sprintf(c_in_0, "%s", c_in);
		c_in_0[char_length+1] = 0;
		return c_in_0;
	}
}


//Vector of doubles
//Write function
void WriteVectorDouble(ofstream &out_file, std::vector<double> &v_double)
{
	//To be able to work with vector of various size, length is stored before the vector itself
	int vector_length, i;
	vector_length = v_double.size();
	out_file.write(reinterpret_cast<char *>(&vector_length), sizeof(int));

	//Write each element of the vector as a double variable
	for(i=0 ; i<vector_length ; i++)
	{
		out_file.write(reinterpret_cast<char *>(&v_double[i]), sizeof(double));
	}
}

//Read function
std::vector<double> ReadVectorDouble(ifstream &in_file)
{
	//Get the length of the vector
	int vector_length, i;
	in_file.read(reinterpret_cast<char *>(&vector_length), sizeof(int));

	//Read vector values
	std::vector<double> v_double(vector_length);

	for(i=0 ; i<vector_length ; i++)
	{
		in_file.read(reinterpret_cast<char *>(&v_double[i]), sizeof(double));
	}

	return v_double;
}


//Vector of floats
//Write function
void WriteVectorFloat(ofstream &out_file, std::vector<float> &v_float)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int vector_length, i;
        vector_length = v_float.size();
        out_file.write(reinterpret_cast<char *>(&vector_length), sizeof(int));

        //Write each element of the vector as a float variable
        for(i=0 ; i<vector_length ; i++)
        {
                out_file.write(reinterpret_cast<char *>(&v_float[i]), sizeof(float));
        }
}

//Read function
std::vector<float> ReadVectorFloat(ifstream &in_file)
{
        //Get the length of the vector
        int vector_length, i;
        in_file.read(reinterpret_cast<char *>(&vector_length), sizeof(int));

        //Read vector values
        std::vector<float> v_float(vector_length);

        for(i=0 ; i<vector_length ; i++)
        {
                in_file.read(reinterpret_cast<char *>(&v_float[i]), sizeof(float));
        }

        return v_float;
}


//Vector of integers
//Write function
void WriteVectorInt(ofstream &out_file, std::vector<int> &v_int)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int vector_length, i;
        vector_length = v_int.size();
        out_file.write(reinterpret_cast<char *>(&vector_length), sizeof(int));

        //Write each element of the vector as a double variable
        for(i=0 ; i<vector_length ; i++)
        {
                out_file.write(reinterpret_cast<char *>(&v_int[i]), sizeof(int));
        }
}

//Read function
std::vector<int> ReadVectorInt(ifstream &in_file)
{
        //Get the length of the vector
        int vector_length, i;
        in_file.read(reinterpret_cast<char *>(&vector_length), sizeof(int));
    //    printf("%d\n", vector_length);
        //Read vector values
        std::vector<int> v_int(vector_length);

        for(i=0 ; i<vector_length ; i++)
        {
                in_file.read(reinterpret_cast<char *>(&v_int[i]), sizeof(int));
      //          printf("%.2f", v_int[i]);
        }

        return v_int;
}


//Matrix of doubles
//Write function
void WriteMatrixDouble(ofstream &out_file, std::vector< std::vector<double> > &m_double)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int n_row, n_col, i, j;
        n_row = m_double.size();
	n_col = m_double[0].size();
        out_file.write(reinterpret_cast<char *>(&n_row), sizeof(int));
	out_file.write(reinterpret_cast<char *>(&n_col), sizeof(int));

        //Write each element of the vector as a double variable
        for(i=0 ; i<n_row ; i++)
        {
		for(j=0 ; j<n_col ; j++)
		{
                	out_file.write(reinterpret_cast<char *>(&m_double[i][j]), sizeof(double));
		}
        }
}

//Read function
std::vector< std::vector<double> > ReadMatrixDouble(ifstream &in_file)
{
        //Get the length of the vector
        int n_row, n_col, i, j;
        in_file.read(reinterpret_cast<char *>(&n_row), sizeof(int));
	in_file.read(reinterpret_cast<char *>(&n_col), sizeof(int));

        //Read vector values
        std::vector< std::vector<double> > m_double(n_row, std::vector<double>(n_col));

        for(i=0 ; i<n_row ; i++)
        {
		for(j=0 ; j<n_col ; j++)
		{
                	in_file.read(reinterpret_cast<char *>(&m_double[i][j]), sizeof(double));
		}
        }

        return m_double;
}


//Matrix of integers
//Write function
void WriteMatrixInt(ofstream &out_file, std::vector< std::vector<int> > &m_int)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int n_row, n_col, i, j;
        n_row = m_int.size();
        n_col = m_int[0].size();
        out_file.write(reinterpret_cast<char *>(&n_row), sizeof(int));
        out_file.write(reinterpret_cast<char *>(&n_col), sizeof(int));

        //Write each element of the vector as a double variable
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col ; j++)
                {
                        out_file.write(reinterpret_cast<char *>(&m_int[i][j]), sizeof(int));
                }
        }
}

//Read function
std::vector< std::vector<int> > ReadMatrixInt(ifstream &in_file)
{
        //Get the length of the vector
        int n_row, n_col, i, j;
        in_file.read(reinterpret_cast<char *>(&n_row), sizeof(int));
        in_file.read(reinterpret_cast<char *>(&n_col), sizeof(int));

        //Read vector values
        std::vector< std::vector<int> > m_int(n_row, std::vector<int>(n_col));

        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col ; j++)
                {
                        in_file.read(reinterpret_cast<char *>(&m_int[i][j]), sizeof(int));
                }
        }

        return m_int;
}


//Matrix of floats
//Write function
void WriteMatrixFloat(ofstream &out_file, std::vector< std::vector<float> > &m_float)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int n_row, n_col, i, j;
        n_row = m_float.size();
        n_col = m_float[0].size();
        out_file.write(reinterpret_cast<char *>(&n_row), sizeof(int));
        out_file.write(reinterpret_cast<char *>(&n_col), sizeof(int));

        //Write each element of the vector as a double variable
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col ; j++)
                {
                        out_file.write(reinterpret_cast<char *>(&m_float[i][j]), sizeof(float));
                }
        }
}

//Read function
std::vector< std::vector<float> > ReadMatrixFloat(ifstream &in_file)
{
        //Get the length of the vector
        int n_row, n_col, i, j;
        in_file.read(reinterpret_cast<char *>(&n_row), sizeof(int));
        in_file.read(reinterpret_cast<char *>(&n_col), sizeof(int));

        //Read vector values
        std::vector< std::vector<float> > m_float(n_row, std::vector<float>(n_col));

        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col ; j++)
                {
                        in_file.read(reinterpret_cast<char *>(&m_float[i][j]), sizeof(float));
                }
        }

        return m_float;
}


//Irregular table of floats
//Write function
void WriteIrrTableFloat(ofstream &out_file, std::vector< std::vector<float> > &m_float)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int n_row, i, j;
	std::vector<int> n_col;
        n_row = m_float.size();
	for(i=0 ; i<n_row ; i++)
	{
		n_col.push_back(m_float[i].size());
	}
        out_file.write(reinterpret_cast<char *>(&n_row), sizeof(int));
	for(i=0 ; i<n_row ; i++)
	{
		out_file.write(reinterpret_cast<char *>(&n_col[i]), sizeof(int));
	}
        

        //Write each element of the vector as a double variable
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col[i] ; j++)
                {
                        out_file.write(reinterpret_cast<char *>(&m_float[i][j]), sizeof(float));
                }
        }
}

//Read function
std::vector< std::vector<float> > ReadIrrTableFloat(ifstream &in_file)
{
        //Get the length of the vector
        int n_row, i, j;
        in_file.read(reinterpret_cast<char *>(&n_row), sizeof(int));
	std::vector<int> n_col(n_row, -1);
	std::vector< std::vector<float> > m_float;
	for(i=0 ; i<n_row ; i++)
	{
		in_file.read(reinterpret_cast<char *>(&n_col[i]), sizeof(int));
		m_float.push_back(std::vector<float>(n_col[i], -1));
	}

        //Read vector values
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col[i] ; j++)
                {
			float buff;
                        in_file.read(reinterpret_cast<char *>(&buff), sizeof(float));
			m_float[i][j] = buff;
                }
        }

        return m_float;
}


//Irregular table of ints
//Write function
void WriteIrrTableInt(ofstream &out_file, std::vector< std::vector<int> > &m_int)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int n_row, i, j;
	std::vector<int> n_col;
        n_row = m_int.size();
	for(i=0 ; i<n_row ; i++)
	{
		n_col.push_back(m_int[i].size());
	}
        out_file.write(reinterpret_cast<char *>(&n_row), sizeof(int));
	for(i=0 ; i<n_row ; i++)
	{
		out_file.write(reinterpret_cast<char *>(&n_col[i]), sizeof(int));
	}

        //Write each element of the vector as a double variable
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col[i] ; j++)
                {
                        out_file.write(reinterpret_cast<char *>(&m_int[i][j]), sizeof(int));
                }
        }
}

//Read function
std::vector< std::vector<int> > ReadIrrTableInt(ifstream &in_file)
{
        //Get the length of the vector
        int n_row, i, j;
        in_file.read(reinterpret_cast<char *>(&n_row), sizeof(int));
	std::vector<int> n_col(n_row, -1);
	std::vector< std::vector<int> > m_int;
	for(i=0 ; i<n_row ; i++)
	{
		in_file.read(reinterpret_cast<char *>(&n_col[i]), sizeof(int));
		m_int.push_back(std::vector<int>(n_col[i], -1));
	}

        //Read vector values
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col[i] ; j++)
                {
			int buff;
                        in_file.read(reinterpret_cast<char *>(&buff), sizeof(int));
			m_int[i][j] = buff;
                }
        }

        return m_int;
}


//Irregular table of doubles
//Write function
void WriteIrrTableDouble(ofstream &out_file, std::vector< std::vector<double> > &m_double)
{
        //To be able to work with vector of various size, length is stored before the vector itself
        int n_row, i, j;
	std::vector<int> n_col;
        n_row = m_double.size();
	for(i=0 ; i<n_row ; i++)
	{
		n_col.push_back(m_double[i].size());
	}
        out_file.write(reinterpret_cast<char *>(&n_row), sizeof(int));
	for(i=0 ; i<n_row ; i++)
	{
		out_file.write(reinterpret_cast<char *>(&n_col[i]), sizeof(int));
	}
        

        //Write each element of the vector as a double variable
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col[i] ; j++)
                {
                        out_file.write(reinterpret_cast<char *>(&m_double[i][j]), sizeof(double));
                }
        }
}

//Read function
std::vector< std::vector<double> > ReadIrrTableDouble(ifstream &in_file)
{
        //Get the length of the vector
        int n_row, i, j;
        in_file.read(reinterpret_cast<char *>(&n_row), sizeof(int));
	std::vector<int> n_col(n_row, -1);
	std::vector< std::vector<double> > m_double;
	for(i=0 ; i<n_row ; i++)
	{
		in_file.read(reinterpret_cast<char *>(&n_col[i]), sizeof(int));
		m_double.push_back(std::vector<double>(n_col[i], -1));
	}

        //Read vector values
        for(i=0 ; i<n_row ; i++)
        {
                for(j=0 ; j<n_col[i] ; j++)
                {
			double buff;
                        in_file.read(reinterpret_cast<char *>(&buff), sizeof(double));
			m_double[i][j] = buff;
                }
        }

        return m_double;
}
