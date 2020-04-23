#include "server.h"

	static const int _uri_buf_size = 400;		// Buffer size for uri
	static const int _html_file_path_buf_size = SRV_MAX_HTML_ROOT_PATH + 1 + _uri_buf_size + 1;

	static const int _file_to_serve_buf_size = 10000;
	static char _file_to_serve_buf[ _file_to_serve_buf_size+1];

	static constexpr int _max_response_size = 99999999;

	static const char _http_ok_header[] = "HTTP/1.1 200 OK\r\n\r\n";
	static const char _http_not_found_header[] = "HTTP/1.1 404 Not found\r\n\r\n";
	static const char _http_failed_to_serve_header[] = "HTTP/1.1 501 Internal Server Error\r\nVersion: HTTP/1.1\r\nContent-Length:0\r\n\r\n";
	static const char _http_empty_message[] = "HTTP/1.1 200 OK\r\nContent-Length:0\r\n\r\n";
	static const char _http_bad_request_message[] = "HTTP/1.1 400 Bad Request\r\nVersion: HTTP/1.1\r\nContent-Length:0\r\n\r\n";
	static const char _http_not_found_message[] = "HTTP/1.1 404 Not Found\r\nVersion: HTTP/1.1\r\nContent-Length:0\r\n\r\n";
	static const char _http_failed_to_serve_message[] = "HTTP/1.1 501 Internal Server Error\r\nVersion: HTTP/1.1\r\nContent-Length:0\r\n\r\n";

	static const char _http_ok_json_header[] = "HTTP/1.1 200 OK\r\nContent-Type:application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
	static const char _invalid_request_json[] = "{\"error\":\"Invalid request\"}";
	static const char _error_json[] = "{\"error\":\"An error occured...\"}";
	static const char _http_logged_in_json_template[] = 
		"HTTP/1.1 200 OK\r\nContent-Type:text/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n{\"error\":\"\",\"sess_id\":\"%s\",\"user\":\"%s\"}";
	static const char _http_login_error_json_message[] = 
		"HTTP/1.1 200 OK\r\nContent-Type:application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n{\"error\":\"Invalid login or password\",\"sess_id\":\"\"}";
	static const char _http_logged_out_json_message[] = 
		"HTTP/1.1 200 OK\r\nContent-Type:text/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n{\"error\":\"\",\"sess_id\":\"\",\"user\":\"\"}";
	static const char _http_log_out_failed_json_message[] = 
		"HTTP/1.1 200 OK\r\nContent-Type:text/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n{\"error\":\"Failed to log out. Not authorized?\"}";
	static const char _http_auth_error_json_message[] = 
		"HTTP/1.1 200 OK\r\nContent-Length:26\r\nContent-Type:application/json\r\nAccess-Control-Allow-Origin: *\r\n\r\n{\"error\":\"Not authorized\"}";

	static const char _http_ok_options_header[] = 
		"HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
		"Access-Control-Allow-Headers: Content-Type\r\n\r\n";

	#define ALLOWED_URI_NUM 4
	static char *_allowed_uri[] = { "/", "/index.html", "/index_bundle.js", "/favicon.ico" }; 

	#define MAX_ID 100
	#define MAX_USER 100
	#define MAX_PASS 100

	static void readHtmlFileAndPrepareResponse( char *file_name, char *html_source_dir, bool is_browser_request, ResponseWrapper &response );

	static void requestSpider( callback_ptr _callback, char *user, char *post, ResponseWrapper &response, ServerDataWrapper &sdw );


	void server_response( int client_socket, char *socket_request_buf, int socket_request_buf_size, 
		char *html_source_dir, char **users_and_passwords, callback_ptr callback )
	{
		char uri[_uri_buf_size+1];
		char *post;
		bool get;
		bool options;
		int uri_status = get_uri_to_serve(socket_request_buf, socket_request_buf_size, uri, _uri_buf_size, &get, &post, &options);

		if (uri_status != 0) { 	// Failed to parse uri - closing socket...
			return;
		}

		server_error_message("server: requested uri=" + std::string(uri) );

		if( strcmp(uri,"/check_connection") == 0 ) { 	// A system message simply to check availability of the server.
			send(client_socket, _http_empty_message, strlen(_http_empty_message), 0);
			return;
		}				

		if( options ) { 	// An OPTIONS request - allowing all
			server_error_message( std::string("server: sending OPTIONS:\n") + std::string(_http_ok_options_header) );
			send(client_socket, _http_ok_options_header, strlen(_http_ok_options_header), 0);
			return;
		}

		ResponseWrapper response;
		ServerDataWrapper sdw;

		if( strcmp( uri, "/api_list" ) == 0 ) { 	// A GET request to serve the list of API function
			requestSpider( callback, nullptr, nullptr, response, sdw );
		}
		else if( get ) 
		{ 	// A GET request from a browser
			if( strcmp( uri, "/" ) == 0 || strcmp(uri, "/index") == 0 ) {
				strcpy( uri, "/index.html" );
				server_error_message( "Redirected to " + std::string( uri ) );
			}
			bool allowed = false;
			for( int i = 0 ; i < ALLOWED_URI_NUM ; i++ ) {
				if( strcmp( _allowed_uri[i], uri ) == 0 ) {
					allowed = true; 
					break;
				}
			} 
			if( !allowed ) {
				send(client_socket, _http_bad_request_message, strlen(_http_bad_request_message), 0);
				return;
			}

			try {
				readHtmlFileAndPrepareResponse( &uri[1], html_source_dir, true, response );
			}
			catch (...) {
				server_error_message( "Failed to create a response..." );
				send(client_socket, _http_failed_to_serve_message, strlen(_http_failed_to_serve_message), 0);
				return;
			}
		}
		else 	// Post != nullptr - an API entry is requested
		{ 	
			char id[ MAX_ID+1 ];
			char user[ AUTH_USER_MAX_LEN+1 ];
			char pass[ MAX_PASS+1 ];
			char sess_id[ AUTH_SESS_ID_LEN+1 ];
			parseJSON(post, user, AUTH_USER_MAX_LEN, pass, MAX_PASS, sess_id, AUTH_SESS_ID_LEN, id, MAX_ID);				

			if( strcmp( id, "login" ) == 0 ) { 	// A login try ? 
				char *auth_sess_id = auth_do(user, pass, users_and_passwords);
				if( auth_sess_id != nullptr ) { 	// Login try ok - sending sess_id 	
					static char buf[ sizeof(_http_logged_in_json_template) + AUTH_SESS_ID_LEN + AUTH_USER_MAX_LEN + 1 ];
					sprintf( buf, _http_logged_in_json_template, auth_sess_id, user );
					send(client_socket, buf, strlen(buf), 0);
				} else { 	// Login try failed - sending 0 bytes
					send(client_socket, _http_login_error_json_message, strlen(_http_login_error_json_message), 0);
				}
				return;
			}
			if( strcmp( id, "logout" ) == 0 ) { 	// A logout try ? 
				if( auth_logout(user, sess_id) ) {
					send(client_socket, _http_logged_out_json_message, strlen(_http_logged_out_json_message), 0);
				} else {
					send(client_socket, _http_log_out_failed_json_message, strlen(_http_log_out_failed_json_message), 0);
				}
				return;
			}

			// Confirming a valid user
			char *user_ptr = nullptr;
			if( strlen(user) > 0 && strlen(pass) > 0 ) { 	// If user and password has been received...
				if( auth_check_user_and_password( user, pass, users_and_passwords ) ) {
					user_ptr = user;
				}
			} else if( strlen(sess_id) > 0 ) { 	// If a user and a session id has been reveived...
				if( auth_confirm(user, sess_id) ) {
					user_ptr = auth_get_user();
				}
			}
			if( user_ptr == nullptr ) {
				send(client_socket, _http_auth_error_json_message, strlen(_http_auth_error_json_message), 0);
				return;
			}
			requestSpider( callback, user_ptr, post, response, sdw );
		}

		int header_sending_result = send(client_socket, response.header, strlen(response.header), 0);
		if (header_sending_result == SOCKET_ERROR) { 	// If error...
			server_error_message( "header send failed: " + std::to_string( WSAGetLastError() ) );
		} else {
			server_error_message( std::string("**** server :\nresponse header sent:\n") + std::string(response.header) + std::string("\n") );
			const char *body_ptr = nullptr;
			if( response.body != nullptr ) {
				body_ptr = response.body;
			} else if( response.body_allocated != nullptr ) {
				body_ptr = response.body_allocated;
			}
			if (body_ptr != nullptr) {
				// Sending the file to client...
				int body_sending_result = send(client_socket, body_ptr, response.body_len, 0);
				if (body_sending_result == SOCKET_ERROR) { 	// If error...
					server_error_message( "send failed: " + std::to_string( WSAGetLastError() ) );
				} else {
					server_error_message("**** server:\nresponse body sent:\n" +std::string(body_ptr) + "\n" );
				}
			}
		}
	}


	static void requestSpider( callback_ptr callback, char *user, char *post, ResponseWrapper &response, ServerDataWrapper &sdw )
	{
		int callback_return; 	// 
		sdw.sd.user = user;
		if( post == nullptr ) {
			sdw.sd.message_id = SERVER_GET_API_LIST;
		} else {			
			sdw.sd.message_id = SERVER_SERVE_API_REQUEST;
		}
		sdw.sd.message = post;
		sdw.sd.sp_response_buf = nullptr;
		callback_return = callback( &sdw.sd );

		if( callback_return < 0 || sdw.sd.sp_response_buf_size == 0 ) { 	// An error 
			response.header = _http_ok_json_header;
			response.body = _invalid_request_json;
			response.body_len = strlen(_invalid_request_json);
		} 
		else { 	// Ok
			if( sdw.sd.sp_response_file ) {
				readHtmlFileAndPrepareResponse( sdw.sd.sp_response_buf, nullptr, false, response );
				return;
			}
			response.header = _http_ok_json_header;			
			response.body = sdw.sd.sp_response_buf;
			response.body_len = sdw.sd.sp_response_buf_size;
		}
	}


	static void readHtmlFileAndPrepareResponse( char *file_name, char *html_source_dir, bool is_browser_request, ResponseWrapper &response )
	{
		// If none of the above - serving a file
		char file_path[ _html_file_path_buf_size ];
		
		if( html_source_dir != nullptr ) {
			strcpy( file_path, html_source_dir );
		} else {
			file_path[0] = '\x0';
		}
		strcat( file_path, file_name );
		server_error_message( "Opening: " + std::string(file_path) );
	
		enum class FileServingErr { ok, file_not_found, failed_to_serve };
		FileServingErr error = FileServingErr::failed_to_serve;

		std::ifstream fin(file_path, std::ios::in | std::ios::binary);
		if (fin) {
			server_error_message( "Opened" );

			// Reading http response body
			fin.seekg(0, std::ios::end);
			long int file_size = static_cast<long int>(fin.tellg());
			fin.seekg(0, std::ios::beg);

			if( file_size > 0 ) {
				if( file_size <= _file_to_serve_buf_size ) { 	// The static buffer is big enough...
					fin.read(_file_to_serve_buf, file_size); 
					if( fin.gcount() == file_size ) {
						response.header = _http_ok_header;
						response.body = _file_to_serve_buf;
						response.body_len = file_size;
						error = FileServingErr::ok;
					} 
				} else if( file_size < _max_response_size ) {
					try { 
						response.body_allocated = new char[file_size + 1];
					} catch(...) { ; }	
					if( response.body_allocated != nullptr ) {
						fin.read(response.body_allocated, file_size); 	// Adding the file to serve
						if( fin.gcount() == file_size ) {
							response.header = _http_ok_header;
							response.body_len = file_size; 
							error = FileServingErr::ok;
						} 
					}
				}
			}
			fin.close();
		} else {
			error = FileServingErr::file_not_found;
		}

		if( error != FileServingErr::ok ) {
			if( is_browser_request ) {
				if( error == FileServingErr::file_not_found )
					response.header = _http_not_found_header;
				else 
					response.header = _http_failed_to_serve_header;
			} else {
				response.header = _http_ok_header;
				if( error == FileServingErr::file_not_found ) {
					response.body = _invalid_request_json;
					response.body_len = strlen(_invalid_request_json);
				}
				else {	
					response.body = _error_json;
					response.body_len = strlen(_error_json);
				}
			} 
		}
	}

