//Need to include BinaryStream.h, cstdio and cstring

//Definition of the class pipesegment
class buildingFootprint
{
        //All properties are stored in a private mode
        private :
                int db_ID_, unik_ID_, raster_ID_, pipeID_connect_, year_constr_, n_storey_, mb_ID_, GMraster_ID_;
		float pop_, area_, crr_, importance_;
		std::string build_type_, struct_type_, found_type_;
		std::vector<double> center_loc_;
		std::vector< std::vector<int> > list_fragFx_;
		std::vector<int> is_connect_;

        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void db_ID(int data_ID) {db_ID_ = data_ID;}
		void mb_ID(int meshblock_ID) {mb_ID_ = meshblock_ID;}
		void unik_ID(int u_ID) {unik_ID_ = u_ID;}
		void raster_ID(int r_ID) {raster_ID_ = r_ID;}
		void GMraster_ID(int r_ID) {GMraster_ID_ = r_ID;}
		void is_connect(std::vector<int> is_conn) {is_connect_ = is_conn;}
		void pipeID_connect(int pipeID) {pipeID_connect_ = pipeID;}
		void year_constr(int y) {year_constr_ = y;}
		void n_storey(int n) {n_storey_ = n;}
		void pop(float p) {pop_ = p;}
		void area(float a) {area_ = a;}
		void crr(float c) {crr_ = c;}
		void build_type(std::string bt) {build_type_ = bt;}
		void struct_type(std::string st) {struct_type_ = st;}
		void found_type(std::string ft) {found_type_ = ft;}
		void center_loc(std::vector<double> center) {center_loc_ = center;}
		void list_fragFx(std::vector< std::vector<int> > fragFx) {list_fragFx_ = fragFx;}
		void importance(float imp) {importance_ = imp;}

		//Get methods
		int db_ID() const {return db_ID_;}
		int mb_ID() const {return mb_ID_;}
		int unik_ID() const {return unik_ID_;}
		int raster_ID() const {return raster_ID_;}
		int GMraster_ID() const {return GMraster_ID_;}
		std::vector<int> is_connect() const {return is_connect_;}
		int pipeID_connect() const {return pipeID_connect_;}
		int year_constr() const {return year_constr_;}
		int n_storey() const {return n_storey_;}
		float pop() const {return pop_;}
		float area() const {return area_;}
		float crr() const {return crr_;}
		std::string build_type() const {return build_type_;}
		std::string struct_type() const {return struct_type_;}
		std::string found_type() const {return found_type_;}
		std::vector<double> center_loc() const {return center_loc_;}
		std::vector< std::vector<int> > list_fragFx() const {return list_fragFx_;}
		float importance() const {return importance_;}

		//Destructor
		~buildingFootprint();


		//Write pipe object in a binary file
		void WriteBuildingFootprint(ofstream &outfile)
		{
			/*
			Writing order:
				1. unik ID
				2. Database ID
				3. Raster ID
				4. Is connected
				5. Pipe ID connected
				6. Construction year
				7. Number of storeys
				8. Population
				9. Area
				10. Building type
				11. Structure type
				12. Center location
				13. List of fragiltiy functions
				14. GM raster ID
				15. Importance (for recovery)
			*/

			//Unik ID
			WriteInt(outfile, unik_ID_);

			//Database ID
			WriteInt(outfile, db_ID_);

			//Meshblock ID
			WriteInt(outfile, mb_ID_);

			//Raster ID
			WriteInt(outfile, raster_ID_);

			//Is connected
			WriteVectorInt(outfile, is_connect_);

			//Pipe ID connected
			WriteInt(outfile, pipeID_connect_);

			//Construction year
			WriteInt(outfile, year_constr_);

			//Number of storeys
			WriteInt(outfile, n_storey_);

			//Population
			WriteFloat(outfile, pop_);

			//Area
			WriteFloat(outfile, area_);

			//CRR
			WriteFloat(outfile, crr_);

			//Building type
			WriteString(outfile, build_type_);

			//Structure type
			WriteString(outfile, struct_type_);

			//Foundation type
			WriteString(outfile, found_type_);

			//Center of location
			WriteVectorDouble(outfile, center_loc_);

			//List of fragility functions
			WriteMatrixInt(outfile, list_fragFx_);

			//GM raster ID
			WriteInt(outfile, GMraster_ID_);

			//Importance
			WriteFloat(outfile, importance_);
		}


		//Read pipe object from a binary file
		void ReadBuildingFootprint(ifstream &infile)
		{
			//Read elements in the same order as written (see above comments and code lines)
			unik_ID(ReadInt(infile));
        		db_ID(ReadInt(infile));
			mb_ID(ReadInt(infile));
			raster_ID(ReadInt(infile));
			is_connect(ReadVectorInt(infile));
			pipeID_connect(ReadInt(infile));
			year_constr(ReadInt(infile));
			n_storey(ReadInt(infile));
			pop(ReadFloat(infile));
			area(ReadFloat(infile));
			crr(ReadFloat(infile));
			build_type(ReadString(infile));
			struct_type(ReadString(infile));
			found_type(ReadString(infile));
			center_loc(ReadVectorDouble(infile));
			list_fragFx(ReadMatrixInt(infile));
			GMraster_ID(ReadInt(infile));
			importance(ReadFloat(infile));
		}


		//Writing the pipeline as binary file
		void SAVE_BuildingFootprint(char *path_str, ofstream &outfile)
		{
			//Concatenate all parts of the final output path
			char *file_name = new char[150];
			int n = sprintf(file_name, "%s/BuildingFootPrint%d.dat", path_str, unik_ID_);

			//Open out stream
			outfile.open(file_name, ios::out|ios::binary);

			//Write the segment
			WriteBuildingFootprint(outfile);

			//Close outstream
			outfile.close();

			//Deallocating memory
			delete file_name;
		}

		//Reading the pipeline as binary file
		void READ_BuildingFootprint(char *file_name, ifstream &infile)
		{
			//Open the i stream
			infile.open(file_name, ios::in|ios::binary);

			//Read pipe segment
			ReadBuildingFootprint(infile);

			//Close outstream
			infile.close();

		}

};

//Implementation of the destructor
buildingFootprint::~buildingFootprint(void)
{

}

