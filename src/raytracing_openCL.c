#include "raytracing_openCL.h"

#include <stdio.h>
#include <CL/cl.h>

void raytracing_openCL(color_t *image, const uint16_t width, const uint16_t height, const uint16_t raysPerPixel, const uint8_t raysDepth, const sphere_t *spheres, const uint16_t numberOfSpheres, const camera_t *camera)
{
    // Template found on "https://github.com/Abercus/openCL/"
	// Load kernel from file kernel/raytracing.cl

	FILE *kernelFile;
	char *kernelSource;
	size_t kernelSize;
    const char* path = "kernel/raytracing.cl";

	kernelFile = fopen(path, "r");

	if (!kernelFile) {

		fprintf(stderr, "No file named %s was found\n", path);

		exit(-1);

	}
	fseek(kernelFile, 0, SEEK_END);
	long fsize = ftell(kernelFile);
	fseek(kernelFile, 0, SEEK_SET);  /* same as rewind(f); */

	kernelSource = (char*)malloc(fsize+1);
	kernelSize = fread(kernelSource, 1, fsize, kernelFile);
	fclose(kernelFile);

	// Getting platform and device information
	cl_platform_id platformId = NULL;
	cl_device_id deviceID = NULL;
	cl_uint retNumDevices;
	cl_uint retNumPlatforms;
	cl_int ret = clGetPlatformIDs(1, &platformId, &retNumPlatforms);
	ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_DEFAULT, 1, &deviceID, &retNumDevices);

	// Creating context.
	cl_context context = clCreateContext(NULL, 1, &deviceID, NULL, NULL,  &ret);


	// Creating command queue
	cl_command_queue commandQueue = clCreateCommandQueue(context, deviceID, 0, &ret);

	// Memory buffers for each array
	cl_mem imageMemObj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height * sizeof(color_t), NULL, &ret);
	cl_mem cameraMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(camera_t), NULL, &ret);
	cl_mem sphereMemObj = clCreateBuffer(context, CL_MEM_READ_ONLY, numberOfSpheres * sizeof(sphere_t), NULL, &ret);


	// Copy lists to memory buffers
	ret = clEnqueueWriteBuffer(commandQueue, cameraMemObj, CL_TRUE, 0, sizeof(camera_t), camera, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(commandQueue, sphereMemObj, CL_TRUE, 0, numberOfSpheres * sizeof(sphere_t), spheres, 0, NULL, NULL);

	// Create program from kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, (const size_t *)&kernelSize, &ret);	

	// Build program
	ret = clBuildProgram(program, 1, &deviceID, NULL, NULL, NULL);
    if (ret != CL_SUCCESS)
    {
        printf("\n\nERROR!!!\n\n");
        size_t len = 0;
        cl_int ret = CL_SUCCESS;
        ret = clGetProgramBuildInfo(program, deviceID, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        char *buffer = calloc(len, sizeof(char));
        ret = clGetProgramBuildInfo(program, deviceID, CL_PROGRAM_BUILD_LOG, len, buffer, NULL);
        printf("%s\n", buffer);
    }

	// Create kernel
	cl_kernel kernel = clCreateKernel(program, "raytracing", &ret);


	// Set arguments for kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&imageMemObj);	
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&sphereMemObj);	
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&cameraMemObj);	
	ret = clSetKernelArg(kernel, 3, sizeof(width), (void *)&width);	
	ret = clSetKernelArg(kernel, 4, sizeof(height), (void *)&height);	
	ret = clSetKernelArg(kernel, 5, sizeof(raysPerPixel), (void *)&raysPerPixel);	
	ret = clSetKernelArg(kernel, 6, sizeof(raysDepth), (void *)&raysDepth);	
	ret = clSetKernelArg(kernel, 7, sizeof(numberOfSpheres), (void *)&numberOfSpheres);	


	// Execute the kernel
	size_t globalItemSize = width * height;
	size_t localItemSize = 64; // globalItemSize has to be a multiple of localItemSize. 1024/64 = 16 

    // Time measure
    struct timeval start, end;
    printf("Trace rays!");
    fflush(stdout);
    gettimeofday(&start, NULL);

    // Render image
	ret = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalItemSize, &localItemSize, 0, NULL, NULL);	

    // Elapsed time
    gettimeofday(&end, NULL);
    printf("\t\t\tDone!\n");
    uint64_t elapsedTime = (end.tv_sec - start.tv_sec) * 1000000ull + (end.tv_usec - start.tv_usec);
    printf("Raytracing_OpenCL elapsed time: %lu us\n", elapsedTime);
    printf("Cycles per pixel: %f\n", elapsedTime * 2.8e3f / (width * height));
    

	// Read from device back to host.
	printf("Data transfert: Device -> Host");
    fflush(stdout);
    gettimeofday(&start, NULL);

    ret = clEnqueueReadBuffer(commandQueue, imageMemObj, CL_TRUE, 0, width * height * sizeof(color_t), image, 0, NULL, NULL);
    
    gettimeofday(&end, NULL);
    printf("\t\t\tDone!\n");
    elapsedTime = (end.tv_sec - start.tv_sec) * 1000000ull + (end.tv_usec - start.tv_usec);
    printf("Data transfert: Device -> Host elapsed time: %lu us\n", elapsedTime);
    printf("Cycles per pixel: %f\n", elapsedTime * 2.8e3f / (width * height));
    


	// Clean up, release memory.
	ret = clFlush(commandQueue);
	ret = clFinish(commandQueue);
	ret = clReleaseCommandQueue(commandQueue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(imageMemObj);
	ret = clReleaseMemObject(sphereMemObj);
	ret = clReleaseMemObject(cameraMemObj);
	ret = clReleaseContext(context);
    free(kernelSource);
}
