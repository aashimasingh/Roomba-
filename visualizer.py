# -*- coding: utf-8 -*-
"""
Created on Sun Apr  8 13:00:10 2018

@author: aashi
"""

import numpy as np
import matplotlib as mlp
import matplotlib.pyplot as pyplot

def visGrid(data, path):
    cmap = mlp.colors.ListedColormap(['Green', 'Red', 'White', 'Yellow', 'Orange', 'Pink', 'Blue'])
    grid = data
    for i in path:
        print (i)
        grid[int(i / 30)][int(i % 30)] = 20
    bounds = [-2, -1.1, -0.1, 4, 7, 10, 17, 20]
    norm = mlp.colors.BoundaryNorm(bounds, cmap.N)
    img = pyplot.imshow(grid, interpolation='nearest',
								cmap = cmap, norm = norm)
    pyplot.show()
    

