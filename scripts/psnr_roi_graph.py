import os
import subprocess
import matplotlib.pyplot as plt
import sys

psnr_plot=[]
x_plot=[]

def isfloat(value):
  try:
    float(value)
    return True
  except:
    return False
if len(sys.argv) != 5:
    print("arg1: input_name;\narg2: output_dir;\narg3: check_name;\narg4: mask_name.")
    exit()
input_name = sys.argv[1]
output_dir = sys.argv[2]
check_name = sys.argv[3]
mask_name = sys.argv[4]

log_path = output_dir + "/" + input_name + "/log/"

#subprocess.call(['rm','-rf', output_dir + "/" + input_name])
print("Name: " + input_name)
for bitrate in range(10, 1000, 100):
    #print("Compress bitrate=" + str(bitrate) + "kbit/s - start")
    #subprocess.call(['bash','compress_by_bitrate.sh', input_name, str(bitrate)])
    subprocess.call(['bash','psnr_roi.sh', check_name, output_dir + "/" + input_name + "/videos/out" + str(bitrate) + "k.mkv", mask_name, log_path + str(bitrate) + "k.txt"])
    file = open(log_path + str(bitrate)+ "k.txt")
    string = file.read();
    string = string.replace(":", " ")
    for s in string.split():
        if isfloat(s): 
            x_plot.append(bitrate)
            psnr_plot.append(float(s))
            print(input_name + " PSNR[", bitrate, "]: ", s)
            break
csv_file = open(output_dir + "/" + input_name + "_roi.csv", "w")

for x in x_plot:
    csv_file.write("%d," % x)

csv_file.write("\n")

for y in psnr_plot:
    csv_file.write("%d," % y)

#plt.plot(x_plot, psnr_plot)
#plt.grid(True)
#plt.show()
