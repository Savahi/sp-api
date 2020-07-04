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
