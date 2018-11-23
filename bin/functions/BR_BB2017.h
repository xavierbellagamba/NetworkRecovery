/*----------------------------*/
//Bellagamba Bradley 2017 BR  //
/*----------------------------*/

//Requires: mkl, sys/time, vector, map
//Contains: BR function, Loading function and Assignment function 

//Break rate according to Bellagamba and Bradley (2017)
pipeNetwork BellagambaBradley2017(pipeNetwork network, std::vector<float> P_compare, std::vector<float> GM_intensity, int ntwk_lvl, int soil_lvl, struct BB2017_tables t, int fx_pos)
{
	/*Bellagamba and Bradley 2017 (1st # = 0):
		ntwk_lvl: level of knowledge on the network. Min 1, Max 4
		soil_lvl: knowledge of the CRR. 0 = no, 1 = yes
		Hash table:
	        	2nd #: for PGV (always equal to 1)
                	3rd #: for material ductility (0: for unknown, 1 for ductile material and 2 for brittle material)
	                4th #: for material (0 for unknown, 1 = AC, 2 = GALV, 3 = CLS, 4 = CI, 5 = Steel, 6 = PVC, 7 = MDPE80, 8 = HDPE, 9 = DI)
        	        5th #: for diameter (0 for unknown, 1 = 0-150, 2 = 150-300 and 3 = 300-600)
			6th #: for CRR (0 for unknown value and 1 for known)
		Note: a CRR of -1 means stiff soil
	*/

	//Number of analyzed pipe segments
	int n;
	n = network.n_pipes();

	//Initialization of the vector of failures
	//std::vector<int>n_f(n, -1);
	int i, j, k;

	//Set the seed as random using GM intensities
	int seed_ID = rand()%32000;

	//Computation of the epsilon vector
	std::vector<float> seed(n);
	VSLStreamStatePtr rng_stream;
	vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed_ID);
	vsRngGaussian(VSL_RNG_METHOD_GAUSSIAN_BOXMULLER, rng_stream, n, &seed[0], 0.0, 1.0);
	vslDeleteStream(&rng_stream);

	//BR compuation for each pipeline
	for(k=0 ; k<n ; k++)
	{
		//Extraction of the parameter for pipeline k
		std::vector<int> BR_prmtr = network.pipe(k)->list_BR()[0];

		//Check GM intensity (cannot be higher than 200cm/s)
		float PGV;

		if(GM_intensity[network.pipe(k)->GMraster_ID()] > 150.0)
		{
			PGV = 150.0;
		}
		else
		{
			PGV = GM_intensity[network.pipe(k)->GMraster_ID()];
		}

		//Adaptation of the levels of knowledge on the pipe object
		int ntwk_lvl_adjust, soil_lvl_adjust;
		//Pipe
		for(ntwk_lvl_adjust=1 ; ntwk_lvl_adjust<ntwk_lvl ; ntwk_lvl_adjust++)
		{
			if(BR_prmtr[ntwk_lvl_adjust+1] == 0)
			{
				break;
			}
		}
		//Soil
		soil_lvl_adjust = BR_prmtr[5];

		//Initialize ln_BR
		float ln_BR = 0.0;

		//Base ln(BR) (only PGV dependent)
		ln_BR += t.coeff_PGV[0][0] * pow(PGV, t.coeff_PGV[0][1]) + t.coeff_PGV[0][2];

		//Future functions
		if(BR_prmtr[2] == 3)
		{
			//Add CRR component
			if(soil_lvl_adjust == 1)
			{
				ln_BR += t.coeff_PGV_CRR[0][0] * pow(network.pipe(k)->CRR_estim(), t.coeff_PGV_CRR[0][1]) + t.coeff_PGV_CRR[0][2];
			}
			else if(soil_lvl_adjust == 2)
			{
				ln_BR += t.coeff_PGV_PH[0][0] * PGV + t.coeff_PGV_PH[0][1];
			}

			//Select correction in function of the pipe functionality
			switch(BR_prmtr[3])
			{
				//Main
				case 1:
					ln_BR -= (.031)*BR_prmtr[4] + 0.741;
					break;
				//Submain
				case 2:
					ln_BR -= (.054)*BR_prmtr[4] + .99;
					break;
			}
		}
		//Normal functions
		else
		{
			//Switch for soil conditions
			if(soil_lvl_adjust == 2)//Stiff soil
			{
				//Computation of the corrected break rate
				//Stiff soil correction
				ln_BR += t.coeff_PGV_PH[0][0] * PGV + t.coeff_PGV_PH[0][1];

				//Network level
				for(i=2 ; i<=ntwk_lvl_adjust ; i++)
				{
					switch(i)
					{
						case 2: //Material ductility
							ln_BR += t.coeff_ductility_PH[BR_prmtr[2]][0] * PGV + t.coeff_ductility_PH[BR_prmtr[2]][1];
							break;

						case 3: //Material
							ln_BR += t.coeff_material_PH[BR_prmtr[3]][0] * PGV + t.coeff_material_PH[BR_prmtr[3]][1];
							break;

						default:
							break;
					}
				}
			}
			else//Unknown or soft soil
			{
				//Computation of the corrected break rate
				//Network level
				for(i=2 ; i<=ntwk_lvl_adjust ; i++)
				{
					switch(i)
					{
						case 2: //Material ductility
							ln_BR += t.coeff_ductility[BR_prmtr[2]][0] * PGV + t.coeff_ductility[BR_prmtr[2]][1];
							break;

						case 3: //Material
							ln_BR += t.coeff_material[BR_prmtr[3]][0] * PGV + t.coeff_material[BR_prmtr[3]][1];
							break;

						case 4: //Diameter
							ln_BR += t.coeff_diameter[BR_prmtr[3]][BR_prmtr[4]][0] * PGV + t.coeff_diameter[BR_prmtr[3]][BR_prmtr[4]][1];
							break;
					}
				}

				//Soil level
				if(soil_lvl_adjust == 1)
				{
					switch(ntwk_lvl_adjust)
					//Look for the exact same position as above
					{
						case 1: //PGV
		                                        ln_BR += t.coeff_PGV_CRR[0][0] * pow(network.pipe(k)->CRR_estim(), t.coeff_PGV_CRR[0][1]) + t.coeff_PGV_CRR[0][2];
		                                        break;

		                                case 2: //Material ductility
		                                        ln_BR += t.coeff_ductility_CRR[BR_prmtr[2]][0] * pow(network.pipe(k)->CRR_estim(), t.coeff_ductility_CRR[BR_prmtr[2]][1]) + t.coeff_ductility_CRR[BR_prmtr[2]][2];
		                                        break;

		                                case 3: //Material
		                                        ln_BR += t.coeff_material_CRR[BR_prmtr[3]][0] * pow(network.pipe(k)->CRR_estim(), t.coeff_material_CRR[BR_prmtr[3]][1]) + t.coeff_material_CRR[BR_prmtr[3]][2];
		                                        break;

		                                case 4: //Diameter
		                                        ln_BR += t.coeff_diameter_CRR[BR_prmtr[3]][BR_prmtr[4]][0] * pow(network.pipe(k)->CRR_estim(), t.coeff_diameter_CRR[BR_prmtr[3]][BR_prmtr[4]][1]) + t.coeff_diameter_CRR[BR_prmtr[3]][BR_prmtr[4]][2];
		                                        break;
					}
				}
			}
		}
		//Add uncertainties
		float sigma = 0.0;

		//For future functions: uncertainty from PGV-CRR
		if(BR_prmtr[2] == 3)
		{
			ntwk_lvl_adjust = 1;
		}

		switch(ntwk_lvl_adjust)
		{
			case 1:
				if(soil_lvl_adjust == 0)//Only PGV known
				{
					sigma = t.u_PGV[0][0];
				}
				else if(soil_lvl_adjust == 1)//PGV and CRR
				{
					sigma = t.u_PGV_CRR[0][0];
				}
				else if(soil_lvl_adjust == 2)//PGV and stiff soil
				{
					sigma = t.u_PGV_PH[0][0];
				}
				break;

			case 2:
				if(soil_lvl_adjust == 0)//PGV and ductility known
        	                {
					sigma = t.u_ductility[BR_prmtr[2]][0];
                        	}
                	        else if(soil_lvl_adjust == 1)//PGV, ductility and CRR
        	                {
					sigma = sigma = t.u_ductility_CRR[BR_prmtr[2]][0];;
	                        }
				else if(soil_lvl_adjust == 2)//PGV, ductility and stiff soil
        	                {
					sigma = sigma = t.u_ductility_PH[BR_prmtr[2]][0];;
	                        }
				break;

			case 3:
				if(soil_lvl_adjust == 0)//PGV, ductility and material
	                        {
					sigma = t.u_material[BR_prmtr[3]][0];
                	        }
        	                else if(soil_lvl_adjust == 1)//PGV, ductility, material and CRR
	                        {
					sigma = t.u_material_CRR[BR_prmtr[3]][0];
                        	}
				else if(soil_lvl_adjust == 2)//PGV, ductility, material and stiff soil
	                        {
					sigma = t.u_material_PH[BR_prmtr[3]][0];
                        	}
				break;

			case 4:
				if(soil_lvl_adjust == 0)//PGV, ductility, material and diameter
	                        {
					sigma = t.u_diameter[BR_prmtr[3]][BR_prmtr[4]];
                        	}
                	        else if(soil_lvl_adjust == 1)//All paramters are known
        	                {
					sigma = t.u_diameter_CRR[BR_prmtr[3]][BR_prmtr[4]];
        	                }
				else if(soil_lvl_adjust == 2)//PGV, ductility, material and stiff soil (no diameter-dependent functions for stiff soil)
	                        {
					sigma = t.u_material_PH[BR_prmtr[3]][0];
                        	}
				break;
		}

		//Generate random number
		float epsilon;
		VSLStreamStatePtr rng_stream;
		vslNewStream(&rng_stream, VSL_BRNG_SFMT19937, seed[k]);
		vsRngGaussian(VSL_RNG_METHOD_GAUSSIAN_BOXMULLER, rng_stream, 1, &epsilon, 0.0, 1.0);
		vslDeleteStream(&rng_stream);

		//Estimate BR
		float BR = exp(ln_BR + epsilon*sigma);

		//Determine number of failures following Poisson distribution
		int n_f = generatePoissonFailure(P_compare[k], network.pipe(k)->length(), BR);

		//Assign number of failure to pipe k
		network.pipe(k)->updateNFailure(fx_pos, n_f);
	}

	//Return
	return network;
}


//Loading function for tables
struct BB2017_tables load_t_BB2017(void)
{
	//Declare variables
	struct BB2017_tables input_tables;
	int i;

	//Loading the coefficient and uncertainty tables
	//Coefficient tables
	//Backbone curve
	input_tables.coeff_PGV = import_float_table("./bin/functions/tables/BB2017/f_PGV.csv");
	//Ductility
	input_tables.coeff_ductility = import_float_table("./bin/functions/tables/BB2017/f_ductility.csv");
	//Material
	input_tables.coeff_material = import_float_table("./bin/functions/tables/BB2017/f_material.csv");
	//Diameter
	input_tables.coeff_diameter = new float**[10];
	for(int i=0 ; i<10 ; i++)
	{
		char *input_file = new char[100];
		int ptr = sprintf(input_file, "./bin/functions/tables/BB2017/f_diameter_%d.csv", i);
		input_tables.coeff_diameter[i] = import_float_table(input_file);
		delete [] input_file;
	}	
	//CRR
	input_tables.coeff_PGV_CRR = import_float_table("./bin/functions/tables/BB2017/f_CRR.csv");
	//Ductility-CRR
	input_tables.coeff_ductility_CRR = import_float_table("./bin/functions/tables/BB2017/f_ductility_CRR.csv");
        //Material-CRR
        input_tables.coeff_material_CRR = import_float_table("./bin/functions/tables/BB2017/f_material_CRR.csv");
        //Diameter-CRR
	input_tables.coeff_diameter_CRR = new float**[10];
	for(int i=0 ; i<10 ; i++)
	{
		char *input_file = new char[100];
		int ptr = sprintf(input_file, "./bin/functions/tables/BB2017/f_diameter_CRR_%d.csv", i);
		input_tables.coeff_diameter_CRR[i] = import_float_table(input_file);
		delete [] input_file;
	}
	//PH
	input_tables.coeff_PGV_PH = import_float_table("./bin/functions/tables/BB2017/f_PH.csv");
	//Ductility-PH
	input_tables.coeff_ductility_PH = import_float_table("./bin/functions/tables/BB2017/f_ductility_PH.csv");
        //Material-PH
        input_tables.coeff_material_PH = import_float_table("./bin/functions/tables/BB2017/f_material_PH.csv");

	//Uncertainty tables
	//Backbone curve
	input_tables.u_PGV = import_float_table("./bin/functions/tables/BB2017/u_PGV.csv");
	//Ductility
	input_tables.u_ductility = import_float_table("./bin/functions/tables/BB2017/u_ductility.csv");
	//Material
	input_tables.u_material = import_float_table("./bin/functions/tables/BB2017/u_material.csv");
	//Diameter
	input_tables.u_diameter = import_float_table("./bin/functions/tables/BB2017/u_diameter.csv");	
	//CRR
	input_tables.u_PGV_CRR = import_float_table("./bin/functions/tables/BB2017/u_CRR.csv");
	//Ductility-CRR
	input_tables.u_ductility_CRR = import_float_table("./bin/functions/tables/BB2017/u_ductility_CRR.csv");
        //Material-CRR
        input_tables.u_material_CRR = import_float_table("./bin/functions/tables/BB2017/u_material_CRR.csv");
        //Diameter-CRR
	input_tables.u_diameter_CRR = import_float_table("./bin/functions/tables/BB2017/u_diameter_CRR.csv");
	//PH
	input_tables.u_PGV_PH = import_float_table("./bin/functions/tables/BB2017/u_PH.csv");
	//Ductility-PH
	input_tables.u_ductility_PH = import_float_table("./bin/functions/tables/BB2017/u_ductility_PH.csv");
        //Material-PH
        input_tables.u_material_PH = import_float_table("./bin/functions/tables/BB2017/u_material_PH.csv");

	//Returning the structure
	return input_tables;
}


//Structure for comparison of char* in the dictionary
struct cmp_str
{
	bool operator()(char const *a, char const *b)
	{
		return std::strncmp(a, b, (int)strlen(a)) < 0;
	}
};


//Assignment function
std::vector<int> assign_BB2017(pipeSegment pipe)
{
	//Initialize the return vector
	std::vector<int> availableBR(6);

	//Bellagamba and Bradley 2017 (index 0)
	//Assigning the hashs
	//Function set hash
	availableBR[0] = 0;

	//IM hash: 1 for PGV
	availableBR[1] = 1;

	//Material hash
	//Transform the char of the pipe line as small
	std::string material_low = pipe.material();
	std::transform(material_low.begin(), material_low.end(), material_low.begin(), ::tolower);

	//Create a dictionary with the existing material names in the database
	std::map<const char *, int, cmp_str> dict_material = {{"ac", 1}, {"galv", 2}, {"cls", 3}, {"ci", 4}, {"steel", 5}, {"pvc", 6}, {"mdpe80", 7},
		{"hdpe", 8}, {"di", 9}, {"upvc", 6}, {"pe", 8}, {"pe100", 8}, {"ldpe", 7}, {"mpvc", 6}, {"rcrr", 0}, {"mldi", 0}, {"mdpe100", 7},
		{"cldi", 0}, {"api", 0}, {"al", 0}, {"abs", 0}, {"", 0}, {"gi", 2}};

	//Get the material hash
	availableBR[3] = dict_material[material_low.c_str()];

	//Ductility hash (deducted from the material hash)
	switch(availableBR[3])
	{
		case 0://Unknown
			availableBR[2] = 0;
			break;

		case 1://AC
			availableBR[2] = 2;
			break;

		case 2://GALV
			availableBR[2] = 2;
			break;

		case 3://CLS
			availableBR[2] = 2;
                        break;

                case 4://CI
			availableBR[2] = 2;
                        break;

                case 5://Steel
			availableBR[2] = 1;
                        break;

		case 6://PVC
			availableBR[2] = 1;
                        break;

                case 7://MDPE80
			availableBR[2] = 1;
                        break;

                case 8://HDPE
			availableBR[2] = 1;
                        break;

		case 9://DI
			availableBR[2] = 1;
                        break;

                default://Considered as unknown
			availableBR[2] = 0;
                        break;

	}

	//Diameter hash
	if((pipe.diameter() > 0) && (pipe.diameter() <= 80))
	{
		availableBR[4] = 1;
	}
	else if((pipe.diameter() > 80) && (pipe.diameter() <= 300))
	{
		availableBR[4] = 2;
	}
	else if(pipe.diameter() > 300)
	{
		availableBR[4] = 3;
	}
	else
	{
		availableBR[4] = 0;
	}

	//Soil hash
	if(abs(pipe.CRR_estim() + 2) < 0.001)//Unknown soil conditions
	{
		availableBR[5] = 0;
	}
	else if(abs(pipe.CRR_estim() + 1) < 0.001)//Stiff soil
	{
		availableBR[5] = 2;
	}
	else //Known CRR
	{
		availableBR[5] = 1;
	}

	//Returning the vector
	return availableBR;
}



