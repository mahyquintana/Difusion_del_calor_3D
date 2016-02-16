#! /bin/sh
#################################
#####  MAHY QUINTANA DIAZ  ######
#################################

compile:
	gcc 1_heat_3D.c -o 1_heat_3D
	h5cc 2_heat_3D_to_h5.c -o 2_3D_to_h5
	h5cc 4_hdf5_to_netcdf.c -lnetcdf -o 4_h5_to_nc 


run: 
	@echo "\nCreando el fichero hdf5...\n"
	./2_3D_to_h5 100
	@echo "\nIniciando visualización 2D: Z=50\n"
	python 3_visualizar_h5_3D_2D.py data.h5 2D Z 50
	@echo "\nExportando hiperlámina XY a NetCDF...\n"
	./4_h5_to_nc data.h5 Z 50
	@echo "\nIniciando visualización 2D del fichero netCDF\n"
	python 5_visualizar_netcdf_2D.py data.nc
	@echo "\nFIN\n"


clean:
	-rm 1_heat_3D
	-rm 2_3D_to_h5
	-rm 4_h5_to_nc
	-rm 4_hdf5_to_netcdf.o
	-rm 2_heat_3D_to_h5.o
	-rm data.nc
	-rm data.h5

