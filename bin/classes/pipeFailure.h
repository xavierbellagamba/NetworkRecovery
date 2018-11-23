//Need to include BinaryStream.h, cstdio, string and cstring

//Definition of the class pipeFailure
class pipeFailure
{
        //All properties are stored in a private mode
        private :
                std::vector<double> location_;
		int failureID_, pipeID_, discoveryDate_, priority_, pipeUnikID_, repairDate_;
		bool repaired_;


        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void location(std::vector<double> loc) {location_ = loc;}
		void failureID(int f_id) {failureID_ = f_id;}
		void pipeID(int p_id) {pipeID_ = p_id;}
		void discoveryDate(int d) {discoveryDate_ = d;}
		void repairDate(int d) {repairDate_ = d;}
		void priority(int p) {priority_ = p;}
		void pipeUnikID(int pu_id) {pipeUnikID_ = pu_id;}
		void repaired(bool r) {repaired_ = r;}

		//Get methods
		std::vector<double> location() const {return location_;}
		int failureID() const {return failureID_;}
		int pipeID() const {return pipeID_;}
		int discoveryDate() const {return discoveryDate_;}
		int repairDate() const {return repairDate_;}
		int priority() const {return priority_;}
		int pipeUnikID() const {return pipeUnikID_;}
		bool repaired() const {return repaired_;}

		//Destructor
		~pipeFailure();
};

//Implementation of the destructor
pipeFailure::~pipeFailure(void)
{

}

