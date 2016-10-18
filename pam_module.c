/*
** Made by Axel VENCATAREDDY on the 18/10/2016
*/

#define PAM_SM_AUTH

#include <security/pam_modules.h>

PAM_EXTERN int  pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
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
  (void)pamh;
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
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
