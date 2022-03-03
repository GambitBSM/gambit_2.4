 #!/usr/bin/env python
import os
import sys

from pylab import *
from scipy.stats import gaussian_kde

analysis_name = "atlas_conf_2019_040"


if len(sys.argv)>1:
    name1 = sys.argv[1]
else:
    stop
if len(sys.argv)>2:
    name2 = sys.argv[2]
else:
    name2 = "2j_1600"

norm = 1

if name2[0:2] == "2j":
  norm = 1763
elif name2[0:2] == "4j":
  norm = 2562
else:
  norm = 1160


print norm

f_data = open(name1+'/Output/SAF/defaultset/'+analysis_name+'/Cutflows/SR'+name2+'.saf','r')

var=[]
while True:
      plot_line = f_data.readline()
      if not plot_line :
        break
      plot_line = map(str,plot_line.split())
      if len(plot_line)>3 and plot_line[3] == 'nentries' :
        var.append(float(plot_line[0]))

print name2 + "\n"
for i in var:
  print i

print "----------"
norm_var = var[-1] / var[1] * norm
norm_err = np.sqrt(var[-1]) / var[1] * norm
print str(norm_var) + r"$\pm$" +  str(norm_err)


