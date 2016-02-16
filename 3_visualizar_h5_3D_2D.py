#!/usr/bin/python 
#−∗−coding: utf−8−∗−

#################################
#####  MAHY QUINTANA DIAZ  ######
#################################

import h5py
import sys

import scipy as sp
import numpy
import matplotlib
matplotlib.use("GTKAgg")
import gobject
from pylab import *

from enthought.mayavi import mlab
from enthought.mayavi.sources.array_source import ArraySource



#3D
def updatefig3():
  global ui, m, temperature
  ui = numpy.array(temperature[m,:,:,:])
  m+=1
  print "Rendering timestep t=", m
  #print "ui[50,50,50] = ", ui[50,50,50]
  if m >= len(temperature):
    return False
  return True

# 2D
def updatefig2():
  global im, m, temperature
  if ((dim == 'Z') or (dim == 'z')):
    im.set_array(temperature[m,:,:,val])
  elif ((dim == 'Y') or (dim == 'y')):
    im.set_array(temperature[m,:,val,:])
  elif ((dim == 'X') or (dim == 'x')):
    im.set_array(temperature[m,val,:,:])
  manager.canvas.draw()
  m+=1
  print "Rendering timestep t=",m
  if(m>=len(temperature)):
   return False
  return True



#Default parameters
dims=1 
dim = 'z'
val = 50

#Input data processing
if (len(sys.argv)>4):
  aux = int(sys.argv[4])
  if not(-1 < aux < 100):
    print "\n(4)Error en los parámetros de entrada, introduzca el siguiente comando para "
    print "obetener ayuda: $ python visualizar_h5.py ayuda\n"
    sys.exit()

  else:
    val= aux

if (len(sys.argv)>3):
  if not((sys.argv[3]=="x") or (sys.argv[3]=="X") or (sys.argv[3]=="y") or (sys.argv[3]=="Y") or (sys.argv[3]=="z") or (sys.argv[3]=="Z")):
    print "\n(3)Error en los parámetros de entrada, introduzca el siguiente comando para "
    print "obetener ayuda: $ python visualizar_h5.py ayuda\n"
    sys.exit()
  
  else:
    dim = sys.argv[3]
    if (len(sys.argv)==4):
      print "\nValor por defecto = 50\n"

if (len(sys.argv)>2):
  if not((sys.argv[2]=="3D") or (sys.argv[2]=="3d") or (sys.argv[2]=="2D") or (sys.argv[2]=="2d")):
    print "\n(2)Error en los parámetros de entrada, introduzca el siguiente comando para "
    print "obetener ayuda: $ python visualizar_h5.py ayuda\n"
    sys.exit()
  
  elif ((sys.argv[2]=="3D") or (sys.argv[2]=="3d")):
    dims=0 #3D
    print "\nSe mostrará la representación 3D\n"


if (len(sys.argv)>1):
  if (sys.argv[1]=="ayuda") :
    print "\n$ python 3_visualizar_h5_3D_2D.py <fichero.h5> <tipo_de_visualización> <dimensión_constante> <valor_de_la_dimensión_constante>\n"
    print "\n   <fichero.h5> fichero generado por el programa heat_3D_h5.c --> data.h5"
    print "\n   <tipo_de_visualización> Existen dos opciones para este parámetro: 2D 3D"
    print "\n   <dimensión_constante> Este parámetro solo será necesario cuando la opción"
    print "     anterior sea: 2D. Con él se indica que dimensión debe permanecer constante."
    print "     Sus posibles valores son: x y z"
    print "\n   <valor_de_la_dimensión_constante> Este parámetro solo será necesario cuando"
    print "      el tipo de visualización sea: 2D. Con él se indica el plano que se quiere "
    print "      visualizar: 0<= valor <100\n"
    sys.exit()

  elif (dims==1):
    print "\nSe mostrará la representación 2D"
    print "Dimensión constante: ",dim 
    print "Valor = {0}\n".format(val)





# Get h5 file name
filename = sys.argv[1]

# Open file
try:
  file = h5py.File( filename, "r" )
except IOError as e:
  print "Error: no se puede abrir el fichero ---> {0}\n".format(filename)
  sys.exit()


# Get temperature data
temperature = file["temperature"]

m=0

if(dims==0): #3D

  while( updatefig3() ):
    if( m % 10 == 0 ):
      fig = mlab.contour3d( ui, colormap='hot', opacity=0.3 )
  
  fig = mlab.contour3d( ui, colormap='hot', opacity=0.3 )
  mlab.show()

else: #2D

  # First frame configuration
  fig = plt.figure(1)
  img = subplot(111)
  if ((dim == 'Z') or (dim == 'z')): 
    im  = img.imshow( temperature[0,:,:,val], cmap=cm.hot, interpolation="nearest", origin="lower", vmax=1.01)
  elif ((dim == 'Y') or (dim == 'y')): 
    im  = img.imshow( temperature[0,:,val,:], cmap=cm.hot, interpolation="nearest", origin="lower", vmax=1.01)
  elif ((dim == 'X') or (dim == 'x')): 
    im  = img.imshow( temperature[0,val,:,:], cmap=cm.hot, interpolation="nearest", origin="lower", vmax=1.01)


  manager = get_current_fig_manager()

  m = 1
  fig.colorbar(im)

  # Update graph
  gobject.idle_add(updatefig2)
  show()
  



