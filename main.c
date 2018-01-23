//************************************************************
// Demo OpenCL application to compute a simple vector addition
// computation between 2 arrays on the GPU
// ************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
//
// OpenCL source code
const char* OpenCLSource[] = {
"__kernel void VectorAdd(__global int* c, __global int* a,__global int* b) \
{ \
 unsigned int n = get_global_id(0); \
 c[n] = a[n] + b[n]; \
}"
};
// Some interesting data for the vectors
int InitialData1[20] = {37,50,54,50,56,0,43,43,74,71,32,36,16,43,56,100,50,25,15,17};
int InitialData2[20] = {35,51,54,58,55,32,36,69,27,39,35,40,16,44,55,14,58,75,18,15};
// Number of elements in the vectors to be added
#define SIZE 100

struct kernel_src_str
{
	char *src;
	size_t size;
};

cl_int LoadKernelSource (char *filename, struct kernel_src_str *kernel_src)
{
	
	FILE *fp = NULL;
	
	fp = fopen (filename, "rb");
	
	if (fp == NULL)
	{
		printf ("\nFailed to open: %s\n", filename); 
		return -1;
	}
	
	fseek (fp, 0, SEEK_END);
	kernel_src->size = ftell (fp);
	rewind (fp);
	
	// prevent re-allocation
	//if (kernel->src) free (kernel->src);
	kernel_src->src =  (char *) malloc (sizeof (char) * kernel_src->size);
	if (! kernel_src->src)
	{
		printf ("\nError Allocating memory to load CL program");
		return -2;
	}
	fread (kernel_src->src, 1, kernel_src->size, fp);

	kernel_src->src[kernel_src->size] = '\0';
	fclose (fp);
	
	return CL_SUCCESS;
}

// Main function
// ************************************************************
int main(int argc, char **argv)
{
     // Two integer source vectors in Host memory
     int HostVector1[SIZE], HostVector2[SIZE];
     //Output Vector
     int HostOutputVector[SIZE];
     // Initialize with some interesting repeating data
     cl_int error;

     for(int c = 0; c < SIZE; c++)
     {
          HostVector1[c] = InitialData1[c%20];
          HostVector2[c] = InitialData2[c%20];
          HostOutputVector[c] = 0;
     }
     printf("TRACE LINE: %d\n", __LINE__);
     //Get an OpenCL platform
     cl_platform_id cpPlatform;
     clGetPlatformIDs(1, &cpPlatform, NULL);
     // Get a GPU device
     cl_device_id cdDevice;
     clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &cdDevice, NULL);
     char cBuffer[1024];
     clGetDeviceInfo(cdDevice, CL_DEVICE_NAME, sizeof(cBuffer), &cBuffer, NULL);
     printf("CL_DEVICE_NAME: %s\n", cBuffer);
     clGetDeviceInfo(cdDevice, CL_DRIVER_VERSION, sizeof(cBuffer), &cBuffer, NULL);
     printf("CL_DRIVER_VERSION: %s\n\n", cBuffer);

     // Create a context to run OpenCL enabled GPU
     cl_context GPUContext = clCreateContextFromType(0, CL_DEVICE_TYPE_GPU, NULL, NULL, &error);
	 if (error != CL_SUCCESS) {
         printf("clCreateContextFromType failed: %d\n", error);
     }
     // Create a command-queue on the GPU device
     cl_command_queue cqCommandQueue = clCreateCommandQueue(GPUContext, cdDevice, 0, &error);
	 if (error != CL_SUCCESS) {
         printf("clCreateCommandQueue failed: %d\n", error);
     }
     // Allocate GPU memory for source vectors AND initialize from CPU memory
     cl_mem GPUVector1 = clCreateBuffer(GPUContext, CL_MEM_READ_ONLY |
     CL_MEM_COPY_HOST_PTR, sizeof(int) * SIZE, HostVector1, NULL);
     cl_mem GPUVector2 = clCreateBuffer(GPUContext, CL_MEM_READ_ONLY |
     CL_MEM_COPY_HOST_PTR, sizeof(int) * SIZE, HostVector2, NULL);
     // Allocate output memory on GPU
     cl_mem GPUOutputVector = clCreateBuffer(GPUContext, CL_MEM_WRITE_ONLY,
     sizeof(int) * SIZE, NULL, NULL);

    struct kernel_src_str src;
    LoadKernelSource("/sdcard/src.cl", &src);

     // Create OpenCL program with source code
     cl_program OpenCLProgram = clCreateProgramWithSource(GPUContext, 1,
             &src.src, &src.size, &error);
	 if (error != CL_SUCCESS) {
         printf("clCreateProgramWithSource failed: %d\n", error);
     }
     printf("TRACE LINE: %d\n", __LINE__);
     // Build the program (OpenCL JIT compilation)
     error = clBuildProgram(OpenCLProgram, 0, NULL, NULL, NULL, NULL);
	 if (error != CL_SUCCESS) {
         printf("clBuildProgram: %d\n", error);
         cl_build_status status;
         clGetProgramBuildInfo(OpenCLProgram, cdDevice, CL_PROGRAM_BUILD_STATUS,
                 sizeof(cl_build_status), &status, NULL);
         printf("build status: %d\n", status);
         size_t len;
         char *buffer;
         clGetProgramBuildInfo(OpenCLProgram, cdDevice, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
         printf("log length: %d\n", len);
         buffer = malloc(len);
         clGetProgramBuildInfo(OpenCLProgram, cdDevice, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
         printf("%s\n", buffer);
     }
     printf("TRACE LINE: %d\n", __LINE__);
     // Create a handle to the compiled OpenCL function (Kernel)
     cl_kernel OpenCLVectorAdd = clCreateKernel(OpenCLProgram, "VectorAdd",
             &error);
     printf("TRACE LINE: %d\n", __LINE__);
	 if (error != CL_SUCCESS) {
         printf("clCreateKernelfailed: %d\n", error);
     }

     printf("TRACE LINE: %d\n", __LINE__);
     // In the next step we associate the GPU memory with the Kernel arguments
     clSetKernelArg(OpenCLVectorAdd, 0, sizeof(cl_mem), (void*)&GPUOutputVector);
     clSetKernelArg(OpenCLVectorAdd, 1, sizeof(cl_mem), (void*)&GPUVector1);
     clSetKernelArg(OpenCLVectorAdd, 2, sizeof(cl_mem), (void*)&GPUVector2);
     printf("TRACE LINE: %d\n", __LINE__);
     // Launch the Kernel on the GPU
     // This kernel only uses global data
     size_t WorkSize[1] = {SIZE}; // one dimensional Range
     error = clEnqueueNDRangeKernel(cqCommandQueue, OpenCLVectorAdd, 1, NULL, 
             WorkSize, NULL, 0, NULL, NULL);
	 if (error != CL_SUCCESS) {
         printf("clEnqueueNDRangeKernel failed: %d\n", error);
     }
     printf("TRACE LINE: %d\n", __LINE__);
     // Copy the output in GPU memory back to CPU memory
     clEnqueueReadBuffer(cqCommandQueue, GPUOutputVector, CL_TRUE, 0,
     SIZE * sizeof(int), HostOutputVector, 0, NULL, NULL);
     printf("TRACE LINE: %d\n", __LINE__);
     // Cleanup
     clReleaseKernel(OpenCLVectorAdd);
     clReleaseProgram(OpenCLProgram);
     clReleaseCommandQueue(cqCommandQueue);
     clReleaseContext(GPUContext);
     clReleaseMemObject(GPUVector1);
     clReleaseMemObject(GPUVector2);
     clReleaseMemObject(GPUOutputVector);
     for( int i =0 ; i < SIZE; i++)
          printf("[%d + %d = %d]\n",HostVector1[i], HostVector2[i], HostOutputVector[i]);
     return 0;
}



