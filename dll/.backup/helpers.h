#pragma once

#define MIME_BUF_SIZE 80
void mimeSetType(char *fn, char *mime_buf, int mime_buf_size);

int get_uri_to_serve(char *b, int b_len, char *uri_buf, int uri_buf_size, bool *get, char **post, bool *options);

bool is_html_request( char *uri );

void parseJSON(char *b, char *user, int max_user, char *pass, int max_pass, char *sess_id, int max_sess_id, char *id, int max_id);

template<class... Args>
void error_message( Args... args ) {
	#ifndef __DEV__
		return;
	#endif
    //(std::cout << ... << args) << std::endl;
    std::fstream log_file("C:\\Users\\lgirs\\Desktop\\papa\\spider\\server\\log.txt", std::fstream::out | std::fstream::app);
    if( log_file ) {
        (log_file << ... << args) << std::endl;
	    log_file.close();
    }
}
