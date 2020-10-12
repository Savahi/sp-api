import matplotlib.pyplot as plt
import matplotlib.dates as md
from datetime import datetime as dt

datestrings = [ 939600000, 940204800, 940809600, 941414400 ]
dates = [dt.fromtimestamp(s) for s in datestrings]
print(dates)
plt_data = range(5,9)
plt.subplots_adjust(bottom=0.2)
plt.xticks( rotation=25 )

ax=plt.gca()
ax.set_xticks(dates)

xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
ax.xaxis.set_major_formatter(xfmt)
plt.plot(dates,plt_data, "o-")
plt.show()

'''

import sys, requests, numpy as np, datetime as dt
import matplotlib.pyplot as plt, matplotlib.dates as mdates

text = 	'object' + '\n' + '\n' + 'template';
plt.text(0, 0, text, ha='left', va='bottom', transform=None )
plt.show()


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
colors = ['r','g','b','y']
yticks=[]
yticklabels=[]
for i in range( len( r['graphs'] ) ):
	xy = np.array(r['graphs'][i]['array'])
	ax.bar( mdates.date2num([ dt.datetime.fromtimestamp(x) for x in xy[:,0] ]), xy[:,1], zs=i, zdir='y', color=colors[i%4], alpha=0.8 )	
	ax.plot( mdates.date2num([ dt.datetime.fromtimestamp(x) for x in xy[:,0] ]), xy[:,1], zs=i, zdir='y', color=colors[i%4], alpha=0.8 )	
	yticks.append(i)
	yticklabels.append(r['graphs'][i]['name'])

ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))

#ax.set_xlabel('X')
#ax.set_ylabel('Y')
#ax.set_zlabel('Z')
ax.set_yticks(yticks)
ax.set_yticklabels(yticklabels)
plt.show()
'''