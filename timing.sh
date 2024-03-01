#/bin/bash

rm -f result_fin.csv

echo "sqrt_spheres;rays_per_pixel;rays_depth;resolution;time_opencl;cpp_opencl;time_data_transfert;cpp_data_transfert;time_cpu; cpp_cpu" | tee -a result.csv

for sqrt_spheres in 2 6 11
do
    for rays_per_pixel in 1 10 50 100
    do
        for rays_depth in 5 15 50
        do
            for width in 256 640 848 1280 1920 2560 3840 7680
            do
                echo "$sqrt_spheres;$rays_per_pixel;$rays_depth;$(($width*$width*9/16));" | tee -a result.csv
                ./raytracing-app $width $(($width * 9 / 16)) $rays_per_pixel $rays_depth $sqrt_spheres | tee -a result.csv
            done
        done
    done
done