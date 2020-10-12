import sys, requests, numpy as np
from datetime import datetime as dt
import matplotlib.pyplot as plt, matplotlib.dates as mdates
from mpl_toolkits.mplot3d import Axes3D 

url = 'http://localhost:8080'
#fname = 'C:\\Program Files (x86)\\Spider Project\\Professional\\Projects\\building.001.sprj'
fname = 'C:\\Program Files (x86)\\Spider Project\\Professional\\Projects\\Investment.002.sprj'

print('!Opening the file...')
r = requests.post( url, json={'command':'openFile', 'fileName':fname, 'sessId':''} )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
docHandle = r['docHandle'] 
print("doc handle=", docHandle)

print('Requesting table handle...')
r = requests.post( url, json={ 'command':'getTableHandle', 'docHandle':str(docHandle), 'table':'GanttAct', 'sessId':'' } )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
print(r)
tableHandle = r['tableHandle'] 

# {"command":"getObject","tableHandle":"18701512"}
print('Requesting table...')
r = requests.post( url, json={ 'command':'getTable', 'tableHandle':str(tableHandle), 'sessId':'' } )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
print(r)


import matplotlib.pyplot as plt
labels = [ x['Name'] for x in r['array'] if 's_sum_A' in x and x['Level'] == '2' ]
sizes = [ round(float(x['s_sum_A']), 1) for x in r['array'] if 's_sum_A' in x and x['Level'] == '2' ]
explode = [ 0 for x in r['array'] if 's_sum_A' in x and x['Level'] == '2' ]
explode[ sizes.index( max(sizes) ) ] = 0.1
 
fig, ax = plt.subplots()
ax.pie(sizes, explode=explode, labels=labels, autopct='%1.1f%%', shadow=True, startangle=90)
ax.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.

text = r['array'][0]['Name'];
text += '\nОбщая стоимость: ' + str(round(float(r['array'][0]['CostTotal'])));
text += '\nФиниш: ' + r['array'][0]['AsapStart'];
starts = [ x['AsapStart'] for x in r['array'] if 's_sum_A' in x and x['Level'] == '2' ]
finishes = [ x['AsapFin'] for x in r['array'] if 's_sum_A' in x and x['Level'] == '2' ]
for i in range(len(sizes)):
	text += '\n' + labels[i] + '. ' + str(sizes[i]) + " руб.  " + str(finishes[i]) 

fig.text(10, 100, text,	ha='left', va='bottom', transform=None )

plt.show()

# get table handle
# get table