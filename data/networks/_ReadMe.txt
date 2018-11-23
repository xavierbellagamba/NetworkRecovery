Network composition
**************************************

Network is composed of 1 folder containing the subsequent csv files:

1. pipes.csv
2. pumps.csv


Databases order:
**************************************

Pipes: (points have to be sorted by segment ID in order for the algorithm to work)
--------------------------------------
X, Y, Pt ID, Segment ID, Pipe ID (database), Diameter, Material, Date of construction (yyyymmdd), Service status, Type of pipe, LRI zone


Pumping stations:
--------------------------------------
X, Y, Pump ID (database), Service status, Pump function, Backup power, Capacity, Number of pumps, Number of integrated wells, Number of integrated reservoirs, LRI zone, Restoration date
