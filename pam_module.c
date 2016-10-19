/*
** Made by Axel VENCATAREDDY on the 18/10/2016
*/

#define PAM_SM_AUTH

#include <security/pam_modules.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void  cleanup(pam_handle_t *pamh, void *data, int error_status)
{
  char *xx;

  (void)pamh;
  (void)error_status;
  if ((xx = data)) {
    while (*xx) {
      *xx++ = '\0';
    }
    free(data);
  }
}

PAM_EXTERN int	pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  int		retval;
  char  *pass;
  const char	*pUsername;

  puts("In Authenticate");
  retval = pam_get_user(pamh, &pUsername, "Username: ");
  printf("Welcome %s\n", pUsername);
  if (retval != PAM_SUCCESS) {
    return (retval);
  }
  pam_get_item(pamh, PAM_AUTHTOK, (const void **)&pass);
  printf("pass = \"%s\"\n", pass);
  pam_set_data(pamh, "pam_module", strdup(pass), &cleanup);
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int	pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char  *pass;

  puts("In open session");
  pam_get_data(pamh, "pam_module", (const void **)&pass);
  printf("pass = \"%s\"\n", pass);
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  puts("In close session");
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}
