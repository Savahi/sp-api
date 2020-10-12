# -*- coding: utf-8 -*-
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

print('Requesting table...')
r = requests.post( url, json={ 'command':'getTable', 'tableHandle':str(tableHandle), 'sessId':'' } )
print('Done!') if r.status_code == 200 else sys.exit(0)
r = r.json()


from datetime import datetime as dt
import plotly.express as px
import pandas as pd

df = pd.DataFrame( r['array'] )

def convert_date(dts):
	dto = dt.strptime(dts, '%d.%m.%y  %H:%M')
	return dto.strftime('%Y-%m-%d %H:%M')

df = pd.DataFrame( r['array'] )
df['Level'] = df['Level'].fillna(0)
df['Start'] = df['Start'].apply(convert_date)
df['Fin'] = df['Fin'].apply(convert_date)
fig = px.timeline(df, x_start="Start", x_end="Fin", y='Name', color="Name")


def generate_table(dataframe, max_rows=10):
	columns = [ 'Name', 'Start', 'Fin' ]
	return html.Table(
		# Header
		[html.Tr([html.Th(col) for col in columns ])] +
		# Body
		[html.Tr([
			html.Td(dataframe.iloc[i][col]) for col in columns
		]) for i in range(min(len(dataframe), max_rows))]
	)
	
import dash
import dash_core_components as dcc
import dash_html_components as html
import plotly.graph_objs as go

app = dash.Dash(__name__)
colors = {
	'bg': '#efefef',
	'header': '#7f7f7f',
	'text':'#4f4f4f',
	'plot':'#ffffff'
}

app.layout = html.Div(style={'backgroundColor': colors['bg']}, children=[
	html.H1(children = 'DASHBOARD',
		style={
			'textAlign': 'center', 'color': colors['header']
		}
	),
	
	html.Div(children = 'A web-app dashboard extension for SP.', style={
		'textAlign': 'center', 'color': colors['text']
	}),
	
	dcc.Graph(figure = fig),

	html.H4(children='A Table'),
	generate_table(df)
])

if __name__ == '__main__':
   app.run_server(debug=True)
