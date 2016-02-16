#!/usr/bin/python 
#−∗−coding: utf−8−∗−



#################################
#####  MAHY QUINTANA DIAZ  ######
#################################


import Scientific.IO.NetCDF as nc
import sys

import scipy as sp
import matplotlib
matplotlib.use('GTKAgg')
import gobject
from pylab import *

if len(sys.argv) != 2:
  print "Error: invalid arguments"
  print "Usage: heat <ncfilename>"
  exit()

#Se obtiene el nombre de la entrada
filename = sys.argv[1]

# Se abre el fichero
ncfile = nc.NetCDFFile( filename, "r" )

# Se extraen los datos de temperatura
temperature = ncfile.variables["temperature"]
frame = 0

# Se avanza en el tiempo
def updatefig(*args):
  global temperature, frame
  im.set_array(temperature[frame,:,:])
  manager.canvas.draw()
  frame+=1
  print "Rendering timestep t=",frame
  if(frame>=len(temperature)):
    return False
  return True
  
# Creación del frame inicial
fig = plt.figure(1)
img = subplot(111)
im  = img.imshow( temperature[0,:,:], cmap=cm.hot, interpolation="nearest",
                  origin="lower" )
manager = get_current_fig_manager()

frame=1
fig.colorbar(im)

# Avanzar en el tiempo y actualizar la figura.
gobject.idle_add(updatefig)
show()
