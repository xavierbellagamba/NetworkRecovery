//Need to include BinaryStream.h, cstdio and cstring

//Definition of the class pipesegment
class singleScenarioPhysicsGM
{
        //All properties are stored in a private mode
        private :
                int map_ID_, n_rasters_, source_ID_;
                double lambda_, spacing_;
		std::string name_;
		//1st line: X-coord, 2nd line: Y-coord
		std::vector< std::vector<double> > raster_;
		//PGV values
		std::vector< std::vector<float> > PGV_;
		//Pipe ID contained in rasters
		std::vector< std::vector <int> > pipe_belong_;
		//Building ID contained in rasters
		std::vector< std::vector <int> > building_belong_;


        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void name(std::string map_name){name_ = map_name;}
		void n_rasters(int n) {n_rasters_ = n;}
		void map_ID(int m_id) {map_ID_ = m_id;}
		void source_ID(int f) {source_ID_ = f;}
		void lambda(double lam) {lambda_ = lam;}
		void spacing(double sp) {spacing_ = sp;}
		void raster(std::vector< std::vector<double> > rast) {raster_ = rast;}
		void PGV(std::vector< std::vector<float> > pgv) {PGV_ = pgv;}
		void pipe_belong(std::vector< std::vector<int> > b_p) {pipe_belong_ = b_p;}
		void building_belong(std::vector< std::vector<int> > b_b) {building_belong_ = b_b;}
		

		//Get methods
		std::string name() const {return name_;}
		int n_rasters() const {return n_rasters_;}
		int map_ID() const {return map_ID_;}
                int source_ID() const {return source_ID_;}
		double lambda() const {return lambda_;}
		double spacing() const {return spacing_;}
		std::vector< std::vector<double> > raster() const {return raster_;}
		std::vector< std::vector<float> > PGV() const {return PGV_;}
		std::vector< std::vector<int> > pipe_belong() const {return pipe_belong_;}
		std::vector< std::vector<int> > building_belong() const {return building_belong_;}

		//Destructor
		~singleScenarioPhysicsGM();

		//Writing the pipeline as binary file
		void SAVE_Map(char *path_str, ofstream &outfile)
		{
			//Concatenate all parts of the final output path
			int n;
			char *file_name = new char[75];
			n = sprintf(file_name, "%s/GM.pgm", path_str);

			//Open out stream
			outfile.open(file_name, ios::out|ios::binary);

			/*
			Writing order:
				0. GM map name
				1. GM map ID
				2. Lambda
				3. Source ID
				4. Number of rasters
				5. Rasters
				6. Spacing
				8. Belonging pipes
				9. Belonging buildings
				10. PGV
			*/

			//GM map name
			WriteString(outfile, name_);

			//GM map ID
			WriteInt(outfile, map_ID_);

			//Lambda
			WriteDouble(outfile, lambda_);

			//Source ID
			WriteInt(outfile, source_ID_);

			//Number of rasters
			WriteInt(outfile, n_rasters_);

			//Rasters
			WriteMatrixDouble(outfile, raster_);

			//Spacing
			WriteDouble(outfile, spacing_);

			//Belonging pipes
			WriteIrrTableInt(outfile, pipe_belong_);

			//Belonging buildings
			WriteIrrTableInt(outfile, building_belong_);

			//PGV
			WriteMatrixFloat(outfile, PGV_);

			//Close outstream
			outfile.close();

			//Deallocating memory
			delete file_name;
		}

		//Reading the pipeline as binary file
		void READ_Map(char *file_name, ifstream &infile)
		{
			//Open the i stream
			infile.open(file_name, ios::in|ios::binary);

			//Read elements in the same order as written (see above comments and code lines)
			name(ReadString(infile));
			map_ID(ReadInt(infile));
        		lambda(ReadDouble(infile));
			source_ID(ReadInt(infile));
			n_rasters(ReadInt(infile));
			raster(ReadMatrixDouble(infile));
			spacing(ReadDouble(infile));
			pipe_belong(ReadIrrTableInt(infile));
			building_belong(ReadIrrTableInt(infile));
			PGV(ReadMatrixFloat(infile));

			//Close outstream
			infile.close();
		}
};

//Implementation of the destructor
singleScenarioPhysicsGM::~singleScenarioPhysicsGM(void)
{

}

