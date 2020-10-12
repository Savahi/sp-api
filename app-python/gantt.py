import sys, requests

url = 'http://localhost:8080'
fname = 'C:\\Program Files (x86)\\Spider Project\\Professional\\Projects\\building.002.sprj'
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
tableHandle = r['tableHandle'] 

# {"command":"getObject","tableHandle":"18701512"}
print('Requesting table...')
r = requests.post( url, json={ 'command':'getTable', 'tableHandle':str(tableHandle), 'sessId':'' } )
#print(r.content)
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()
#print(r)

import plotly.express as px
import pandas as pd
from datetime import datetime as dt

def convert_date(dts):
	dto = dt.strptime(dts, '%d.%m.%y  %H:%M')
	return dto.strftime('%Y-%m-%d %H:%M')

df = pd.DataFrame( r['array'] )
df['Level'] = df['Level'].fillna(0)
df['Start'] = df['Start'].apply(convert_date)
df['Fin'] = df['Fin'].apply(convert_date)
fig = px.timeline(df, x_start="Start", x_end="Fin", y='Name', color="Name")
fig.show()
