# -*- coding: utf-8 -*-
import sys, requests, numpy as np
from datetime import datetime as dt
import matplotlib.pyplot as plt, matplotlib.dates as mdates

url = 'http://localhost:8080'	# Spider HTTP Server 
fname = 'C:\\Program Files (x86)\\Spider Project\\Professional\\Projects\\01km_exp.008.sprj' 	# Full path to the project

print('!Opening the file...')
r = requests.post( url, json={'command':'openFile', 'fileName':fname} )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
docHandle = r['docHandle'] 
print("doc handle=", docHandle)

print('Getting a phase handle...')
r = requests.post( url, json={'command':'getObjectHandle', "docHandle":str(docHandle), "table":"GanttAct", "index":"0" } )
print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
phaseHandle = r['objectHandle']

print('Getting a template handle...')
r = requests.post( url, json={'command':'getObjectHandle', "docHandle":str(docHandle), "table":"ReportSpend", "code":"TotalCost" } )
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


fig, ax = plt.subplots(1, 1)
fig.suptitle(r['project']['name'])

text = r['object']['name'] + '\n' + dt.fromtimestamp(r['project']['curTime']).strftime('%Y-%m-%d %H:%M') + '\n'  
text += "Ver.: " + str(r['project']['version']) + '\n' 
text += r['template']['name'] + '\n' + r['template']['title']
plt.text(10, 10, text,	ha='left', va='bottom', transform=None )

xy = np.array(r['graphs'][0]['array'])
ax.plot([ dt.fromtimestamp(x) for x in xy[:,0] ], xy[:,1], 'o-')
ax.set_ylabel(r['graphs'][0]['name'])
ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
ax.set_xlabel('Время')

plt.show()