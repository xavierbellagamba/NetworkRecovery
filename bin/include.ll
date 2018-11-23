#include <stdio.h>
#include <map>
#include <cstdio>
#include <mkl.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <array>
#include <iostream>
#include <fstream>
#include <cmath>
#include <functional>
#include <algorithm>
#include <stdlib.h>
#include <sys/stat.h>
#include <typeinfo>
#include <vector>
#include <sys/time.h>
#include <exception>
#include <mpi.h>
#include <dirent.h>
#include "./functions/fileList.h"
#include "types.h"
#include "./functions/atomicClock.h"
#include "./functions/BinaryStream.h"
#include "./functions/import_CSV.h"
#include "./functions/Cholesky_sym_L.h"
#include "./classes/pipeSegment.h"
#include "./classes/pumpStation.h"
#include "./classes/singleScenarioEmpiricalGM.h"
#include "./classes/singleScenarioPhysicsGM.h"
#include "./classes/soilMap.h"
#include "./classes/pipeNetwork.h"
#include "./classes/buildingFootprint.h"
#include "./classes/buildingCommunity.h"
#include "./functions/BR_functions.h"
#include "./functions/BR_BB2017.h"
#include "./functions/GM_map_type.h"
#include "./functions/CreateGMMap.h"
#include "./functions/CreateSoilMap.h"
#include "./functions/pipe_connection.h"
#include "./functions/MCS.h"
#include "./functions/CreateNetwork.h"
#include "./functions/CreateCommunity.h"
#include "./functions/prepareMCS.h"
#include "./classes/pipeFailure.h"
#include "./classes/damageCollection.h"
#include "./functions/GA.h"
#include "./functions/historic.h"
#include "./functions/recovery.h"
#include "./functions/recoveryCmp.h"




#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
char* COLOR_CYAN = "\x1b[36m";
char* COLOR_RESET = "\x1b[0m";




