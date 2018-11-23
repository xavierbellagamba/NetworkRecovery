//*********************************************************************************
//Function returning a vector of string containing the file in a specific directory
//*********************************************************************************

//Requires: vector, dirent, string

std::vector<std::string> fileDir(char* folderName, char* fileBase)
{
	//Initialize variables
	std::vector<std::string> fileList;
	DIR *dirp;
	struct dirent *directory;

	//Open directory
	dirp = opendir(folderName);

	//Check if empty, if not check files
	if(dirp)
	{
		while((directory = readdir(dirp)) != NULL)
		{
			if(0 == strncmp(fileBase, directory->d_name, (int)strlen(fileBase)))
			{
				fileList.push_back(directory->d_name);
			}
		}

		//Close directory
		closedir(dirp);
	}

	//Return results
	return fileList;
}


//********************************
//Index of network-map matching
//********************************

//Requires: string, vector

int str_cmp_list_index(std::string file_name, std::vector<std::string> list)
{
	//Basic variables
	int i;

	//Length of the list
	int n = list.size();

	//Isolate uniqueness in the network name (catchment number)
	int pos_ = strcspn(file_name.c_str(), "_");
	std::string str_cmp = file_name.substr(pos_, file_name.size());

	//Length of the string
	int l = str_cmp.size();

	//Using str_find
	int ind = -1;
	for(i=0 ; i<n ; i++)
	{
		//If found a match, break instruction
		pos_ = strcspn(list[i].c_str(), "_");
		std::string sub_list = list[i].substr(pos_, list[i].size());

		//int cmp = strncmp(str_cmp.c_str(), sub_list.c_str(), l);
		int cmp = strcmp(str_cmp.c_str(), sub_list.c_str());
		if(cmp == 0)
 		{
			ind = i;
			break;
		}

	}

	//Return result
	return ind;
}



