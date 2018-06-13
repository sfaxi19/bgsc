import matplotlib.pyplot as plt
import matplotlib.markers as marks
import numpy as np
import sys

def readCSV(filepath):
    file = open(filepath)
    str1 = file.read()
    x_plot = str1.split('\n')[0].replace(',', ' ').split(' ')[:-1]
    y_plot = str1.split('\n')[1].replace(',', ' ').split(' ')[:-1]
    x_plot = [float(i) for i in x_plot]
    y_plot = [float(i) for i in y_plot]
    print("x: " + str(x_plot))
    print("y: " + str(y_plot))
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
    '_'              #hline marker
]
#   ================    ===============================
print(styles)
####################################################################
#                          Input data
####################################################################
args_len = len(sys.argv)
if(args_len < 2):
    print("Please add plots")
    exit(1)
plots = []
legends = []
for i in range(1, len(sys.argv)):
    plots.append(readCSV(sys.argv[i]))
    legends.append(sys.argv[i].replace('/','.').split('.')[-2])

plots
i = 0
for p in plots:
    #xnew = np.linspace(min(p[0]), max(p[0]), 1000)
    #power_smooth = BSpline(p[0], p[1], xnew)
    #print(power_smooth)
    plt.plot(p[0], p[1], styles[i], linewidth=2)
    i=i+1
#plt.title('Example')
plt.ylabel(r'$\mathtt{PSNR_{ROI}  (dB)}$', fontsize=20)
plt.xlabel(r'$\mathtt{Bitrate(kbit/s)}$', fontsize=20)
plt.legend(legends, loc='lower right')
plt.grid()
plt.show()

