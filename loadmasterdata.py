# -*- coding: utf-8 -*-
"""
Created on Fri Mar 16 19:26:43 2018

@author: aashi
"""
import numpy as np
data = np.load("master_data.npy")

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
            if (data[i][j] == -1):
                invalid.append(numericLabel[i][j])
            elif (data[i][j] == -2):
                charging.append(numericLabel[i][j])
            else:
                valid.append(numericLabel[i][j])
                    
labelNodes()

def checkAround(i,j):
    arc = []
    if (numericLabel[i][j] in charging):
        arc.append(-1)
        #arc.append(numericLabel[i][j])
    if (numericLabel[i+1][j] in valid):
        arc.append(numericLabel[i+1][j])
    if (numericLabel[i][j+1] in valid):
        arc.append(numericLabel[i][j+1])
    if (numericLabel[i-1][j] in valid):
        arc.append(numericLabel[i-1][j])
    if (numericLabel[i][j-1] in valid):
        arc.append(numericLabel[i][j-1])
    if ((numericLabel[i][j-1] in charging) or (numericLabel[i-1][j] in charging)):
        #arc.append(numericLabel[i][j-1])
        arc.append(900)
    return arc

arclist = []
for i in range(1,len(data)-1):
    for j in range(1,len(data)-1):
        if numericLabel[i][j] in valid:
            arclist.append(checkAround(i,j))
        if numericLabel[i][j] in charging:
            print("charging!!!")
            arclist.append(checkAround(i,j))
            
#print(arclist)
        