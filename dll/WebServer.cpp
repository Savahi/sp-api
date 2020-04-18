//#include "Stdafx.h"
//#pragma hdrstop

// SampleApp.cpp

//#include <stdio.h>
//#include <windows.h>
#include <string>
//#include <iostream>
//#include <thread>
//#include "Globals.hpp"
#include <iostream>
#include <fstream>
#include "windows.h"
#include "WebServer.hpp"
//#include "test.h"

//#pragma comment(lib, "user32.lib")

//typedef int (*TEST_DLL_TEST)(void);
//TEST_DLL_TEST ptest;

static SERVER_DLL_START p_server_start;

using namespace std;

char _callback_error_code;
#define RESPONSE_BUFFER 100000
char _callback_response[RESPONSE_BUFFER+1];

int callback ( ServerData *sd ) {

  if( sd->message_id == SERVER_NOTIFICATION_MESSAGE ) { // Simply to inform SP about smth.
    printf("********\n%s\n", sd->message);
    return 0;
  } else {
	printf("********\nA MESSAGE FROM SERVER TO SP:\n");
    printf("ID: %d\n", sd->message_id);
	if( sd->user != nullptr ) {	
    	printf("USER: %s\n", sd->user);
	} else {
    	printf("USER: nullptr\n");
	}
	if( sd->message != nullptr ) {	
    	printf("MESSAGE: %s\n", sd->message);
	}
  }

  _callback_error_code = -1;
  _callback_response[0] = '\x0';
  sd->sp_response_buf = _callback_response;
  sd->sp_free_response_buf = false;

  if( sd->message_id == SERVER_GET_API_LIST ) {

		std::ifstream fin("files\\api_list.json", std::ios::in | std::ios::binary);
		if (fin) {
			// Reading http response body
			fin.seekg(0, std::ios::end);
			uintmax_t file_size = fin.tellg();
			fin.seekg(0, std::ios::beg);

			if( file_size < RESPONSE_BUFFER ) {
				fin.read(_callback_response, file_size); 	// Adding the file to serve
			    sd->sp_response_buf_size = file_size;
				_callback_error_code = 0;
			}
			fin.close();
		}   
  }
  return _callback_error_code;
}

//#include <mutex>
//std::mutex mtx;

//int main(int argc, char **argv )
//static int message=0;

static char* users_and_passwords [] = { "admin", "admin", 0};
char* MPath = "C:\\Users\\lgirs\\Desktop\\papa\\spider\\server-api\\dll";
static StartServerData Data;

//void StartWebServer (int argc, char** argv)
int main (int argc, char** argv)
{
  HINSTANCE hServerDLL;

  Data.IP = "127.0.0.1";
  Data.Port = "8080";
  Data.UsersPasswords = users_and_passwords;
  Data.ExePath = MPath;
  Data.Message = 0;


  /*
  char *default_ip = "127.0.0.1";
  char *ip;
  if (argc > 1) {
    ip = argv[1];
  } else {
    ip = default_ip;
  }
  char *default_port = "8000";
  char *port;
  if (argc > 2) {
    port = argv[2];
  } else {
    port = default_port;
  }
  */

  hServerDLL = LoadLibrary ("server");
  if (hServerDLL != NULL)
  {
    p_server_start = (SERVER_DLL_START) GetProcAddress (hServerDLL, "start");

    if (p_server_start != NULL) {
      //int (*callback_ptr)(ServerData *) = callback;
      //cerr << "Server is about to start!" << endl;

      //p_server_start (ip, port, users_and_passwords, callback_ptr, &message);
      p_server_start (&Data, callback);

      cerr << "The server has started! Press <CTRL-C> to stop the server..."  << endl;
      string s;
      cin >> s;
      //message = 100;
    } else {
      //cerr << "The server has not started!" << endl;
    }
       FreeLibrary(hServerDLL);
  }
   //return 0;
}
