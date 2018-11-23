//Need to include BinaryStream.h, cstdio and cstring

//Definition of the class pipesegment
class pumpStation
{
        //All properties are stored in a private mode
        private :
                int year_, n_pumps_, db_ID_, unik_ID_, raster_ID_, capacity_, generator_, anchored_, n_wells_, n_reservoirs_, restoration_date_;
		std::vector<float> los_;
		std::vector<int> pipe_in_ID_;
		std::vector<int> pipe_out_ID_;
		std::vector< std::vector<int> > list_FF_;
                float CRR_estim_;
		std::vector<double> center_loc_;
                std::string struct_type_, found_type_;

        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void n_pumps(int n) {n_pumps_ = n;}
		void restoration_date(int d) {restoration_date_ = d;}
		void n_wells(int n) {n_wells_ = n;}
		void n_reservoirs(int n) {n_reservoirs_ = n;}
		void los(std::vector<float> l) {los_ = l;}
		void raster_ID(int r_ID) {raster_ID_ = r_ID;}
		void pipe_in_ID(std::vector<int> pipe) {pipe_in_ID_ = pipe;}
		void pipe_out_ID(std::vector<int> pipe) {pipe_out_ID_ = pipe;}
		void db_ID(int data_ID) {db_ID_ = data_ID;}
		void unik_ID(int u_ID) {unik_ID_ = u_ID;}
		void capacity(int c) {capacity_ = c;}
		void year(int y) {year_ = y;}
		void CRR_estim(float crr) {CRR_estim_ = crr;}
		void list_FF(std::vector< std::vector<int> > l_FF) {list_FF_ = l_FF;}
		void center_loc(std::vector<double> center) {center_loc_ = center;}
		void struct_type(std::string s) {struct_type_ = s;}
		void found_type(std::string f) {found_type_ = f;}
		void generator(int gen) {generator_ = gen;}
		void anchored(int anch) {anchored_ = anch;}

		//Get methods
		int n_pumps() const {return n_pumps_;}
		int restoration_date() const {return restoration_date_;}
		int n_wells() const {return n_wells_;}
		int n_reservoirs() const {return n_reservoirs_;}
		std::vector<float> los() const {return los_;}
		int raster_ID() const {return raster_ID_;}
		std::vector<int> pipe_in_ID() const {return pipe_in_ID_;}
		std::vector<int> pipe_out_ID() const {return pipe_out_ID_;}
		int db_ID() const {return db_ID_;}
		int unik_ID() const {return unik_ID_;}
		int capacity() const {return capacity_;}
		int year() const {return year_;}
		float CRR_estim() const {return CRR_estim_;}
		std::vector< std::vector<int> > list_FF() const {return list_FF_;}
		std::vector<double> center_loc() const {return center_loc_;}
		std::string struct_type() const {return struct_type_;}
		std::string found_type() const {return found_type_;}
		int generator() const {return generator_;}
		int anchored() const {return anchored_;}


		//Destructor
		~pumpStation();


		//Write pipe object in a binary file
		void WritePumpStation(ofstream &outfile)
		{
			/*
			Writing order:
				1. unik ID
				2. CCCID
				3. Number of pumps
				4. Raster ID
				5. Pipe in ID
				6. Pipe out ID
				7. Capacity (max daily flow [m3/d])
				8. Year built
				9. CRR
				10. List of fragility functions
				11. LOS
				12. Center location
				13. Structure type
				14. Foundation type
				15. Possess a backup power generation system
				16. Has anchored components
				17. Number of integrated wells
				18. Number of integrated reservoirs
				19. Restoration date
			*/

			//Unik ID
			WriteInt(outfile, unik_ID_);

			//CCCID
			WriteInt(outfile, db_ID_);

			//Nmber of pumps
			WriteInt(outfile, n_pumps_);

			//Raster ID
			WriteInt(outfile, raster_ID_);

			//Pipe in ID
			WriteVectorInt(outfile, pipe_in_ID_);

			//Pipe out ID
			WriteVectorInt(outfile, pipe_out_ID_);

			//Capacity
			WriteInt(outfile, capacity_);

			//Year
			WriteInt(outfile, year_);

			//CRR
			WriteFloat(outfile, CRR_estim_);

			//List of fragility functions
			WriteMatrixInt(outfile, list_FF_);

			//LOS
			WriteVectorFloat(outfile, los_);

			//Center location
			WriteVectorDouble(outfile, center_loc_);

			//Structre type
			WriteString(outfile, struct_type_);

			//Foundation type
			WriteString(outfile, found_type_);

			//Generator
			WriteInt(outfile, generator_);

			//Anchored components
			WriteInt(outfile, anchored_);

			//Number of wells
			WriteInt(outfile, n_wells_);

			//Number of reservoirs
			WriteInt(outfile, n_reservoirs_);

			//Restoration date
			WriteInt(outfile, restoration_date_);
		}


		//Read pipe object from a binary file
		void ReadPumpStation(ifstream &infile)
		{
			//Read elements in the same order as written (see above comments and code lines)
			unik_ID(ReadInt(infile));
			db_ID(ReadInt(infile));
			n_pumps(ReadInt(infile));
			raster_ID(ReadInt(infile));
			pipe_in_ID(ReadVectorInt(infile));
			pipe_out_ID(ReadVectorInt(infile));
			capacity(ReadInt(infile));
			year(ReadInt(infile));
			CRR_estim(ReadFloat(infile));
			list_FF(ReadMatrixInt(infile));
			los(ReadVectorFloat(infile));
			center_loc(ReadVectorDouble(infile));
			struct_type(ReadString(infile));
			found_type(ReadString(infile));
			generator(ReadInt(infile));
			anchored(ReadInt(infile));
			n_wells(ReadInt(infile));
			n_reservoirs(ReadInt(infile));
			restoration_date(ReadInt(infile));
		}


		//Writing the pipeline as binary file
		void SAVE_Pump(char *path_str, ofstream &outfile)
		{
			//Concatenate all parts of the final output path
			char *file_name = new char[150];
			int n = sprintf(file_name, "%s/Pump%d.dat", path_str, unik_ID_);

			//Open out stream
			outfile.open(file_name, ios::out|ios::binary);

			/*
			Writing order:
				1. unik ID
				2. CCCID
				3. Number of pumps
				4. Raster ID
				5. Pipe in ID
				6. Pipe out ID
				7. Capacity (max daily flow [m3/d])
				8. Year built
				9. CRR
				10. List of fragility functions
				11. LOS
				12. Center location
				13. Structure type
				14. Foundation type
				15. Possess a backup power generation system
				16. Has anchored components
				17. Number of wells
				18. Number of reservoirs
				19. Restoration date
			*/

			//Write the segment
			WritePumpStation(outfile);

			//Close outstream
			outfile.close();

			//Deallocating memory
			delete file_name;
		}

		//Reading the pipeline as binary file
		void READ_Pump(char *file_name, ifstream &infile)
		{
			//Open the i stream
			infile.open(file_name, ios::in|ios::binary);

			//Read pipe segment
			ReadPumpStation(infile);

			//Close outstream
			infile.close();

		}

};

//Implementation of the destructor
pumpStation::~pumpStation(void)
{

}

