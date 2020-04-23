#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <string>
#include <fstream>
#include <iostream>

#define _WIN32_WINNT 0x501
#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVER_DLL_EXPORT
#include "WebServer.hpp"

#include "auth.h"
#include "helpers.h"

#define SRV_MAX_EXE_PATH 400
#define SRV_HTML_ROOT_DIR "html\\"
#define SRV_MAX_HTML_ROOT_PATH (SRV_MAX_EXE_PATH + 1 + sizeof(SRV_HTML_ROOT_DIR))

class ResponseWrapper {
	public:

	const char *header;
	const char *body;
	char *body_allocated;
	unsigned int body_len;

	ResponseWrapper(): header(nullptr), body(nullptr), body_allocated(nullptr) {
		;
	}

	~ResponseWrapper() {
		if( body_allocated != nullptr ) {
			delete [] body_allocated;
		}
	}
};

class ServerDataWrapper {
	public:

	ServerData sd;
	
	ServerDataWrapper() {
		sd.user = nullptr;
		sd.message = nullptr;
		sd.sp_response_buf = nullptr;
		sd.sp_free_response_buf = false;
  		sd.sp_response_file = false;
	}

	~ServerDataWrapper() {
		if( sd.sp_free_response_buf == true ) {
			delete [] sd.sp_response_buf;
		}
	}
};


void server_error_message( const std::string &errmsg );

void server_response( int client_socket, char *socket_request_buf, int socket_request_buf_size, 
	char *html_source_dir, char **users_and_passwords, callback_ptr _callback );


#endif
