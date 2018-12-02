import matplotlib.pyplot as plt
import matplotlib.markers as marks
import numpy as np
import sys
import argparse

def readCSV(filepath):
    file = open(filepath)
    str1 = file.read()
    x_plot = str1.split('\n')[0].replace(',', ' ').split(' ')[:-1]
    y_plot = str1.split('\n')[1].replace(',', ' ').split(' ')[:-1]
    #print(x_plot)
    #print(y_plot)
    x_plot = [float(i) for i in x_plot]
    y_plot = [float(i) for i in y_plot]
    #print("x: " + str(x_plot))
    #print("y: " + str(y_plot))
    pair = [x_plot, y_plot]
    return pair


#   ================    ===============================
#   character           description
#   ================    ===============================
styles=[
    '-',             #solid line style
    '--',            #dashed line style
    '-.',            #dash-dot line style
#    ':',             #dotted line style
#    '.',             #point marker
#    ',',             #pixel marker
#    'o',             #circle marker
    '-v',             #triangle_down marker
    '-^',             #triangle_up marker
    '-<',             #triangle_left marker
    '->',             #triangle_right marker
    '1',             #tri_down marker
    '2',             #tri_up marker
    '3',             #tri_left marker
    '4',             #tri_right marker
    's',             #square marker
    'p',             #pentagon marker
    '*',             #star marker
    'h',             #hexagon1 marker
    'H',             #hexagon2 marker
    '+',             #plus marker
    'x',             #x marker
    'D',             #diamond marker
    'd',             #thin_diamond marker
    '|',             #vline marker
    '_',             #hline marker
    'ro'
]
#   ================    ===============================
print(styles)
####################################################################
#                          Input data
####################################################################

parser = argparse.ArgumentParser(description='Plot master.')
parser.add_argument('plots', metavar='N', nargs='+',
                    help='name of files which plot')
parser.add_argument('--xlabel', dest='xlabel', default="x", #"Bitrate(kbit/s)",
                    help='xlabel for PyPlot')
parser.add_argument('--ylabel', dest='ylabel', default="y", #"PSNR_{ROI}  (dB)",
                    help='ylabel for PyPlot')
parser.add_argument('--style', dest='style',
                    help='style for PyPlot')
parser.add_argument('--title', dest='title', default="Figure",
                    help='title for PyPlot')

args = parser.parse_args()

print("plots:  " + str(args.plots))
print("xLabel: " + args.xlabel)
print("yLabel: " + args.ylabel)
print("title:  " + args.title)

plots = []
legends = []
for i in range(0, len(args.plots)):
    plots.append(readCSV(args.plots[i]))
    legends.append(args.plots[i].replace('/','.').split('.')[-2])

#plots
i = 0
for p in plots:
    #xnew = np.linspace(min(p[0]), max(p[0]), 1000)
    #power_smooth = BSpline(p[0], p[1], xnew)
    #print(power_smooth)
    plt.plot(p[0], p[1], args.style if (args.style) else styles[++i], linewidth=2)
    i=i+1
plt.title(args.title)
plt.ylabel(r'$\mathtt{' + args.ylabel + '}$', fontsize=20)
plt.xlabel(r'$\mathtt{' + args.xlabel + '}$', fontsize=20)
plt.legend(legends, loc='lower right')
plt.grid()
plt.show()

