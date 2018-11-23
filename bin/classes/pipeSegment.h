//Need to include BinaryStream.h, cstdio and cstring

//Definition of the class pipesegment
class pipeSegment
{
        //All properties are stored in a private mode
        private :
                int diameter_, year_, n_cnct_ , n_pts_, db_ID_, unik_ID_, raster_ID_, GMraster_ID_;
		std::vector< std::vector<int> > pipe_connections_;
		std::vector<int> connected2pump_;
		std::vector<int> n_breaks_;
		std::vector< std::vector<int> > list_BR_;
                float length_, CRR_estim_;
		std::vector<double> center_loc_;
		std::vector< std::vector<double> > points_;
                std::string material_, pipe_status_, pipe_type_;

        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void n_pts(int n) {n_pts_ = n;}
		void n_breaks(std::vector<int> n_br) {n_breaks_ = n_br;}
		void connected2pump(std::vector<int> c2p) {connected2pump_ = c2p;}
		void raster_ID(int r_ID) {raster_ID_ = r_ID;}
		void GMraster_ID(int r_ID) {GMraster_ID_ = r_ID;}
		void n_cnct(int c) {n_cnct_ = c;}
		void db_ID(int data_ID) {db_ID_ = data_ID;}
		void unik_ID(int u_ID) {unik_ID_ = u_ID;}
		void diameter(int d) {diameter_ = d;}
		void year(int y) {year_ = y;}
		void CRR_estim(float crr) {CRR_estim_ = crr;}
		void pipe_connections(std::vector< std::vector<int> > connect_list) {pipe_connections_ = connect_list;}
		void list_BR(std::vector< std::vector<int> > l_BR) {list_BR_ = l_BR;}
		void length(float l) {length_ = l;}
		void center_loc(std::vector<double> center) {center_loc_ = center;}
		void points(std::vector< std::vector<double> > pts)
		{
			//Assign point locations
			points_ = pts;

			//Compute and assign center of the pipeline
			int n, i;
			double E_loc = 0, N_loc = 0;
			std::vector<double> center_l(2);
			n = pts.size();

			for(i=0 ; i<n ; i++)
			{
				E_loc += pts[0][i];
				N_loc += pts[1][i];
			}

			center_l[0] = E_loc/n;
			center_l[1] = N_loc/n;

			center_loc(center_l);

		}
		void material(std::string m) {material_ = m;}
		void pipe_status(std::string s) {pipe_status_ = s;}
		void pipe_type(std::string t) {pipe_type_ = t;}

		//Get methods
		int n_pts() const {return n_pts_;}
		int n_cnct() const {return n_cnct_;}
		std::vector<int> n_breaks() const {return n_breaks_;}
		std::vector<int> connected2pump() const {return connected2pump_;}
		int raster_ID() const {return raster_ID_;}
		int GMraster_ID() const {return GMraster_ID_;}
                int db_ID() const {return db_ID_;}
                int unik_ID() const {return unik_ID_;}
		int diameter() const {return diameter_;}
		int year() const {return year_;}
		float CRR_estim() const {return CRR_estim_;}
		std::vector< std::vector<int> > pipe_connections() const {return pipe_connections_;}
		std::vector< std::vector<int> > list_BR() const {return list_BR_;}
		float length() const {return length_;}
		std::vector< std::vector<double> > points() const {return points_;}
		std::string material() const {return material_;}
		std::string pipe_status() const {return pipe_status_;}
		std::string pipe_type() const {return pipe_type_;}
		std::vector<double> center_loc() const {return center_loc_;}

		//Update number of breaks for given BR function
		void updateNFailure(int pos, int n)
		{
			n_breaks_[pos] = n;
		}

		//Destructor
		~pipeSegment();


		//Write pipe object in a binary file
		void WritePipeSegment(ofstream &outfile)
		{
			/*
			Writing order:
				1. unik ID
				2. CCCID
				3. Material
				4. Diameter in mm
				5. Length in m
				6. LRI zone
				7. Pipe type
				8. Pipe status
				9. Year laid
				10. Number of points
				11. Points
				12. Number of connections
				13. Connections
				14. Raster ID
				15. GM raster ID
				16. List of available break rate functions as hash table
				17. Number of breaks
				18. List of connected pumps
			*/

			//Unik ID
			WriteInt(outfile, unik_ID_);

			//CCCID
			WriteInt(outfile, db_ID_);

			//Material
			WriteString(outfile, material_);

			//Diameter
			WriteInt(outfile, diameter_);

			//Length
			WriteFloat(outfile, length_);

			//CRR
			WriteFloat(outfile, CRR_estim_);

			//Pipe type
			WriteString(outfile, pipe_type_);

			//Pipe status
			WriteString(outfile, pipe_status_);

			//Year laid
			WriteInt(outfile, year_);

			//Number of points
			WriteInt(outfile, n_pts_);

			//Points
			WriteMatrixDouble(outfile, points_);

			//Number of connections
			WriteInt(outfile, n_cnct_);

			//Connections
			WriteMatrixInt(outfile, pipe_connections_);

			//Raster ID
			WriteInt(outfile, raster_ID_);

			//GM Raster ID
			WriteInt(outfile, GMraster_ID_);

			//List BR
			WriteMatrixInt(outfile, list_BR_);

			//Number of breaks
			WriteVectorInt(outfile, n_breaks_);

			//Connected to pump
			WriteVectorInt(outfile, connected2pump_);
		}


		//Read pipe object from a binary file
		void ReadPipeSegment(ifstream &infile)
		{
			//Read elements in the same order as written (see above comments and code lines)
			unik_ID(ReadInt(infile));
        		db_ID(ReadInt(infile));
			material(ReadString(infile));
			diameter(ReadInt(infile));
			length(ReadFloat(infile));
			CRR_estim(ReadFloat(infile));
			pipe_type(ReadString(infile));
			pipe_status(ReadString(infile));
			year(ReadInt(infile));
			n_pts(ReadInt(infile));
			points(ReadMatrixDouble(infile));
			n_cnct(ReadInt(infile));
			pipe_connections(ReadMatrixInt(infile));
			raster_ID(ReadInt(infile));
			GMraster_ID(ReadInt(infile));
			list_BR(ReadMatrixInt(infile));
			n_breaks(ReadVectorInt(infile));
			connected2pump(ReadVectorInt(infile));
		}


		//Writing the pipeline as binary file
		void SAVE_Pipe(char *path_str, ofstream &outfile)
		{
			//Concatenate all parts of the final output path
			char *file_name = new char[150];
			int n = sprintf(file_name, "%s/Pipe%d.dat", path_str, unik_ID_);

			//Open out stream
			outfile.open(file_name, ios::out|ios::binary);

			/*
			Writing order:
				1. unik ID
				2. CCCID
				3. Material
				4. Diameter in mm
				5. Length in m
				6. LRI zone
				7. Pipe type
				8. Pipe status
				9. Year laid
				10. Number of points
				11. Points
				12. Number of connections
				13. Connections
				14. Raster ID
				15. List of available break rate functions as hash table
				16. Number of breaks
				17. List of connected pumps
			*/

			//Write the segment
			WritePipeSegment(outfile);

			//Close outstream
			outfile.close();

			//Deallocating memory
			delete [] file_name;
		}

		//Reading the pipeline as binary file
		void READ_Pipe(char *file_name, ifstream &infile)
		{
			//Open the i stream
			infile.open(file_name, ios::in|ios::binary);

			//Read pipe segment
			ReadPipeSegment(infile);

			//Close outstream
			infile.close();

		}

};

//Implementation of the destructor
pipeSegment::~pipeSegment(void)
{

}

