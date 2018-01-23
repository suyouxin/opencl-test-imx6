__kernel void VectorAdd(__global int* c, __global int* a,__global int* b)
{
    unsigned int n = get_global_id(0);
    c[n] = a[n] + b[n];
}

