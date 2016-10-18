/*
** Made by Axel VENCATAREDDY on the 18/10/2016
*/

#define PAM_SM_AUTH

#include <security/pam_modules.h>

PAM_EXTERN int  pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  return (0);
}
