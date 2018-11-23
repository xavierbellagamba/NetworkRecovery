//Need to include BinaryStream.h, cstdio, string and cstring

//Definition of the class damageCollection
class damageCollection
{
        //All properties are stored in a private mode
        private :
                std::vector<pipeFailure> failureCollection_;
		int n_failures_;	


        //Set-get methods in public (use of the overload for simplification of the command)
        public :
                //Set methods
		void failureCollection(std::vector<pipeFailure> f) {failureCollection_ = f;}
		void n_failures(int n) {n_failures_ = n;}

		//Get methods
		std::vector<pipeFailure> failureCollection() const {return failureCollection_;}
		int n_failures() const {return n_failures_;}

		//Accessor
		pipeFailure* failure(int failure_ID) {return &failureCollection_[failure_ID];}

		//Destructor
		~damageCollection();
};

//Implementation of the destructor
damageCollection::~damageCollection(void)
{

}

