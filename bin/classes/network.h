//Need to include pipeSegment.h, pumpStation.h BinaryStream.h, cstdio and cstring
class network
{
	//All properties are stored in a private mode
	private:
		int n_SPassets_, n_souces_, status_year_, network_ID_, prep_lvl_;
		std::string network_name_, network_type_;
		std::vector<int> SPasset_frag_fx_;
		std::vector<float> frag_fx_trust_;
		std::vector< std::vector<double> > raster_;
		std::vector< std::vector<int> > nearest_neighbour_;
		std::vector< std::vector<int> > SPasset_belong_;

	//Set-get methods in public (use of the overload for simplification of the command)
	public:
		//Set methods
		void n_SPassets(int n) {n_SPassets_ = n;}
		void n_sources(int n) {n_sources_ = n;}
		void status_year(int y_s) {status_year_ = y_s;}
		void network_ID(int nt_ID) {network_ID_ = nt_ID;}
		void prep_lvl(int lvl) {prep_lvl_ = lvl;}
		void network_name(std::string nt_name) {network_name_ = nt_name;}
		void network_type(std::string nt_type) {network_type_ = nt_type;}
		void SPasset_frag_fx(std::vector<int> fx){SPasset_frag_fx_ = fx;}
		void frag_fx_trust(std::vector<float> trust){frag_fx_trust_ = trust;}
		void raster(std::vector< std::vector<double> > r) {raster_ = r;}
		void nearest_neighbour(std::vector< std::vector<int> > n) {nearest_neighbour_ = n;}
		void SPasset_belong(std::vector< std::vector<int> > n) {SPasset_belong_ = n;}

		//Get methods
		int n_SPassets() const {return n_SPassets_;}
		int n_sources() const {return n_sources_;}
		int status_year() const {return status_year_;}
		int network_ID() const {return network_ID_;}
		int prep_lvl() const {return prep_lvl_;}
		std::string network_name() const {return network_name_;}
		std::string network_type() const {return network_type_;}
		std::vector<int> SPasset_frag_fx() const {return SPasset_frag_fx_;}
		std::vector<float> frag_fx_trust() const {return frag_fx_trust_;}
		std::vector< std::vector<double> > raster() const {return raster_;}
		std::vector< std::vector<int> > nearest_neighbour() const {return nearest_neighbour_;}
		std::vector< std::vector<int> > SPasset_belong() const {return SPasset_belong_;}

		//Destructor
		~pipeNetwork();




}
//Implementation of the destructor
network::~network(void)
{

}




//Definition of the class pipesegment
class pipeNetwork : public network
{
        //All properties are stored in a private mode
        private :
		std::vector<pipeSegment> pipe_collection_;
		std::vector<pipeSegment*> pipe_coll_ptr_;
		std::vector<pumpStation> pump_collection_;
		std::vector<pumpStation*> pump_coll_ptr_;
		std::vector< std::vector<int> > submain_belong_;

        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void pipe_collection(std::vector<pipeSegment> collect_list) {pipe_collection_ = collect_list;}
		void pipe_coll_ptr(std::vector<pipeSegment*> coll_ptr) {pipe_coll_ptr_ = coll_ptr;}
		void pump_collection(std::vector<pumpStation> collect_list) {pump_collection_ = collect_list;}
		void pump_coll_ptr(std::vector<pumpStation*> coll_ptr) {pump_coll_ptr_ = coll_ptr;}
		void submain_belong(std::vector< std::vector<int> > n) {submain_belong_ = n;}

		//Get methods
                std::vector<pipeSegment> pipe_collection() const {return pipe_collection_;}
		std::vector<pipeSegment*> pipe_coll_ptr() const {return pipe_coll_ptr_;}
		std::vector<pumpStation> pump_collection() const {return pump_collection_;}
		std::vector<pumpStation*> pump_coll_ptr() const {return pump_coll_ptr_;}
		std::vector< std::vector<int> > submain_belong() const {return submain_belong_;}

		//Accessors
		pipeSegment *pipe(int pipe_ID) {return &pipe_collection_[pipe_ID];}
		pumpStation *pump(int pump_ID) {return &pump_collection_[pump_ID];}

		//Destructor
		~pipeNetwork();

		//Writing the pipeline as binary file
		void SAVE_Network(char *path_str, ofstream &outfile)
		{
			//Concatenate all parts of the final output path
			int n, i;
			char *file_name = new char[100];
			n = sprintf(file_name, "%s/Network.dat", path_str);

			//Open out stream
			outfile.open(file_name, ios::out|ios::binary);

			/*
			Writing order:
				1. Network ID
				2. Network name
				3. Status year
				4. Vector of fragility function ID
				5. Vector of fragility function trust
				6. Number of pipe segments
				7. Pipe segments as objects
				8. Number of pumping stations
				9. Pumping stations
				10. Level of preparation of the network
				11. Raster
				12. Nearest neighbour raster ID
				13. Pipe ID per raster
				14. Submain ID per raster
			*/

			//Network ID
			WriteInt(outfile, network_ID_);

			//Network name
                        WriteString(outfile, network_name_);

			//Status year
			WriteInt(outfile, status_year_);

			//Pipe fragility fx ID
			WriteVectorInt(outfile, pipe_frag_fx_);

			//Fragility fx trust
			WriteVectorFloat(outfile, frag_fx_trust_);

			//Number of segments
			WriteInt(outfile, n_pipes_);

			//Pipeline collection
			for(i=0 ; i<n_pipes_ ; i++)
			{
				pipe_collection_[i].WritePipeSegment(outfile);
			}

			//Number of pumpiing stations
			WriteInt(outfile, n_pumps_);

			//Pumping station collection
			for(i=0 ; i<n_pumps_ ; i++)
			{
				pump_collection_[i].WritePumpStation(outfile);
			}

			//Level of preparation
			WriteInt(outfile, prep_lvl_);

			//Raster
			WriteMatrixDouble(outfile, raster_);

			//Nearest neighbours
			WriteIrrTableInt(outfile, nearest_neighbour_);

			//Pipe Belong
			WriteIrrTableInt(outfile, pipe_belong_);

			//Submain belong
			WriteIrrTableInt(outfile, submain_belong_);

			//Close outstream
			outfile.close();

			//Deallocating memory
			delete [] file_name;
		}

		//Reading the pipeline as binary file
		void READ_Network(char *file_name, ifstream &infile)
		{
			//Open the i stream
			infile.open(file_name, ios::in|ios::binary);

			//Read elements in the same order as written (see above comments and code lines)
			network_ID(ReadInt(infile));
			network_name(ReadString(infile));
        		status_year(ReadInt(infile));
			pipe_frag_fx(ReadVectorInt(infile));
			frag_fx_trust(ReadVectorFloat(infile));
			n_pipes(ReadInt(infile));
			std::vector<pipeSegment> pipe_collector(n_pipes_);
			pipe_coll_ptr(std::vector<pipeSegment*>(n_pipes_));
			for(int i=0 ; i<n_pipes_ ; i++)
			{
				pipe_coll_ptr_[i] = &pipe_collector[i];
				pipe_collector[i].ReadPipeSegment(infile);
			}
			pipe_collection(pipe_collector);

			n_pumps(ReadInt(infile));

			std::vector<pumpStation> pump_collector(n_pumps_);
			pump_coll_ptr(std::vector<pumpStation*>(n_pumps_));	
			for(int i=0 ; i<n_pumps_ ; i++)
			{
				pump_coll_ptr_[i] = &pump_collector[i];
				pump_collector[i].ReadPumpStation(infile);
			}
			pump_collection(pump_collector);

			prep_lvl(ReadInt(infile));
			raster(ReadMatrixDouble(infile));
			nearest_neighbour(ReadIrrTableInt(infile));
			pipe_belong(ReadIrrTableInt(infile));
			submain_belong(ReadIrrTableInt(infile));

			//Close outstream
			infile.close();
		}

};

//Implementation of the destructor
pipeNetwork::~pipeNetwork(void)
{

}

