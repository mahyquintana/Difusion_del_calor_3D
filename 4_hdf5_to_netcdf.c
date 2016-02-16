/*
#################################
#####  MAHY QUINTANA DIAZ  ######
#################################
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <netcdf.h>

#define DX .01  
#define DY .01  
#define NC_FILE_NAME "data.nc"
#define ERR { H5Eprint( H5E_DEFAULT, stdout ); exit(2); }
#define ERR2(e) { printf( "Error: %s\n", nc_strerror(e) ); exit(2); }

int main(int argc,char *argv[]){
  

   int i, j, k;
   int x, y, z, t;
   int  boolean;
   int ndims=0;
   int tndims=0;
   int error=0;
   hid_t file_id;
   hid_t dataset_id,dataset2_id;
   hid_t datatype_id, datatype2_id;
   hid_t dataspace_id, dataspace2_id; 
   herr_t status;
   x=y=z=0;

   //Valores por defecto
   int val=50;
   z=1;
   char *filename = "data.h5";
   char *dims_letras[6]= {"z","Z","x","X","y","Y"};
  


 
   // Procesando la entrada:
   if (argc > 3) {
      val = atoi(argv[3]);
      if (val<0){
         printf("\nError: tercer parámetro no válido\n\n");
         exit(1);
      }
   }
   if (argc > 2) {
      boolean = 0;
      x=y=z=0;
      for(i=0;i<6;i++)
         if(strcmp(argv[2],dims_letras[i])==0){
            boolean = 1;
            if((i==0)||(i==1))z=1;
            else if((i==2) || (i==3))x=1;
            else if((i==4) || (i==5))y=1;
         }
      if (!boolean){
         printf("\nError en el segundo parametro, los valores aceptados son: x, y, z\n\n");
         exit(1);
      }
   }

   if (argc > 1) {
      if((strcmp(argv[1],"ayuda")==0) || (strcmp(argv[1],"Ayuda")==0)){
         printf("\n $ ./4_h5_to_nc <fichero_hdf5.h5> <dimensión_constante> <valor_de_la_dimensión>\n\n");
         printf("   <fichero_hdf5.h5> fichero generado por el programa heat_3D_h5.c --> data.h5\n\n");
         printf("   <dimensión_constante> Este parámetro indica que dimensión debe permanecer\n");
         printf("                         constante. Sus posibles valores son: x y z\n\n");
         printf("   <valor_de_la_dimensión> Este valor indica el plano que se quiere visualizar:\n");
         printf("                           0<= valor <100\n\n");
         exit(1);
      }
      else{
         filename = argv[1];
      }
   }
   if (argc == 1) {
      printf("\n Por defecto se tomarán los siguientes valores: \n\n");
      printf(" Fichero de entrada: data.h5 \n");
      printf(" Dimensión constante: Z \n");
      printf(" Valor de Z: 50 \n\n");

   }

//###########################################
//#####             HDF5                #####
//############################################################################################################################

   file_id = H5Fopen( filename, H5F_ACC_RDONLY, H5P_DEFAULT ); 		//Abrir fichero
   if( file_id < 0 ) { ERR; }


   //Dataset TEMPERATURE
   dataset_id = H5Dopen2(file_id, "/temperature", H5P_DEFAULT);		//Abrir Dataset
   if( dataset_id < 0 ) { ERR; }

   datatype_id = H5Dget_type( dataset_id );				//Tipo de datos del dataset
   if( datatype_id < 0 ) { ERR; }
   if (!H5Tequal( datatype_id, H5T_NATIVE_DOUBLE )){ ERR; }             //Comprobación de tipos

   dataspace_id = H5Dget_space (dataset_id);				//Abrir dataspace
   if( dataspace_id < 0 ) { ERR; }

   ndims = H5Sget_simple_extent_ndims( dataspace_id );			//Dimensiones del dataspace --> 4
   if( ndims < 0 ) { ERR; }

   hsize_t dims[ndims];
   hsize_t maxdims[ndims];
   error = H5Sget_simple_extent_dims(dataspace_id, dims, maxdims );	//Tamaño de cada dimensión ---> t | 100 | 100 | 100
   if( error < 0 ) { ERR; }

   hsize_t dims2[ndims-1];
   hsize_t sel_offset[ndims];// desde 0 hasta t de dos dimensiones, la dimensión restante es constante.
   if(z){
      dims2[0]= dims[1];
      dims2[1]= dims[2];
      dims2[2]= 1;
    
      if(val>dims[3]){
         printf("\nError: tercer parámetro no válido\n\n");
         exit(1);
      }    

      sel_offset[0]=0;
      sel_offset[1]=0;
      sel_offset[2]=0;
      sel_offset[3]=val;
   }
   else if(y){
      dims2[0]= dims[1];
      dims2[1]= 1;
      dims2[2]= dims[3];

      if(val>dims[2]){
         printf("\nError: tercer parámetro no válido\n\n");
         exit(1);
      }   

      sel_offset[0]=0;
      sel_offset[1]=0;
      sel_offset[2]=val;
      sel_offset[3]=0;
   }
   else if(x){
      dims2[0]= 1;
      dims2[1]= dims[2];
      dims2[2]= dims[3];

      if(val>dims[1]){
         printf("\nError: tercer parámetro no válido\n\n");
         exit(1);
      }   

      sel_offset[0]=0;
      sel_offset[1]=val;
      sel_offset[2]=0;
      sel_offset[3]=0;
   }

   hid_t memS = H5Screate_simple( 3, dims2, NULL );
   hsize_t sel_length[4]={1,dims2[0],dims2[1],dims2[2]}; // Una lamina 2D

   double *buffer;//H5T_NATIVE_DOUBLE   
   if (!(buffer=(double*) malloc(sizeof(double)*dims2[0]*dims2[1]*dims2[2]))) //Memoria necesaria para una hiperlamina nx+ny+nz
      printf("\nERROR AL RESERVAR MEMORIA\n\n");

   t = dims[0];


   //Dataset T
   dataset2_id = H5Dopen2(file_id, "/t", H5P_DEFAULT);		//Abrir Dataset
   if( dataset_id < 0 ) { ERR; }

   datatype2_id = H5Dget_type( dataset2_id );				//Tipo de datos del dataset
   if( datatype2_id < 0 ) { ERR; }
   if (!H5Tequal( datatype_id, H5T_NATIVE_DOUBLE )){ ERR; }             //Comprobación de tipos

   dataspace2_id = H5Dget_space (dataset2_id);				//Abrir dataspace
   if( dataspace_id < 0 ) { ERR; }

   tndims = H5Sget_simple_extent_ndims( dataspace2_id );		//Dimensiones del dataspace --> 1
   if( ndims < 0 ) { ERR; }

   hsize_t tdims[tndims];
   hsize_t tmaxdims[tndims];
   error = H5Sget_simple_extent_dims(dataspace2_id, tdims, tmaxdims );	//Tamaño de cada dimensión ---> t 
   if( error < 0 ) { ERR; }

   double buffert;

   hsize_t tpoints=1;
   hsize_t tcoord=0;
   hid_t memT = H5Screate_simple( 1, &tpoints, NULL );


//###########################################
//#####            NetCDF               #####
//#########################################################################################################################
   // Se crea el fichero NetCDF
   // NC_CLOBBER indica sobreescribir si ya existe.
   int ncid, retval;
   if( retval = nc_create( NC_FILE_NAME, NC_CLOBBER, &ncid ) ) {
     ERR2(retval);
   }

 
   // Definición de las dimensiones x e y.
   int x_dimid, y_dimid, t_dimid;
   if( retval = nc_def_dim( ncid, "x", dims[1], &x_dimid ) ) {
     ERR2(retval);
   }
   if( retval = nc_def_dim( ncid, "y", dims[1], &y_dimid ) ) {
     ERR2(retval);
   }
  
   // Definición de la dimensión t.
   if( retval = nc_def_dim( ncid, "t", dims[0], &t_dimid ) ) {
     ERR2(retval);
   }


   // Define coordinate variables for x and y
   int x_varid, y_varid, t_varid;
   if( retval = nc_def_var( ncid, "x", NC_DOUBLE, 1, &x_dimid, &x_varid ) ) {
     ERR2(retval);
   }
   if( retval = nc_def_var( ncid, "y", NC_DOUBLE, 1, &y_dimid, &y_varid ) ) {
     ERR2(retval);
   }
   if( retval = nc_def_var( ncid, "t", NC_DOUBLE, 1, &t_dimid, &t_varid ) ) {
     ERR2(retval);
   }

   int varid;
   int dimids[3] = { t_dimid, x_dimid, y_dimid };
   if( retval = nc_def_var( ncid, "temperature", NC_DOUBLE, 3, dimids, &varid )){
     ERR2(retval);
   }
  
   // Se escriben las unidades de x,y,z,t y temperatura.
   char * space_units = "meters";
   char * time_units = "seconds since start of the experiment";
   char * temp_units = "kelvin";
   if( retval = nc_put_att_text( ncid, x_varid, "units", strlen(space_units),
                                space_units ) ) {
     ERR2(retval);
   }
   if( retval = nc_put_att_text( ncid, y_varid, "units", strlen(space_units),
                                space_units ) ) {
     ERR2(retval);
   }
   if( retval = nc_put_att_text( ncid, t_varid, "units", strlen(time_units),
                                time_units ) ) {
     ERR2(retval);
   }
   if( retval = nc_put_att_text( ncid, varid, "units", strlen(temp_units),
                                temp_units ) ) {
     ERR2(retval);
   }
   double scale_factor = 300.0;
   if( retval = nc_put_att_double( ncid, varid, "scale_factor", NC_DOUBLE, 1,
                                   &scale_factor ) ) {
     ERR2(retval);
   }

   // Se terminan las definiciones
   if( retval = nc_enddef( ncid ) ) {
     ERR2(retval);
   }
	  
   // Se escriben las coordenadas x
   size_t pos;
   for( pos = 0; pos < dims[1]; ++pos ) {
     double x = DX*pos;
     if( retval = nc_put_var1_double( ncid, x_varid, &pos, &x ) ) {
       ERR2(retval);
     }
   }

   // Se escriben las coordenadas y
   for( pos = 0; pos < dims[2]; ++pos ) {
     double y = DY*pos;
     if( retval = nc_put_var1_double( ncid, y_varid, &pos, &y ) ) {
       ERR2(retval);
     }
   }

   size_t corner_vector[3] = {0,0,0};
   size_t edge_lengths[3] = {1, dims[1], dims[2]};


//#################################################################################################################
     
   printf("\n##########################################\n");
   printf("######       Dataspace info:        ######\n");
   printf("##########################################\n");
   
   printf("\n Número de dimensiones = %d\n",ndims);
   
   printf(" Tamaño de la dimensión 0 = %d\n",dims[0]);
   printf(" Tamaño de la dimensión 1 = %d\n",dims[1]);
   printf(" Tamaño de la dimensión 2 = %d\n",dims[2]);
   printf(" Tamaño de la dimensión 3 = %d\n",dims[3]);

   printf("\n##########################################\n");

   if(z)  printf("\n Exportando el plano XY: ");
   else if(y) printf("\n Exportando el plano XZ: ");
   else if(x) printf("\n Exportando el plano YZ: ");
   printf("\n");
  
   size_t iter=0;

   for(i=0;i<t;++i){

      // Se selecciona una lamina 2D
      status = H5Sselect_hyperslab (dataspace_id, H5S_SELECT_SET, sel_offset, NULL, sel_length, NULL);
      if( status < 0 ) { ERR; }

      //Se lee la lamina 2D
      status = H5Dread (dataset_id, datatype_id, memS, dataspace_id, H5P_DEFAULT, buffer);
      if( status < 0 ) { ERR; }

      //Escribir en netCDF
      if(retval=nc_put_vara_double(ncid, varid, corner_vector, edge_lengths, buffer)) {
         ERR2(retval);
      }


      // Se selecciona un valor
      status = H5Sselect_hyperslab (dataspace2_id, H5S_SELECT_SET, &tcoord, NULL, &tpoints, NULL);
      if( status < 0 ) { ERR; }

      //Se lee un valor del dataset.
      status = H5Dread (dataset2_id, datatype2_id, memT, dataspace2_id, H5P_DEFAULT, &buffert);
      if( status < 0 ) { ERR; }
   
      //Escribir en netCDF.
       if( retval = nc_put_var1_double( ncid, t_varid, &iter, &buffert ) ) {
         ERR2(retval);
       }


      //Avanzar un paso en el tiempo
      ++tcoord;
      ++corner_vector[0];
      ++sel_offset[0];
      ++iter;
   }

   printf("\n Done.\n\n");

  //Cerrar NetCDF
  if( retval = nc_close( ncid ) ) {
    ERR(retval);
  }

  //Cerrar HDF5
  if( H5Sclose( dataspace_id ) <  0 ) { ERR; }
  if( H5Dclose( dataset_id ) <  0 ) { ERR; }
  if( H5Fclose( file_id ) <  0 ) { ERR; }

  //Se libera la memoria.
  free(buffer);


  return 0;


}

