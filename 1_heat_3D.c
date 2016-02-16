/*
#################################
#####  MAHY QUINTANA DIAZ  ######
#################################
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DX .01   // Interval size in x direction
#define DY .01   // Interval size in y direction
#define DZ .01   // Interval size in y direction
#define A 0.5     // Diffusion constant
#define STEPS 500 // Timesteps to compute

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

int main() {
  double *u, *ui;
  int i,j,t,k;

  int nx = 1/DX;
  int ny = 1/DY;
  int nz = 1/DZ;

  // To save CPU cycles, we'll compute Delta x^2 and Delta y^2 only once
  double dx2 = DX*DX;
  double dy2 = DY*DY;
  double dz2 = DZ*DZ;
  double dt = dx2*dy2*dz2/(2*A*(dx2+dy2+dz2));

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

  // Calculate all timesteps
  for( t = 0; t < STEPS; ++t ) {
    printf( "TIMESTEP = %d\n", t );
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
    print_matrix(u,nx,ny,nz);
    memcpy( ui, u, nx*ny*nz*sizeof(double) );
  }

  free(u);
  free(ui);
}
