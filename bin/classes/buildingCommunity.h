//Need to include buildingFootprint.h, BinaryStream.h, cstdio and cstring

//Definition of the class building footprint
class buildingCommunity
{
        //All properties are stored in a private mode
        private :
                int n_buildings_, status_year_, community_ID_;
		std::vector<buildingFootprint> building_collection_;
		std::vector<buildingFootprint*> building_coll_ptr_;
                std::string community_name_;
		std::vector< std::vector<double> > raster_;
		std::vector< std::vector<int> > nearest_neighbour_;
		std::vector< std::vector<int> > building_belong_;

        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void n_buildings(int n) {n_buildings_ = n;}
		void status_year(int y_s) {status_year_ = y_s;}
		void community_ID(int cm_ID) {community_ID_ = cm_ID;}
		void building_collection(std::vector<buildingFootprint> collect_list) {building_collection_ = collect_list;}
		void community_name(std::string cm_name) {community_name_ = cm_name;}
		void building_coll_ptr(std::vector<buildingFootprint*> coll_ptr) {building_coll_ptr_ = coll_ptr;}
		void raster(std::vector< std::vector<double> > r) {raster_ = r;}
		void nearest_neighbour(std::vector< std::vector<int> > n) {nearest_neighbour_ = n;}
		void building_belong(std::vector< std::vector<int> > n) {building_belong_ = n;}

		//Get methods
		int n_buildings() const {return n_buildings_;}
		int status_year() const {return status_year_;}
		int community_ID() const {return community_ID_;}
                std::vector<buildingFootprint> building_collection() const {return building_collection_;}
		std::string community_name() const {return community_name_;}
		std::vector<buildingFootprint*> building_coll_ptr() const {return building_coll_ptr_;}
		std::vector< std::vector<double> > raster() const {return raster_;}
		std::vector< std::vector<int> > nearest_neighbour() const {return nearest_neighbour_;}
		std::vector< std::vector<int> > building_belong() const {return building_belong_;}

		//Accessors
		buildingFootprint *building(int building_ID) {return &building_collection_[building_ID];}

		//Destructor
		~buildingCommunity();

		//Writing the pipeline as binary file
		void SAVE_Community(char *path_str, ofstream &outfile)
		{
			//Concatenate all parts of the final output path
			int n, i;
			char *file_name = new char[100];
			n = sprintf(file_name, "%s/Community.dat", path_str);

			//Open out stream
			outfile.open(file_name, ios::out|ios::binary);

			/*
			Writing order:
				1. Community ID
				2. Community name
				3. Status year
				4. Number of buidling footprints
				5. Building footprints as objects
				6. Raster
				7. Nearest neighbour raster ID
				8. Building ID per raster
			*/

			//Community ID
			WriteInt(outfile, community_ID_);

			//Community name
                        WriteString(outfile, community_name_);

			//Status year
			WriteInt(outfile, status_year_);

			//Number of footprints
			WriteInt(outfile, n_buildings_);

			//Building collection
			for(i=0 ; i<n_buildings_ ; i++)
			{
				building_collection_[i].WriteBuildingFootprint(outfile);
			}
			
			//Raster
			WriteMatrixDouble(outfile, raster_);

			//Nearest neighbours
			WriteIrrTableInt(outfile, nearest_neighbour_);

			//Building Belong
			WriteIrrTableInt(outfile, building_belong_);

			//Close outstream
			outfile.close();

			//Deallocating memory
			delete [] file_name;
		}

		//Reading the pipeline as binary file
		void READ_Community(char *file_name, ifstream &infile)
		{
			//Open the i stream
			infile.open(file_name, ios::in|ios::binary);

			//Read elements in the same order as written (see above comments and code lines)
			community_ID(ReadInt(infile));
			community_name(ReadString(infile));
        		status_year(ReadInt(infile));
			n_buildings(ReadInt(infile));
			std::vector<buildingFootprint> building_collector(n_buildings_);
			building_coll_ptr(std::vector<buildingFootprint*>(n_buildings_));

			for(int i=0 ; i<n_buildings_ ; i++)
			{
				building_coll_ptr_[i] = &building_collector[i];
				building_collector[i].ReadBuildingFootprint(infile);
			}
			building_collection(building_collector);

			raster(ReadMatrixDouble(infile));
			nearest_neighbour(ReadIrrTableInt(infile));
			building_belong(ReadIrrTableInt(infile));

			//Close outstream
			infile.close();
		}

};

//Implementation of the destructor
buildingCommunity::~buildingCommunity(void)
{

}

