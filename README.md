##Descripción:

Este código simula la difusión del calor en 3D, además, guarda todos los
valores intermedios en un fichero y permite su representación gráfica. El 
objetivo principal de este proyecto es aprender a usar netcdf y hdf5.


1_heat_3D.c: Código en C que resuelve la difusión del calor en 3D.

2_heat_3D_h5.c: Programa, escrito en C, que calcula 350 iteraciones de la 
                difución del calor en 3D. Los resultados intermedios son
                almacenados en un fichero en formato HDF5 con nombre 
                data.h5. Para realizar otro número de iteraciones solo hay 
                que pasárselo como parámetro de entrada.

3_visualizar_h5_3D_2D.py: Programa, escrito en phyton, que permite la
                          representación  gráfica tanto 3D cómo 2D, del 
                          fichero generado por el programa anterior.
                          Para obtener información acerca de su uso 
                          introducir el comando:
                          $ python 3_visualizar_h5_3D_2D.py ayuda

4_hdf5_to_netcdf.c: Programa, escrito en C, que exporta la evolución 
                    temporal de una lámina 2D a un fichero en formato 	
                    NetCDF. Para obtener información acerca de su uso
                    introducir el comando:
                    $ ./4_h5_to_nc ayuda

5_visualizar_netcdf_2d.py: Programa escrito en python que nos permite
                           visualizar gráficamente la evolución temporal
                           de la lámina 2D almacenada en el fichero con 
                           formato NetCDF (data.nc). Hay que pasarle el 
                           fichero como parámetro de entrada.

##Compilar y ejecutar: 

    Para poder compilar y ejecutar todo correctamente se necesita tener
    instaladas las librerías hdf5 y netcdf.
   
    Compilar fuentes: make compile
    Ejecución automática de muestra: make run
    Limpiar archivos creados al compilar y ejecutar: make clean

 
