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
  char		*pass;
  const char	*pUsername;

  retval = pam_get_user(pamh, &pUsername, "Username: ");
  if (retval != PAM_SUCCESS) {
    return (retval);
  }
  pam_get_item(pamh, PAM_AUTHTOK, (const void **)&pass);
  pam_set_data(pamh, "pam_module_pass", strdup(pass), &cleanup);
  pam_set_data(pamh, "pam_module_user", strdup(pUsername), &cleanup);
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

char	*firstInCapital(char *str)
{
  char	*ret;

  if (!str || !str[0] || (ret = malloc(strlen(str))) == NULL)
    return (ret);
  strcpy(ret, str);
  if (ret[0] <= 'z' && ret[0] >= 'a')
    ret[0] += 'A' - 'a';
  return (ret);
}

PAM_EXTERN int  pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char  *pass;
  char  *user;
  char	*capUser;
  char  cmd[1024];
  char  cipherFileName[256];

  pam_get_data(pamh, "pam_module_pass", (const void **)&pass);
  pam_get_data(pamh, "pam_module_user", (const void **)&user);
  strcpy(cipherFileName, "cipher");
  capUser = firstInCapital(user);
  strcpy(cmd, "echo \"");
  strcat(cmd, pass);
  strcat(cmd, "\" | sudo cryptsetup luksOpen /home/");
  strcat(cmd, user);
  strcat(cmd, "/");
  strcat(cipherFileName, capUser);
  strcat(cmd, cipherFileName);
  strcat(cmd, " ");
  strcat(cmd, cipherFileName);
  strcat(cmd, " && sudo mount /dev/mapper/");
  strcat(cmd, cipherFileName);
  strcat(cmd, " /home/");
  strcat(cmd, user);
  strcat(cmd, "/cipher");
  system(cmd);
  if (capUser)
    free(capUser);
  (void)flags;
  (void)argc;
  (void)argv;
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char	*user;
  char	cmd[256];

  pam_get_data(pamh, "pam_module_user", (const void **)&user);
  strcpy(cmd, "sudo umount /home/");
  strcat(cmd, user);
  strcat(cmd, "/cipher ; sudo cryptsetup luksClose cipher");
  user = firstInCapital(user);
  strcat(cmd, user);
  system(cmd);
  if (user)
    free(user);
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
