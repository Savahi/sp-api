#pragma once

#ifndef __AUTH_H
#define __AUTH_H

#define AUTH_UNKNOWN_ERROR -1
#define AUTH_ERROR_TOO_MANY_SESSIONS -2
#define AUTH_ERROR_FAILED_TO_WRITE_SESSION -3
#define AUTH_ERROR_INVALID_LOGIN_OR_PASSWORD -10

#define AUTH_SESS_ID_LEN 40
#define AUTH_USER_MAX_LEN 20
	
bool auth_check_user_and_password( char *user, char *pass, char *users_and_passwords[] );
char *auth_do(char *user, char *pass, char **users_and_passwords, int *error_code=nullptr);
bool auth_confirm(char *user, char *sess_id, bool b_update_session=true, bool b_validate_sess_id_only=true);
bool auth_logout(char *user, char *sess_id);

char *auth_get_sess_id( void );
char *auth_get_user( void );

#endif
