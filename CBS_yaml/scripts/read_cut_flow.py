 #!/usr/bin/env python
import os
import sys

from pylab import *
from scipy.stats import gaussian_kde

analysis_name = "atlas_conf_2019_040"
norm = 1763

if len(sys.argv)>1:
    name1 = sys.argv[1]
else:
    stop
if len(sys.argv)>2:
    name2 = sys.argv[2]
else:
    name2 = "2j_1600"

f_data = open(name1+'/Output/SAF/defaultset/'+analysis_name+'/Cutflows/SR'+name2+'.saf','r')

var=[]
while True:
      plot_line = f_data.readline()
      if not plot_line :
        break
      plot_line = map(str,plot_line.split())
      if len(plot_line)>3 and plot_line[3] == 'nentries' :
        var.append(float(plot_line[0]))

var = np.array(var) / var[1] * norm
for i in var:
  print i



