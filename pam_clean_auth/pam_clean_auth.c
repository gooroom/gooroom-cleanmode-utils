#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>

/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    
    int retval=0xff;
    const char *user;

    if (pam_get_user (pamh, &user, NULL) != PAM_SUCCESS) {
	return PAM_USER_UNKNOWN;
    }                                                                                  

    struct passwd *user_entry = getpwnam (user);
    char *dir = NULL;
    char *tmpdir = NULL;

    dir = g_strdup_printf ("/tmp/%s", user_entry->pw_name);

    char *flag = NULL;
    flag = g_strdup_printf ("/tmp/.cleanmode");
	
    if (!g_file_test (flag, G_FILE_TEST_EXISTS|G_FILE_TEST_IS_DIR))
    {
	char *normal,*username_f = NULL;
	normal = g_strdup_printf ("/tmp/.normal");
	
	g_mkdir_with_parents (normal, 0700);
	
	username_f = g_strdup_printf("/bin/touch /tmp/.normal/%s", user);
        g_spawn_command_line_sync (username_f, NULL, NULL, NULL, NULL);

	g_free(normal);
	g_free(username_f);
    }
    
    else
    {
        char *cmd_mount, *cmd_chown, *cmd_skel, *username_f, *disable_cmd = NULL;

	g_mkdir_with_parents (dir, 0700);

	cmd_mount = g_strdup_printf("/bin/mount tmpfs %s -t tmpfs",dir);
        g_spawn_command_line_sync (cmd_mount, NULL, NULL, NULL, NULL);
        
        cmd_skel = g_strdup_printf("/bin/cp -rT /etc/skel/ %s",dir);
        g_spawn_command_line_sync (cmd_skel, NULL, NULL, NULL, NULL);

        cmd_chown = g_strdup_printf("/bin/chown -R %d:%d %s",user_entry->pw_uid, user_entry->pw_gid, dir);
        g_spawn_command_line_sync (cmd_chown, NULL, NULL, NULL, NULL);
        
	username_f = g_strdup_printf("/bin/touch /tmp/.cleanmode/%s", user);
        g_spawn_command_line_sync (username_f, NULL, NULL, NULL, NULL);

	disable_cmd = g_strdup_printf("/bin/chmod -x /bin/su /usr/bin/sudo");
        g_spawn_command_line_sync (disable_cmd, NULL, NULL, NULL, NULL);

	g_free(cmd_mount);
        g_free(cmd_skel);
	g_free(cmd_chown);
	g_free(username_f);
	g_free(disable_cmd);
    }

    g_free(dir);
    g_free(flag);

    return PAM_SUCCESS;
}

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv ) {

    int retval=0xff;

    const char *user = NULL;
    if (pam_get_user (pamh, &user, NULL) != PAM_SUCCESS) {
	return PAM_USER_UNKNOWN;
    }
    
    struct passwd *user_entry = getpwnam (user);
    char *dir = NULL;
    dir = g_strdup_printf ("/tmp/%s", user_entry->pw_name);

    char *flag = NULL;
    flag = g_strdup_printf ("/tmp/.cleanmode");

    if( *user )
    {
	char *cmd_usermod = NULL;

	if (g_file_test (flag, G_FILE_TEST_EXISTS|G_FILE_TEST_IS_DIR))
	{
	    cmd_usermod = g_strdup_printf ("/usr/sbin/usermod -d /tmp/%s %s",user, user);
	    g_spawn_command_line_sync (cmd_usermod, NULL, NULL, NULL, NULL);
 	}

	else
	{
	    cmd_usermod = g_strdup_printf ("/usr/sbin/usermod -d /home/%s %s",user, user);
	    g_spawn_command_line_sync (cmd_usermod, NULL, NULL, NULL, NULL);
	}
	g_free(cmd_usermod); 
    }
    return PAM_SUCCESS;
}

PAM_EXTERN int
pam_sm_chauthtok (pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int
pam_sm_open_session (pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}

PAM_EXTERN int
pam_sm_close_session (pam_handle_t *pamh, int flags, int argc, const char **argv)
{
    return PAM_SUCCESS;
}
 
