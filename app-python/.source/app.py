import sys, requests, numpy as np, datetime as dt
import matplotlib.pyplot as plt, matplotlib.dates as mdates
from mpl_toolkits.mplot3d import Axes3D 

print('Requesting data...')
r = requests.post( 'http://localhost:8080', json={'command':'graphs'} )
print(r.headers)
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()

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
