# -*- coding: utf-8 -*-
import sys, requests

import dash
import dash_core_components as dcc
import dash_html_components as html
#import plotly.graph_objs as go

app = dash.Dash(__name__)
colors = {
	'bg': '#efefef',
	'header': '#7f7f7f',
	'text':'#4f4f4f',
	'plot':'#ffffff'
}

url = 'http://localhost:8080'
fname = 'C:\\Program Files (x86)\\Spider Project\\Professional\\Projects\\01km_exp.008.sprj'

r = requests.post( url, json={'command':'openFile', 'fileName':fname, 'sessId':''} )
print('Project opened.') if r.status_code == 200 else sys.exit(0)
docHandle = r.json()['docHandle'] 

r = requests.post( url, json={ 'command':'getTableHandle', 'docHandle':str(docHandle), 'table':'GanttAct', 'sessId':'' } )
print('Table handle obtained.') if r.status_code == 200 else sys.exit(0)
tableHandle = r.json()['tableHandle'] 

r = requests.post( url, json={ 'command':'getTable', 'tableHandle':str(tableHandle), 'sessId':'' } )
print('Table content obtained.') if r.status_code == 200 else sys.exit(0)
r = r.json()

from datetime import datetime as dt
import plotly.express as px
import pandas as pd

def convert_date(dts):
	dto = dt.strptime(dts, '%d.%m.%y  %H:%M')
	return dto.strftime('%Y-%m-%d %H:%M')

df = pd.DataFrame( r['array'] )
df['Level'] = df['Level'].fillna(0)
df['Start'] = df['Start'].apply(convert_date)
df['Fin'] = df['Fin'].apply(convert_date)
fig_gantt = px.timeline(df, x_start="Start", x_end="Fin", y='Name', color="Name", title='Диаграмма Гантта')

dfp2 = df.loc[ (df['Level'] == '2') & (pd.notnull(df['c_sum_кс'])) ]
dfp2 = dfp2.reset_index()
dfp2['c_sum_кс'] = dfp2['c_sum_кс'].astype(float).round(0)
dfp2['pie_names'] = dfp2['Name'] + ": " + dfp2['c_sum_кс'].astype(str) + " р."
fig_pie2 = px.pie( dfp2, values='c_sum_кс', names='pie_names', title='Уровень 2' ) 

dfp3 = df.loc[ (df['Level'] == 0) & (pd.notnull(df['c_sum_кс'])) ]
dfp3 = dfp3.reset_index()
dfp3['c_sum_кс'] = dfp3['c_sum_кс'].astype(float).round(0)
dfp3['pie_names'] = dfp3['Name'] + ": " + dfp3['c_sum_кс'].astype(str) + " р."
fig_pie3 = px.pie( dfp3, values='c_sum_кс', names='pie_names', title='Уровень 3' ) 

def generate_table(dataframe, max_rows=10):
	column_names = [ 'Название', 'Начало', 'Окончание', 'Стоимость' ]
	columns = [ 'Name', 'Start', 'Fin', 'c_sum_кс' ]
	return html.Table([		
	  html.Thead(		
		[html.Tr([html.Th(col) for col in column_names ])]
	  ),
	  html.Tbody([
		html.Tr([
			html.Td(dataframe.iloc[i][col]) for col in columns
		]) for i in range(min(len(dataframe), max_rows))
	  ])
	])
	
app.layout = html.Div(style={'backgroundColor': colors['bg']}, children=[
	html.H1(children = r['array'][0]['Name'],
		style={
			'textAlign': 'center', 'color': colors['header']
		}
	),
		
	dcc.Graph(figure = fig_pie2),

	dcc.Graph(figure = fig_pie3),

	dcc.Graph(figure = fig_gantt),

	html.H4(children='Таблица операций'),
	generate_table(df)
])

if __name__ == '__main__':
   app.run_server(debug=True)
