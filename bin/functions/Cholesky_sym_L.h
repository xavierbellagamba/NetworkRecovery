//Return number of elements of the packed Cholesky decomposition
int Cholesky_n_elem(int c)
{
	int n = 0.5*(c*c + c);
	return n;
}


//Function returning the L(Cholesky) triangular matrix for a symmetric input matrix
std::vector< std::vector<float> > Cholesky_sym_L(std::vector< std::vector<float> > M)
{
	//Initialize variables
	int i, j, k, flag_1;
	int n_raster = M.size();
	std::vector< std::vector<float> > L(n_raster, std::vector<float>(n_raster, 0.0));


	//Compute Cholesky decomposition
	for(k=0 ; k<n_raster ; k++)
	{
		//Work with the input matrix to fill L
		if(k==0)
		{
			//Initialize counter
			flag_1 = 0;

			for(j=k ; j<n_raster ; j++)
			{
				//Check if first column of the loop k
				if(k==j) //First column
				{
					//Operation on each element of the first column (iterate on rows)
					for(i=j ; i<n_raster ; i++)
					{
						if(flag_1==0)//First element => sqrt
						{
							L[i][j] = sqrt(M[i][j]);
							flag_1++;
						}
						else
						{
							L[i][j] = M[i][j]/L[k][k];
						}
					}
				}
				else
				{
					//Operation on other columns
					for(i=j ; i<n_raster ; i++)
					{
						L[i][j] = M[i][j] - L[i][k]*L[j][k];
					}
				}
			}
		}
		//Work with the L matrix only
		else
		{
			//Initialize counter
                        flag_1 = 0;

                        for(j=k ; j<n_raster ; j++)
                        {
                                //Check if first column of the loop k
                                if(k==j) //First column
                                {
                                        //Operation on each element of the first column (iterate on rows)
                                        for(i=j ; i<n_raster ; i++)
                                        {
                                                if(flag_1==0)//First element => sqrt
                                                {
                                                        L[i][j] = sqrt(L[i][j]);
							flag_1++;
                                                }
                                                else
                                                {
                                                        L[i][j] = L[i][j]/L[k][k];
                                                }
                                        }
                                }
                                else
                                {
                                        //Operation on other columns
                                        for(i=j ; i<n_raster ; i++)
                                        {
                                                L[i][j] = L[i][j] - L[i][k]*L[j][k];
                                        }
                                }
			}
		}
	}


	//Return the Cholesky L factor
	return L;
}



//Function returning the Cholesky L factor packed in row major
std::vector<float> Cholesky_sym_L_packed(std::vector< std::vector<float> > M)
{
	//Compute the decomposition
	std::vector< std::vector<float> > L = Cholesky_sym_L(M);


	//Reorganize data as a vector
        float progress;
	int i, j, progress_status;
	int n = Cholesky_n_elem(M.size());
	std::vector<float> L_packed(n);


	//Foloowing proposed packing scheme of intel for L-decomposed cov matrices
	for(i=0 ; i<M.size() ; i++)
	{
		for(j=0 ; j<=i ; j++)
		{
			int k = j + (i+1)*(i)/2;
			L_packed[k] = L[i][j];
		}
	}


	//Return packed L
	return L_packed;
}

