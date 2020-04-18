#pragma once

#ifndef __AUTH_H
#define __AUTH_H

#define AUTH_SESS_ID_BUF_SIZE 100
#define AUTH_USER_NAME_BUF_SIZE 40
	
char *auth_do(char *user, char *pass, char **users_and_passwords);
bool auth_confirm(char *user, char *sess_id, bool b_update_session=true);
bool auth_logout(char *user, char *sess_id);

char *auth_get_sess_id( void );
char *auth_get_user_name( void );

#endif
