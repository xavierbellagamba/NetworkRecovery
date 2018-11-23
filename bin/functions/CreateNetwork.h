/*Create Network

Contains 2 functions: 
	- OriginalNetwork_csv (saves the original network geometry)
	- CreateNetworkEmpiricalGM
	- CreateNetworkPhysicsGM

Both main functions require the same input: the original name of the network (without extension), the name of map (without its extension), the name of the network to be created, the max length of a pipe segment and the year of the analyzed network

*/


/*###############################################################################################################################

originalNetwork_csv: Save the network geometry for post-processing purposes as csv file

###############################################################################################################################*/
void OriginalNetwork_csv(pipeNetwork originalNetwork, char *network_folder)
{
	//Variable declaration
	char *str_csv_file = new char[200];
	int buff;
	ofstream outfile;

	//Pipe geometry
	//Name of the file
	buff = sprintf(str_csv_file, "%s/_post_network_pipegeometry.csv", network_folder);

	//Open ofstream
	outfile.open(str_csv_file, ios::out);

	//Write the file		
	for(int i=0 ; i<originalNetwork.n_pipes() ; i++)
	{
		int str_ptr;
		char *str_pipe = new char[100];

		//Pipe ID
		str_ptr = sprintf(str_pipe, "%d ;", originalNetwork.pipe(i)->unik_ID());
		outfile.write(str_pipe, strlen(str_pipe));
			
		//Points location (Each point represented as E ; N
		for(int j=0 ; j<originalNetwork.pipe(i)->n_pts() ; j++)
		{
			char *str_loc = new char[30];
			str_ptr = sprintf(str_loc, "%f ; %f ; ", originalNetwork.pipe(i)->points()[0][j], 
				originalNetwork.pipe(i)->points()[1][j]);
			outfile.write(str_loc, strlen(str_loc));
			delete [] str_loc;
                }
		outfile.write("\n", strlen("\n"));

		delete [] str_pipe;
	}
	outfile.close();

	//Pump location
	//Name of the file
	buff = sprintf(str_csv_file, "%s/_post_network_pump.csv", network_folder);

	//Open ofstream
	outfile.open(str_csv_file, ios::out);

	//Write the file		
	for(int i=0 ; i<originalNetwork.n_pumps() ; i++)
	{
		int str_ptr;
		char *str_pump = new char[100];

		//Pump ID
		str_ptr = sprintf(str_pump, "%d ; %f ; %f ; %d", originalNetwork.pump(i)->unik_ID(), 
			originalNetwork.pump(i)->restoration_date(), originalNetwork.pump(i)->center_loc()[0], 
			originalNetwork.pump(i)->center_loc()[1]);

		outfile.write(str_pump, strlen(str_pump));
			
		outfile.write("\n", strlen("\n"));

		delete [] str_pump;
	}
	outfile.close();

	//Clear char*
	delete [] str_csv_file;
}


/*###############################################################################################################################

importNetworkData: Import data points from csv file

###############################################################################################################################*/
std::vector<networkPoint> importNetworkData(char *origin_network_path)
{
	//Declare basic variables
	int i, j;

	//Row count
	int row_count = Row_counter(origin_network_path);


	//Create the networPoints structure
	std::vector<networkPoint> ntwkPt(row_count);
	string line_buffer;
	int arg_counter;
	char *tk_bf;


	//Initiate data stream
	ifstream data_stream;
	data_stream.open(origin_network_path);


	//Import each point of the discretized network as a structure
	for(i=0 ; i<row_count ; i++)
	{
		arg_counter = 0;
		getline(data_stream, line_buffer);
		std::vector<char> v(line_buffer.length() + 1);
		strcpy(&v[0], line_buffer.c_str());
		char *line_buffer_char = &v[0];
		char *token_buffer = strtok(line_buffer_char, ";,");
		++arg_counter;
		while(arg_counter <= 11)
		{
			switch(arg_counter)
			{
				case 1 :
					ntwkPt[i].coord[0] = atof(token_buffer);
					break;

				case 2 : 
					ntwkPt[i].coord[1] = atof(token_buffer); 
					break;

				case 3 : 
					ntwkPt[i].IDs[0] = atoi(token_buffer); 
					break;

				case 4 : 
                                        ntwkPt[i].IDs[1] = atoi(token_buffer); 
                                        break;

				case 5 : 
                                        ntwkPt[i].IDs[2] = atoi(token_buffer); 
                                        break;

				case 6 : 
                                        ntwkPt[i].diameter = atoi(token_buffer); 
                                        break;

				case 7 :
					ntwkPt[i].material = token_buffer;
					break;

				case 8 : 
                                        ntwkPt[i].year = atoi(token_buffer); 
                                        break;

				case 9 :
                                        ntwkPt[i].pipe_status = token_buffer; 
                                        break;

				case 10 : 
                                        ntwkPt[i].pipe_type = token_buffer;
                                        break;

				case 11 :
					if((token_buffer != NULL) && (token_buffer[0] != '\0'))
					{
						ntwkPt[i].LRI_zone = atoi(token_buffer);
					}
					else
					{
						ntwkPt[i].LRI_zone = -3;
					}
					switch(ntwkPt[i].LRI_zone)
					{
						case 0:
							ntwkPt[i].CRR_estim = 0.032;
							break;

						case 1:
							ntwkPt[i].CRR_estim = 0.065;
							break;

						case 2:
							ntwkPt[i].CRR_estim = 0.13;
							break;

						case 3:
							ntwkPt[i].CRR_estim = 0.195;
							break;

						case 4:
							ntwkPt[i].CRR_estim = 0.26;
							break;

						case -1:
							ntwkPt[i].CRR_estim = 0.265;
							break;

						case -2:
							ntwkPt[i].CRR_estim = -1.0;
							break;

						default:
							ntwkPt[i].CRR_estim = -2.0;
							break;
					}
                                        break;

			}
			++arg_counter;
			token_buffer = strtok(NULL, ";,");
		}
	}


	//Close data stream
	data_stream.close();


	//Return the results
	return ntwkPt;
}


/*###############################################################################################################################

createRawPipeSegment: Create pipe segments objects from networkPoint structures with a given max length

###############################################################################################################################*/
int createRawPipeSegment(std::vector<networkPoint> ntwkPt, float max_pipe_length)
{
	//Initialize basic variables
	int i, j, k;
	int row_count = ntwkPt.size();
	float tol = 0.0001;


	//Create the folder for tempopary storage of the objects
	mkdir("./temp", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	//Create the objects
	//Initialize necessary variables
	float pipeLength, incrLength;
	std::vector< std::vector<double> > pipePts(2);
	int point_counter, pipe_counter = 0;
	pipeSegment *pipe_segment;
	pipeSegment newSegment;
	pipe_segment = &newSegment;
	char *file_str = "temp";
	ofstream pipe_saver;


	//Initialization of the first object variables
	//Point collection
	pipePts[0].push_back(ntwkPt[0].coord[0]);
        pipePts[1].push_back(ntwkPt[0].coord[1]);
	//Pipe lenght
	pipeLength = 0.0;
	//Pipe counter
	point_counter = 1;
	//Pseudo-connection
	std::vector<int> pseudo_connection(3, -1);
	//Pseudo-available BRs
	std::vector< std::vector<int> > pseudo_BR(1, std::vector<int>(6, 0));
	//Pseudo breaks
	std::vector<int> pseudo_breaks(1, -1);
	std::vector<float> pseudo_fl(1, -1);
	//Pseudo connected pumps
	std::vector<int> pseudo_pump(1);


	//Loop over the network points
	for(i=0 ; i<row_count-1 ; i++)
	{
		//Distance between point i and i+1
		incrLength = sqrt(pow(ntwkPt[i].coord[0] - ntwkPt[i+1].coord[0], 2) + pow(ntwkPt[i].coord[1] - ntwkPt[i+1].coord[1], 2));

		/*If next point is of same LRI and ID and that the total length is inf. than 100m:
		Update the length and add point to the collection. Connections will be created once all objects are created*/
		if(((abs(ntwkPt[i].IDs[2] - ntwkPt[i+1].IDs[2]) < tol) &&
			(abs(ntwkPt[i].LRI_zone - ntwkPt[i+1].LRI_zone) < tol) &&
			(pipeLength + incrLength <= max_pipe_length)) ||
			((point_counter < 2) && (abs(ntwkPt[i].IDs[2] - ntwkPt[i+1].IDs[2]) < tol)))
		{
			//Update length
			pipeLength = pipeLength + incrLength;

			//Add point i+1 to the collection
			++point_counter;
			pipePts[0].push_back(ntwkPt[i+1].coord[0]);
			pipePts[1].push_back(ntwkPt[i+1].coord[1]);

			if(i == row_count-2)/*Last i+1 point management*/
			{
				//Creation of the object
        	                pipe_segment = new pipeSegment;

        	                //Assign value to the object properties
        	                newSegment.unik_ID(pipe_counter);
        	                newSegment.db_ID(ntwkPt[i].IDs[2]);
        	                newSegment.n_pts(point_counter);
        	                newSegment.diameter(ntwkPt[i].diameter);
        	                newSegment.year(ntwkPt[i].year);
        	                newSegment.points(pipePts);
        	                newSegment.length(pipeLength);
        	                newSegment.CRR_estim(ntwkPt[i].CRR_estim);
        	                newSegment.material(ntwkPt[i].material);
        	                newSegment.pipe_status(ntwkPt[i].pipe_status);
        	                newSegment.pipe_type(ntwkPt[i].pipe_type);
				newSegment.pipe_connections(pseudo_BR);
        	                newSegment.list_BR(pseudo_BR);
        	                newSegment.n_cnct(1);
        	                newSegment.raster_ID(-1);
				newSegment.GMraster_ID(-1);
        	                newSegment.n_breaks(pseudo_breaks);
				newSegment.connected2pump(pseudo_pump);

        	                //Save the object
        	                newSegment.SAVE_Pipe(file_str, pipe_saver);

        	                //Destroy the object
        	                delete pipe_segment;

				//Artificailly increment pipe_counter (C indices start at 0)
				pipe_counter++;
			}
		}
		//Management of isolated point (not part of a segment)
		else if(((point_counter < 2) && (abs(ntwkPt[i].IDs[2] - ntwkPt[i+1].IDs[2]) > tol))) 
		{
			//Re-initialize pipe parameter (length an point collection)
			point_counter = 1;
                        pipeLength = 0.0;
                        pipePts[0].clear();
                        pipePts[1].clear();

			//Set first point of the new pipe segment as i+1
			pipePts[0].push_back(ntwkPt[i+1].coord[0]);
                        pipePts[1].push_back(ntwkPt[i+1].coord[1]);
		}
		else
		{
			//Creation of the object
			pipe_segment = new pipeSegment;

			//Assign value to the object properties
			newSegment.unik_ID(pipe_counter);
			newSegment.db_ID(ntwkPt[i].IDs[2]);
			newSegment.n_pts(point_counter);
			newSegment.diameter(ntwkPt[i].diameter);
			newSegment.year(ntwkPt[i].year);
			newSegment.points(pipePts);
			newSegment.length(pipeLength);
			newSegment.CRR_estim(ntwkPt[i].CRR_estim);
			newSegment.material(ntwkPt[i].material);
			newSegment.pipe_status(ntwkPt[i].pipe_status);
			newSegment.pipe_type(ntwkPt[i].pipe_type);
			newSegment.pipe_connections(pseudo_BR);
			newSegment.list_BR(pseudo_BR);
			newSegment.n_cnct(1);
			newSegment.raster_ID(-1);
			newSegment.GMraster_ID(-1);
			newSegment.n_breaks(pseudo_breaks);
			newSegment.connected2pump(pseudo_pump);

			//Save the object
			newSegment.SAVE_Pipe(file_str, pipe_saver);

			//Destroy the object
			delete pipe_segment;

			//Re-initialization and update of the variables
			++pipe_counter;
			point_counter = 1;
			pipeLength = 0.0;
			pipePts[0].clear();
			pipePts[1].clear();

			//Initialize first points of the next pipe segment
			//If same pipeline (same CCCID)
                        if(abs(ntwkPt[i].IDs[2] - ntwkPt[i+1].IDs[2]) < tol)
                        {
                                //Add point i as first point of the new segment
                                pipePts[0].push_back(ntwkPt[i].coord[0]);
                                pipePts[1].push_back(ntwkPt[i].coord[1]);
                                point_counter++;

				//Add related length
				pipeLength = sqrt(pow(ntwkPt[i].coord[0] - ntwkPt[i+1].coord[0], 2) +
					 pow(ntwkPt[i].coord[1] - ntwkPt[i+1].coord[1], 2));
                        }

			//Add point i+1 as the second point of the new segment
			pipePts[0].push_back(ntwkPt[i+1].coord[0]);
			pipePts[1].push_back(ntwkPt[i+1].coord[1]);
		}
	}


	//Return number of created pipes
	return pipe_counter;
}


/*###############################################################################################################################

loadTempPipeSegment : load temporary pipe segment objects and erase the the temporary files

###############################################################################################################################*/
std::vector<pipeSegment> loadTempPipeSegment(int n_pipes)
{
	//Declare basic variables
	int i, n;
	ifstream data_stream;


	//Reload all objects as a vector of objects
	std::vector<pipeSegment> networkSegment(n_pipes);


	//Reloading
	for(i=0 ; i<n_pipes ; i++)
	{
		char *file_str = new char[25];
        	n = sprintf(file_str, "temp/Pipe%d.dat", i);
        	networkSegment[i].READ_Pipe(file_str, data_stream);
		delete [] file_str;
	}

	
	//Erase temporary files
	for(i=0 ; i<n_pipes ; i++)
	{
		char *file_str = new char[25];
        	n = sprintf(file_str, "temp/Pipe%d.dat", i);
        	remove(file_str);
		delete [] file_str;
	}


	//Return results
	return networkSegment;
}


/*###############################################################################################################################

removeDoubleNtwkPts : remove double entries in the pipe segments objects (due to network discretization)

###############################################################################################################################*/
std::vector<pipeSegment> removeDoubleNtwkPts(std::vector<pipeSegment> networkSegment)
{
	//Initialize basic variables
	int i, j, k;
	int n_pipe = networkSegment.size();


	//Remove double entries in pipeline points
	float dist;


	for(i=0 ; i<n_pipe ; i++)
	{
		//Copy matrix of location points
		std::vector< std::vector<double> > copy_pts(2, std::vector<double>(networkSegment[i].n_pts()));
		copy_pts = networkSegment[i].points();

		//For each points, check if previous one is a double
		for(j=networkSegment[i].n_pts()-2 ; j>=0 ; j--)
		{
			dist = sqrt(pow(networkSegment[i].points()[0][j] - networkSegment[i].points()[0][j+1] , 2) +
				pow(networkSegment[i].points()[1][j] - networkSegment[i].points()[1][j+1] , 2));

			//If distance between the 2 points is less than 5cm, points are considered the same
			if(dist<0.05)
			{
				copy_pts[0].erase(copy_pts[0].begin()+j+1);
				copy_pts[1].erase(copy_pts[1].begin()+j+1);
			}
		}

		//Set the copy as the new matrix location points
		networkSegment[i].points(copy_pts);
		networkSegment[i].n_pts(copy_pts[0].size());
	}


	//Return resutls
	return networkSegment;
}


/*###############################################################################################################################

removeBadSegment : remove segments containing only one point

###############################################################################################################################*/
std::vector<pipeSegment> removeBadSegment(std::vector<pipeSegment> networkSegment)
{
	//Initialize basic variables
	int i, j, k;
	int n_pipe = networkSegment.size();

	for(i=n_pipe-1 ; i>=0 ; i--)
	{
		if(networkSegment[i].n_pts() < 2)
		{
			networkSegment.erase(networkSegment.begin()+i);
		}
	}

	//Return the results
	return networkSegment;
}


/*###############################################################################################################################

assignFinalPipeID : assign the final pipe ID

###############################################################################################################################*/
std::vector<pipeSegment> assignFinalPipeID(std::vector<pipeSegment> networkSegment)
{
	//Initialize basic variables
	int i, j, k;
	int n_pipe = networkSegment.size();

	for(i=0 ; i<n_pipe ; i++)
	{
		networkSegment[i].unik_ID(i);
	}

	//Return results
	return networkSegment;
}


/*###############################################################################################################################

createNetworkMap : create a blank raster map for the network as a base for the nearest neighbour algorithm

###############################################################################################################################*/
std::vector< std::vector<double> > createNetworkMap(std::vector<pipeSegment> networkSegment, float max_pipe_length)
{
	//Initialize basic variables
	int i, j, k;
	int n_pipe = networkSegment.size();
	float dist;
	double raster_size = max_pipe_length;


	//Get x and y max and min
	double x_min = 10000000000, x_max = 0, y_min = 1000000000, y_max = 0;
	for(i=0 ; i<n_pipe ; i++)
	{
		//Update x_min
		if(x_min>networkSegment[i].center_loc()[0])
		{
			memcpy(&x_min, &networkSegment[i].center_loc()[0], sizeof(double));
		}

		//Update x_max
		if(x_max<networkSegment[i].center_loc()[0])
		{
			memcpy(&x_max, &networkSegment[i].center_loc()[0], sizeof(double));
		}

		//Update y_min
		if(y_min>networkSegment[i].center_loc()[1])
		{
			memcpy(&y_min, &networkSegment[i].center_loc()[1], sizeof(double));
		}

		//Update y_max
		if(y_max<networkSegment[i].center_loc()[1])
		{
			memcpy(&y_max, &networkSegment[i].center_loc()[1], sizeof(double));
		}
	}


	//Adapt x_min & co
	x_min = x_min - max_pipe_length;
	x_max = x_max + max_pipe_length;
	y_min = y_min - max_pipe_length;
	y_max = y_max + max_pipe_length;


	//Create the blank raster map
	//Dimension on the x-axis
	int n_x = (x_max-x_min)/raster_size + 1;
	//Dimension on the y-axis
	int n_y = (y_max-y_min)/raster_size + 1;
	//Number of raster
	int n_r = n_x*n_y;

	//Initialize vector of coordinates
	std::vector< std::vector<double> > raster(n_r, std::vector<double>(2));


	//Create the coordinate for each raster
	for(i=0 ; i<n_r ; i++)
	{
		raster[i][0] = x_min+(i%n_x)*raster_size;
		raster[i][1] = y_min+((i/n_x)%n_y)*raster_size;
	}


	//Return the map
	return raster;
}


/*###############################################################################################################################

getNearestNeighbourRaster : determine nearest neighbour raster on the network map

###############################################################################################################################*/
std::vector< std::vector<int> > getNearestNeighbourRaster(std::vector< std::vector<double> > raster)
{
	//Initialize basic variables
	int i, j, k;
	int n_raster = raster.size();
	float dist;
	float spacing = raster[1][0] - raster[0][0];


	//Find and assign nearest neighbours (1 raster away, incl. diagonales)
	std::vector< std::vector<int> > nearest_neighbours (n_raster, std::vector<int>(0));
	float min_dist = spacing * sqrt(2.0) + spacing/50;

	for(i=0 ; i<n_raster-1 ; i++)
	{
		nearest_neighbours[i].push_back(i);

		for(j=i+1 ; j<n_raster ; j++)
		{
			dist = sqrt(pow(raster[i][0]-raster[j][0], 2) + pow(raster[i][1]-raster[j][1], 2));
			if(dist<min_dist)
			{
				nearest_neighbours[i].push_back(j);
				nearest_neighbours[j].push_back(i);
			}
		}
	}

	nearest_neighbours[n_raster-1].push_back(n_raster-1);


	//Return nearest neighbour raster ID
	return nearest_neighbours;
}


/*###############################################################################################################################

assignRasterID2Pipe : assign raster ID to pipe segment which have their centroid within it

###############################################################################################################################*/
std::vector<pipeSegment> assignRasterID2Pipe(std::vector<pipeSegment> networkSegment, std::vector< std::vector<double> > raster)
{
	//Initialize basic variables
	int i, j;
	int n_raster = raster.size();
	int n_pipe = networkSegment.size();
	float dist;
	float d_max = (raster[1][0] - raster[0][0])/2;

	//5. Look for the closest raster center to each pipeline geometric center
	for(i=0 ; i<n_pipe ; i++)
	{
		//Initialize variables
		float closest_dist = 10000000000.0;//Initialize large so will be updated on the first iteration
		dist = 0.0;
		int ID_closest = -1;

		for(j=0 ; j<n_raster ; j++)
		{
			//Compute distance between raster j and pipeline i
			dist = sqrt(pow(networkSegment[i].center_loc()[0] - raster[j][0], 2) + 
				pow(networkSegment[i].center_loc()[1] - raster[j][1], 2));

			//Update closest raster ID and closest distance is needed
			if(dist < closest_dist)
			{
				//Update the distance to the closest raster
				memcpy(&closest_dist, &dist, sizeof(float));

				//Update the ID of the closest raster
				memcpy(&ID_closest, &j, sizeof(int));
			}

			//Quit loop if closest dist is smaller than sqrt(c²)
			if(closest_dist<d_max)
			{
				break;
			}
		}

		//Assign raster to pipeline
		networkSegment[i].raster_ID(ID_closest);
	}


	//Return updated pipe segments
	return networkSegment;
}


/*###############################################################################################################################

assignPipeID2Raster : assign pipe ID to raster in which their centroid is located

###############################################################################################################################*/
std::vector< std::vector<int> > assignPipeID2Raster(std::vector<pipeSegment> networkSegment, std::vector< std::vector<double> > raster)
{
	//Initialize basic variables
	int i, j;
	int n_raster = raster.size();
	int n_pipe = networkSegment.size();


	//Update the object map with the pipe ID
	//Initilize vector
	std::vector< std::vector<int> > belong_pipe(n_raster);

	for(i=0 ; i<n_pipe ; i++)
	{
		//Push-back ID of the pipe
		belong_pipe[networkSegment[i].raster_ID()].push_back(i);
	}


	//Return collection of pipe ID
	return belong_pipe;
}


/*###############################################################################################################################

assignSubmainID2Raster : assign submain ID to raster in which their centroid is located

###############################################################################################################################*/
std::vector< std::vector<int> > assignSubmainID2Raster(std::vector<pipeSegment> networkSegment, std::vector< std::vector<double> > raster)
{
	//Initialize basic variables
	int i, j;
	int n_raster = raster.size();
	int n_pipe = networkSegment.size();


	//Update the object map with the pipe ID
	//Initilize vector
	std::vector< std::vector<int> > belong_submain(n_raster);

	for(i=0 ; i<n_pipe ; i++)
	{
		//If submain
		std::string str_pipetype(networkSegment[i].pipe_type());
		int is_sub = strncmp(str_pipetype.c_str(), "Submain", 5);

		if(is_sub == 0)
		{
			belong_submain[networkSegment[i].raster_ID()].push_back(i);
		}
	}


	//Return collection of pipe ID
	return belong_submain;
}


/*###############################################################################################################################

getPipeConnection : determine pipe connection in function of their node proximity

###############################################################################################################################*/
std::vector<pipeSegment> getPipeConnection(std::vector<pipeSegment> networkSegment, std::vector< std::vector<int> > nearest_neighbour, 
	std::vector< std::vector<int> > belong_pipe)
{
	//Initialize basic variables
	int i, j, k, l;
	int n_pipe = networkSegment.size();
	float dist;


	//Initialize connection collector
	std::vector< std::vector< std::vector<int> > > connect_collector(n_pipe, std::vector< std::vector<int> >(3));

	//For each pipeline
	for(i=0 ; i<n_pipe ; i++)
	{
		//For each point of the pipeline
		for(j=0 ; j<networkSegment[i].n_pts() ; j++)
		{
			//For each nearest neighbour rasters
			for(int ri=0 ; ri<nearest_neighbour[networkSegment[i].raster_ID()].size() ; ri++)
			{
				int ind_near_neighbour = nearest_neighbour[networkSegment[i].raster_ID()][ri];
				int n_pipe_raster = belong_pipe[ind_near_neighbour].size();

				//Check if one of the pipe segments is connected with pipe segment i
				for(k=0 ; k<n_pipe_raster ; k++)
				{
					int ind_pipe_k = belong_pipe[ind_near_neighbour][k];

					//For every point of the tested pipeline
					for(l=0 ; l<networkSegment[ind_pipe_k].n_pts() ; l++)
					{
						if(ind_pipe_k != i)
						{
							//Compute the distance between the 2 selected points
							dist = sqrt(pow(networkSegment[i].points()[0][j] - networkSegment[ind_pipe_k].points()
								[0][l] , 2) + pow(networkSegment[i].points()[1][j] - networkSegment
								[ind_pipe_k].points()[1][l] , 2));

							//If the distance between the 2 points are less than 5cm, the 2 pipelines are
								//considered connected
							if(dist<0.05)
							{
								//Add the connection to both pipelines
								//First
								//Point ID of the pipeline
								connect_collector[i][0].push_back(j);
								//Unik ID of the connected pipeline
								connect_collector[i][1].push_back(ind_pipe_k);
								//Point ID of the connected pipeline
								connect_collector[i][2].push_back(l);
							}
						}
					}
				}
			}
		}

		//Remove potential duplicates
		for(j=connect_collector[i][0].size() ; j>=1 ; j--)
		{
			for(k=j-1 ; k>=0 ; k--)
			{
				if(connect_collector[i][0][j] == connect_collector[i][0][k] && 
					connect_collector[i][1][j] == connect_collector[i][1][k] && 
					connect_collector[i][2][j] == connect_collector[i][2][k])
				{
					connect_collector[i][0].erase(connect_collector[i][0].begin()+j);
					connect_collector[i][1].erase(connect_collector[i][1].begin()+j);
					connect_collector[i][2].erase(connect_collector[i][2].begin()+j);
					break;
				}
			}
		}
	}


	//Update the connections and save object
	for(i=0 ; i<n_pipe ; i++)
	{
		//Set the connections and their number
		networkSegment[i].pipe_connections(connect_collector[i]);
		networkSegment[i].n_cnct(connect_collector[i][0].size());
	}


	//Return updated objects
	return networkSegment;
}


/*###############################################################################################################################

assignFailureRateFx : assign failure rate function to each pipelines using the available functions

###############################################################################################################################*/
std::vector<pipeSegment> assignFailureRateFx(std::vector<pipeSegment> networkSegment)
{
	//Initialize basic variables
	int i;
	int n_pipe = networkSegment.size();


	//Assign FR fx
	for(i=0 ; i<n_pipe ; i++)
	{
		//Initialize collection vector
		std::vector< std::vector<int> > BR_vector;

		//Set the available BR functions
		BR_vector = BR_fx_assign(networkSegment[i]);
		networkSegment[i].list_BR(BR_vector);
	}

	
	//Return updated pipe segments
	return networkSegment;
}


/*###############################################################################################################################

importPumpData: Import pump data from csv file

###############################################################################################################################*/
std::vector<pumpStation> importPumpData(char *origin_pump_path)
{
	//Declare basic variables
	int i;
	int row_count = Row_counter(origin_pump_path);


	//Create the networPoints structure
	std::vector<pumpSt> PpStation(row_count);
	string line_buffer;
	int arg_counter;
	char *tk_bf;


	//Initiate data stream
	ifstream data_stream;
	data_stream.open(origin_pump_path);


	//Import each point of the discretized network as a structure
	for(i=0 ; i<row_count ; i++)
	{
		arg_counter = 0;
		getline(data_stream, line_buffer);
		std::vector<char> v(line_buffer.length() + 1);
		strcpy(&v[0], line_buffer.c_str());
		char *line_buffer_char = &v[0];
		char *token_buffer = strtok(line_buffer_char, ";,");
		++arg_counter;
		while(arg_counter <= 12)
		{
			switch(arg_counter)
			{
				case 1 :
					PpStation[i].center_loc.push_back(atof(token_buffer));
					break;

				case 2 : 
					PpStation[i].center_loc.push_back(atof(token_buffer));
					break;

				case 3 : 
					PpStation[i].ID = atoi(token_buffer); 
					break;

				case 4 : 
					PpStation[i].service_status = token_buffer;
					break;

				case 5 :
					PpStation[i].function = token_buffer;
					break;

				case 6 : 
                                        PpStation[i].bckpower = atoi(token_buffer); 
                                        break;

				case 7 :
					PpStation[i].capacity = atoi(token_buffer); 
                                        break;

				case 8 : 
                                        PpStation[i].n_pumps = atoi(token_buffer); 
                                        break;

				case 9 : 
                                        PpStation[i].n_wells = atoi(token_buffer); 
                                        break;

				case 10 :
					PpStation[i].n_reservoirs = atoi(token_buffer); 
                                        break;

				case 11 : 
                                        PpStation[i].LRI = atoi(token_buffer);
					switch(PpStation[i].LRI)
					{
						case 0:
							PpStation[i].CRR_estim = 0.032;
							break;

						case 1:
							PpStation[i].CRR_estim = 0.065;
							break;

						case 2:
							PpStation[i].CRR_estim = 0.13;
							break;

						case 3:
							PpStation[i].CRR_estim = 0.195;
							break;

						case 4:
							PpStation[i].CRR_estim = 0.26;
							break;

						case -1:
							PpStation[i].CRR_estim = 0.265;
							break;

						case -2:
							PpStation[i].CRR_estim = -1.0;
							break;

						default:
							PpStation[i].CRR_estim = -2.0;
							break;

					}
                                        break;

				case 12 :
					PpStation[i].restoration_date = atoi(token_buffer);
					break;

			}
			++arg_counter;
			token_buffer = strtok(NULL, ";,");
		}
	}

	//Close data stream
	data_stream.close();


	//Creation of the objects
	std::vector<pumpStation> station(row_count);

	//Initialize pseudo-list of fragility functions and los
	std::vector< std::vector<int> > frag_fx {{-1}};
	std::vector<int> pseudo_pipe_in {{-1}};
	std::vector<int> pseudo_pipe_out {{-1}};


	//Populate the created objects
	for(i=0 ; i<row_count ; i++)
	{
		station[i].n_pumps(PpStation[i].n_pumps);
		station[i].n_wells(PpStation[i].n_wells);
		station[i].n_reservoirs(PpStation[i].n_reservoirs);
		//If no restoration needed, assign los = 1
		if(PpStation[i].restoration_date < 1)
		{
			std::vector<float> los (1, 1.0);
			station[i].los(los);
		}
		//Else, assign 0
		else
		{
			std::vector<float> los (1, 0.0);
			station[i].los(los);
		}
		station[i].raster_ID(-1);
		station[i].pipe_in_ID(pseudo_pipe_in);
		station[i].pipe_out_ID(pseudo_pipe_out);
		station[i].db_ID(PpStation[i].ID);
		station[i].unik_ID(i);
		station[i].capacity(PpStation[i].capacity);
		station[i].year(-1);
		station[i].CRR_estim(PpStation[i].CRR_estim);
		station[i].list_FF(frag_fx);
		station[i].center_loc(PpStation[i].center_loc);
		station[i].struct_type("Unknown");
		station[i].found_type("Unknown");
		station[i].generator(PpStation[i].bckpower);
		station[i].anchored(1);
		station[i].restoration_date(PpStation[i].restoration_date);
	}


	//Return pumping station objects
	return station;
}


/*###############################################################################################################################

assignRasterID2Pump : assign raster ID to pipe segment which have their centre within it

###############################################################################################################################*/
std::vector<pumpStation> assignRasterID2Pump(std::vector<pumpStation> station, std::vector< std::vector<double> > raster)
{
	//Initialize basic variables
	int i, j;
	int n_st = station.size();
	int n_raster = raster.size();
	float dist;
	float d_max = sqrt(pow((raster[0][0] - raster[1][0])/2, 2)*2);


	//Assign raster to stations
	for(i=0 ; i<n_st ; i++)
	{
		//Initialize variables
		float closest_dist = 10000000000.0;//Initialize large so will be updated on the first iteration
		dist = 0.0;
		int ID_closest = -1;

		for(j=0 ; j<n_raster ; j++)
		{
			//Compute distance between raster j and pipeline i
			dist = sqrt(pow(station[i].center_loc()[0] - raster[j][0], 2) + 
				pow(station[i].center_loc()[1] - raster[j][1], 2));

			//Update closest raster ID and closest distance is needed
			if(dist < closest_dist)
			{
				//Update the distance to the closest raster
				memcpy(&closest_dist, &dist, sizeof(float));

				//Update the ID of the closest raster
				memcpy(&ID_closest, &j, sizeof(int));
			}

			//Quit loop if closest dist is smaller than sqrt(c²)
			if(closest_dist<d_max)
			{
				break;
			}
		}

		//Assign raster to building
		station[i].raster_ID(ID_closest);
	}


	//Return updated pipeSegment
	return station;
}



/*###############################################################################################################################

getPumpConnection : determine to which pipeline the pumping stations are connected to

###############################################################################################################################*/
std::vector<pumpStation> getPumpConnection(std::vector<pumpStation> station, std::vector<pipeSegment> networkSegment, 
	std::vector< std::vector<int> > nearest_neighbour, std::vector< std::vector<int> > belong_pipe)
{
	//Initialize basic variables
	int i, j, k, l;
	int n_st = station.size();
	float dist;


	//For each pumping station
	for(i=0 ; i<n_st ; i++)
	{
		//Initialize vector of connected pipelines
		std::vector<int> pipe2pump;

		//For each nearest neighbour rasters
		for(int ri=0 ; ri<nearest_neighbour[station[i].raster_ID()].size() ; ri++)
		{
			int ind_near_neighbour = nearest_neighbour[station[i].raster_ID()][ri];

			//Check if one of the pipe segments is connected with pipe segment i
			for(k=0 ; k<belong_pipe[ind_near_neighbour].size() ; k++)
			{
				int ind_pipe_k = belong_pipe[ind_near_neighbour][k];

				//For every point of the tested pipeline
				for(l=0 ; l<networkSegment[ind_pipe_k].n_pts() ; l++)
				{
					//Compute the distance between the 2 selected points
					dist = sqrt(pow(station[i].center_loc()[0] - networkSegment[ind_pipe_k].points()[0][l] , 2) +
							 pow(station[i].center_loc()[1] - networkSegment[ind_pipe_k].points()[1][l] , 2));

					//If the distance between the 2 points are less than 10cm, pipe k is assumed connected with station i
					if(dist<0.1)
					{
						//Add the connection to the station
						pipe2pump.push_back(ind_pipe_k);
					}
				}
			}
		}


		//Remove duplicates
		pipe2pump = removeDuplicates(pipe2pump);

		//Assign to object
		station[i].pipe_out_ID(pipe2pump);

		//Clear vector
		pipe2pump.clear();
	}


	//Return updated pipeSegment
	return station;
}


/*###############################################################################################################################

assignCRR: Assign the CRR to the pipe segments

###############################################################################################################################*/
std::vector<pipeSegment> assignCRR(std::vector<pipeSegment> networkSegment, char *soil_name)
{
	//Declare basic variables
	int i, j;
	int n_pipe = networkSegment.size();
	int n_raster;
	float dist, d_max;
	ifstream infile;

	//Construct full path of soil map
	char *soil_path = new char[200];
	i = sprintf(soil_path, "./gen/CRR/%s/Soil.dat", soil_name);

	//Load soil map
	soilMap CRRMap;
	CRRMap.READ_Map(soil_path, infile);
	d_max = float(CRRMap.spacing())/2;
	n_raster = CRRMap.n_rasters();

	//For each pipe segment, check if inside a raster
	for(i=0 ; i<n_pipe ; i++)
	{
		//Initialize variables
		float closest_dist = 10000000000.0;//Initialize large so will be updated on the first iteration
		dist = 0.0;
		int ID_closest = -1;

		for(j=0 ; j<n_raster ; j++)
		{
			//Compute distance between raster j and pipeline i
			dist = sqrt(pow(networkSegment[i].center_loc()[0] - CRRMap.raster()[j][0], 2) + 
				pow(networkSegment[i].center_loc()[1] - CRRMap.raster()[j][1], 2));

			//Update closest raster ID and closest distance is needed
			if(dist < closest_dist)
			{
				//Update the distance to the closest raster
				memcpy(&closest_dist, &dist, sizeof(float));

				//Update the ID of the closest raster
				memcpy(&ID_closest, &j, sizeof(int));
			}

			//Quit loop if closest dist is smaller than sqrt(c²)
			if(closest_dist<d_max)
			{
				break;
			}
		}

		//Assign the right CRR
		networkSegment[i].CRR_estim(CRRMap.CRR()[ID_closest]);
	}

	//Delete arrays
	delete [] soil_path;

	//Retrun pipe segment
	return networkSegment;
}


/*###############################################################################################################################

createPipeSegment: Create pipe segments objects containing FR fx and final geometric description

###############################################################################################################################*/
std::vector<pipeSegment> createPipeSegment(char *origin_network_path, char *soil_name, float max_pipe_length)
{
	//Import pipe data from csv
	std::vector<networkPoint> ntwkPt = importNetworkData(origin_network_path);

	//Create the raw pipe segments as temporary files
	int n_pipe = createRawPipeSegment(ntwkPt, max_pipe_length);

	//Load temporary files
	std::vector<pipeSegment> networkSegment = loadTempPipeSegment(n_pipe);

	//Remove double entries
	networkSegment = removeDoubleNtwkPts(networkSegment);

	//Remove pipe segment with only 1 point
	networkSegment = removeBadSegment(networkSegment);

	//Assign final pipe ID
	networkSegment = assignFinalPipeID(networkSegment);

	//Assign failure rate funcions to each pipeline
	networkSegment = assignFailureRateFx(networkSegment);

	//Assign CRR if required
	if(strcmp(soil_name, "None") != 0)
	{
		networkSegment = assignCRR(networkSegment, soil_name);
	}

	//Return pipe segment objects
	return networkSegment;
}


/*###############################################################################################################################

createPumpStation: Create pump stations objects 

###############################################################################################################################*/
std::vector<pumpStation> createPumpStation(char *origin_pump_path, std::vector< std::vector<double> > raster, 
	std::vector<pipeSegment> networkSegment, std::vector< std::vector<int> > nearest_neighbour, 
	std::vector< std::vector<int> > belong_pipe)
{
	//Import pipe data from csv
	std::vector<pumpStation> station = importPumpData(origin_pump_path);

	//Assign Raster ID to pump stations
	station = assignRasterID2Pump(station, raster);

	//Connection to pipeline
	station = getPumpConnection(station, networkSegment, nearest_neighbour, belong_pipe);

	//Return pipe segment objects
	return station;
}


/*###############################################################################################################################

removeNonConnectedPipes : remove pipes which aren't connected to any pump station

###############################################################################################################################*/
pipeNetwork removeNonConnectedPipes(pipeNetwork network, char *origin_pump_path)
{
	//Declare base variables
	int i, j;
	int count_removed = 0;
	int n_pumps = network.n_pumps();
	int n_pipes = network.n_pipes();

	//Use the pipe connection algorithm from MCS
	//Initiliaze one empty list of connected pipes
	std::vector<int> pipeList_pump (network.n_pipes(), 0);
	//For each pump
	for(i=0 ; i<n_pumps ; i++)
	{
		//For each pipeline connected to the pump, run connectivity algorithm
		for(j=0 ; j<network.pump(i)->pipe_out_ID().size() ; j++)
		{
			pipe_connect_new(pipeList_pump, &network, network.pump(i)->pipe_out_ID()[j], 0);
		}
	}

	//Remove pipes getting 0
	std::vector<pipeSegment> sgmt_cpy = network.pipe_collection();
	for(i=n_pipes-1 ; i>=0 ; i--)
	{
		if(pipeList_pump[i] < 1)
		{
			count_removed++;
			sgmt_cpy.erase(sgmt_cpy.begin()+i);
		}
	}

	//Update network (re-do the connectivity analysis)
	for(i=0 ; i<sgmt_cpy.size() ; i++)
	{
		sgmt_cpy[i].unik_ID(i);
	}

	//Assign ID to raster
	sgmt_cpy = assignRasterID2Pipe(sgmt_cpy, network.raster());

	//Assign pipe an submain ID to raster
	std::vector< std::vector<int> > belong_pipe;
	std::vector< std::vector<int> > belong_submain;
	belong_pipe = assignPipeID2Raster(sgmt_cpy, network.raster());
	belong_submain = assignSubmainID2Raster(sgmt_cpy, network.raster());

	//Get pipe connection
	sgmt_cpy = getPipeConnection(sgmt_cpy, network.nearest_neighbour(), belong_pipe);

	//Re-import pump stations
	std::vector<pumpStation> station;
	station = createPumpStation(origin_pump_path, network.raster(), sgmt_cpy, network.nearest_neighbour(), belong_pipe);

	//Update network object the the connectivity specific elements
	network.pipe_belong(belong_pipe);
	network.submain_belong(belong_submain);
	network.pump_collection(station);
	network.n_pumps(station.size());
	network.pipe_collection(sgmt_cpy);
	network.n_pipes(sgmt_cpy.size());

	//Return updated network
	printf("%d pipes have been removed!\n", count_removed);
	return network;
}


/*###############################################################################################################################

createNetwork : create a network object containing pipe and pump stations objects from csv files

###############################################################################################################################*/
void createNetwork(char *origin_ntwk_name, char *ntwk_name, char *soil_name, float max_pipe_length, unsigned ntwk_year, int prep_lvl)
{
	//Create result folder
	char *ntwk_folder = new char[100];
	int ptr_1 = sprintf(ntwk_folder, "./gen/networks/%s", ntwk_name);
	mkdir("./gen", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("./gen/networks", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir(ntwk_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	//ID number of the network
	unsigned ntwk_ID = 0;


	//Path of the files
	//Pipes
	char *origin_pipe_path = new char[100];
	ptr_1 = sprintf(origin_pipe_path, "./data/networks/%s/pipes.csv", origin_ntwk_name);
	//Pumping stations
	char *origin_pump_path = new char[100];
	ptr_1 = sprintf(origin_pump_path, "./data/networks/%s/pumps.csv", origin_ntwk_name);


	//Initialize empty network object
	pipeNetwork network;

	//Intiialize potentially unused values
	std::vector< std::vector<double> > emptyDoubleMatrix(1, std::vector<double>(1, 0));
	std::vector< std::vector<int> > emptyIntMatrix(1, std::vector<int>(1, 0));
	network.n_pumps(0);
	network.raster(emptyDoubleMatrix);
	network.nearest_neighbour(emptyIntMatrix);
	network.submain_belong(emptyIntMatrix);
	network.pipe_belong(emptyIntMatrix);


	//Create the pipe objects
	printf("\x1b[36mImport pipe segment data from: %s/pipes.csv\x1b[0m\n", origin_ntwk_name);
	std::vector<pipeSegment> networkSegment = createPipeSegment(origin_pipe_path, soil_name, max_pipe_length);


	//Pre-allocate varaibles for connectivity analysis
	printf("%d pipes imported\n", networkSegment.size());
	std::vector< std::vector<double> > raster;
	std::vector< std::vector<int> > nearest_neighbour;
	std::vector< std::vector<int> > belong_pipe;
	std::vector< std::vector<int> > belong_submain;
	std::vector<pumpStation> station;

	//Do the connectivity analysis if specified by the user
	if(prep_lvl == 2)
	{
		//Create blank raster map
		printf("Create a grid to reference the pipes\n");
		raster = createNetworkMap(networkSegment, max_pipe_length);


		//Get map raster nearest neighbours
		nearest_neighbour = getNearestNeighbourRaster(raster);


		//Assign raster ID to pipelines
		printf("Created map contains %d rasters\n", raster.size());
		printf("Assign raster ID to pipes\n");
		networkSegment = assignRasterID2Pipe(networkSegment, raster);


		//Assign pipe an submain ID to raster
		belong_pipe = assignPipeID2Raster(networkSegment, raster);
		belong_submain = assignSubmainID2Raster(networkSegment, raster);


		//Determine pipe connection
		printf("Raster ID assigned to pieps\n");
		printf("Determine pipe connectivity\n");
		networkSegment = getPipeConnection(networkSegment, nearest_neighbour, belong_pipe);


		//Create pumping station objects
		printf("Pipe connectivity determined\n");
		printf("\x1b[36mImport pipe segment data from: %s/pipes.csv\x1b[0m\n", origin_ntwk_name);
		station = createPumpStation(origin_pump_path, raster, networkSegment, nearest_neighbour, belong_pipe);


		//Update network object the the connectivity specific elements
		printf("%d pump stations imported\n", station.size());
		network.raster(raster);
		network.nearest_neighbour(nearest_neighbour);
		network.pipe_belong(belong_pipe);
		network.submain_belong(belong_submain);
		network.pump_collection(station);
		network.n_pumps(station.size());
	}

	//Assemble the network object
	network.network_ID(ntwk_ID);
	network.network_name(ntwk_name);
	network.status_year(ntwk_year);
	network.n_pipes(networkSegment.size());
	network.pipe_collection(networkSegment);
	network.prep_lvl(prep_lvl);

	//If prepared for connectivity analysis, remove non-connected pipes
	if(prep_lvl == 2)
	{
		printf("Removing non-connected pipes\n");
		network = removeNonConnectedPipes(network, origin_pump_path);
	}


	//Save the network object and create of the .csv file used in the post-processing
	printf("\x1b[32mExporting geometry for post-processing saving the network as %s/Network.dat\x1b[0m\n", ntwk_name);
	ofstream outfile;
	network.SAVE_Network(ntwk_folder, outfile);
	OriginalNetwork_csv(network, ntwk_folder);


	//Delete the char*
	delete [] origin_pipe_path, origin_pump_path, ntwk_folder;
}



