#pragma once

#ifndef __AUTH_H
#define __AUTH_H

#define AUTH_ERROR_TOO_MANY_SESSIONS -1
#define AUTH_ERROR_FAILED_TO_WRITE_SESSION -2
#define AUTH_ERROR_INVALID_LOGIN_OR_PASSWORD -10

#define AUTH_SESS_ID_BUF_SIZE 100
#define AUTH_USER_NAME_BUF_SIZE 40
	
bool auth_check_user_and_password( char *user, char *pass, char *users_and_passwords[] );
char *auth_do(char *user, char *pass, char **users_and_passwords, int *error_code=nullptr);
bool auth_confirm(char *user, char *sess_id, bool b_update_session=true);
bool auth_logout(char *user, char *sess_id);

char *auth_get_sess_id( void );
char *auth_get_user_name( void );

#endif
