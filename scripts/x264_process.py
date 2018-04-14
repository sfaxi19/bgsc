import subprocess
import sys
import os
import time
import shutil

def getBitrate(result):
    res_list = str(result).split('\\n')
    len_res = len(res_list)
    #print("Length res list: " + str(len_res))
    #print(res_list[len_res-1])
    #print('OUT:\n' + str(out))
    lineIdx = 0
    bitrate = 0
    for s in res_list:
        if (s.find('encoded') >= 0):
            #print(str(lineIdx) + ": " + res_list[lineIdx])
            last_str = s.split(' ')
            bitrate = last_str[len(last_str)-2]
            break
        lineIdx = lineIdx + 1
    return bitrate

def getPSNR(result):
    res_list = str(result).split('\\n')
    len_res = len(res_list)
    lineIdx = 0
    psnr = 0
    for s in res_list:
        if (s.find('PSNR =') >= 0):
            #print(str(lineIdx) + ": " + res_list[lineIdx])
            last_str = s.split(' ')
            psnr = last_str[len(last_str)-1]
            break
        lineIdx = lineIdx + 1
    return psnr

def x264_process(qp, input_dir, output_dir):
    output_file = output_dir + '/' + 'out' + qp + '.mkv'
    print('Output_file:' + output_file)
    x264_command = ['x264', '--crf', qp, '--bframes', '0', input_dir + '/in%d.bmp', '-o', output_file]
    psnr_command = ['./bgsc', '-psnr', output_file, check_psnr_file, mask_file]
    x264_cmd = subprocess.Popen(x264_command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    #psnr_cmd = subprocess.Popen(psnr_command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    err, out = x264_cmd.communicate()
    bitrate = getBitrate(out)
    time.sleep(5)
    psnr_cmd = subprocess.Popen(psnr_command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out1 = psnr_cmd.communicate()
    #print('Bitrate: ' + str(bitrate))
    psnr = getPSNR(out1)
    #print('PSNR:    ' + str(psnr))
    return bitrate, psnr

##################################
#          Input args
##################################
if (len(sys.argv) != 5):
    print("arg1: qp_min")
    print("arg2: qp_max")
    print("arg3: qp_step")
    print("arg4: input_dir")
    exit(1)
##################################
#            Constants
##################################
check_psnr_file = 'check_psnr.avi'
mask_file = 'mask.mkv'
##################################
#            Input data
##################################
qp_min = int(sys.argv[1])
qp_max = int(sys.argv[2])
qp_step = int(sys.argv[3])

input_dir = sys.argv[4]
output_dir = 'x264_process/' + input_dir

print('Input dir:  ' + input_dir)
print('Output dir: ' + output_dir)
try:
    shutil.rmtree(output_dir)
except FileNotFoundError:
    print('fail remove: ' + output_dir)
os.makedirs(output_dir)

csv_file = open("x264_process/" + input_dir + "_roi.csv", "w")
x_plot = []
y_plot = []
for qp in range(qp_min, qp_max, qp_step):
    bitrate, psnr = x264_process(str(qp), input_dir, output_dir)
    x_plot.append(float(bitrate))
    y_plot.append(float(psnr))
    print('qp: ' + str(qp) + ' bitrate: ' + str(bitrate) + ' kbit/s PSNR: '+ str(psnr) + ' dB')

##################################
#           Save to CSV
##################################
for x in x_plot:
    csv_file.write("%f," % x)

csv_file.write("\n")

for y in y_plot:
    csv_file.write("%f," % y)

