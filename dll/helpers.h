#pragma once

#ifndef __HELPERS_H
#define __HELPERS_H

#define MIME_BUF_SIZE 80
void mimeSetType(char *fn, char *mime_buf, int mime_buf_size);

int getUriToServe(char *b, char *uri_buf, int uri_buf_size, char **get, char **post);

bool is_html_request( char *uri );

void parseJSON(char *b, char *user, int max_user, char *pass, int max_pass, char *sess_id, int max_sess_id, char *id, int max_id);

#endif
