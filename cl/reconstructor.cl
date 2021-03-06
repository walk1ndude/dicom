#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

float calcElem(image3d_t src,
               __global float * cas,
               float4 pos,
               int offset,
               float coeff);

int reflect(int maxDir,
            int curP);

float calcElem(image3d_t src,
               __global float * cas,
               float4 pos,
               int offset,
               float coeff) {
    int width = get_image_width(src);
    
    int casPos = (int) round(pos.x * width + offset);
    float i = 0.0f;
    
    float elem = 0.0f;
    
    while (i < width) {
        elem += (read_imagef(src, sampler, (float4) (i, pos.y, pos.z, 0.0f)).x *
                 cas[casPos ++]);
        i += 1.0f;
    }

    return elem * coeff;
}

int reflect(int maxDir,
            int curP) {
    float k1 = 1 - step((float) curP, 0.0f);
    float k2 = step((float) curP, (float) maxDir);

    return int(round(2 * maxDir * k2 - (k1 + k2) * curP + (1 - k1) * (1 - k2) * curP - k1 - k2));
}

__kernel void gauss1d(__read_only image3d_t src,
                      __write_only image3d_t dst,
                      __constant float * gaussTab,
                      uint dirX,
                      uint dirY,
                      uint dirZ,
                      int kernGaussSize) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
    
    float sum = 0.0f;
    int4 posR = (int4) (0);

    __local int w;
    __local int h;
    __local int d;

    w = get_image_width(src);
    h = get_image_height(src);
    d = get_image_depth(src);
    
    for (int i = - kernGaussSize + 1; i != kernGaussSize; ++ i) {
        posR.x = reflect(w, pos.x + dirX * i);
        posR.y = reflect(h, pos.y + dirY * i);
        posR.z = reflect(d, pos.z + dirZ * i);
        
        sum += (gaussTab[kernGaussSize + i] * read_imagef(src, sampler, posR).x);
    }

    write_imagef(dst, pos, (float4) (sum));
}

__kernel void calcTables(__global float * cas,
                         __global float * tanTable,
                         __global float * radTable,
                         int width,
                         int height,
                         float twoPiN) {
    const int2 pos = {get_global_id(0), get_global_id(1)};
    const float2 origin = {pos.x - width / 2.0f, pos.y - height / 2.0f};

    const int posT = pos.y * width + pos.x;

    const float xyPiN = pos.x * pos.y * twoPiN;
    cas[posT] = sin(xyPiN) + cos(xyPiN);
    
    tanTable[posT] = - atan2pi(origin.y, origin.x) * 180.0f;
    radTable[posT] = sqrt(origin.y * origin.y + origin.x * origin.x);

    float k = step(tanTable[posT], 0.0f);
    float k180 = 180.0f * (1 - k);

    radTable[posT] = radTable[posT] - (1 - k) * 2 * radTable[posT];
    tanTable[posT] = min(tanTable[posT] + k180, tanTable[posT] * k + k180);
}

__kernel void fourier2d(__read_only image3d_t src,
                        __write_only image3d_t dst,
                        __global float * cas,
                        __global float * tanTable,
                        __global float * radTable) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
    
    const int4 size = {get_image_width(src), get_image_depth(src),
                       get_image_width(dst), get_image_height(dst)};

    const float4 center = {size.x / 2.0f, size.y / 2.0f,
                           size.z / 2.0f, size.w / 2.0f};

    const float2 pad = {size.x / (float) size.z, size.y / (float) size.w};

    const int posT = pos.y * size.z + pos.x;

    float4 srcPos = {radTable[posT], pos.z, tanTable[posT], 0.0f};
    
    const float sinoX = (center.z + srcPos.x) * pad.x;
    srcPos.x = round(sinoX + (srcPos.x < 0 ? 1 : -1) * center.x);
        
    const float dhtValue = calcElem(src, cas, srcPos, (int) center.z - center.x, 1.0f);
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    write_imagef(dst,
                 (int4) (pos.x + (pos.x < center.z ? 1 : -1) * center.z,
                         pos.y + (pos.y < center.w ? 1 : -1) * center.w,
                         pos.z,
                         0),
                 (float4) (( ((int) round(sinoX) % 2 ? -1 : 1) * dhtValue)));
}

__kernel void dht1dTranspose(__read_only image3d_t src,
                             __write_only image3d_t dst,
                             __global float * cas,
                             float coeff) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
    const float4 dhtValue = (float4) (calcElem(src, cas, convert_float4(pos), 0, coeff));
    
    barrier(CLK_LOCAL_MEM_FENCE);
    
    write_imagef(dst, (int4) (pos.y, pos.x, pos.z, 0), dhtValue);
}

__kernel void butterflyDht2d(__read_only image3d_t src,
                             __write_only image3d_t dst) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};

    const int4 size = {get_image_width(src), get_image_height(src),
                       get_image_width(dst), get_image_height(dst)};

    const int4 center = {size.x / 2, size.y / 2,
                         size.z / 2, size.w / 2};

    int4 positions = {pos.x, pos.y, (size.x - pos.x) % size.x, (size.y - pos.y) % size.y};

    const float4 readPixels = {
            read_imagef(src, sampler, (int4) (positions.x, positions.y, pos.z, 0)).x,
            read_imagef(src, sampler, (int4) (positions.x, positions.w, pos.z, 0)).x,
            read_imagef(src, sampler, (int4) (positions.z, positions.y, pos.z, 0)).x,
            read_imagef(src, sampler, (int4) (positions.z, positions.w, pos.z, 0)).x
    };

    const float E = ((readPixels.x + readPixels.w) - (readPixels.y + readPixels.z)) / 2.0f;

    positions.x += center.x;
    positions.y += center.y;

    positions.z -= center.x;
    positions.w -= center.y;

    positions.xz -= (center.x - center.z);
    positions.yw -= (center.y - center.w);
 
    barrier(CLK_LOCAL_MEM_FENCE);
    
    write_imagef(dst, (int4) (positions.x, positions.y, pos.z, 0), (readPixels.x - E));
    write_imagef(dst, (int4) (positions.x, positions.w, pos.z, 0), (readPixels.y + E));
    write_imagef(dst, (int4) (positions.z, positions.y, pos.z, 0), (readPixels.z + E));
    write_imagef(dst, (int4) (positions.z, positions.w, pos.z, 0), (readPixels.w - E));
}
