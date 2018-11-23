//Function returning the value of the atomic clock

//Requires: <sys/time.h>

int getClock()
{
	//Initialize random seed
	srand(time(NULL));

	//Generate one random number
	return rand();
}
