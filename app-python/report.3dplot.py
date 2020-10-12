import sys, requests, numpy as np
from datetime import datetime as dt
import matplotlib.pyplot as plt, matplotlib.dates as mdates
from mpl_toolkits.mplot3d import Axes3D 

url = 'http://localhost:8080'
fname = 'C:\\Program Files (x86)\\Spider Project\\Professional\\Projects\\investment.002.sprj'

print('!Opening the file...')
r = requests.post( url, json={'command':'openFile', 'fileName':fname} )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
docHandle = r['docHandle'] 
print("doc handle=", docHandle)

print('Getting a phase handle...')
r = requests.post( url, json={'command':'getObjectHandle', "docHandle":str(docHandle), "table":"GanttAct", "code":"Inv1", "structCode":"struct_main" } )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
phaseHandle = r['objectHandle']

print('Getting a template handle...')
r = requests.post( url, json={'command':'getObjectHandle', "docHandle":str(docHandle), "table":"ReportSpend", "code":"app2" } )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
print(r)
templateHandle = r['objectHandle']

print('Getting a graph...')
r = requests.post( url, json={'command':'getReport', "objectHandle":str(phaseHandle), "templateHandle":str(templateHandle)} )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
print(r)

'''
graphs_number = len(r['graphs'])
fig, ax = plt.subplots(graphs_number, 1)
fig.suptitle(r['project']['name'])

text = r['object']['name'] + '\n' + dt.fromtimestamp(r['project']['curTime']).strftime('%Y-%m-%d %H:%M') + '\n'  
text += "Ver.: " + str(r['project']['version']) + '\n' 
text += r['template']['name'] + '\n' + r['template']['title']
plt.text(10, 10, text,	ha='left', va='bottom', transform=None )

for i in range(graphs_number):
	xy = np.array(r['graphs'][i]['array'])
	ax[i].plot([ dt.fromtimestamp(x) for x in xy[:,0] ], xy[:,1], 'o-')
	ax[i].set_ylabel(r['graphs'][i]['name'])
	ax[i].xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
ax[-1].set_xlabel('Время')
'''

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
text = r['object']['name'] + '\n' + dt.fromtimestamp(r['project']['curTime']).strftime('%Y-%m-%d %H:%M') + '\n'  
text += "Ver.: " + str(r['project']['version']) + '\n' 
text += r['template']['name'] + '\n' + r['template']['title']
fig.text(10, 10, text,	ha='left', va='bottom', transform=None )

fig.suptitle(r['project']['name'])
colors = ['r','g','b','y']
yticks=[]
yticklabels=[]
for i in range( len( r['graphs'] ) ):
	xy = np.array(r['graphs'][i]['array'])
	#ax.bar( mdates.date2num([ dt.fromtimestamp(x) for x in xy[:,0] ]), xy[:,1], zs=i, zdir='y', color=colors[i%4], alpha=0.8 )	
	ax.plot( mdates.date2num([ dt.fromtimestamp(x) for x in xy[:,0] ]), xy[:,1], zs=i, zdir='y', color=colors[i%4], alpha=0.8 )	
	yticks.append(i)
	yticklabels.append(r['graphs'][i]['name'])

ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))

#ax.set_xlabel('X')
#ax.set_ylabel('Y')
#ax.set_zlabel('Z')
ax.set_yticks(yticks)
ax.set_yticklabels(yticklabels)
plt.show()

plt.show()