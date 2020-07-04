import requests

url = 'http://localhost:8080'
data = {'id':'login', 'user':'admin', 'pass':'admin'}
r = requests.post( url, json=data, timeout=5 )

print( r.status_code )
print(r)
print( r.json() )