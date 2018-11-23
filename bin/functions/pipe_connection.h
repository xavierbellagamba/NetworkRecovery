/*###############################################################################################################################

Function to remove duplicates from an int std::vector

###############################################################################################################################*/
std::vector<int> removeDuplicates(std::vector<int> v)
{
	std::sort(v.begin(), v.end()); 
	auto last = std::unique(v.begin(), v.end());
	v.erase(last, v.end());
	return v;
}


/*###############################################################################################################################

Recursive algorithm to check the pipe connectivity
--------------------------------------------------

Change the value of std::vector at indice pipe_ID if pipeline pipe_ID is undamaged. Explore all branches.
Return a std::vector of int of length "number of pipe segments" containing the connection status of all pipes in respect to the 
source.
-1: Unchecked
0: Disconnected
1 or 2: Connected

###############################################################################################################################*/
void pipe_connect_new(std::vector<int> & connected_pipe_list, pipeNetwork *ntwk, int pipe_ID, int i_fx)
{
	//Undamaged pipe segment
	if(ntwk->pipe(pipe_ID)->n_breaks()[i_fx] < 1)
	{
		//Declare variables
		int i, j, k;
		bool in_list = false;

		//Check if pipe segment already in the list
		if(connected_pipe_list[pipe_ID] == 1)
		{
			in_list = true;
		}

		//If not in the list, add it
		if(in_list == false)
		{
			//Add the current pipe segment to the list of connected pipes
			connected_pipe_list[pipe_ID] = 1;
		}


		//Check if connections of current pipe segment are already in the list and undamaged
		for(i=0 ; i<ntwk->pipe(pipe_ID)->n_cnct() ; i++)
		{
			int next_pipe_ID = ntwk->pipe(pipe_ID)->pipe_connections()[1][i];
			bool is_candidate = true;
			if(connected_pipe_list[next_pipe_ID]>=1 || ntwk->pipe(next_pipe_ID)->n_breaks()[i_fx] > 0)
			{
				is_candidate = false;
			}

			//If is a good candidate, recursive call
			if(is_candidate)
			{
				pipe_connect_new(connected_pipe_list, ntwk, next_pipe_ID, i_fx);
			}		
		}

		//Explore remaining branches
		for(i=0 ; i<connected_pipe_list.size() ; i++)
		{
			if(connected_pipe_list[i] == 1)
			{
				//Update i to avoid infinite loop
				connected_pipe_list[i] = 2;

				for(j=0 ; j<ntwk->pipe(i)->n_cnct() ; j++)
				{
					int next_pipe_ID = ntwk->pipe(i)->pipe_connections()[1][j];

					//Is the connection already explored or failed
					bool explored = false;

					if((connected_pipe_list[next_pipe_ID] >= 1) || (ntwk->pipe(next_pipe_ID)->n_breaks()[i_fx]>0))
					{
						explored = true;
					}

					//If not, recursive call
					if(explored == false)
					{
						pipe_connect_new(connected_pipe_list, ntwk, next_pipe_ID, i_fx);
					}
				}
			}
		}
	}
}

