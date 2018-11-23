//Need to include BinaryStream.h, cstdio, string and cstring

//Definition of the class pipesegment
class soilMap
{
        //All properties are stored in a private mode
        private :
                int map_ID_, n_rasters_;
                double spacing_;
		std::string name_;
		//1st line: X-coord, 2nd line: Y-coord
		std::vector< std::vector<double> > raster_;
		//CRR of each raster
		std::vector<float> CRR_;
		//Input value
		std::vector<float> soilInput_;
		//Input name
		std::string inputName_;
		//Pipe ID contained in rasters
		std::vector< std::vector <int> > pipe_belong_;
		//Building ID contained in rasters
		std::vector< std::vector <int> > building_belong_;


        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void name(std::string map_name){name_ = map_name;}
		void inputName(std::string iName){inputName_ = iName;}
		void n_rasters(int n) {n_rasters_ = n;}
		void map_ID(int m_id) {map_ID_ = m_id;}
		void spacing(double sp) {spacing_ = sp;}
		void raster(std::vector< std::vector<double> > rast) {raster_ = rast;}
		void CRR(std::vector<float> crr) {CRR_ = crr;}
		void soilInput(std::vector<float> input) {soilInput_ = input;}
		void pipe_belong(std::vector< std::vector<int> > b_p) {pipe_belong_ = b_p;}
		void building_belong(std::vector< std::vector<int> > b_b) {building_belong_ = b_b;}
		

		//Get methods
		std::string name() const {return name_;}
		std::string inputName() const {return inputName_;}
		int n_rasters() const {return n_rasters_;}
		int map_ID() const {return map_ID_;}
		double spacing() const {return spacing_;}
		std::vector< std::vector<double> > raster() const {return raster_;}
		std::vector<float> CRR() const {return CRR_;}
		std::vector<float> soilInput() const {return soilInput_;}
		std::vector< std::vector<int> > pipe_belong() const {return pipe_belong_;}
		std::vector< std::vector<int> > building_belong() const {return building_belong_;}

		//Destructor
		~soilMap();

		//Writing the pipeline as binary file
		void SAVE_Map(char *path_str, ofstream &outfile)
		{
			//Concatenate all parts of the final output path
			int n;
			char *file_name = new char[75];
			n = sprintf(file_name, "%s/Soil.dat", path_str);

			//Open out stream
			outfile.open(file_name, ios::out|ios::binary);

			/*
			Writing order:
				0. Soil map name
				1. Soil map ID
				2. Number of rasters
				3. Rasters
				4. Spacing
				5. CRR
				6. Input name
				7. Input value
				8. Belonging pipes
				9. Belonging buildings
			*/

			//GM map name
			WriteString(outfile, name_);

			//GM map ID
			WriteInt(outfile, map_ID_);

			//Number of rasters
			WriteInt(outfile, n_rasters_);

			//Rasters
			WriteMatrixDouble(outfile, raster_);

			//Spacing
			WriteDouble(outfile, spacing_);

			//CRR
			WriteVectorFloat(outfile, CRR_);

			//Input name
			WriteString(outfile, inputName_);
			
			//Input value
			WriteVectorFloat(outfile, soilInput_);

			//Belonging pipes
			WriteIrrTableInt(outfile, pipe_belong_);

			//Belonging buildings
			WriteIrrTableInt(outfile, building_belong_);

			//Close outstream
			outfile.close();

			//Deallocating memory
			delete [] file_name;
		}

		//Reading the pipeline as binary file
		void READ_Map(char *file_name, ifstream &infile)
		{
			//Open the i stream
			infile.open(file_name, ios::in|ios::binary);

			//Read elements in the same order as written (see above comments and code lines)
			name(ReadString(infile));
			map_ID(ReadInt(infile));
			n_rasters(ReadInt(infile));
			raster(ReadMatrixDouble(infile));
			spacing(ReadDouble(infile));
			CRR(ReadVectorFloat(infile));
			inputName(ReadString(infile));
			soilInput(ReadVectorFloat(infile));
			pipe_belong(ReadIrrTableInt(infile));
			building_belong(ReadIrrTableInt(infile));

			//Close outstream
			infile.close();
		}
};

//Implementation of the destructor
soilMap::~soilMap(void)
{

}

