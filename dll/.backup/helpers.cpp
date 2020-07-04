#include <string.h>
#include <string>
#include "helpers.h"                                                        


	void mimeSetType(char *fn, char *mime_buf, int mime_buf_size) {
		int l = strlen(fn);
		if (l > 4 && (tolower(fn[l-1]) == 's' && tolower(fn[l-2]) == 's' && tolower(fn[l-3]) == 'c' && fn[l-4] == '.')) {
			strcpy(mime_buf, "text/css; charset=utf-8"); 	// .css
		} else if( (l > 5 && tolower(fn[l-1]) == 'n' && tolower(fn[l-2]) == 'o' && tolower(fn[l-3]) == 's' && tolower(fn[l-4]) == 'j' && fn[l-5] == '.') ) {
			strcpy(mime_buf, "text/json; charset=utf-8"); 	// .json
		} else if( (l > 5 && tolower(fn[l-1]) == 'g' && tolower(fn[l-2]) == 'e' && tolower(fn[l-3]) == 'p' && tolower(fn[l-4]) == 'j' && fn[l-5] == '.') ) {
			strcpy(mime_buf, "text/jpeg"); 	// .jpeg
		} else if( (l > 4 && tolower(fn[l-1]) == 'g' && tolower(fn[l-2]) == 'p' && tolower(fn[l-3]) == 'j' && fn[l-4] == '.') ) {
			strcpy(mime_buf, "text/jpeg"); 	// .jpg
		} else if( (l > 4 && tolower(fn[l-1]) == 'g' && tolower(fn[l-2]) == 'n' && tolower(fn[l-3]) == 'p' && fn[l-4] == '.') ) {
			strcpy(mime_buf, "text/png"); 	// .jpg
		} else if( (l > 4 && tolower(fn[l-1]) == 'f' && tolower(fn[l-2]) == 'i' && tolower(fn[l-3]) == 'g' && fn[l-4] == '.') ) {
			strcpy(mime_buf, "text/gif"); 	// .jpg
		} else if( (l > 5 && tolower(fn[l-1]) == 'f' && tolower(fn[l-2]) == 'f' && tolower(fn[l-3]) == 'i' && tolower(fn[l-4]) == 't' && fn[l-5] == '.') ) {
			strcpy(mime_buf, "text/tiff"); 	// .json
		} else if( (l > 4 && tolower(fn[l-1]) == 'f' && tolower(fn[l-2]) == 'i' && tolower(fn[l-3]) == 't' && fn[l-4] == '.') ) {
			strcpy(mime_buf, "text/tif"); 	// .jpg
		} else if( (l > 4 && tolower(fn[l-1]) == 'm' && tolower(fn[l-2]) == 't' && tolower(fn[l-3]) == 'h' && fn[l-4] == '.') ) {
			strcpy(mime_buf, "text/html; charset=utf-8"); 	// .html
		} else if( (l > 4 && tolower(fn[l-1]) == 'l' && tolower(fn[l-2]) == 'm' && tolower(fn[l-3]) == 't' && tolower(fn[l-4]) == 'h' && fn[l-5] == '.' ) ) {
			strcpy(mime_buf, "text/html; charset=utf-8"); 	// .html
		} else if( (l > 4 && tolower(fn[l-1]) == 't' && tolower(fn[l-2]) == 'x' && tolower(fn[l-3]) == 't' && fn[l-4] == '.') ) {
			strcpy(mime_buf, "text/plain; charset=utf-8"); 	// .jpg
		} else {
			strcpy(mime_buf, "text/html; charset=utf-8");
		}
	}

	void create_cookie( char *sessId, char *user, char *cookie_buf, unsigned int cookie_buf_size ) {
		if (sessId == nullptr) {
			sprintf(cookie_buf, "Set-Cookie: sessid=deleted; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\nSet-Cookie: user=Not Authorized; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n");
		}
		else {
			if( strlen(user) == 0 ) {
				sprintf(cookie_buf, "Set-Cookie: sessid=%s; path=/;\r\n", sessId);
			} else {
				sprintf(cookie_buf, "Set-Cookie: sessid=%s; path=/;\r\nSet-Cookie: user=%s; path=/;\r\n", sessId, user);
			}
		}
	}


	int get_uri_to_serve(char *b, int b_len, char *uri_buf, int uri_buf_size, bool *get, char **post, bool *options) 
	{
		*get = false;
		*post = nullptr;
		*options = false;

		// Searching for "GET", "POST" or "OPTIONS"
		int uri_index = -1;

		int i = 0; 	// To skip leading spaces if exist...
		for( ; i < b_len ; i++) {
			if( b[i] != ' ' ) {
				break;
			}
		}

		if( i < b_len - 3 ) {	// Is it a GET?
			if (tolower(b[i]) == 'g' && tolower(b[i + 1]) == 'e' && tolower(b[i + 2]) == 't') {
				uri_index = i+3;
				*get = true;
			}
		}
		if( *get == false && i < b_len - 4 ) { 	// Is it a POST?
			if (tolower(b[i]) == 'p' && tolower(b[i + 1]) == 'o' && tolower(b[i + 2]) == 's' && tolower(b[i + 3]) == 't') {
				uri_index = i+4;
				int post_index = -1;
				for (int j = i+4; j < b_len - 4; j++) {
					if (tolower(b[j]) == '\r' && tolower(b[j+1]) == '\n' && tolower(b[j+2]) == '\r' && tolower(b[j+3]) == '\n') {
						post_index = j + 4;
						*post = &b[post_index];
						break;
					}
				}
				if( post_index == -1 ) { 	// A POST request, but no post data...
					return -1;
				}
			}
		}
		if( *get == false && *post == nullptr && i < b_len - 7 ) { 	// Is it an OPTIONS?
			if (tolower(b[i]) == 'o' && tolower(b[i+1]) == 'p' && tolower(b[i+2]) == 't' && tolower(b[i+3]) == 'i' &&
				tolower(b[i+4]) == 'o' && tolower(b[i+5]) == 'n' && tolower(b[i+6]) == 's' ) {
				uri_index = i+7;
				*options = true;
			}
	
		}
		if( uri_index == -1 ) {
			return -1;
		}

		int first_path_index = -1;
		for (int i = uri_index; i < b_len - 1; i++) {
			if (b[i] == ' ') {
				continue;
			}
			if (b[i] == '/') {
				first_path_index = i;
				break;
			}
		}
		if (first_path_index != -1) {
			int last_path_index = first_path_index + 1;
			for ( ; last_path_index < b_len; last_path_index++) {
				if (b[last_path_index] != ' ' && b[last_path_index] != '\r' && b[last_path_index] != '\n' && b[last_path_index] != '?') {
					continue;
				}
				break;
			}
			int path_len = last_path_index - first_path_index;
			if (first_path_index != -1 && last_path_index != -1 && path_len < uri_buf_size) {
				strncpy_s(uri_buf, uri_buf_size, &b[first_path_index], path_len);
				uri_buf[path_len] = '\x0';
				return 0;
			}
		}
		return -1;
	}


bool is_html_request( char *uri ) {

	int l = strlen(uri);
	if (l > 4 && (tolower(uri[l-1]) == 'm' && tolower(uri[l-2]) == 't' && tolower(uri[l-3]) == 'h' && uri[l-4] == '.')) {
		return true;
	}
	if( (l > 5 && tolower(uri[l-1]) == 'l' && tolower(uri[l-2]) == 'm' && tolower(uri[l-3]) == 't' && tolower(uri[l-4]) == 'h' && uri[l-5] == '.') ) {
		return true;
	}

	return false;
}


static int findFirstIndexAfterSubstringEnds( char *b, int b_len, char *substring ) {
	int r = -1;

	int s_len = strlen(substring);
	for( int i = 0 ; i < b_len ; i++ ) {
		int k = 0;
		bool mismatched = false;
		for( int j = i ; j < b_len-1 && k < s_len ; j++ ) {	
			if( b[j] != substring[k] ) {
				mismatched = true;
				break;
			} 
			k++;
		}		
		if( !mismatched && k == s_len ) {
			r = i + k;
			break;
		}
	}
	return r;
}


static void readJsonValueIntoString( char *b, int b_len, int start_index, char *string_buffer, int string_buffer_size ) {
	int j = 0;

	for( int i = start_index ; i < b_len && j < string_buffer_size ; i++, j++ ) {
		if( b[i] == '"' ) {
			break;
		}
		string_buffer[j] = b[i];
	}
	string_buffer[j] = '\x0';
}


void parseJSON(char *b, char *user, int max_user, char *pass, int max_pass, char *sess_id, int max_sess_id, char *id, int max_id) {
	int b_len = strlen(b);
		
	user[0] = '\x0';
	int user_index = findFirstIndexAfterSubstringEnds( b, b_len, "\"user\":\"" );
	if( user_index != -1 ) {
		readJsonValueIntoString( b, b_len, user_index, user, max_user );
	}				

	pass[0] = '\x0';
	int pass_index = findFirstIndexAfterSubstringEnds( b, b_len, "\"pass\":\"" );
	if( pass_index != -1 ) {
		readJsonValueIntoString( b, b_len, pass_index, pass, max_pass );
	}				

	sess_id[0] = '\x0';
	int sess_id_index = findFirstIndexAfterSubstringEnds( b, b_len, "\"sess_id\":\"" );
	if( sess_id_index != -1 ) {
		readJsonValueIntoString( b, b_len, sess_id_index, sess_id, max_sess_id );
	}				

	id[0] = '\x0';
	int id_index = findFirstIndexAfterSubstringEnds( b, b_len, "\"id\":\"" );
	if( id_index != -1 ) {
		readJsonValueIntoString( b, b_len, id_index, id, max_id );
	}				
}

/*
	static const char _http_header_bad_request[] = "HTTP/1.1 400 Bad Request\r\nVersion: HTTP/1.1\r\nContent-Length:0\r\n\r\n";
	static const char _http_header_not_found[] = "HTTP/1.1 404 Not Found\r\nVersion: HTTP/1.1\r\nContent-Length:0\r\n\r\n";
	static const int _http_header_buf_size = sizeof(_http_header_template) + COOKIE_BUF_SIZE + MIME_BUF_SIZE + 100; 

	void create_http_header_bad_request( char *dst, int dst_buf_size ) {
		if( dst_buf_size > sizeof(_http_header_bad_request) ) {			
			strcpy( dst, _http_header_bad_request );
		} else {
			strcpy( dst, "" );
		}
	}

	void create_http_header_not_found( char *dst, int dst_buf_size ) {
		if( dst_buf_size > sizeof(_http_header_not_found) ) {			
			strcpy( dst, _http_header_not_found );
		} else {
			strcpy( dst, "" );
		}
	}

	static const char _http_header_template[] = "HTTP/1.1 200 OK\r\nVersion: HTTP/1.1\r\n%sContent-Type:%s\r\nContent-Length:%lu\r\n\r\n";

	void create_http_header( char *dst, int dst_buf_size, char *cookie, char *mime, unsigned long int cl ) {
		if( dst_buf_size > sizeof(_http_header_not_found) ) {			
			strcpy( dst, _http_header_not_found );
		} else {
			strcpy( dst, "" );
		}
	}

*/

