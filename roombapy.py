# -*- coding: utf-8 -*-
"""
Created on Fri Mar 23 13:31:45 2018

@author: aashi
"""

from docplex.cp.model import CpoModel
import numpy as np
data = np.load("master_data.npy")
#to convert above to a 2D array
data2d = np.reshape(data, (30,900))

numericLabel = np.empty([30,30])
invalid = []
charging = []
valid = []

def labelNodes():
    
    k = 0
    for i in range(len(data)):
        for j in range(len(data)):
            numericLabel[i][j] = k
            k = k+1
    
    for i in range(len(data)):
        for j in range(len(data)):
            if (data[0][i][j] == -1):
                invalid.append(numericLabel[i][j])
            elif (data[0][i][j] == -2):
                charging.append(numericLabel[i][j])
            else:
                valid.append(numericLabel[i][j])
                    
labelNodes()

def checkAround(i,j):
    arc = []
    if (numericLabel[i][j] in valid):
        arc.append(numericLabel[i][j])
    if (numericLabel[i][j] in charging):
        arc.append(-1)
    if ((i+1) < len(data)):
        if (numericLabel[i+1][j] in valid):
            arc.append(numericLabel[i+1][j])
    if ((j+1)< len(data)):
        if (numericLabel[i][j+1] in valid):
            arc.append(numericLabel[i][j+1])
    if ((i-1) >= 0):
        if (numericLabel[i-1][j] in valid):
            arc.append(numericLabel[i-1][j])
    if ((j-1) >= 0):
        if (numericLabel[i][j-1] in valid):
            arc.append(numericLabel[i][j-1])
        if ((numericLabel[i][j-1] in charging) or (numericLabel[i-1][j] in charging)):
        #arc.append(numericLabel[i][j-1])
            arc.append(900)
    return arc

arclist = []
for i in range(0,len(data)):
    for j in range(0,len(data)):
        if numericLabel[i][j] in valid:
            arclist.append(checkAround(i,j))
        if numericLabel[i][j] in charging:
            print("charging!!!")
            arclist.append(checkAround(i,j))
            
#print(arclist)
dirtnodelist = []
for i in range(len(data)):
    for j in range(len(data)):
        if numericLabel[i][j] in valid:
            minilist = []
            minilist.append(numericLabel[i][j])
            minilist.append(data[i][j])
            dirtnodelist.append(minilist)
#-----------------------------------------------------------------------------
# Initialize the problem data
#-----------------------------------------------------------------------------
capacity = 10000
rate_dirt_pickup = 1
numScenarios = 30


#-----------------------------------------------------------------------------
# Build the model
#-----------------------------------------------------------------------------

# Create CPO model
mdl = CpoModel()

#x_ij = mdl.binary_var_dict((source, sink) for source in )
num_arc_dvars = 0
for arc in arclist :
    num_arc_dvars = num_arc_dvars + len(arc) -1

dvarlist = mdl.binary_var_list(num_arc_dvars)
arcdict = {}

decVarIndex = 0
for nodeList in arclist :
    arcdict[nodeList[0]] = {}
    for i in range(1 , len(nodeList)):
        arcdict[nodeList[0]][nodeList[i]] = decVarIndex
        decVarIndex += 1

#nested dictionary and integer_cplex_list for dis+, dis-,
dplusList = mdl.integer_var_list(len(valid) * numScenarios) 
dminusList = mdl.integer_var_list(len(valid) * numScenarios)  

decVarIndex = 0
dirt_plusminus_dict = {}
for scenario in range(numScenarios):
    dirt_plusminus_dict[scenario] = {}
    for node in range(len(valid)):
        dirt_plusminus_dict[scenario][node] = decVarIndex
        decVarIndex +=1
        
#nested dictionary and d_s, will hold amount of dirt not index
dirt_dict = {}
for scenario in range(numScenarios) :
    dirt_dict[scenario] = {}
    nodecounter = 0
    for node in valid:
        dirt_dict[scenario][nodecounter] = data2d[scenario][int(node)] 
        nodecounter +=1