/*Definition of all types used in the creation of the objects and simulations

In order: 
	1. Map generation
	2. Network generation
	3. Community generation
	4. MCS
	5. Function BR_BB2017
	6. Function BR_functions
	7. historic
	8. GA_parameters



*/

//1
//Definition of the structure network_point
struct IM_pt
{
	double coord[2];
	float mu;
	float sigma;
};

//2
//Definition of the structure network_point
struct networkPoint
{
	double coord[2];
	int IDs[3];
	int diameter;
	std::string material;
	std::string pipe_type;
	std::string pipe_status;
	int LRI_zone;
	int year;
	float CRR_estim;
};

struct pumpSt
{
	std::vector<double> center_loc;
	int ID;
	std::string service_status;
	std::string function;
	int bckpower;
	int capacity;
	int n_pumps;
	int n_wells;
	int n_reservoirs;
	int LRI;
	float CRR_estim;
	int restoration_date;
};

//3
//Definition of the building footprint structure (out of the csv)
struct buildingFP
{
	int db_ID, mb_ID;
	float pop, area, crr, importance;
	std::vector<double> center_loc;
	std::string buildType;
};

//4


//5
//Structure for BB2017 coefficient tables
struct BB2017_tables
{
	//Loading the coefficient and uncertainty tables
	//Coefficient tables
	//Backbone curve
	float **coeff_PGV;
	//Ductility
	float **coeff_ductility;
	//Material
	float **coeff_material;
	//Diameter
	float ***coeff_diameter;
	//CRR
	float **coeff_PGV_CRR;
	//Ductility-CRR
	float **coeff_ductility_CRR;
        //Material-CRR
        float **coeff_material_CRR;
        //Diameter-CRR
	float ***coeff_diameter_CRR;
	//PH
	float **coeff_PGV_PH;
	//Ductility-PH
	float **coeff_ductility_PH;
        //Material-PH
        float **coeff_material_PH;

	//Uncertainty tables
	//Backbone curve
	float **u_PGV;
	//Ductility
	float **u_ductility;
	//Material
	float **u_material;
	//Diameter
	float **u_diameter;
	//CRR
	float **u_PGV_CRR;
	//Ductility-CRR
	float **u_ductility_CRR;
        //Material-CRR
        float **u_material_CRR;
        //Diameter-CRR
	float **u_diameter_CRR;
	//PH
	float **u_PGV_PH;
	//Ductility-PH
	float **u_ductility_PH;
        //Material-PH
        float **u_material_PH;
};

//6
//Structure for coefficient tables
struct BR_tables
{
	BB2017_tables t_BB2017;
};

