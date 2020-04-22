#include "server.h"
#include "auth.h"
#include "helpers.h"

using std::cerr;
using std::endl;

static const int _maxExePath = 400;
static char *_exePath = nullptr;

static bool _th_start = false;
static std::thread::native_handle_type _th_handle;

static int server( void );

static callback_ptr _callback;
static char *_ip;
static char *_port;
static char **_users_and_passwords;
static int *_from_sp_message;

int start( StartServerData *ssd, callback_ptr callback ) {
	if( _th_start ) {
		return 0;
	}	

	_exePath = ssd->ExePath;
	if( strlen(_exePath) >= _maxExePath ) {
		return -1;
	}
	_ip = ssd->IP;
	_port = ssd->Port;
	_users_and_passwords = ssd->UsersPasswords;
	_from_sp_message = &ssd->Message;
	_callback = callback;

	std::thread th(server);	
	_th_handle = th.native_handle();
	//th.join(); // Must be removed when integrated with SP
	th.detach();

	_th_start = true;
	return 1;
} 


void error_message( const std::string &errmsg ) {
	const size_t buf_size=2000;
	char buf[buf_size+1];
	ServerData sd;
	sd.message_id = SERVER_NOTIFICATION_MESSAGE;
	if( errmsg.length() > buf_size ) {
		strncpy( buf, errmsg.c_str(), buf_size );
		buf[buf_size] = '\x0';
	} else {
		strcpy( buf, errmsg.c_str() );
	}	
	sd.message = buf;
	_callback(&sd);
}


	static char _html_source_root[] = "html\\";
	static const int _html_source_dir_buf_size = _maxExePath + 1 + sizeof(_html_source_root);
	static char _html_source_dir[_html_source_dir_buf_size + 1]; 			// Root directory for html applications
	static const int _uri_buf_size = 400;		// Buffer size for uri
	static const int _html_file_path_buf_size = _html_source_dir_buf_size + 1 + _uri_buf_size + 1;

	static const char _http_ok_header[] = "HTTP/1.1 200 OK\r\n\r\n";
	static const char _http_not_found_header[] = "HTTP/1.1 404 Not found\r\n\r\n";
	static const char _http_empty_message[] = "HTTP/1.1 200 OK\r\nContent-Length:0\r\n\r\n";
	static const char _http_header_template[] = "HTTP/1.1 200 OK\r\nVersion: HTTP/1.1\r\nContent-Type:%s\r\nContent-Length:%lu\r\n\r\n";
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

	static const int _http_header_buf_size = sizeof(_http_header_template) + MIME_BUF_SIZE + 100; 

	static void readHtmlFileAndPrepareResponse( 
		char *file_name, 	// A file name to serve 
		const char **response_header, 	// A buffer for response header of size _http_header_buf_size
		const char **response_body, 	// Pointer to the menory allocated for response (header+body), must be freed with "del"
		bool *free_response_body ) 	// If true, response_body must be freed with del
	{
		char *response_buf;

		// If none of the above - serving a file
		char file_path[ _html_file_path_buf_size ];
		
		strcpy( file_path, _html_source_dir );
		strcat( file_path, file_name );
		error_message( "Opening: " + std::string(file_path) );
		
		std::ifstream fin(file_path, std::ios::in | std::ios::binary);
		if (fin) {
			error_message( "Opened" );

			// Reading http response body
			fin.seekg(0, std::ios::end);
			uintmax_t file_size = fin.tellg();
			fin.seekg(0, std::ios::beg);

			response_buf = new char[file_size + 1];
			fin.read(response_buf, file_size); 	// Adding the file to serve
			fin.close();

			*response_header = _http_ok_header;
			*response_body = response_buf;
			*free_response_body = true;
			return;
		}
		error_message("Failed to open...");
		*response_header = _http_not_found_header;
		*response_body = nullptr;
		*free_response_body = false;
	}


	static void requestSpider( char *user, char *post, const char **response_header, const char **response_body, bool *free_response_body ) 
	{
		int callback_return; 	// 
		ServerData sd;
		sd.user = user;
		if( post == nullptr ) {
			sd.message_id = SERVER_GET_API_LIST;
		} else {			
			sd.message_id = SERVER_SERVE_API_REQUEST;
		}
		sd.message = post;
		sd.sp_response_buf = nullptr;
		callback_return = _callback( &sd );

		if( callback_return < 0 || sd.sp_response_buf_size == 0 ) { 	// An error 
			*response_header = _http_ok_json_header;
			*response_body = _invalid_request_json;
			*free_response_body = false;
		} 
		else { 	// Ok
			*response_header = _http_ok_json_header;
			*response_body = sd.sp_response_buf;
			*free_response_body = sd.sp_free_response_buf;
		}
	}


static const int _socket_request_buf_size = 1024*5000;
static char _socket_request_buf[_socket_request_buf_size + 1];

#define ALLOWED_URI_NUM 4
static char *_allowed_uri[] = { "/", "/index.html", "/index_bundle.js", "/favicon.ico" }; 

#define MAX_ID 100
#define MAX_USER 100
#define MAX_PASS 100
#define MAX_SESS_ID AUTH_SESS_ID_LEN

static int server( void )
{
	WSADATA wsaData; //  use Ws2_32.dll
	size_t result;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		error_message( "WSAStartup failed:\n" + std::to_string( result ) );
		return result;
	}

	struct addrinfo* addr = NULL; // holds socket ip etc

	// To be initialized with constants and values...
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET; // AF_INET - using net to work with socket
	hints.ai_socktype = SOCK_STREAM; // A socket of a "stream" type
	hints.ai_protocol = IPPROTO_TCP; // TCP protocol
	hints.ai_flags = AI_PASSIVE; // The socket should take incoming connections

	result = getaddrinfo(_ip, _port, &hints, &addr); // Port 8000 is used
	if (result != 0) { 		// If failed...
		error_message( "getaddrinfo failed:\n" + std::to_string( result ) );
		WSACleanup(); // unloading  Ws2_32.dll
		return 1;
	}
	// Creating a socket
	int listen_socket = socket(addr->ai_family, addr->ai_socktype,
		addr->ai_protocol);
	if (listen_socket == INVALID_SOCKET) { 		// If failed to create a socket...
		error_message( "Error at socket:\n" + std::to_string( WSAGetLastError() ) );
		freeaddrinfo(addr);
		WSACleanup();
		return 1;
	}

	// Binsding the socket to ip-address
	result = bind(listen_socket, addr->ai_addr, (int)addr->ai_addrlen);
	if (result == SOCKET_ERROR) { 		// If failed to bind...
		error_message( "bind failed with error:\n" + std::to_string( WSAGetLastError() ) );
		freeaddrinfo(addr);
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	// Init listening...
	if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
		error_message( "listen failed with error:\n" + std::to_string( WSAGetLastError() ) );
		closesocket(listen_socket);
		WSACleanup();
		return 1;
	}

	int client_socket = INVALID_SOCKET;

	strcpy( _html_source_dir, _exePath);
	strcat( _html_source_dir, "\\" );
	strcat( _html_source_dir, _html_source_root );

	for (;;) {
		// Accepting an incoming connection...
		error_message( "accepting..." );
		client_socket = accept(listen_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET) {
			closesocket(listen_socket);
			WSACleanup();
			return 1;
		}

		if( _from_sp_message != nullptr ) {
			if( *_from_sp_message == 100 ) { 	// 100 is the exit code 
				closesocket(listen_socket);
				WSACleanup();
				_th_start = false;
				return 1;
			}
		}

		result = recv(client_socket, _socket_request_buf, _socket_request_buf_size, 0);

		if (result == SOCKET_ERROR ) { 	// Error receiving data
			cerr << "server: recv failed: " << result << endl;
			closesocket(client_socket);
			continue;
		}
		else if (result == 0) { 		// The connection was closed by the client...
			error_message("server: connection closed by the client...");
			// closesocket(client_socket); // May be unnecessary
			continue;
		} 
		else if( result >= _socket_request_buf_size ) {
			send(client_socket, _http_empty_message, strlen(_http_empty_message), 0);
			closesocket(client_socket);
			continue;
		}

		_socket_request_buf[result] = '\x0';
		error_message( "server [request]:\n" + std::string(_socket_request_buf) + "\nlength=" + std::to_string( result ) );

		char uri[_uri_buf_size+1];
		char *post;
		bool get;
		bool options;
		int uri_status = getUriToServe(_socket_request_buf, uri, _uri_buf_size, &get, &post, &options);

		if (uri_status != 0) { 	// Failed to parse uri - closing socket...
			closesocket(client_socket);
			continue;
		}

		error_message("server: requested uri=" + std::string(uri) );

		if( strcmp(uri,"/check_connection") == 0 ) { 	// A system message simply to check availability of the server.
			send(client_socket, _http_empty_message, strlen(_http_empty_message), 0);
			closesocket(client_socket);
			continue;
		}				

		if( options ) { 	// An OPTIONS request - allowing all
cerr << "server: sending OPTIONS:\n" << _http_ok_options_header;
			send(client_socket, _http_ok_options_header, strlen(_http_ok_options_header), 0);
			closesocket(client_socket);
			continue;
		}

		const char *response_header = nullptr;
		const char *response_body = nullptr;
		bool free_response_body = false;

		if( strcmp( uri, "/api_list" ) == 0 ) { 	// A GET request to serve the list of API function
			requestSpider( nullptr, nullptr, &response_header, &response_body, &free_response_body );
		}
		else if( get ) 
		{ 	// A GET request from browser
			if( strcmp( uri, "/" ) == 0 || strcmp(uri, "/index") == 0 ) {
				strcpy( uri, "/index.html" );
				error_message( "Redirected to " + std::string( uri ) );
			}
			bool found = false;
			for( int i = 0 ; i < ALLOWED_URI_NUM ; i++ ) {
				if( strcmp( _allowed_uri[i], uri ) == 0 ) {
					found = true; 
					break;
				}
			} 
			if( !found ) {
				send(client_socket, _http_bad_request_message, strlen(_http_bad_request_message), 0);
				closesocket(client_socket);
				continue;
			}

			try {
				readHtmlFileAndPrepareResponse( &uri[1], &response_header, &response_body, &free_response_body );
			}
			catch (...) {
				error_message( "Failed to create response..." );
				send(client_socket, _http_failed_to_serve_message, strlen(_http_failed_to_serve_message), 0);
				closesocket(client_socket);
				continue;
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
				char *auth_sess_id = auth_do(user, pass, _users_and_passwords);
				if( auth_sess_id != nullptr ) { 	// Login try ok - sending sess_id 	
					static char buf[ sizeof(_http_logged_in_json_template) + AUTH_SESS_ID_LEN + AUTH_USER_MAX_LEN + 1 ];
					sprintf( buf, _http_logged_in_json_template, auth_sess_id, user );
					send(client_socket, buf, strlen(buf), 0);
				} else { 	// Login try failed - sending 0 bytes
					send(client_socket, _http_login_error_json_message, strlen(_http_login_error_json_message), 0);
				}
				closesocket(client_socket);
				continue;
			}
			if( strcmp( id, "logout" ) == 0 ) { 	// A logout try ? 
				if( auth_logout(user, sess_id) ) {
					send(client_socket, _http_logged_out_json_message, strlen(_http_logged_out_json_message), 0);
				} else {
					send(client_socket, _http_log_out_failed_json_message, strlen(_http_log_out_failed_json_message), 0);
				}
				closesocket(client_socket);
				continue;
			}

			// Confirming a valid user
			char *user_ptr = nullptr;
			if( strlen(user) > 0 && strlen(pass) > 0 ) { 	// If user and password has been received...
				if( auth_check_user_and_password( user, pass, _users_and_passwords ) ) {
					user_ptr = user;
				}
			} else if( strlen(sess_id) > 0 ) { 	// If a user and a session id has been reveived...
				if( auth_confirm(user, sess_id) ) {
					user_ptr = auth_get_user();
				}
			}
			if( user_ptr == nullptr ) {
				send(client_socket, _http_auth_error_json_message, strlen(_http_auth_error_json_message), 0);
				closesocket(client_socket);
				continue;
			}
			requestSpider( user_ptr, post, &response_header, &response_body, &free_response_body );
		}

		result = send(client_socket, response_header, strlen(response_header), 0);
		if (result == SOCKET_ERROR) { 	// If error...
			error_message( "header send failed: " + std::to_string( WSAGetLastError() ) );
		} else {
			cerr << "**** server :\nresponse header sent:\n" << response_header << endl;
			if (response_body != nullptr) {
				// Sending the file to client...
				result = send(client_socket, response_body, strlen(response_body), 0);
				if (result == SOCKET_ERROR) { 	// If error...
					error_message( "send failed: " + std::to_string( WSAGetLastError() ) );
				} else {
					cerr << "**** server:\nresponse body sent:\n" << response_body << endl;
				}
	
				if( free_response_body ) {
					delete[] response_body;
				}
			}
		}
		closesocket(client_socket);
		continue;
	}

	// Closing everything...
	closesocket(listen_socket);
	freeaddrinfo(addr);
	WSACleanup();
	return 0;
}
