#include "lines.h"
#include "common.h"
#include <cmath>
#include "Mandelbrot.h"
#include <cuComplex.h>
#include <complex>
#include <chrono>

using namespace std;
using namespace mandelbrot;


__global__ void kernel(int * d_fractal, int * d_histogram, int step,double scale, double xCenter, double yCenter){


  unsigned int xIndex = threadIdx.x + blockIdx.x * blockDim.x;
  unsigned int yIndex = threadIdx.y + blockIdx.y * blockDim.y;

  const int tid  = yIndex * step + xIndex;

  //__shared__ int s_histo[Mandelbrot::MAX_ITERATIONS];
  //int sx = threadIdx.x + threadIdx.y * blockDim.x;
  //Initialize shared histogram to 0
  //if ( sx < Mandelbrot::MAX_ITERATIONS)
    //s_histo[sx] = 0;
  //__syncthreads();

  if ((xIndex < M_WIDTH) && (yIndex < M_HEIGHT)){
    int iterations = 0;

    double div = tid / M_HEIGHT;

    int x = tid % M_WIDTH;
    int y = floor(div);

    double xFractal = (x - M_WIDTH / 2) * scale + xCenter;
    double yFractal = (y - M_HEIGHT / 2) * scale + yCenter;

    double2 a; a.x = 0.0, a.y = 0.0;
    cuDoubleComplex z = a;
    cuDoubleComplex c = make_cuDoubleComplex(xFractal, yFractal);


    while(iterations < Mandelbrot::MAX_ITERATIONS) {
      z = cuCadd(cuCmul(z, z), c);

      if(cuCabs(z) > 2) {
        break;
      }

      iterations++;
    }

    d_fractal[tid] = iterations;
    atomicAdd(&d_histogram[iterations], 1);
  }
  __syncthreads();

  // Copy infro from shared memory to global memory
  //if (sx < Mandelbrot::MAX_ITERATIONS)
    //atomicAdd(&d_histogram[sx], s_histo[sx]);
}

void runCuda(int * m_fractal,int * m_histogram, double scale, double xCenter, double yCenter){

  int * d_fractal, * d_histogram;
  int fractalBytes = M_WIDTH * M_HEIGHT * sizeof(int);
  int histogramBytes = Mandelbrot::MAX_ITERATIONS * sizeof(int);

  d_fractal = (int *)malloc(fractalBytes);
  d_histogram = (int *)malloc(histogramBytes);
  memset(d_fractal, 0, fractalBytes);
  memset(d_histogram, 0, histogramBytes);


  SAFE_CALL(cudaMalloc<int>(&d_histogram, histogramBytes), "CUDA Malloc Failed");
  SAFE_CALL(cudaMalloc<int>(&d_fractal, fractalBytes), "CUDA Malloc Failed");

  SAFE_CALL(cudaMemcpy(d_histogram, m_histogram, histogramBytes, cudaMemcpyHostToDevice), "CUDA Memcpy Host To Device Failed");
  SAFE_CALL(cudaMemcpy(d_fractal, m_fractal,fractalBytes, cudaMemcpyHostToDevice), "CUDA Memcpy Host To Device Failed");

  const dim3 block(16, 16);
  const dim3 grid((int)ceil((float)M_WIDTH / block.x), (int)ceil((float)M_HEIGHT/ block.y));

  printf("Runtime.\n");
  float cpuTime = 0.0;
  auto start =  chrono::high_resolution_clock::now();
  kernel<<<grid, block >>>(d_fractal, d_histogram, M_WIDTH, scale, xCenter, yCenter);
  auto end =  chrono::high_resolution_clock::now();
  chrono::duration<float, std::milli> duration_ms = end - start;
  cpuTime = duration_ms.count();
  printf("Runtime: %f\n", cpuTime);
  //SAFE_CALL(cudaDeviceSynchronize(), "Kernel Launch Failed");
  // SAFE_CALL kernel error
  //SAFE_CALL(cudaGetLastError(), "Error with last error");

  printf("Copy\n");
  SAFE_CALL(cudaMemcpy(m_fractal, d_fractal, fractalBytes, cudaMemcpyDeviceToHost), "CUDA Memcpy Device To Device Failed");
  SAFE_CALL(cudaMemcpy(m_histogram, d_histogram, histogramBytes, cudaMemcpyDeviceToHost), "CUDA Memcpy Device To Device Failed");

  SAFE_CALL(cudaFree(d_fractal), "CUDA Free Failed");
  SAFE_CALL(cudaFree(d_histogram), "CUDA Free Failed");

  // Reset device
  //SAFE_CALL(cudaDeviceReset(), "Error reseting");
  printf("SALE DE KERNEL\n" );

}
