
from matplotlib import pyplot as plt
from matplotlib import cm
import csv
import os
import glob
import numpy as np
import gc 
import statistics as st
from matplotlib.patches import Polygon
from matplotlib.collections import PatchCollection


##########################################################################################################

#writeCSV : write csv tables (has to be list of list)

##########################################################################################################
def writeCSV(list2w, filePath):
	with open(filePath, 'w') as f:
		writer = csv.writer(f)
		if type(list2w[0]) == list:
			writer.writerows(list2w)
		else:
			writer.writerow(list2w)
	f.close()


##########################################################################################################

#cm2inch : convert centimeters into invhes for plots

##########################################################################################################
def cm2inch(value):
	return value/2.54


##########################################################################################################

#getCoastlineGeometry : load the Christchurch Coastline geometry

##########################################################################################################
def getCoastlineGeometry():
	full_path_c = "./data/_postprocessing/Coastline.csv"

	Coast_poly = []

	with open(full_path_c) as csvfile:
		readCSV = csv.reader(csvfile)

		for row in readCSV:
			Coast_poly.append([row[0], row[1]])

	csvfile.close()

	return Coast_poly


##########################################################################################################

#getPortHillsGeometry : load the Christchurch PortHills geometry

##########################################################################################################
def getPortHillsGeometry():
	full_path_ph = "./data/_postprocessing/PortHill_Boundary.csv"

	PH_poly = []

	with open(full_path_ph) as csvfile:
		readCSV = csv.reader(csvfile)

		for row in readCSV:
			PH_poly.append([row[0], row[1]])

	csvfile.close()

	return PH_poly


##########################################################################################################

#getPipeGeometry : load the network geometry

##########################################################################################################
def getPipeGeometry(network_name):
	pipegeo_path = "./gen/networks/" + network_name + "/_post_network_pipegeometry.csv"

	points_loc = []

	with open(pipegeo_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')

		for row in readCSV:
			pipe_raw_loc = row[1:-1]
			pipe_pts_loc_E = []
			pipe_pts_loc_N = []

			for i in range(len(pipe_raw_loc)):
				if i%2 == 0: #Easting
					pipe_pts_loc_E.append(float(pipe_raw_loc[i]))

				else: #Northing
					pipe_pts_loc_N.append(float(pipe_raw_loc[i]))

			pipe_pts_loc = [pipe_pts_loc_E, pipe_pts_loc_N]
			points_loc.append(pipe_pts_loc)

	csvfile.close()

	return points_loc


##########################################################################################################

#getPumpGeometry : load the network geometry

##########################################################################################################
def getPumpGeometry(network_name):
	pumpgeo_path = "./gen/networks/" + network_name + "/_post_network_pump.csv"

	pump_loc = []
	pump_day = []

	with open(pumpgeo_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')

		for row in readCSV:
			pump_day.append(int(row[3]))
			pump_pts_loc_E = 0
			pump_pts_loc_N = 0
			b_loc = row[1:3]

			for i in range(len(b_loc)):
				if i == 0: #Easting
					pump_pts_loc_E = float(b_loc[i])
				else: #Northing
					pump_pts_loc_N = float(b_loc[i])

			pumpst_loc = [pump_pts_loc_E, pump_pts_loc_N]
			pump_loc.append(pumpst_loc)

	csvfile.close()

	return [pump_loc, pump_day]


##########################################################################################################

#getCommunityGeometry : load the community geometry

##########################################################################################################
def getCommunityGeometry(commgeo_name, axis_limit):
	commgeo_path = "./gen/communities/" + commgeo_name + "/_post_community_geometry.csv"

	build_area = []
	build_loc = []
	build_pop = []
	build_type = []
	build_util = []

	adapt_coeff = np.sqrt(max(axis_limit[1]-axis_limit[0], axis_limit[3]-axis_limit[2]))*(-0.03)+5.25

	with open(commgeo_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')

		for row in readCSV:
			build_pts_loc_E = 0
			build_pts_loc_N = 0
			build_area.append(adapt_coeff*np.sqrt(float(row[1])))
			build_pop.append(float(row[2]))
			build_type.append(row[3].strip(' '))
			build_util.append(float(row[4]))
			
			b_loc = [row[5], row[6]]

			for i in range(len(b_loc)):
				if i == 0: #Easting
					build_pts_loc_E = float(b_loc[i])
				else: #Northing
					build_pts_loc_N = float(b_loc[i])

			building_loc = [build_pts_loc_E, build_pts_loc_N]
			build_loc.append(building_loc)

	csvfile.close()

	return [build_loc, build_area, build_pop, build_type, build_util]


##########################################################################################################

#getBuildingLocation : extract location of buildings

##########################################################################################################
def getBuildingLocation(raw_res):
	n = []
	for i in range(len(raw_res[0])):
		n.append(raw_res[0][i])

	return n


##########################################################################################################

#getBuildingArea : extract vector of building area

##########################################################################################################
def getBuildingArea(raw_res):
	n = []
	for i in range(len(raw_res[1])):
		n.append(raw_res[1][i])

	return n


##########################################################################################################

#getBuildingPop : extract vector of building population

##########################################################################################################
def getBuildingPop(raw_res):
	n = []
	for i in range(len(raw_res[2])):
		n.append(raw_res[2][i])

	return n

##########################################################################################################

#getBuildingType : extract vector of building type

##########################################################################################################
def getBuildingType(raw_res):
	n = []
	for i in range(len(raw_res[3])):
		n.append(raw_res[3][i])

	return n

##########################################################################################################

#getBuildingUtility : extract vector of building utility

##########################################################################################################
def getBuildingUtility(raw_res):
	n = []
	for i in range(len(raw_res[4])):
		n.append(raw_res[4][i])

	return n

##########################################################################################################

#getFailureGeometry : load the location of the pipe failures

##########################################################################################################
def getFailureGeometry(failgeo_name):
	failgeo_path = "./data/failure_collection/" + failgeo_name + ".csv"

	failure_loc = []

	with open(failgeo_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=',')
    
		for row in readCSV:
			loc = []
			loc.append(float(row[0]))
			loc.append(float(row[1]))

			failure_loc.append(loc)

	csvfile.close()

	return failure_loc


##########################################################################################################

#getRawResultPipeFailureSingleSim : load the raw results for number of failures and failure per segment from 1 simulation

##########################################################################################################
def getRawResultPipeFailureSingleSim(sim_path, i_fx, pipe_nbr_break, i_sim):
	failure_path = sim_path + "/_post_network_pipefailure.csv"

	total_nbr_break = 0
	i = 0

	with open(failure_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')
    
		for row in readCSV:
			pipe_break = int(row[i_fx+1])
			total_nbr_break = total_nbr_break + pipe_break

			if i_sim == 0:
				if pipe_break > 0:
					pipe_nbr_break.append(1)
				else:
					pipe_nbr_break.append(0)
			else:
				if pipe_break > 0:
					pipe_nbr_break[i] = pipe_nbr_break[i] + 1
			i = i+1

	csvfile.close()

	return [total_nbr_break, pipe_nbr_break]


##########################################################################################################

#getRawResultPipeConnectSingleSim : load the raw results for pipe connectivity from 1 simulation

##########################################################################################################
def getRawResultPipeConnectSingleSim(sim_path, i_fx, pipe_connect, i_sim):
	pipeconnect_path = sim_path + "/_post_network_pipeconnection.csv"

	i = 0

	with open(pipeconnect_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')

		for row in readCSV:
			pipe_connected = int(row[i_fx+1])

			if i_sim == 0:
				if pipe_connected < 0:
					pipe_connect.append(1)
				else:
					pipe_connect.append(0)
			else:
				if pipe_connected < 0:
					pipe_connect[i] = pipe_connect[i] + 1
			i = i+1

	csvfile.close()

	return pipe_connect


##########################################################################################################

#getRawResultBuildingConnectSingleSim : load the raw results for building connectivity from 1 simulation

##########################################################################################################
def getRawResultBuildingConnectSingleSim(sim_path, i_fx, build_pop, build_util, build_type, build_connect, i_sim):
	buildingconnect_path = sim_path + "/_post_community_buildingconnection.csv"

	i = 0
	n_build_disconnect = 0
	n_people_disconnect = 0
	n_business = 0
	n_medical = 0
	n_critical = 0
	n_school = 0
	utility = 0

	with open(buildingconnect_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')

		for row in readCSV:
			build_connected = int(row[i_fx+1])

			if i_sim == 0:
				if build_connected > 0:
					build_connect.append(0)
				else:
					build_connect.append(1)
					n_build_disconnect = n_build_disconnect + 1
					n_people_disconnect = n_people_disconnect + build_pop[i]

					if build_type[i] == 'business':
						n_business = n_business + 1
					elif build_type[i] == 'medical':
						n_medical = n_medical + 1
					elif build_type[i] == 'critical':
						n_critical = n_critical + 1
					elif build_type[i] == 'school':
						n_school = n_school + 1

					utility = utility + build_util[i]
			else:
				if build_connected <= 0:
					build_connect[i] = build_connect[i] + 1
					n_build_disconnect = n_build_disconnect + 1
					n_people_disconnect = n_people_disconnect + build_pop[i]

					if build_type[i] == 'business':
						n_business = n_business + 1
					elif build_type[i] == 'medical':
						n_medical = n_medical + 1
					elif build_type[i] == 'critical':
						n_critical = n_critical + 1
					elif build_type[i] == 'school':
						n_school = n_school + 1

					utility = utility + build_util[i]

			i = i+1

	csvfile.close()
                
	return [build_connect, n_build_disconnect, n_people_disconnect, n_business, n_medical, n_critical, n_school, utility]


##########################################################################################################

#getRawResultPipeFailure : load the raw results for pipeline failrues from all simulations

##########################################################################################################
def getRawResultPipeFailure(root_sim_path, i_fx, str_folder):
	n_sim = len(glob.glob1(root_sim_path, str_folder))
	simul_name = glob.glob1(root_sim_path, str_folder)

	pipe_nbr_break = []
	sim_break = []

	for sim in range(n_sim):
		if 'day_' in str_folder:
			sim_path = root_sim_path + '/' + str_folder
		else:
			sim_path = root_sim_path + simul_name[sim]

		result_sim = getRawResultPipeFailureSingleSim(sim_path, i_fx, pipe_nbr_break, sim)
		pipe_nbr_break = result_sim[1]
		sim_break.append(result_sim[0])

	pipe_nbr_break[:] = [x/n_sim for x in pipe_nbr_break]

	return [sim_break, pipe_nbr_break]


##########################################################################################################

#getRawResultPipeNumberFailure : extract matrix of failure per pipe

##########################################################################################################
def getRawResultPipeNumberFailure(raw_res):
	n = []
	for i in range(len(raw_res)):
		n.append(raw_res[i][1])

	return n


##########################################################################################################

#getRawResultNumberFailure : extract vector of failures on the network

##########################################################################################################
def getRawResultNumberFailure(raw_res):
	n = []
	for i in range(len(raw_res)):
		n.append(raw_res[i][0])

	return n


##########################################################################################################

#getRawResultPipeConnectivity : load the raw results for pipeline connectivity from all simulations

##########################################################################################################
def getRawResultPipeConnectivity(root_sim_path, i_fx, str_folder):
	sgmt_connect = []

	n_sim = len(glob.glob1(root_sim_path, str_folder))
	simul_name = glob.glob1(root_sim_path, str_folder)

	for sim in range(n_sim):
		if 'day_' in str_folder:
			sim_path = root_sim_path + '/' + str_folder
		else:
			sim_path = root_sim_path + simul_name[sim]
		
		sgmt_connect = getRawResultPipeConnectSingleSim(sim_path, i_fx, sgmt_connect, sim)

	sgmt_connect[:] = [x/n_sim for x in sgmt_connect]

	return sgmt_connect


##########################################################################################################

#getRawResultBuildingConnectivity : load the raw results for building connectivity from all simulations

##########################################################################################################
def getRawResultBuildingConnectivity(root_sim_path, i_fx, str_folder, build_pop, build_util, build_type):
	build_connect = []
	n_build_disconnect = []
	n_people_disconnect = []
	n_business = []
	n_medical = []
	n_critical = []
	n_school = []
	utility = []

	n_sim = len(glob.glob1(root_sim_path, str_folder))
	simul_name = glob.glob1(root_sim_path, str_folder)

	for sim in range(n_sim):
		if 'day_' in str_folder:
			sim_path = root_sim_path + '/' + str_folder
		else:
			sim_path = root_sim_path + simul_name[sim]

		result_sim = getRawResultBuildingConnectSingleSim(sim_path, i_fx, build_pop, build_util, build_type, build_connect, sim)
		build_connect = result_sim[0]
		n_build_disconnect.append(result_sim[1])
		n_people_disconnect.append(result_sim[2])
		n_business.append(result_sim[3])
		n_medical.append(result_sim[4])
		n_critical.append(result_sim[5])
		n_school.append(result_sim[6])
		utility.append(result_sim[7])

	build_connect[:] = [x/n_sim for x in build_connect]

	return [build_connect, n_build_disconnect, n_people_disconnect, n_business, n_medical, n_critical, n_school, utility]


##########################################################################################################

#getResultNumberDisconnectBuilding : extract vector of number of disconncted buidlings

##########################################################################################################
def getResultNumberDisconnectBuilding(raw_res):
	n = []
	for i in range(len(raw_res)):
		n.append(raw_res[i][1])

	return n


##########################################################################################################

#getResultDisconnectBuilding : extract matrix of building connectivity

##########################################################################################################
def getResultDisconnectBuilding(raw_res):
	n = []
	for i in range(len(raw_res)):
		n.append(raw_res[i][0])

	return n


##########################################################################################################

#getResultDisconnectPeople : extract vector disconnected people

##########################################################################################################
def getResultDisconnectPeople(cnnt_build, pop):
	n = []
	for i in range(len(cnnt_build)):
		n_per_sim = 0
		for j in range(len(pop)):
			if cnnt_build[i][j] == 0:
				n_per_sim = n_per_sim + pop[j]

		n.append(n_per_sim)

	return n


##########################################################################################################

#evaluateProbaPipeFailure : evaluate the probability of failure for 1 scenario based on loaded simulations

##########################################################################################################
def evaluateProbaPipeFailure(pipe_failure):
	P_f = []
	n_pipe = len(pipe_failure[0])
	n_sim = len(pipe_failure)

	for i in range(n_pipe):
		P_f_pipe = 0

		for j in range(n_sim):
			if pipe_failure[j][i]>0:
				P_f_pipe = P_f_pipe + 1

		P_f.append(P_f_pipe/n_sim)

	return P_f


##########################################################################################################

#evaluateProbaPipeConnect : evaluate the probability of disconnection (pipe) for 1 scenario based on loaded simulations

##########################################################################################################
def evaluateProbaPipeConnect(pipe_connect):
	P_f_disc = []	
	n_pipe = len(pipe_connect[0])
	n_sim = len(pipe_connect)

	for i in range(n_pipe):
		P_f_disc_pipe = 0

		for j in range(n_sim):            
			P_f_disc_pipe = P_f_disc_pipe + pipe_connect[j][i]

		P_f_disc.append(1-(P_f_disc_pipe/n_sim))

	return P_f_disc


##########################################################################################################

#evaluateProbaBuildingConnect : evaluate the probability of disconnection (building) for 1 scenario based on loaded simulations

##########################################################################################################
def evaluateProbaBuildingConnect(building_connect):
	P_f_disc = []	
	n_pipe = len(building_connect[0])
	n_sim = len(building_connect)

	for i in range(n_pipe):
		P_f_disc_build = 0

		for j in range(n_sim):            
			P_f_disc_build = P_f_disc_build + building_connect[j][i]

		P_f_disc.append(1-(P_f_disc_build/n_sim))

	return P_f_disc


##########################################################################################################

#getNumberFragilityFunction : determine the number of fragility funcitons used to assess the pipe network

##########################################################################################################
def getNumberFragilityFunction(root_sim_path):
	simul_name = glob.glob1(root_sim_path,"sim_*")
	sim_path = root_sim_path + simul_name[0]

	failure_path = sim_path + "/_post_network_pipefailure.csv"

	with open(failure_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')
		for row in readCSV:
			n_fx = len(row)-1
			break

	csvfile.close()

	return n_fx


##########################################################################################################

#getAxisLimit : deterine the axis limits based on pipe geometry

##########################################################################################################
def getAxisLimit(network_name):
	pipegeo_path = "./gen/networks/" + network_name + "/_post_network_pipegeometry.csv"

	x_max_ = -1
	x_min_ = 1000000000
	y_max_ = -1
	y_min_ = 1000000000

	with open(pipegeo_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')
    
		points_loc = []
    
		for row in readCSV:
			pipe_raw_loc = row[1:-1]

			for i in range(len(pipe_raw_loc)):
				if i%2 == 0: #Easting
					if x_max_<float(pipe_raw_loc[i]):
						x_max_ = float(pipe_raw_loc[i])
					if x_min_>float(pipe_raw_loc[i]):
						x_min_ = float(pipe_raw_loc[i])
                    
				else: #Northing
					if y_max_<float(pipe_raw_loc[i]):
						y_max_ = float(pipe_raw_loc[i])
					if y_min_>float(pipe_raw_loc[i]):
						y_min_ = float(pipe_raw_loc[i])
     
	#Update x, y, max and min
	x_max = x_max_ + (x_max_-x_min_)*0.1
	x_min = x_min_ - (x_max_-x_min_)*0.1
	y_max = y_max_ + (y_max_-y_min_)*0.1
	y_min = y_min_ - (y_max_-y_min_)*0.1

	if x_min < 1554000:
		x_min = 1554000
	if x_max > 1585000:
		x_max = 1585000
	if y_min < 5174000:
		y_min = 5174000
	if y_max > 5191000:
		y_max = 5191000

	csvfile.close()

	return [x_min, x_max, y_min, y_max]


##########################################################################################################

#sturgesRule : compute the appropriate number of bins for an histogram using the Sturges rule

##########################################################################################################
def sturgesRule(n):
	n_bin_hist = int(np.ceil(np.log2(n))+1)

	return n_bin_hist


##########################################################################################################

#getStatisticsNumberFailure : return basic statistics for number of failures from 1 scenario

##########################################################################################################
def getStatisticsNumberFailure(n_failure):
	mean_f = np.mean(n_failure, dtype=np.float64)
	med_f = st.median(n_failure)
	std_f = np.std(n_failure, dtype=np.float64)

	return [mean_f, med_f, std_f]


##########################################################################################################

#getTopography : return patch collection representing the topography

##########################################################################################################
def getTopography():
	coastline = getCoastlineGeometry()
	portHills = getPortHillsGeometry()

	solid_shape = []
	solid_shape.append(Polygon(portHills, True))
	solid_shape.append(Polygon(coastline, True))
	p = PatchCollection(solid_shape, alpha=0.5)

	return p


##########################################################################################################

#getTopographyColor : return vector of color for topography

##########################################################################################################
def getTopographyColor():
	return ['#8b4513', '#deb887'] 


##########################################################################################################

#getDiscreteColorbar : return a colorbar object using user-defined parameters

##########################################################################################################
def getDiscreteColorbar(str_colormap_name, n_inter, v_max, ax):
	cmap = cm.get_cmap(str_colormap_name, n_inter)
	norm = plt.Normalize(vmin = 0, vmax = v_max)

	sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
	sm.set_array([]) 
	bounds = np.linspace(0, v_max, n_inter+1)

	return [sm, bounds]


##########################################################################################################

#plotFailureHistogram : plot the histogram of failure number for 1 scenario using 1 fragility function set

##########################################################################################################
def plotFailureHistogram(n_failure, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_failure)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_failure)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_failure, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_failure)-min(n_failure)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Number of pipe failures')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotDisconnectBuildingHistogram : plot the histogram of buildings for 1 scenario using 1 fragility function set

##########################################################################################################
def plotDisconnectBuildingHistogram(n_b, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_b)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_b)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_b, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_b)-min(n_b)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Number of disconnected buildings')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotDisconnectPeopleHistogram : plot the histogram of people for 1 scenario using 1 fragility function set

##########################################################################################################
def plotDisconnectPeopleHistogram(n_p, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_p)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_p)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_p, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_p)-min(n_p)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Number of disconnected people')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotDisconnectBusinessHistogram : plot the histogram of business buildings for 1 scenario using 1 fragility function set

##########################################################################################################
def plotDisconnectBusinessHistogram(n_p, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_p)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_p)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_p, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_p)-min(n_p)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Number of disconnected business buildings')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotDisconnectMedicalHistogram : plot the histogram of medical buildings for 1 scenario using 1 fragility function set

##########################################################################################################
def plotDisconnectMedicalHistogram(n_p, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_p)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_p)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_p, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_p)-min(n_p)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Number of disconnected medical buildings')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotDisconnectCriticalHistogram : plot the histogram of critical buildings for 1 scenario using 1 fragility function set

##########################################################################################################
def plotDisconnectCriticalHistogram(n_p, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_p)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_p)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_p, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_p)-min(n_p)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Number of disconnected critical buildings')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotDisconnectSchoolHistogram : plot the histogram of school buildings for 1 scenario using 1 fragility function set

##########################################################################################################
def plotDisconnectSchoolHistogram(n_p, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_p)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_p)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_p, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_p)-min(n_p)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Number of disconnected school buildings')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotDisconnectUtilityHistogram : plot the histogram of utility for 1 scenario using 1 fragility function set

##########################################################################################################
def plotDisconnectUtilityHistogram(n_p, ax=None):
	if ax is None:
		ax = plt.gca()

	n_sim = len(n_p)
	n_bin = sturgesRule(n_sim)

	[mean_f, med_f, std_f] = getStatisticsNumberFailure(n_p)
	str_text = "Mean: " + str(float("{0:.2f}".format(mean_f))) + "\nMedian: " + str(float("{0:.2f}".format(med_f))) + "\nStd: " + str(float("{0:.2f}".format(std_f)))

	y, x, _ = ax.hist(n_p, bins=n_bin)
	ax.grid(True)
	ax.text(0.8*(max(n_p)-min(n_p)), 0.8*y.max(), str_text, fontsize=11)
	ax.set_xlabel('Disconnected utility')
	ax.set_ylabel('Number of occurences')

	return ax


##########################################################################################################

#plotPipeProbaFailure : plot the network assets with their probability of failure

##########################################################################################################
def plotPipeProbaFailure(pipe_loc, P_f, plot_lim, ax):
	if ax is None:
		ax = plt.gca()

	topography = getTopography()
	topo_color = getTopographyColor()

	ax.add_collection(topography)
	topography.set_color(topo_color)

	[sm, bounds] = getDiscreteColorbar('jet', 9, max(P_f), ax)
		
	for i in range(len(pipe_loc)):
		im = ax.plot(pipe_loc[i][0][:], pipe_loc[i][1][:], color=sm.to_rgba(P_f[i]))

	ax.grid(True)
	ax.set_xlim(plot_lim[0], plot_lim[1])
	ax.set_ylim(plot_lim[2], plot_lim[3])
	ax.set_xlabel('NZTM2000 Easting, [m]')
	ax.set_ylabel('NZTM2000 Northing, [m]')

	cbar = plt.colorbar(sm, ax=ax)
	cbar.set_ticks(bounds, update_ticks=True)
	cbar.set_ticklabels(np.round(bounds, 2), update_ticks=True)
	cbar.ax.set_ylabel('Probability of failure', rotation=90)

	return ax


##########################################################################################################

#plotPipeProbaDisconnect : plot the network assets with their probability of disconnection

##########################################################################################################
def plotPipeProbaDisconnect(pipe_loc, pump_loc, P_disc, pump_day, plot_lim, ax):
	if ax is None:
		ax = plt.gca()

	topography = getTopography()
	topo_color = getTopographyColor()

	ax.add_collection(topography)
	topography.set_color(topo_color)

	[sm, bounds] = getDiscreteColorbar('jet', 9, max(P_disc), ax)
		
	for i in range(len(pipe_loc)):
		im = ax.plot(pipe_loc[i][0][:], pipe_loc[i][1][:], color=sm.to_rgba(P_disc[i]))

	for i in range(len(pump_loc)):
		im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', color='k', s=120)

	ax.grid(True)
	ax.set_xlim(plot_lim[0], plot_lim[1])
	ax.set_ylim(plot_lim[2], plot_lim[3])
	ax.set_xlabel('NZTM2000 Easting, [m]')
	ax.set_ylabel('NZTM2000 Northing, [m]')

	cbar = plt.colorbar(sm, ax=ax)
	cbar.set_ticks(bounds, update_ticks=True)
	cbar.set_ticklabels(np.round(bounds, 2), update_ticks=True)
	cbar.ax.set_ylabel('Probability of service reduction', rotation=90)

	return ax


##########################################################################################################

#plotBuildingProbaDisconnect : plot the network assets with their probability of disconnection

##########################################################################################################
def plotBuildingProbaDisconnect(pipe_loc, pump_loc, build_loc, build_area, P_disc, pump_day, plot_lim, ax):
	if ax is None:
		ax = plt.gca()

	topography = getTopography()
	topo_color = getTopographyColor()

	ax.add_collection(topography)
	topography.set_color(topo_color)

	[sm, bounds] = getDiscreteColorbar('jet', 9, max(P_disc), ax)
		
	for i in range(len(pipe_loc)):
		im = ax.plot(pipe_loc[i][0][:], pipe_loc[i][1][:], color='k')

	for i in range(len(pump_loc)):
		if pump_day[i] < 1:
			im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', color='k', s=120, zorder=2)
		else:
			im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', facecolors='none', edgecolors='k', s=120, zorder=2)

	for i in range(len(build_loc)):
		im = ax.scatter(build_loc[i][0], build_loc[i][1], s=build_area[i], color=sm.to_rgba(P_disc[i]))

	ax.grid(True)
	ax.set_xlim(plot_lim[0], plot_lim[1])
	ax.set_ylim(plot_lim[2], plot_lim[3])
	ax.set_xlabel('NZTM2000 Easting, [m]')
	ax.set_ylabel('NZTM2000 Northing, [m]')

	cbar = plt.colorbar(sm, ax=ax)
	cbar.set_ticks(bounds, update_ticks=True)
	cbar.set_ticklabels(np.round(bounds, 2), update_ticks=True)
	cbar.ax.set_ylabel('Probability of service reduction', rotation=90)

	return ax


##########################################################################################################

#plotSinglePredictiveScenario : save the required plots for a particular scenario

##########################################################################################################
def plotSinglePredictiveScenario(GM_name, network_name, community_name, lvl, plotResult, exportResult):
	#Level of analysis managed by lvl:
		#0: distribution of failure number
		#1: proba of failure of assets
		#2: proba of disconnection of assets
		#3: proba of disconnection of buildings

	#Initialize all potentially used variables
	pipe_loc = []
	pump_loc = []
	pump_day = []
	build_loc = []
	build_area = []
	build_pop = []
	build_type = []
	build_util = []
	n_failure = []
	P_f = []
	P_disc = []
	P_b_disc = []
	plot_lim = []

	#Change font
	plt.rcParams["font.family"] = "serif"

	#Determine simulation folder path and number of fragility functions used
	str_folder = 'sim_*'
	sim_root_folder = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/simulations/'
	n_fx = getNumberFragilityFunction(sim_root_folder)

	#Load required geometries
	if lvl >= 1:
		plot_lim = getAxisLimit(network_name)
		pipe_loc = getPipeGeometry(network_name)
	if lvl >= 2:
		pump_data = getPumpGeometry(network_name)
		pump_loc = pump_data[0]
		pump_day = pump_data[1]
	if lvl >= 3:
		build_loc = getBuildingLocation(getCommunityGeometry(community_name, plot_lim))
		build_area = getBuildingArea(getCommunityGeometry(community_name, plot_lim))
		build_pop = getBuildingPop(getCommunityGeometry(community_name, plot_lim))
		build_type = getBuildingType(getCommunityGeometry(community_name, plot_lim))
		build_util = getBuildingUtility(getCommunityGeometry(community_name, plot_lim))

	#For each fragility function
	for i in range(n_fx):
		#Failure number histogram
		if lvl >= 0:
			pipe_f_results = getRawResultPipeFailure(sim_root_folder, i, str_folder)
			if plotResult:
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_failure_dist_' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotFailureHistogram(pipe_f_results[0], ax=ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()
			if exportResult:
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_failure_data_' + str(i) + '.csv'
				writeCSV(pipe_f_results[0], csv_path)
			

		#Pipeline proba of failure
		if lvl >= 1:
			if plotResult:
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/failure_proba_' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotPipeProbaFailure(pipe_loc, pipe_f_results[1], plot_lim, ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()


		#Proba of pipe disconnection
		if lvl >= 2:
			if plotResult:
				P_disc = getRawResultPipeConnectivity(sim_root_folder, i, str_folder)
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/pipe_disconnect_proba' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotPipeProbaDisconnect(pipe_loc, pump_loc, P_disc, pump_day, plot_lim, ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()


		#Proba of building disconnection
		if lvl == 3:
			#GIS representation
			build_res = getRawResultBuildingConnectivity(sim_root_folder, i, str_folder, build_pop, build_util, build_type)
			if plotResult:
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/building_disconnect_proba' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotBuildingProbaDisconnect(pipe_loc, pump_loc, build_loc, build_area, build_res[0], pump_day, plot_lim, ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

				#Histogram of disconnected building number
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_building_disconnect' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotDisconnectBuildingHistogram(build_res[1], ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

				#Histogram of disconnected business building number
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_business_disconnect' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotDisconnectBusinessHistogram(build_res[3], ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

				#Histogram of disconnected medical building number
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_medical_disconnect' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotDisconnectMedicalHistogram(build_res[4], ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

				#Histogram of disconnected business building number
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_critical_disconnect' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotDisconnectCriticalHistogram(build_res[5], ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

				#Histogram of disconnected people
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_population_disconnect' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotDisconnectPeopleHistogram(build_res[2], ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

				#Histogram of disconnected people
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_school_disconnect' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotDisconnectPeopleHistogram(build_res[6], ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

				#Histogram of lost utility
				fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/utility_disconnect' + str(i) + '.pdf'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotDisconnectUtilityHistogram(build_res[7], ax1)
				fig.tight_layout()  
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()
				gc.collect()

			if exportResult:
				#GIS
				GIS_build_res = []
				for j in range(len(build_loc)):
					GIS_build_res.append([build_loc[j][0], build_loc[j][1], build_res[0][j]])
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/Pdisc_building_data_' + str(i) + '.csv'
				writeCSV(GIS_build_res, csv_path)

				#N buildings
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_building_data_' + str(i) + '.csv'
				writeCSV(build_res[1], csv_path)

				#N businesses
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_business_data_' + str(i) + '.csv'
				writeCSV(build_res[3], csv_path)

				#N medical
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_medical_data_' + str(i) + '.csv'
				writeCSV(build_res[4], csv_path)

				#N critical
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_critical_data_' + str(i) + '.csv'
				writeCSV(build_res[5], csv_path)

				#Population
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/population_data_' + str(i) + '.csv'
				writeCSV(build_res[2], csv_path)

				#Utility
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/utility_data_' + str(i) + '.csv'
				writeCSV(build_res[7], csv_path)

				#N schools
				csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_school_data_' + str(i) + '.csv'
				writeCSV(build_res[6], csv_path)
	return GIS_build_res


##########################################################################################################

#getMaxHistoricalRecoveryTime : get the maximum recovery time

##########################################################################################################
def getMaxHistoricalRecoveryTime(root_sim_path):
	n_day = 0

	n_sim = len(glob.glob1(root_sim_path,"sim_*"))
	simul_name = glob.glob1(root_sim_path,"sim_*")

	for i in range(n_sim):
		root_reco_path = root_sim_path + simul_name[i]

		if n_day < len(glob.glob1(root_reco_path,"day_*")):
			n_day = len(glob.glob1(root_reco_path,"day_*"))

	return n_day


##########################################################################################################

#getMaxRecoveryTime : get the maximum recovery time

##########################################################################################################
def getMaxRecoveryTime(root_sim_path):
	n_day = 0

	n_sim = len(glob.glob1(root_sim_path,"sim_*"))
	simul_name = glob.glob1(root_sim_path,"sim_*")

	for i in range(n_sim):
		root_reco_path = root_sim_path + simul_name[i] + '/recovery/'

		if n_day < len(glob.glob1(root_reco_path,"day_*")):
			n_day = len(glob.glob1(root_reco_path,"day_*"))

	return n_day


##########################################################################################################

#getRawResultBuildingConnectSingleSimRecovery : load the raw results for building connectivity from 1 simulation

##########################################################################################################
def getRawResultBuildingConnectSingleSimRecovery(sim_path, i_fx, build_pop, build_util, build_type, build_connect, i_sim):
	buildingconnect_path = sim_path + "/_post_community_buildingconnection.csv"

	i = 0
	n_build_disconnect = 0
	n_people_disconnect = 0
	n_business = 0
	n_medical = 0
	n_critical = 0
	n_school = 0
	utility = 0

	with open(buildingconnect_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')

		for row in readCSV:
			build_connected = int(row[i_fx+1])

			if i_sim == 0:
				if build_connected > 0:
					build_connect.append(0)
				else:
					build_connect.append(1)
					n_build_disconnect = n_build_disconnect + 1
					n_people_disconnect = n_people_disconnect + build_pop[i]

					if build_type[i] == 'business':
						n_business = n_business + 1
					elif build_type[i] == 'medical':
						n_medical = n_medical + 1
					elif build_type[i] == 'critical':
						n_critical = n_critical + 1
					elif build_type[i] == 'school':
						n_school = n_school + 1

					utility = utility + build_util[i]
			else:
				if build_connected <= 0:
					build_connect[i] = build_connect[i] + 1
					n_build_disconnect = n_build_disconnect + 1
					n_people_disconnect = n_people_disconnect + build_pop[i]

					if build_type[i] == 'business':
						n_business = n_business + 1
					elif build_type[i] == 'medical':
						n_medical = n_medical + 1
					elif build_type[i] == 'critical':
						n_critical = n_critical + 1
					elif build_type[i] == 'school':
						n_school = n_school + 1

					utility = utility + build_util[i]
			i = i+1

	csvfile.close()
                
	return [build_connect, n_build_disconnect, n_people_disconnect, n_business, n_medical, n_critical, n_school, utility]


##########################################################################################################

#getBuildingHistoricalRecoveryRawResults : get the raw results of building recovery

##########################################################################################################
def getBuildingHistoricalRecoveryRawResults(root_sim_path, build_pop, build_util, build_type, n_day):
	n_sim = len(glob.glob1(root_sim_path,"sim_*"))
	simul_name = glob.glob1(root_sim_path,"sim_*")

	build_reco = []
	n_build_reco = []
	n_people_reco = []
	n_people_med = []
	n_people_mean = []
	n_people_std = []
	n_business_med = []
	n_business_mean = []
	n_business_std = []
	n_medical_med = []
	n_medical_mean = []
	n_medical_std = []
	n_critical_med = []
	n_critical_mean = []
	n_critical_std = []
	n_school_med = []
	n_school_mean = []
	n_school_std = []
	utility_med = []
	utility_mean = []
	utility_std = []
	n_build_med = []
	n_build_mean = []
	n_build_std = []

	#For over day
	for i in range(n_day):
		#Initialize n_people_day and n_build_day
		n_build_day = []
		n_people_day = []
		n_business_day = []
		n_medical_day = []
		n_critical_day = []
		n_school_day = []
		utility_day = []

		#For over simulation
		for j in range(n_sim):
			#Compile simulation - day name
			folder_str = root_sim_path + simul_name[j]

			#Check existence
			if os.path.exists(folder_str + '/day_' + str(i)):
				#Gather results
				result_1d1s = getRawResultBuildingConnectivity(folder_str, 0, 'day_' + str(i), build_pop, build_util, build_type)

				#Accumulate status of buildings
				if not build_reco:
					build_reco = result_1d1s[0]
				else:
					for k in range(len(build_reco)):
						build_reco[k] = build_reco[k] + result_1d1s[0][k]

				#Get number of people disconnected
				n_people_day.append(result_1d1s[1][0])

				#Get number of building disconnected
				n_build_day.append(result_1d1s[2][0])

				#Get number of business buildings disconnected
				n_business_day.append(result_1d1s[3][0])

				#Get number of medical buildings disconnected
				n_medical_day.append(result_1d1s[4][0])

				#Get number of critical buildings disconnected
				n_critical_day.append(result_1d1s[5][0])

				#Get number of critical buildings disconnected
				n_school_day.append(result_1d1s[6][0])

				#Get number of medical buildings disconnected
				utility_day.append(result_1d1s[7][0])

			else:
				#Number of people and buildings disconnected == 0
				n_people_day.append(0)
				n_build_day.append(0)
				n_business_day.append(0)
				n_medical_day.append(0)
				n_critical_day.append(0)
				n_school_day.append(0)
				utility_day.append(0)

		#Compute statistics for for day i of number of people and number of buildings
		stat_people = getStatisticsNumberFailure(n_people_day)
		stat_build = getStatisticsNumberFailure(n_build_day)
		stat_business = getStatisticsNumberFailure(n_business_day)
		stat_medical = getStatisticsNumberFailure(n_medical_day)
		stat_critical = getStatisticsNumberFailure(n_critical_day)
		stat_school = getStatisticsNumberFailure(n_school_day)
		stat_utility = getStatisticsNumberFailure(utility_day)
		
		#Append stats
		n_people_med.append(stat_people[1])
		n_people_mean.append(stat_people[0])
		n_people_std.append(stat_people[2])
		n_build_med.append(stat_build[1])
		n_build_mean.append(stat_build[0])
		n_build_std.append(stat_build[2])
		n_business_med.append(stat_business[1])
		n_business_mean.append(stat_business[0])
		n_business_std.append(stat_business[2])
		n_medical_med.append(stat_medical[1])
		n_medical_mean.append(stat_medical[0])
		n_medical_std.append(stat_medical[2])
		n_critical_med.append(stat_critical[1])
		n_critical_mean.append(stat_critical[0])
		n_critical_std.append(stat_critical[2])
		n_school_med.append(stat_school[1])
		n_school_mean.append(stat_school[0])
		n_school_std.append(stat_school[2])
		utility_med.append(stat_utility[1])
		utility_mean.append(stat_utility[0])
		utility_std.append(stat_utility[2])

	#Divide accumulated status of buildings by n_sim to get average disconnect time
	build_reco[:] = [x/n_sim for x in build_reco]

	#Build result list
	n_people_reco = [n_people_mean, n_people_med, n_people_std]
	n_build_reco = [n_build_mean, n_build_med, n_build_std]
	n_business_reco = [n_business_mean, n_business_med, n_business_std]
	n_medical_reco = [n_medical_mean, n_medical_med, n_medical_std]
	n_critical_reco = [n_critical_mean, n_critical_med, n_critical_std]
	n_school_reco = [n_school_mean, n_school_med, n_school_std]
	utility_reco = [utility_mean, utility_med, utility_std]

	return [build_reco, n_build_reco, n_people_reco, n_business_reco, n_medical_reco, n_critical_reco, n_school_reco, utility_reco]


##########################################################################################################

#getBuildingRecoveryRawResults : get the raw results of building recovery

##########################################################################################################
def getBuildingRecoveryRawResults(root_sim_path, build_pop, build_util, build_type, i_fx, n_day):
	n_sim = len(glob.glob1(root_sim_path,"sim_*"))
	simul_name = glob.glob1(root_sim_path,"sim_*")

	build_reco = []
	n_build_reco = []
	n_people_reco = []
	n_people_med = []
	n_people_mean = []
	n_people_std = []
	n_business_med = []
	n_business_mean = []
	n_business_std = []
	n_medical_med = []
	n_medical_mean = []
	n_medical_std = []
	n_critical_med = []
	n_critical_mean = []
	n_critical_std = []
	n_school_med = []
	n_school_mean = []
	n_school_std = []
	utility_med = []
	utility_mean = []
	utility_std = []
	n_build_med = []
	n_build_mean = []
	n_build_std = []

	#For over day
	for i in range(n_day):
		#Initialize n_people_day and n_build_day
		n_build_day = []
		n_people_day = []
		n_business_day = []
		n_medical_day = []
		n_critical_day = []
		n_school_day = []
		utility_day = []

		#For over simulation
		for j in range(n_sim):
			#Compile simulation - day name
			folder_str = root_sim_path + simul_name[j] + '/recovery'

			#Check exitence
			if os.path.exists(folder_str + '/day_' + str(i)):
				#Gather results
				result_1d1s = getRawResultBuildingConnectivity(folder_str, 0, 'day_' + str(i), build_pop, build_util, build_type)

				#Accumulate status of buildings
				if not build_reco:
					build_reco = result_1d1s[0]
				else:
					for k in range(len(build_reco)):
						build_reco[k] = build_reco[k] + result_1d1s[0][k]

				#Get number of people disconnected
				n_people_day.append(result_1d1s[1][0])

				#Get number of building disconnected
				n_build_day.append(result_1d1s[2][0])

				#Get number of business buildings disconnected
				n_business_day.append(result_1d1s[3][0])

				#Get number of medical buildings disconnected
				n_medical_day.append(result_1d1s[4][0])

				#Get number of critical buildings disconnected
				n_critical_day.append(result_1d1s[5][0])

				#Get number of critical buildings disconnected
				n_school_day.append(result_1d1s[6][0])

				#Get number of medical buildings disconnected
				utility_day.append(result_1d1s[7][0])

			else:
				#Number of people and buildings disconnected == 0
				n_people_day.append(0)
				n_build_day.append(0)
				n_business_day.append(0)
				n_medical_day.append(0)
				n_critical_day.append(0)
				n_school_day.append(0)
				utility_day.append(0)

		#Compute statistics for for day i of number of people and number of buildings
		stat_people = getStatisticsNumberFailure(n_people_day)
		stat_build = getStatisticsNumberFailure(n_build_day)
		stat_business = getStatisticsNumberFailure(n_business_day)
		stat_medical = getStatisticsNumberFailure(n_medical_day)
		stat_critical = getStatisticsNumberFailure(n_critical_day)
		stat_school = getStatisticsNumberFailure(n_school_day)
		stat_utility = getStatisticsNumberFailure(utility_day)
		
		#Append stats
		n_people_med.append(stat_people[1])
		n_people_mean.append(stat_people[0])
		n_people_std.append(stat_people[2])
		n_build_med.append(stat_build[1])
		n_build_mean.append(stat_build[0])
		n_build_std.append(stat_build[2])
		n_business_med.append(stat_business[1])
		n_business_mean.append(stat_business[0])
		n_business_std.append(stat_business[2])
		n_medical_med.append(stat_medical[1])
		n_medical_mean.append(stat_medical[0])
		n_medical_std.append(stat_medical[2])
		n_critical_med.append(stat_critical[1])
		n_critical_mean.append(stat_critical[0])
		n_critical_std.append(stat_critical[2])
		n_school_med.append(stat_school[1])
		n_school_mean.append(stat_school[0])
		n_school_std.append(stat_school[2])
		utility_med.append(stat_utility[1])
		utility_mean.append(stat_utility[0])
		utility_std.append(stat_utility[2])

	#Divide accumulated status of buildings by n_sim to get average disconnect time
	build_reco[:] = [x/n_sim for x in build_reco]

	#Build result list
	n_people_reco = [n_people_mean, n_people_med, n_people_std]
	n_build_reco = [n_build_mean, n_build_med, n_build_std]
	n_business_reco = [n_business_mean, n_business_med, n_business_std]
	n_medical_reco = [n_medical_mean, n_medical_med, n_medical_std]
	n_critical_reco = [n_critical_mean, n_critical_med, n_critical_std]
	n_school_reco = [n_school_mean, n_school_med, n_school_std]
	utility_reco = [utility_mean, utility_med, utility_std]

	return [build_reco, n_build_reco, n_people_reco, n_business_reco, n_medical_reco, n_critical_reco, n_school_reco, utility_reco]


##########################################################################################################

#getNumberBuildingRecoveryResults : get the results of building recovery

##########################################################################################################
def getNumberBuildingRecoveryResults(raw_res, n_day):
	mean = []
	std = []
	med = []

	for i in range(n_day):
		day = []

		for j in range(len(raw_res[0])):
			if i < len(raw_res[0][j]):
				day.append(raw_res[0][j][i])
			else:
				day.append(0)

		mean.append(np.mean(day, dtype=np.float64))
		med.append(st.median(day))
		std.append(np.std(day, dtype=np.float64))

	return [mean, med, std]


##########################################################################################################

#getNumberPeopleRecoveryResults : get the results of people recovery

##########################################################################################################
def getNumberPeopleRecoveryResults(raw_res, n_day):
	mean = []
	std = []
	med = []

	for i in range(n_day):
		day = []

		for j in range(len(raw_res[2])):
			if i < len(raw_res[2][j]):
				day.append(raw_res[2][j][i])
			else:
				day.append(0)

		mean.append(np.mean(day, dtype=np.float64))
		med.append(st.median(day))
		std.append(np.std(day, dtype=np.float64))

	return [mean, med, std]


##########################################################################################################

#getBuildingRecoveryResults : get the results of building recovery

##########################################################################################################
def getBuildingRecoveryResults(raw_res, n_day):
	mean = []

	n_sim = len(raw_res[1])

	#build
	for i in range(len(raw_res[1][0][0])):
		build = []

		#day
		for j in range(n_day):
			day = 0
			#sim
			for k in range(len(raw_res[1])):
				if j < len(raw_res[1][k]):
					day = day + (1-raw_res[1][k][j][i])

			build.append(day/n_sim)

		mean.append(sum(build))

	return mean


##########################################################################################################

#getHistoricalFailureRecovery : get the date of discovery and repair of historical failures

##########################################################################################################
def getHistoricalFailureRecovery(sim_root_folder):
	repair_path = sim_root_folder + "/_post_damage.csv"

	repair = []

	with open(repair_path, encoding = "ISO-8859-1") as csvfile:
		readCSV = csv.reader(csvfile, delimiter=';')

		for row in readCSV:
			status = []
			status.append(int(row[0]))
			status.append(int(row[1]))

			repair.append(status)
			
	csvfile.close()

	return repair


##########################################################################################################

#plotBuildingTimeDisconnect : plot the network assets with their probability of disconnection

##########################################################################################################
def plotBuildingTimeDisconnect(pipe_loc, pump_loc, build_loc, build_area, time_disconnect, plot_lim, ax=None):
	if ax is None:
		ax = plt.gca()

	topography = getTopography()
	topo_color = getTopographyColor()

	ax.add_collection(topography)
	topography.set_color(topo_color)

	[sm, bounds] = getDiscreteColorbar('jet', 9, max(time_disconnect), ax)
		
	for i in range(len(pipe_loc)):
		im = ax.plot(pipe_loc[i][0][:], pipe_loc[i][1][:], color='k')

	for i in range(len(pump_loc)):
		im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', color='k', s=120)

	for i in range(len(build_loc)):
		im = ax.scatter(build_loc[i][0], build_loc[i][1], s=build_area[i], color=sm.to_rgba(time_disconnect[i]))

	ax.grid(True)
	ax.set_xlim(plot_lim[0], plot_lim[1])
	ax.set_ylim(plot_lim[2], plot_lim[3])
	ax.set_xlabel('NZTM2000 Easting, [m]')
	ax.set_ylabel('NZTM2000 Northing, [m]')

	cbar = plt.colorbar(sm, ax=ax)
	cbar.set_ticks(bounds, update_ticks=True)
	cbar.set_ticklabels(np.round(bounds, 2), update_ticks=True)
	cbar.ax.set_ylabel('Mean recovery time [day]', rotation=90)

	return ax


##########################################################################################################

#plotBuildingHistoricalDisconnectStatus : plot the network assets with their functionality at time t

##########################################################################################################
def plotBuildingHistoricalDisconnectStatus(pipe_loc, pump_loc, build_loc, build_area, fail_loc, status, fail_reco, day, pump_day, plot_lim, ax=None):
	if ax is None:
		ax = plt.gca()

	topography = getTopography()
	topo_color = getTopographyColor()

	ax.add_collection(topography)
	topography.set_color(topo_color)

	col = ['b', '0.2']
		
	for i in range(len(pipe_loc)):
		im = ax.plot(pipe_loc[i][0][:], pipe_loc[i][1][:], color='k', zorder=3)

	for i in range(len(pump_loc)):
		if pump_day[i] < day:
			im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', color='k', s=120, zorder=2)
		else:
			im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', facecolors='none', edgecolors='k', s=120, zorder=2)
	#Replace by a if like the fail loc style
	for i in range(len(build_loc)):
		im = ax.scatter(build_loc[i][0], build_loc[i][1], s=build_area[i], color=col[int(status[i])], zorder=1)

	for i in range(len(fail_loc)):
		col_f = 'r'
		if (fail_reco[i][0]<=day) & (fail_reco[i][1]>day):
			col_f = 'y'
		elif (fail_reco[i][0]<day) & (fail_reco[i][1]<=day):
			col_f = 'g'
		im = ax.scatter(fail_loc[i][0], fail_loc[i][1], s=5, marker='s', color=col_f, zorder=4)

	ax.grid(True)

	ax.set_xlim(plot_lim[0], plot_lim[1])
	ax.set_ylim(plot_lim[2], plot_lim[3])
	ax.set_xlabel('NZTM2000 Easting, [m]')
	ax.set_ylabel('NZTM2000 Northing, [m]')

	str_text = 'Recovery day: ' + str(day)
	ax.text(0.70*plot_lim[1]+0.30*plot_lim[0], 0.95*plot_lim[3]+0.05*plot_lim[2], str_text, fontsize=14)

	return ax


##########################################################################################################

#plotBuildingDisconnectStatus : plot the network assets with their functionality at time t

##########################################################################################################
def plotBuildingDisconnectStatus(pipe_loc, pump_loc, build_loc, build_area, status, day, pump_day, plot_lim, ax=None):
	if ax is None:
		ax = plt.gca()

	topography = getTopography()
	topo_color = getTopographyColor()

	ax.add_collection(topography)
	topography.set_color(topo_color)

	col = ['b', '0.2']
		
	for i in range(len(pipe_loc)):
		im = ax.plot(pipe_loc[i][0][:], pipe_loc[i][1][:], color='k', zorder=3)

	for i in range(len(pump_loc)):
		if pump_day[i] < day:
			im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', color='k', s=120, zorder=2)
		else:
			im = ax.scatter(pump_loc[i][0], pump_loc[i][1], marker='^', facecolors='none', edgecolors='k', s=120, zorder=2)
	#Replace by a if like the fail loc style
	for i in range(len(build_loc)):
		im = ax.scatter(build_loc[i][0], build_loc[i][1], s=build_area[i], color=col[status[i]], zorder=1)

	ax.grid(True)

	ax.set_xlim(plot_lim[0], plot_lim[1])
	ax.set_ylim(plot_lim[2], plot_lim[3])
	ax.set_xlabel('NZTM2000 Easting, [m]')
	ax.set_ylabel('NZTM2000 Northing, [m]')

	str_text = 'Recovery day: ' + str(day)
	ax.text(0.70*plot_lim[1]+0.30*plot_lim[0], 0.95*plot_lim[3]+0.05*plot_lim[2], str_text, fontsize=14)

	return ax



##########################################################################################################

#plotNumberBuildingRecovery : plot the histogram of failure number for 1 scenario using 1 fragility function set

##########################################################################################################
def plotNumberBuildingRecovery(mean_t, std_t, n_day, ax=None):
	if ax is None:
		ax = plt.gca()

	mean_m_std = []
	mean_p_std = []

	for i in range(n_day):
		if (mean_t[i] - std_t[i]) < 0:
			mean_m_std.append(0)
		else:
			mean_m_std.append(mean_t[i] - std_t[i])
		if (mean_t[i] + std_t[i]) > max(mean_t):
			mean_p_std.append(max(mean_t))
		else:
			mean_p_std.append(mean_t[i] + std_t[i])

	mean_t.append(0)
	mean_m_std.append(0)
	mean_p_std.append(0)

	ax.plot(range(n_day+1), mean_t, linestyle='-', color='k', linewidth=1.5, label=r'$\mu$')
	ax.plot(range(n_day+1), mean_p_std, linestyle='--', color='grey', linewidth=1.5, label=r'$\mu +/- \sigma$')
	ax.plot(range(n_day+1), mean_m_std, linestyle='--', color='grey', linewidth=1.5)

	ax.legend(loc=1)

	ax.grid(True)
	ax.set_xlabel('Recovery time, [day]')
	ax.set_ylabel('Number of disconnected buildings')

	return ax


##########################################################################################################

#plotNumberPeopleRecovery : plot the histogram of failure number for 1 scenario using 1 fragility function set

##########################################################################################################
def plotNumberPeopleRecovery(mean_t, std_t, n_day, ax=None):
	if ax is None:
		ax = plt.gca()

	mean_m_std = []
	mean_p_std = []

	for i in range(n_day):
		if (mean_t[i] - std_t[i]) < 0:
			mean_m_std.append(0)
		else:
			mean_m_std.append(mean_t[i] - std_t[i])
		if (mean_t[i] + std_t[i]) > max(mean_t):
			mean_p_std.append(max(mean_t))
		else:
			mean_p_std.append(mean_t[i] + std_t[i])

	mean_t.append(0)
	mean_m_std.append(0)
	mean_p_std.append(0)

	ax.plot(range(n_day+1), mean_t, linestyle='-', color='k', linewidth=1.5, label=r'$\mu$')
	ax.plot(range(n_day+1), mean_p_std, linestyle='--', color='grey', linewidth=1.5, label=r'$\mu +/- \sigma$')
	ax.plot(range(n_day+1), mean_m_std, linestyle='--', color='grey', linewidth=1.5)

	ax.legend(loc=1)

	ax.grid(True)
	ax.set_xlabel('Recovery time, [day]')
	ax.set_ylabel('Number of disconnected people')

	return ax


##########################################################################################################

#plotNumberBusinessRecovery : plot the restoration curve (business buildings) for 1 scenario using 1 fragility function set

##########################################################################################################
def plotNumberBusinessRecovery(mean_t, std_t, n_day, ax=None):
	if ax is None:
		ax = plt.gca()

	mean_m_std = []
	mean_p_std = []

	for i in range(n_day):
		if (mean_t[i] - std_t[i]) < 0:
			mean_m_std.append(0)
		else:
			mean_m_std.append(mean_t[i] - std_t[i])
		if (mean_t[i] + std_t[i]) > max(mean_t):
			mean_p_std.append(max(mean_t))
		else:
			mean_p_std.append(mean_t[i] + std_t[i])

	mean_t.append(0)
	mean_m_std.append(0)
	mean_p_std.append(0)

	ax.plot(range(n_day+1), mean_t, linestyle='-', color='k', linewidth=1.5, label=r'$\mu$')
	ax.plot(range(n_day+1), mean_p_std, linestyle='--', color='grey', linewidth=1.5, label=r'$\mu +/- \sigma$')
	ax.plot(range(n_day+1), mean_m_std, linestyle='--', color='grey', linewidth=1.5)

	ax.legend(loc=1)

	ax.grid(True)
	ax.set_xlabel('Recovery time, [day]')
	ax.set_ylabel('Number of disconnected business buildings')

	return ax


##########################################################################################################

#plotNumberMedicalRecovery : plot the restoration curve (medical buildings) for 1 scenario using 1 fragility function set

##########################################################################################################
def plotNumberMedicalRecovery(mean_t, std_t, n_day, ax=None):
	if ax is None:
		ax = plt.gca()

	mean_m_std = []
	mean_p_std = []

	for i in range(n_day):
		if (mean_t[i] - std_t[i]) < 0:
			mean_m_std.append(0)
		else:
			mean_m_std.append(mean_t[i] - std_t[i])
		if (mean_t[i] + std_t[i]) > max(mean_t):
			mean_p_std.append(max(mean_t))
		else:
			mean_p_std.append(mean_t[i] + std_t[i])

	mean_t.append(0)
	mean_m_std.append(0)
	mean_p_std.append(0)

	ax.plot(range(n_day+1), mean_t, linestyle='-', color='k', linewidth=1.5, label=r'$\mu$')
	ax.plot(range(n_day+1), mean_p_std, linestyle='--', color='grey', linewidth=1.5, label=r'$\mu +/- \sigma$')
	ax.plot(range(n_day+1), mean_m_std, linestyle='--', color='grey', linewidth=1.5)

	ax.legend(loc=1)

	ax.grid(True)
	ax.set_xlabel('Recovery time, [day]')
	ax.set_ylabel('Number of disconnected medical buildings')

	return ax


##########################################################################################################

#plotNumberCriticalRecovery : plot the restoration curve (critical buildings) for 1 scenario using 1 fragility function set

##########################################################################################################
def plotNumberCriticalRecovery(mean_t, std_t, n_day, ax=None):
	if ax is None:
		ax = plt.gca()

	mean_m_std = []
	mean_p_std = []

	for i in range(n_day):
		if (mean_t[i] - std_t[i]) < 0:
			mean_m_std.append(0)
		else:
			mean_m_std.append(mean_t[i] - std_t[i])
		if (mean_t[i] + std_t[i]) > max(mean_t):
			mean_p_std.append(max(mean_t))
		else:
			mean_p_std.append(mean_t[i] + std_t[i])

	mean_t.append(0)
	mean_m_std.append(0)
	mean_p_std.append(0)

	ax.plot(range(n_day+1), mean_t, linestyle='-', color='k', linewidth=1.5, label=r'$\mu$')
	ax.plot(range(n_day+1), mean_p_std, linestyle='--', color='grey', linewidth=1.5, label=r'$\mu +/- \sigma$')
	ax.plot(range(n_day+1), mean_m_std, linestyle='--', color='grey', linewidth=1.5)

	ax.legend(loc=1)

	ax.grid(True)
	ax.set_xlabel('Recovery time, [day]')
	ax.set_ylabel('Number of disconnected critical buildings')

	return ax


##########################################################################################################

#plotNumberSchoolRecovery : plot the restoration curve (school buildings) for 1 scenario using 1 fragility function set

##########################################################################################################
def plotNumberSchoolRecovery(mean_t, std_t, n_day, ax=None):
	if ax is None:
		ax = plt.gca()

	mean_m_std = []
	mean_p_std = []

	for i in range(n_day):
		if (mean_t[i] - std_t[i]) < 0:
			mean_m_std.append(0)
		else:
			mean_m_std.append(mean_t[i] - std_t[i])
		if (mean_t[i] + std_t[i]) > max(mean_t):
			mean_p_std.append(max(mean_t))
		else:
			mean_p_std.append(mean_t[i] + std_t[i])

	mean_t.append(0)
	mean_m_std.append(0)
	mean_p_std.append(0)

	ax.plot(range(n_day+1), mean_t, linestyle='-', color='k', linewidth=1.5, label=r'$\mu$')
	ax.plot(range(n_day+1), mean_p_std, linestyle='--', color='grey', linewidth=1.5, label=r'$\mu +/- \sigma$')
	ax.plot(range(n_day+1), mean_m_std, linestyle='--', color='grey', linewidth=1.5)

	ax.legend(loc=1)

	ax.grid(True)
	ax.set_xlabel('Recovery time, [day]')
	ax.set_ylabel('Number of disconnected school buildings')

	return ax


##########################################################################################################

#plotUtilityRecovery : plot the restoration curve (utility) for 1 scenario using 1 fragility function set

##########################################################################################################
def plotUtilityRecovery(mean_t, std_t, n_day, ax=None):
	if ax is None:
		ax = plt.gca()

	mean_m_std = []
	mean_p_std = []

	for i in range(n_day):
		if (mean_t[i] - std_t[i]) < 0:
			mean_m_std.append(0)
		else:
			mean_m_std.append(mean_t[i] - std_t[i])
		if (mean_t[i] + std_t[i]) > max(mean_t):
			mean_p_std.append(max(mean_t))
		else:
			mean_p_std.append(mean_t[i] + std_t[i])

	mean_t.append(0)
	mean_m_std.append(0)
	mean_p_std.append(0)

	ax.plot(range(n_day+1), mean_t, linestyle='-', color='k', linewidth=1.5, label=r'$\mu$')
	ax.plot(range(n_day+1), mean_p_std, linestyle='--', color='grey', linewidth=1.5, label=r'$\mu +/- \sigma$')
	ax.plot(range(n_day+1), mean_m_std, linestyle='--', color='grey', linewidth=1.5)

	ax.legend(loc=1)

	ax.grid(True)
	ax.set_xlabel('Recovery time, [day]')
	ax.set_ylabel('Utility')

	return ax


##########################################################################################################

#plotSingleHistoricalRecovery : save the required plots for a particular observed recovery process

##########################################################################################################
def plotSingleHistoricalRecovery(recovery_name, network_name, community_name, failure_name, recovery_type=None, plotResults=False, extractResults=True, movieResults=False):
	#Change recovery type into standard one
	if recovery_type == None:
		recovery_type = 'simulations'

	#Initialize all potentially used variables
	pipe_loc = []
	pump_loc = []
	pump_day = []
	build_loc = []
	build_area = []
	build_pop = []
	build_type = []
	build_util = []
	plot_lim = []
	n_day = 0

	#Change font
	plt.rcParams["font.family"] = "serif"

	#Determine simulation folder path and number of fragility functions used
	str_folder = 'sim_*'
	sim_root_folder = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/' + recovery_type + '/'

	#Load required geometries
	plot_lim = getAxisLimit(network_name)
	pipe_loc = getPipeGeometry(network_name)
	pump_data = getPumpGeometry(network_name)
	pump_loc = pump_data[0]
	pump_day = pump_data[1]
	build_loc = getBuildingLocation(getCommunityGeometry(community_name, plot_lim))
	build_area = getBuildingArea(getCommunityGeometry(community_name, plot_lim))
	build_pop = getBuildingPop(getCommunityGeometry(community_name, plot_lim))
	build_type = getBuildingType(getCommunityGeometry(community_name, plot_lim))
	build_util = getBuildingUtility(getCommunityGeometry(community_name, plot_lim))
	fail_loc = getFailureGeometry(failure_name)

	n_day = 0
	recovery_result = []


	if recovery_type == 'simulations':
		n_day = getMaxHistoricalRecoveryTime(sim_root_folder)
		recovery_result = getBuildingHistoricalRecoveryRawResults(sim_root_folder, build_pop, build_util, build_type, n_day)
	else:
		n_day = getMaxRecoveryTime(sim_root_folder)
		recovery_result = getBuildingRecoveryRawResults(sim_root_folder, build_pop, build_util, build_type, 0, n_day)

	#Plot the results
	if plotResults:
		#Number of reconnected building
		#build_res = getNumberBuildingRecoveryResults(recovery_result, n_day)
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/n_reconnect_building_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBuildingRecovery(recovery_result[1][0], recovery_result[1][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()	

		#Number of reconnected people
		#people_res = getNumberPeopleRecoveryResults(recovery_result, n_day)
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/n_reconnect_people_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberPeopleRecovery(recovery_result[2][0], recovery_result[2][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected business buildings
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/n_reconnect_business_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[3][0], recovery_result[3][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected medical buildings
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/n_reconnect_medical_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberMedicalRecovery(recovery_result[4][0], recovery_result[4][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected critical buildings
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/n_reconnect_critical_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[5][0], recovery_result[5][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected school buildings
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/n_reconnect_school_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[6][0], recovery_result[6][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected utility
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/reconnect_utility_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[7][0], recovery_result[7][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Mean time of disconnection
		#time_disconnect = getBuildingRecoveryResults(recovery_result, n_day)
		fig_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/mean_recovery_time_' + recovery_type + '.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotBuildingTimeDisconnect(pipe_loc, pump_loc, build_loc, build_area, recovery_result[0], plot_lim, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()


	if extractResults:
		#GIS
		GIS_meanT = []
		for j in range(len(build_loc)):
			GIS_meanT.append([build_loc[j][0], build_loc[j][1], recovery_result[0][j]])
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/MeanDiscT_data_.csv'
		writeCSV(GIS_meanT, csv_path)

		#N buildings
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_building_data.csv'
		writeCSV(recovery_result[1], csv_path)

		#Population
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_population_data.csv'
		writeCSV(recovery_result[2], csv_path)

		#N businesses
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_business_data.csv'
		writeCSV(recovery_result[3], csv_path)

		#N schools
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_school_data.csv'
		writeCSV(recovery_result[6], csv_path)

		#N medical
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_medical_data.csv'
		writeCSV(recovery_result[4], csv_path)

		#N critical
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_critical_data.csv'
		writeCSV(recovery_result[5], csv_path)

		#Utility
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_utility_data.csv'
		writeCSV(recovery_result[7], csv_path)
		return recovery_result


	if movieResults:
		#Serviceability status
		if recovery_type == 'simulations':
			for i in range(len(recovery_result[1][0])):
				folder_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/dbdRecoSim/sim_' + str(i)
				os.makedirs(folder_path)

				sim_folder = sim_root_folder + 'sim_' + str(i) + '/'
				n_day = len(glob.glob1(sim_folder,"day_*"))
				fail_reco = getHistoricalFailureRecovery(sim_folder)

				for j in range(n_day):
					result_1d1s = getRawResultBuildingConnectivity(sim_folder, 0, 'day_' + str(j), build_pop)
					status = result_1d1s[0]
					fig_path = folder_path + '/day_' + str(j) +'.png'
					fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
					fig.set_size_inches(cm2inch(28), cm2inch(20))
					plotBuildingHistoricalDisconnectStatus(pipe_loc, pump_loc, build_loc, build_area, fail_loc, status, fail_reco, j, plot_lim, ax1)
					fig.tight_layout()
					fig.savefig(fig_path, dpi=600)
					fig.clf()
					plt.close()

				str_input = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/dbdRecoSim/sim_' + str(i) + '/day_%d.png'
				str_output = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/dbdRecoSim/sim_' + str(i) + '/reco.mov'
				echo = 'ffmpeg -f image2 -r 1 -i ' + str_input + ' -vf format=rgb32 -vcodec qtrle -pix_fmt rgb24 -y ' + str_output
				os.system(echo)

		elif recovery_type == 'GA' or recovery_type == 'random':
			for i in range(len(recovery_result[1][0])):
				folder_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/dbdReco_'+ recovery_type + '/sim_' + str(i)
				os.makedirs(folder_path)

				n_day = len(glob.glob1(folder_path,"day_*"))

				for j in range(n_day):
					result_1d1s = getRawResultBuildingConnectivity(folder_path, 0, 'day_' + str(j), build_pop)
					status = result_1d1s[0]
					fig_path = folder_path + '/day_' + str(j) +'.png'
					fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
					fig.set_size_inches(cm2inch(28), cm2inch(20))
					plotBuildingDisconnectStatus(pipe_loc, pump_loc, build_loc, build_area, status, j, pump_day, plot_lim, ax1)
					fig.tight_layout()
					fig.savefig(fig_path, dpi=600)
					fig.clf()
					plt.close()

				str_input = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/dbdReco_'+ recovery_type + '/sim_' + str(i) + '/day_%d.png'
				str_output = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/dbdReco_'+ recovery_type + '/sim_' + str(i) + '/reco.mov'
				echo = 'ffmpeg -f image2 -r 1 -i ' + str_input + ' -vf format=rgb32 -vcodec qtrle -pix_fmt rgb24 -y ' + str_output
				os.system(echo)
		


##########################################################################################################

#plotSingleRecovery : save the required plots for a particular predicted recovery process

##########################################################################################################
def plotSingleRecovery(GM_name, network_name, community_name, plotResults=False, extractResults=True, movieResults=False):
	#Initialize all potentially used variables
	pipe_loc = []
	pump_loc = []
	pump_day = []
	build_loc = []
	build_area = []
	build_pop = []
	build_type = []
	build_util = []
	plot_lim = []
	n_day = 0

	#Change font
	plt.rcParams["font.family"] = "serif"

	#Load required geometries
	plot_lim = getAxisLimit(network_name)
	pipe_loc = getPipeGeometry(network_name)
	pump_data = getPumpGeometry(network_name)
	pump_loc = pump_data[0]
	pump_day = pump_data[1]
	build_loc = getBuildingLocation(getCommunityGeometry(community_name, plot_lim))
	build_area = getBuildingArea(getCommunityGeometry(community_name, plot_lim))
	build_pop = getBuildingPop(getCommunityGeometry(community_name, plot_lim))
	build_type = getBuildingType(getCommunityGeometry(community_name, plot_lim))
	build_util = getBuildingUtility(getCommunityGeometry(community_name, plot_lim))

	#Determine simulation folder path and number of fragility functions used
	str_folder = 'sim_*'
	root_sim_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/simulations/'
	simul_name = glob.glob1(root_sim_path, str_folder)

	#Load recovery results
	n_day = getMaxRecoveryTime(root_sim_path)
	recovery_result = getBuildingRecoveryRawResults(root_sim_path, build_pop, build_util, build_type, 0, n_day)

	if plotResults:
		#Number of reconnected building
		fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_reconnect_building.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBuildingRecovery(recovery_result[1][0], recovery_result[1][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()	

		#Number of reconnected people
		fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_reconnect_people.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberPeopleRecovery(recovery_result[2][0], recovery_result[2][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected business buildings
		fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_reconnect_business.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[3][0], recovery_result[3][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected medical buildings
		fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_reconnect_medical.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberMedicalRecovery(recovery_result[4][0], recovery_result[4][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected critical buildings
		fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/n_reconnect_critical.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[5][0], recovery_result[5][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected school buildings
		fig_path = './gen/MCS/historical/' + network_name + '/' + GM_name + '/n_reconnect_school.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[6][0], recovery_result[6][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Number of reconnected utility
		fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/utility_reconnect_people.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotNumberBusinessRecovery(recovery_result[7][0], recovery_result[7][2], n_day, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()

		#Mean time of disconnection
		fig_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/mean_recovery_time.pdf'
		fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
		fig.set_size_inches(cm2inch(28), cm2inch(20))
		plotBuildingTimeDisconnect(pipe_loc, pump_loc, build_loc, build_area, recovery_result[0], plot_lim, ax1)
		fig.tight_layout()  
		fig.savefig(fig_path, dpi=600)
		fig.clf()
		plt.close()


	if extractResults:
		#GIS
		GIS_meanT = []
		for j in range(len(build_loc)):
			GIS_meanT.append([build_loc[j][0], build_loc[j][1], recovery_result[0][j]])
		csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/MeanDiscT_data_.csv'
		writeCSV(GIS_meanT, csv_path)

		#N buildings
		csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/recoCurve_building_data.csv'
		writeCSV(recovery_result[1], csv_path)

		#Population
		csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/recoCurve_population_data.csv'
		writeCSV(recovery_result[2], csv_path)

		#N businesses
		csv_path = './gen/MCS/historical/' + network_name + '/' + recovery_name + '/recoCurve_business_data.csv'
		writeCSV(recovery_result[3], csv_path)

		#N schools
		csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/recoCurve_school_data.csv'
		writeCSV(recovery_result[6], csv_path)

		#N medical
		csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/recoCurve_medical_data.csv'
		writeCSV(recovery_result[4], csv_path)

		#N critical
		csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/recoCurve_critical_data.csv'
		writeCSV(recovery_result[5], csv_path)

		#Utility
		csv_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/recoCurve_utility_data.csv'
		writeCSV(recovery_result[7], csv_path)


	if movieResults:
		#Serviceability status
		for i in range(len(recovery_result[1][0])):
			folder_path = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/dbdReco/sim_' + str(i)
			os.makedirs(folder_path)

			sim_folder = root_sim_path + 'sim_' + str(i) + '/recovery/'
			n_day = len(glob.glob1(sim_folder,"day_*"))

			for j in range(0, len(recovery_result[1][i]), 1):
				result_1d1s = getRawResultBuildingConnectivity(sim_folder, 0, 'day_' + str(j), build_pop)
				status = result_1d1s[0]
				fig_path = folder_path + '/day_' + str(j) +'.png'
				fig, (ax1) = plt.subplots(1, 1, sharex=False, sharey=False)
				fig.set_size_inches(cm2inch(28), cm2inch(20))
				plotBuildingDisconnectStatus(pipe_loc, pump_loc, build_loc, build_area, status, j, pump_day, plot_lim, ax1)
				fig.tight_layout()
				fig.savefig(fig_path, dpi=600)
				fig.clf()
				plt.close()

			str_input = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/dbdReco/sim_' + str(i) + '/day_%d.png'
			str_output = './gen/MCS/predictions/' + network_name + '/' + GM_name + '/dbdReco/sim_' + str(i) + '/reco.mov'
			echo = 'ffmpeg -f image2 -r 1 -i ' + str_input + ' -vf format=rgb32 -vcodec qtrle -pix_fmt rgb24 -y ' + str_output
			os.system(echo)



