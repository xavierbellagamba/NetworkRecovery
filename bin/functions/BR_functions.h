/*----------------------------*/
//Break rate functions library//
/*----------------------------*/

//Needs to include vector, map and mkl

/*
Functions are called using a hash. The hash table is given at the begining of each new function set
Contains:
	- Assignment function (returns a vector of available BR fx ID for a given pipeline)
	- Break rate function from Bellagamba and Bradley 2017 (first # = 0)
	-
	-
*/

//List of the function sets
//BB17
pipeNetwork BellagambaBradley2017(pipeNetwork network, std::vector<float> P_compare, std::vector<float> GM_intensity, int ntwk_lvl, 
int soil_lvl, struct BB2017_tables t, int fx_pos);
std::vector<int> assign_BB2017(pipeSegment pipe);
struct BB2017_tables load_t_BB2017(void);


//Return the number of breaks following a Poissionan occurrence
//*****************************
//Recursive factorial function
//*****************************
unsigned long long factorial(int n)
{
	if(n==1)
	{
		return 1;
	}
	else
	{
		return factorial(n-1)*n;
	}
}

//********************
//Pipe break function
//********************
int generatePoissonFailure(float P_compare, float pipeLength, float BR)
{
	//Initialize varaibles
	int n_breaks = 0;
	double P;

	//Test if the pipe segment if damaged
	P = exp((-BR) * pipeLength/1000.0);

	if(P_compare < P)//Undamaged
	{
		return 0;
	}
	else//If damaged, how many breaks
	{
		while(P_compare > P)
		{
			n_breaks++;
			P += pow((BR * pipeLength/1000.0), n_breaks) * exp((-BR) * pipeLength/1000.0)/factorial(n_breaks);
		}

		//Return number of breaks
		return n_breaks;
	}
}


//Assignment function
std::vector< std::vector<int> > BR_fx_assign(pipeSegment pipe)
{
	//Initialize the return vector
	std::vector< std::vector<int> > availableBR(1, std::vector<int>(6));

	//Bellagamba and Bradley 2017
	availableBR[0] = assign_BB2017(pipe);

	//Other functions could be implemented here

	//Returning the vector
	return availableBR;

}


//Break rate switch function(select the wished function set to analyze the pipe network and return the break rate natural log)
pipeNetwork BR_fx(pipeNetwork network, std::vector<float> P_compare, std::vector<float> GM_intensity, std::vector<int> fx_set, 
int ntwk_lvl, int soil_lvl, struct BR_tables BR_t)
{
	//Number of sets to test
	int i;
	int n = fx_set.size();

	//Initialize matrix of failure
	std::vector< std::vector<int> > n_f(n, std::vector<int>(network.n_pipes(), -1));

	//Generate failures based on the selected functions
	for(i=0 ; i<n ;i++)
	{
		//Selection of the BR function set
		switch(fx_set[i])
		{
			case 0:
				network = BellagambaBradley2017(network, P_compare, GM_intensity, ntwk_lvl, soil_lvl, BR_t.t_BB2017, i);
				break;

			default :
				network = BellagambaBradley2017(network, P_compare, GM_intensity, ntwk_lvl, soil_lvl, BR_t.t_BB2017, i);
				break;
		}
	}

	//Return table of results
	return network;
}

//Loading function (loads tables from all functions)
struct BR_tables load_BR_tables(void)
{
	//Declaring variables
	struct BR_tables input_tables;

	//Load Bellagamba Bradley 2017 BR tables
	input_tables.t_BB2017 = load_t_BB2017();	

	//Returning the structure
	return input_tables;
}







