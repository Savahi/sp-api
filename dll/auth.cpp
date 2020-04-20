#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <string>
#include <filesystem>
#include <string.h>
#include "auth.h"

	using namespace std;
	
	static int read_sessions(fstream &f);
	static int find_session(char *sess_id);

	uint64_t timeSinceEpochMillisec() {
		using namespace std::chrono;
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	static const uint64_t _sess_inactivity = 30*60*1000;
	static const char *_sess_file_name = "server.sess";
	
	const unsigned long _sess_id_size = AUTH_SESS_ID_BUF_SIZE;
	const unsigned long _sess_user_name_size = AUTH_USER_NAME_BUF_SIZE;
	static char *_sess_id = nullptr;
	static char *_sess_user_name = nullptr;
	
	struct Sess {
		char sess_id[_sess_id_size+1];
		char sess_user_name[_sess_user_name_size+1];
		uint64_t sess_time;
	};

	static const unsigned long int _sess_buf_capacity = 20;
	static Sess _sess_buf[_sess_buf_capacity];


	static const char sid_valid_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXWZ0123456789!@#$%^&*";
	static const int sid_valid_chars_num = sizeof(sid_valid_chars);

	void generate_session_id( char *user, char *sess_id, int sess_id_len ) 
	{
		unsigned int user_sum = 0;
		for( int i = 0 ; i < strlen(user) ; i++ ) {
			int n = static_cast<int>(user[i]);
			user_sum += n + i+1;
		}
		unsigned long int seed = timeSinceEpochMillisec() - user_sum*1000;
	    std::srand(seed);

		for( int i = 0 ; i < sess_id_len ; i++ ) {
	    	int index = std::rand() % sid_valid_chars_num;
			sess_id[i] = sid_valid_chars[index];
		}
		sess_id[sess_id_len] = '\x0';
	}	


	char *auth_get_sess_id( void ) {
		return _sess_id;
	}


	char *auth_get_user_name( void ) {
		return _sess_user_name;
	}


	static void sess_buf_reset() {
		for( int i = 0 ; i < _sess_buf_capacity ; i++ ) {
			strcpy(_sess_buf[i].sess_id, "0");				
			_sess_buf[i].sess_time = 0;				
		}
	}

	static int auth_create(const char *sess_id, char *sess_user_name) {
		int r = AUTH_ERROR_TOO_MANY_SESSIONS;
		fstream f;
		_sess_id = nullptr;
		_sess_user_name = nullptr;

		int status = read_sessions(f);
		if (status >= 0) {
			uint64_t now = timeSinceEpochMillisec();
			int available_index = -1;
			for (int i = 0; i < _sess_buf_capacity; i++) {
				if( strncmp( _sess_buf[i].sess_user_name, sess_user_name, strlen(sess_user_name) ) == 0 ) { // Reauth found
					available_index = i;
					break;
				}
			}
			if( available_index == -1 ) {
				for (int i = 0; i < _sess_buf_capacity; i++) {
					if( (now - _sess_buf[i].sess_time) >= _sess_inactivity ) { // An old session found
						available_index = i;
						break;
					}
				}
			}
			if( available_index != -1 ) {
				strcpy(_sess_buf[ available_index ].sess_id, sess_id);
				strcpy(_sess_buf[ available_index ].sess_user_name, sess_user_name);
				_sess_buf[ available_index ].sess_time = now;
				try {
					f.write( (char*)&_sess_buf[0], sizeof(_sess_buf) );
				} catch(...) {
					r = AUTH_ERROR_FAILED_TO_WRITE_SESSION;
				}
				_sess_id = _sess_buf[ available_index ].sess_id;
				_sess_user_name = _sess_buf[ available_index ].sess_user_name;
				r = 0;
			}
			f.close();
		}
		return r;
	}


	static int auth_delete(char *user, char *sess_id) {
		int r = -1;
		fstream f;
		_sess_id = nullptr;
		_sess_user_name = nullptr;

		int status = read_sessions(f);
		if (status >= 0) {
			for (int i = 0; i < _sess_buf_capacity; i++) {
				if( strncmp( _sess_buf[i].sess_user_name, user, strlen(_sess_buf[i].sess_user_name) ) != 0 ) {
					continue; 
				}
				if( strncmp( _sess_buf[i].sess_id, sess_id, strlen(_sess_buf[i].sess_id) ) != 0 ) {
					continue;
				}
				strcpy(_sess_buf[i].sess_id, "0");
				_sess_buf[i].sess_time = 0;
				try {
					f.write((char*)&_sess_buf[0], sizeof(_sess_buf));
					r = 0;
				} catch(...) {
					r = -2;
				}
				break;
			}
			f.close();
		}
		return r;
	}


	static int auth_confirm_and_update(char *user, char *sess_id, bool b_update_session) {
		_sess_id = nullptr;
		_sess_user_name = nullptr;

		if( sess_id == nullptr ) {
			return -1; 
		}
		if( strlen(sess_id) < 20 ) {
			return -1;
		}
		int r = -1;
		fstream f;	
		int status = read_sessions(f);
		if (status == sizeof(_sess_buf)) { 		// Might be as well simply "if( status > 0)"
//cerr << "SESSION READ" << endl;
			for (int i = 0; i < _sess_buf_capacity; i++) {
				if( strncmp( _sess_buf[i].sess_id, sess_id, strlen(_sess_buf[i].sess_id) ) == 0 ) { 
//cerr << "CMP OK" << endl;
					uint64_t now = timeSinceEpochMillisec();
//cerr << "TIME SINCE: " << (now - _sess_buf[i].sess_time) << endl;
					if (now - _sess_buf[i].sess_time < _sess_inactivity) {
						if( b_update_session ) {
							_sess_buf[i].sess_time = now;
							try {
								f.write((char*)&_sess_buf[0], sizeof(_sess_buf));
								r = i;
							} catch(...) {
								;
							}
						} else {
							r = i;
						}
						_sess_id = _sess_buf[i].sess_id;
						_sess_user_name = _sess_buf[i].sess_user_name;
						break;
					}
				}
			}
			f.close();
		}
		return r;
	}


	static int read_sessions(fstream &f) {
		int r=-1;
		try {
			if (!std::experimental::filesystem::exists(_sess_file_name) ) {
				f.open(_sess_file_name, ios::out | ios::binary);
				sess_buf_reset();
				r = 0;
			} else {
				uintmax_t fsize = std::experimental::filesystem::file_size(_sess_file_name);
				if (fsize != sizeof(_sess_buf) ) { 	// The session file is corruped?
					f.open(_sess_file_name, ios::out | ios::binary | ios::trunc); 	// Truncating then
					sess_buf_reset();
					r = 0;
				} else {
					f.open(_sess_file_name, ios::in | ios::out | ios::binary);
					if(f) {
						f.read( (char*)&_sess_buf[0], sizeof(_sess_buf) );
						if(f) {
							r = f.gcount();
						} else {
							sess_buf_reset();
							r = 0;
						}
						f.seekg(ios::beg);
					}
				}
			}
		} catch(...) {
			if(f) {		
				f.close();
			}
			r = -1;
		}
		return r;
	}


	bool auth_logout(char *user, char *sess_id) {
		return (auth_delete(user, sess_id) >= 0);
	}


	bool auth_confirm(char *user, char *sess_id, bool b_update_session) {
		return ( auth_confirm_and_update(user, sess_id, b_update_session) >= 0 );
	}

	
	bool auth_check_user_and_password( char *user, char *pass, char *users_and_passwords[] ) {
		bool pass_ok = false;
		for( int i = 0 ; users_and_passwords[i] != nullptr ; i+=2 ) {
			if( !( strcmp(users_and_passwords[i], user ) == 0) ) {
				continue;
			}
			if( !( strcmp(users_and_passwords[i+1], pass ) == 0) ) {
				continue;
			}
			if( strlen(user) > _sess_user_name_size ) {
				continue;
			}
			pass_ok = true;
			break;
		}		
		return pass_ok;
	}


	char *auth_do( char *user, char *pass, char *users_and_passwords[], int *error_code ) {
		char *r = nullptr;
		
		bool pass_ok = 	auth_check_user_and_password( user, pass, users_and_passwords );
		if( pass_ok ) {
			char buf[AUTH_SESS_ID_BUF_SIZE+1];
			generate_session_id( user, buf, 40 );
cerr << "auth_do: session id generated: " << buf << "\n";
			int status = auth_create( buf, user );
			if (status >= 0) {
				r = _sess_id;
			} else if( error_code != nullptr ) {
				*error_code = status; 	// Too many sessions opened or failed to write session file
			}
		} else if( error_code != nullptr ) {
			*error_code = AUTH_ERROR_INVALID_LOGIN_OR_PASSWORD; // Invalid login or/and password
		}
		return r;
	}
