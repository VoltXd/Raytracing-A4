import csv
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.lines import Line2D

LIST_RES = [int(256 **2 * 9 / 16),
            int(640 **2 * 9 / 16),
            #int(848 **2 * 9 / 16),
            int(1280**2 * 9 / 16), 
            int(1920**2 * 9 / 16), 
            int(2560**2 * 9 / 16), 
            int(3840**2 * 9 / 16), 
            int(7680**2 * 9 / 16)]
LIST_RAYS_DEPTH = [5, 15, 50]
LIST_RAYS_PER_PIXEL = [1, 10, 50, 100]
LIST_SQRT_SPHERE = [2, 6, 11]
N_SAMPLES = 1

# Order: time = { winsize:{ n_feat:{ n_thread:{ res:time } } } }
dict_t_opencl = {}
dict_cpp_opencl = {}
dict_t_datatransfert = {}
dict_cpp_datatransfert = {}
dict_t_cpu = {}
dict_cpp_cpu = {}

for sqrt_s in LIST_SQRT_SPHERE:
    dict_t_opencl[sqrt_s] = {}
    dict_cpp_opencl[sqrt_s] = {}
    dict_t_datatransfert[sqrt_s] = {}
    dict_cpp_datatransfert[sqrt_s] = {}
    dict_t_cpu[sqrt_s] = {}
    dict_cpp_cpu[sqrt_s] = {}
    for rpp in LIST_RAYS_PER_PIXEL:
        dict_t_opencl[sqrt_s][rpp] = {}
        dict_cpp_opencl[sqrt_s][rpp] = {}
        dict_t_datatransfert[sqrt_s][rpp] = {}
        dict_cpp_datatransfert[sqrt_s][rpp] = {}
        dict_t_cpu[sqrt_s][rpp] = {}
        dict_cpp_cpu[sqrt_s][rpp] = {}
        for rd in LIST_RAYS_DEPTH:
            dict_t_opencl[sqrt_s][rpp][rd] = {}
            dict_cpp_opencl[sqrt_s][rpp][rd] = {}
            dict_t_datatransfert[sqrt_s][rpp][rd] = {}
            dict_cpp_datatransfert[sqrt_s][rpp][rd] = {}
            dict_t_cpu[sqrt_s][rpp][rd] = {}
            dict_cpp_cpu[sqrt_s][rpp][rd] = {}
            for res in LIST_RES:
                dict_t_opencl[sqrt_s][rpp][rd][res] = 0
                dict_cpp_opencl[sqrt_s][rpp][rd][res] = 0
                dict_t_datatransfert[sqrt_s][rpp][rd][res] = 0
                dict_cpp_datatransfert[sqrt_s][rpp][rd][res] = 0
                dict_t_cpu[sqrt_s][rpp][rd][res] = 0
                dict_cpp_cpu[sqrt_s][rpp][rd][res] = 0

# print(dict_t_convolve)

with open('result.csv', "r") as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=';')
    rows = []
    line_count = 0
    for row in csv_reader:
        line_count += 1
        if line_count == 1:
            continue
        rows.append(row)
    
    sqrt_s = 0
    rpp = 0
    rd = 0
    res = ""
    for row in rows:
        sqrt_s = int(row[0])
        rpp = int(row[1])
        rd = int(row[2])
        res = int(row[3])
        dict_t_opencl[sqrt_s][rpp][rd][res] = float(row[4])
        dict_cpp_opencl[sqrt_s][rpp][rd][res] = float(row[5])
        dict_t_datatransfert[sqrt_s][rpp][rd][res] = float(row[6])
        dict_cpp_datatransfert[sqrt_s][rpp][rd][res] = float(row[7])
        dict_t_cpu[sqrt_s][rpp][rd][res] = float(row[8])
        dict_cpp_cpu[sqrt_s][rpp][rd][res] = float(row[9])


DICT_MARKERS = { LIST_RAYS_DEPTH[0]:'o', LIST_RAYS_DEPTH[1]:'+', LIST_RAYS_DEPTH[2]:'*' }
DICT_LINE = { LIST_SQRT_SPHERE[0]:'-', LIST_SQRT_SPHERE[1]:':', LIST_SQRT_SPHERE[2]:'-.' }
DICT_COLOR = { LIST_RAYS_PER_PIXEL[0]:'black', LIST_RAYS_PER_PIXEL[1]:'y', LIST_RAYS_PER_PIXEL[2]:'g', LIST_RAYS_PER_PIXEL[3]:'r' }
LEGEND_ELEMENTS = [Line2D([0], [0], marker="o", color="black", markerfacecolor="black"),
                   Line2D([0], [0], marker="+", color="black", markerfacecolor="black"),
                   Line2D([0], [0], marker="*", color="black", markerfacecolor="black"),
                   Line2D([0], [0], linestyle="-", color="black"),
                   Line2D([0], [0], linestyle=":", color="black"),
                   Line2D([0], [0], linestyle="-.", color="black"),
                   Line2D([0], [0], color="black"),
                   Line2D([0], [0], color="y"),
                   Line2D([0], [0], color="g"),
                   Line2D([0], [0], color="r")]
LEGEND_LABELS = [f"Rays depth:{LIST_RAYS_DEPTH[0]}", 
                 f"Rays depth:{LIST_RAYS_DEPTH[1]}", 
                 f"Rays depth:{LIST_RAYS_DEPTH[2]}", 
                 f"Number of spheres:{LIST_SQRT_SPHERE[0]**2 + 4}",
                 f"Number of spheres:{LIST_SQRT_SPHERE[1]**2 + 4}",
                 f"Number of spheres:{LIST_SQRT_SPHERE[2]**2 + 4}",
                 f"Rays per pixel:{LIST_RAYS_PER_PIXEL[0]}",
                 f"Rays per pixel:{LIST_RAYS_PER_PIXEL[1]}",
                 f"Rays per pixel:{LIST_RAYS_PER_PIXEL[2]}",
                 f"Rays per pixel:{LIST_RAYS_PER_PIXEL[3]}"]
for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_t_cpu[sqrt_s][rpp][rd][res] for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("Temps CPU [µs]")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()

for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_cpp_cpu[sqrt_s][rpp][rd][res] for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("CPP CPU")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()

for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_t_opencl[sqrt_s][rpp][rd][res] for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("Temps OpenCL GPU [µs]")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()

for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_cpp_opencl[sqrt_s][rpp][rd][res] for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("CPP OpenCL GPU")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()

for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_t_datatransfert[sqrt_s][rpp][rd][res] for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("Temps Transfert mémoire [µs]")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()

for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_cpp_datatransfert[sqrt_s][rpp][rd][res] for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("CPP Transfert mémoire")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()

for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_t_cpu[sqrt_s][rpp][rd][res] / (dict_t_datatransfert[sqrt_s][rpp][rd][res] + dict_t_opencl[sqrt_s][rpp][rd][res]) for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("Gain Temps CPU/GPU")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()

for sqrt_s in LIST_SQRT_SPHERE:
    for rpp in LIST_RAYS_PER_PIXEL:
        for rd in LIST_RAYS_DEPTH:
            plt.plot(LIST_RES, [dict_cpp_cpu[sqrt_s][rpp][rd][res] / (dict_cpp_datatransfert[sqrt_s][rpp][rd][res] + dict_cpp_opencl[sqrt_s][rpp][rd][res]) for res in LIST_RES], color=DICT_COLOR[rpp], marker=DICT_MARKERS[rd], linestyle=DICT_LINE[sqrt_s])#, label=f"WS:{ws}, MaxFeat:{feat}, Threads:{thread}")      
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Résolution [pixels]")
plt.ylabel("Gain CPP CPU/GPU")
plt.legend(LEGEND_ELEMENTS, LEGEND_LABELS)
plt.show()