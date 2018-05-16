import subprocess
import sys
import os
import time
import shutil
import argparse


def getBitrate(result):
    res_list = str(result).split('\\n')
    len_res = len(res_list)
    # print("Length res list: " + str(len_res))
    # print(res_list[len_res-1])
    # print('OUT:\n' + str(out))
    lineIdx = 0
    bitrate = 0
    for s in res_list:
        if (s.find('encoded') >= 0):
            # print(str(lineIdx) + ": " + res_list[lineIdx])
            last_str = s.split(' ')
            bitrate = last_str[len(last_str) - 2]
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
            # print(str(lineIdx) + ": " + res_list[lineIdx])
            last_str = s.split(' ')
            psnr = last_str[len(last_str) - 1]
            break
        lineIdx = lineIdx + 1
    return psnr


def x264_process(qp, input_file, output_dir):
    output_file = output_dir + '/' + 'out' + str(qp) + '.mkv'
    print('\noutput_file:' + output_file)
    x264_command = ['x264', '--qp', str(qp), '--bframes', '0', '--ipratio', '1', '--output-csp', 'rgb', '--no-deblock',
                    input_file, '-o', output_file]
    psnr_command = ['bgsc', '-psnr', output_file, psnr_file, mask_file]
    # x264 --qp 1 --ipratio 1 --bframes 0 --output-csp rgb rebuilt/in%d.bmp --no-deblock -o out2.mkv
    x264_cmd = subprocess.Popen(x264_command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    err, out = x264_cmd.communicate()
    bitrate = getBitrate(out)
    time.sleep(5)
    psnr_cmd = subprocess.Popen(psnr_command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out1 = psnr_cmd.communicate()
    # print('Bitrate: ' + str(bitrate))
    psnr = getPSNR(out1)
    # print('PSNR:    ' + str(psnr))
    return bitrate, psnr


####################################################################
#                          Input data
####################################################################
parser = argparse.ArgumentParser(description='\"x264_process\" test suite.')
parser.add_argument('--qp-min', dest='qp_min',
                    type=int,
                    default=0,
                    help='min qp for test suite')
parser.add_argument('--qp-max', dest='qp_max',
                    type=int,
                    default=50,
                    help='max qp for test suite')
parser.add_argument('--qp-step', dest='qp_step',
                    type=int,
                    default=10,
                    help='step for inc qp param')
parser.add_argument('--in-file', dest='in_file',
                    required=True,
                    help='Path to input file(s)')
parser.add_argument('--out-dir', dest='out_dir',
                    required=True,
                    help='Path to dir with input images')
parser.add_argument('--psnr-file', dest='psnr_file',
                    required=True,
                    help='Path to file for check PSNR')
parser.add_argument('--mask-file', dest='mask_file',
                    required=True,
                    help='Path to file for calc PSNR for ROI')

args = parser.parse_args()

qp_min = args.qp_min
qp_max = args.qp_max
qp_step = args.qp_step
input_file = args.in_file
output_dir = args.out_dir
psnr_file = args.psnr_file
mask_file = args.mask_file
####################################################################
#                          Print input data
####################################################################

print("qp_min:     " + str(qp_min))
print("qp_max:     " + str(qp_max))
print("qp_step:    " + str(qp_step))
print("input file:  " + input_file)
print("output dir: " + output_dir)
print("psnr file:  " + psnr_file)
try:
    shutil.rmtree(output_dir)
except:
    print('Fail remove: ' + output_dir)
os.makedirs(output_dir)

x_plot = []
y_plot = []
for qp in range(qp_min, qp_max, qp_step):
    bitrate, psnr = x264_process(qp, input_file, output_dir)
    x_plot.append(float(bitrate))
    y_plot.append(float(psnr))
    print('qp: ' + str(qp) + ' bitrate: ' + str(bitrate) + ' kbit/s PSNR: ' + str(psnr) + ' dB')

##################################
#           Save to CSV
##################################
csv_file = open(output_dir + "/roi.csv", "w")
for x in x_plot:
    csv_file.write("%f," % x)
csv_file.write("\n")
for y in y_plot:
    csv_file.write("%f," % y)
