import re

f = open('api-source.txt', 'r', encoding='utf-8')
if f:
	data = f.read()

	#CLOSE (); - exit.
	#CLOSEALL (SAVE : save);
	entries = re.split(r'[\n]{2}', data);
	#print(entries)

	json = '[\n'
	num_entries = 0

	for e in entries:
		m = re.match( '([A-Za-z0-9]+) *\(([A-Za-z0-9 \,\:]*)\)[ \-\;]*([^\n]*)\n(.*)', e )
		if m:
			n_groups = len(m.groups())
			if not n_groups > 2:
				continue
			e_id = m.groups()[0].strip().lower()

			e_params = m.groups()[1].strip()

			e_title = m.groups()[2].strip()
			if e_title.endswith('.'):
				e_title = e_title[:-1]

			e_description = ''
			if n_groups > 3:  	
				e_description = re.sub( '\s+', ' ', m.groups()[3].strip() )
			params_json = ''
			params_splitted = e_params.split(',')
			for p in params_splitted:
				mp = re.match( '([A-Za-z0-9]+) *\: *([A-Za-z0-9]+)', p )
				if mp:
					if len( mp.groups() ) > 0:
						p = mp.groups()[0]
				if len(params_json) > 0:
					params_json += ','
				params_json += '"%s":""' % (p.strip().lower())				
			if len(params_json) > 0:
				params_json = '"id":"%s",'%(e_id) + params_json
				params_json += ',"user":"admin","sess_id":"<Your session id goes here...>"'
	
			entry_json = '{"id":"%s","title":"%s","description":"%s","sample_request":{%s}}' % (e_id, e_title, e_description, params_json)				
			
			if num_entries > 0:
				json += ',\n'
			json += entry_json
			num_entries += 1
	# end of for
	json += '\n]'

	print(json)

	f.close()