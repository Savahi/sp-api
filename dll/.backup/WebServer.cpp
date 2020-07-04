#include <string>
#include <iostream>
#include <fstream>
#include "windows.h"
#include "WebServer.hpp"

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

    if( sd->message_id == SERVER_API_LIST ) {
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

static StartServerData Data;

int main (int argc, char** argv)
{
    HINSTANCE hServerDLL;

    Data.IP = "127.0.0.1";
    Data.Port = "8000";
    Data.ExePath = nullptr;
    Data.HtmlPath = "html\\";

    hServerDLL = LoadLibrary ("server");
    if (hServerDLL != NULL)
    {
        std::cout << "Starting!" << std::endl;
        Data.Message = ssd_Start;
        p_server_start = (SERVER_DLL_START) GetProcAddress (hServerDLL, "start");

        if (p_server_start != NULL) {
            p_server_start (&Data, callback);
            cerr << "The server has started! Press <ENTER> to stop the server..."  << endl;
            cin.get();
            Data.Message = ssd_Stop;
            p_server_start (&Data, callback);
            cerr << "The server is stopped! Press <ENTER> to exit the program..."  << endl;
            cin.get();
    } else {
      cerr << "The server has not started!" << endl;
    }
    FreeLibrary(hServerDLL);
}
