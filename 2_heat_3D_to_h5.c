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

#define DX .01   // Interval size in x direction
#define DY .01   // Interval size in y direction
#define DZ .01   // Interval size in y direction
#define A 0.5     // Diffusion constant


#define H5_FILE_NAME "data.h5"
#define ERR { H5Eprint( H5E_DEFAULT, stdout ); exit(2); }

void print_matrix( double * u, int nx, int ny, int nz ) {
  int i,j,k;

  for( i = 1; i < nx-1; ++i ) {
    for( j = 1; j < ny-1; ++j ) {
      for( k = 1; k < nz-1; ++k ) {
        printf( "%lf ", u[k*nx*ny+i*ny+j] );
      }
    }
    printf( "\n" );
  }
}

int main(int argc,char *argv[]){

  int STEPS = 350; // Number of iterations
  
  if(argc == 2){
     STEPS=atoi(argv[1]);
  }


  double *u, *ui;
  int i,j,t,k;
  int nx = 1/DX;
  int ny = 1/DY;
  int nz = 1/DZ;

  // To save CPU cycles, we'll compute Delta x^2 and Delta y^2 only once
  double dx2 = DX*DX;
  double dy2 = DY*DY;
  double dz2 = DZ*DZ;
  //double dt = dx2*dy2*dz2/(2*A*(dx2+dy2+dz2));
  double dt = 0.00003;

  u = (double*)malloc(sizeof(double)*nx*ny*nz);
  ui = (double*)malloc(sizeof(double)*nx*ny*nz);

  // Initialize matrices
  for( i = 0; i < nx; ++i ) {
    for( j = 0; j < ny; ++j ) {
      for( k = 0; k < nz; ++k ) {
        double dist_to_center = pow(i*DX-0.5,2)+pow(j*DY-0.5,2)+pow(k*DZ-0.5,2);
        if( ( dist_to_center < 0.1 ) &&
            ( dist_to_center > 0.05 ) ) {
          ui[k*ny*nz+i*ny+j] = 1.0;
        } else {
          ui[k*ny*nz+i*ny+j] = 0.0;
        }
      }
    }
  }

  memcpy( u, ui, nx*ny*sizeof(double) );

  // Create HDF5 file.
  // H5F_ACC_TRUNC = overwrites existing data
  hid_t file_id;
  file_id = H5Fcreate( H5_FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );
  if( file_id < 0 ) { ERR; }

  // Create temperature dataspace.
  hsize_t dims[4] = { STEPS+1, nx, ny, nz };
  hid_t dataspace_temp = H5Screate_simple( 4, dims, NULL  );
  if( dataspace_temp < 0 ) { ERR; }

  // Create the Dimension Scale for the X variable and write values
  double xds_data[nx];
  hsize_t xdims = nx;
  hid_t xS = H5Screate_simple( 1, &xdims, NULL );
  hid_t xD = H5Dcreate( file_id, "x", H5T_NATIVE_DOUBLE, xS, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );
  for( i = 0; i < nx; ++i ) {
    xds_data[i] = DX*i;
  }
  herr_t status = H5Dwrite( xD, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, xds_data);
  status = H5DSset_scale( xD, "x" );

  // Create the Dimension Scale for the Y variable and write values
  double yds_data[ny];
  hsize_t ydims = ny;
  hid_t yS = H5Screate_simple( 1, &ydims, NULL );
  hid_t yD = H5Dcreate( file_id, "y", H5T_NATIVE_DOUBLE, yS, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );
  for( i = 0; i < ny; ++i ) {
    yds_data[i] = DY*i;
  }
  status = H5Dwrite( yD, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, yds_data);
  status = H5DSset_scale( yD, "y" );

  // Create the Dimension Scale for the Z variable and write values
  double zds_data[nz];
  hsize_t zdims = nz;
  hid_t zS = H5Screate_simple( 1, &zdims, NULL );
  hid_t zD = H5Dcreate( file_id, "z", H5T_NATIVE_DOUBLE, zS, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );
  for( i = 0; i < nz; ++i ) {
    zds_data[i] = DZ*i;
  }
  status = H5Dwrite( zD, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,zds_data);
  status = H5DSset_scale( zD, "z" );

  // Create the Dimension Scale for the t variable and write values 
  double tds_data[STEPS+1];
  hsize_t tdims = STEPS+1;
  hid_t tS = H5Screate_simple( 1, &tdims, NULL );
  hid_t tD = H5Dcreate( file_id, "t", H5T_NATIVE_DOUBLE, tS, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );
  for( i = 0; i < STEPS+1; ++i ) {
    tds_data[i] = i*dt;
  }
  status = H5Dwrite( tD, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, tds_data);
  status = H5DSset_scale( tD, "t" );


  // Create the temperature dataset into the root group of the file
  hid_t dataset_temp = H5Dcreate( file_id, "temperature", H5T_NATIVE_DOUBLE, dataspace_temp, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );
  if( dataset_temp < 0 ) { ERR; }

  H5DSattach_scale( dataset_temp, tD, 0 );
  H5DSattach_scale( dataset_temp, xD, 1 );
  H5DSattach_scale( dataset_temp, yD, 2 );
  H5DSattach_scale( dataset_temp, zD, 3 );

  // Write the initial state
  hid_t tempSel = H5Dget_space( dataset_temp );
  if( tempSel < 0 ) { ERR; }
  hsize_t sel_offset[4] = {0,0,0,0};
  hsize_t sel_length[4] = {1, nx, ny, nz};
  hid_t memS = H5Screate_simple( 3, &dims[1], NULL );
  if( memS < 0 ) { ERR; }
  H5Sselect_hyperslab( tempSel, H5S_SELECT_SET, sel_offset, NULL, sel_length,
                       NULL );
  if( tempSel < 0 ) { ERR; }
  status = H5Dwrite( dataset_temp, H5T_NATIVE_DOUBLE, memS, tempSel,
                            H5P_DEFAULT, ui );
  if( status < 0 ) { ERR; }



  // Calculate all timesteps
  for( t = 0; t < STEPS; ++t ) {
    for( i = 1; i < nx-1; ++i ) {
      for( j = 1; j < ny-1; ++j ) {
        for( k = 1; k < nz-1; ++k ) {
          u[k*ny*nz+i*ny+j] = ui[k*ny*nz+i*ny+j] + A*dt*( (ui[k*ny*nz+(i+1)*ny+j] - 2*ui[k*ny*nz+i*ny+j] + ui[k*ny*nz+(i-1)*ny+j])/dx2 + //x-1 | x | x+1
                                                          (ui[k*ny*nz+i*ny+j-1]   - 2*ui[k*ny*nz+i*ny+j] + ui[k*ny*nz+i*ny+j+1])/dy2 +   //y-1 | y | y+1
                                                          (ui[(k+1)*ny*nz+i*ny+j] - 2*ui[k*ny*nz+i*ny+j] + ui[(k-1)*ny*nz+i*ny+j])/dy2   //z-1 | z | z+1
                                                        );
        }
      }
    }
    // print_matrix(u,nx,ny,nz);
    printf( "TIMESTEP = %d\n", t );
    memcpy( ui, u, nx*ny*nz*sizeof(double) );


    // Write the temperature data to the file
    tempSel = H5Dget_space( dataset_temp ); // The dataspace has changed!
    sel_offset[0] = t+1;
    H5Sselect_hyperslab( tempSel, H5S_SELECT_SET, sel_offset, NULL, sel_length,NULL );
    status = H5Dwrite( dataset_temp, H5T_NATIVE_DOUBLE, memS, tempSel, H5P_DEFAULT, u);
    if( status < 0 ) { ERR; }

  }

  // Close the file. This frees up any internal NetCDF resources associated
  // with the file, and flushes any buffers.
  if( H5Fclose( file_id ) <  0 ) { ERR; }

  free(u);
  free(ui);
}
