/*
Test if GM map is empirical or physics-based simulated
*/

int GM_map_type(char *map_name)
{
	//Try block to check if map is physics-based or empirical
	int GMM_type;

	//Folder path
	char *map_path = new char[100];
	int ptr = sprintf(map_path, "./gen/GM/%s", map_name);

	//Get the file name of the map
	std::vector<std::string> map_file = fileDir(map_path, "GM");

	//Extract extension
	std::string extension = map_file[0].substr(map_file[0].size()-3, 3);

	//If extension is egm
	if(extension.compare("egm") == 0)
	{
		//Is empirical
		GMM_type = 0;
	}
	//If extension is pgm
	else if(extension.compare("pgm") == 0)
	{
		//Is physics-based
		GMM_type = 1;
	}
	else
	{
		printf("No valid ground motion in the folder...\n");
	}

	delete [] map_path;

	return GMM_type;
}
