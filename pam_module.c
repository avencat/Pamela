/*
** Made by Axel VENCATAREDDY on the 18/10/2016
*/

#define UNUSED(x) (void)(x)

#define PAM_SM_AUTH
#define	BUFSIZE		2048

#include <security/pam_modules.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

// Erase the memory at the end of pam use
void  cleanup(pam_handle_t *pamh, void *data, int error_status)
{
  char *xx;

  if ((xx = data)) {
    while (*xx) {
      *xx++ = '\0';
    }
    free(data);
  }
  UNUSED(pamh);
  UNUSED(error_status);
}

// Call when a user authenticate
PAM_EXTERN int	pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  int		retval;
  char		*pass;
  const char	*pUsername;

  // Retrieve the username
  retval = pam_get_user(pamh, &pUsername, "Username: ");
  if (retval != PAM_SUCCESS) {
    return (retval);
  }
  // Retrieve the password
  pam_get_item(pamh, PAM_AUTHTOK, (const void **)&pass);
  // Save the password in memory so that you can access it when opening a session
  pam_set_data(pamh, "pam_module_pass", strdup(pass), &cleanup);
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}

// Set the module-specific credentials of the user
PAM_EXTERN int	pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  UNUSED(pamh);
  return (PAM_SUCCESS);
}

// Establishes the account's usability and the user's accessibility to the system
PAM_EXTERN int  pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  UNUSED(pamh);
  UNUSED(argc);
  UNUSED(argv);
  UNUSED(flags);
  return (PAM_SUCCESS);
}

// Put the first letter of a word in capital
char	*firstInCapital(char *str)
{
  char	*ret;

  if (!str || !str[0] || (ret = malloc(strlen(str))) == NULL)
    return (NULL);
  strcpy(ret, str);
  if (ret && ret[0] && ret[0] <= 'z' && ret[0] >= 'a')
    ret[0] += 'A' - 'a';
  return (ret);
}

// Called whenever a session is opening
PAM_EXTERN int  pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char  *pass;
  char  *user;
  char	*capUser;
  char  cmd[BUFSIZE];
  int	retval;

  // We retrieve the password so that we can open the LUKS container
  if (pam_get_data(pamh, "pam_module_pass", (const void **)&pass) != PAM_SUCCESS)
    pass = NULL;
  // We retrieve the username
  retval = pam_get_user(pamh, (const char **)&user, "Username: ");
  if (retval != PAM_SUCCESS) {
    return (retval);
  }
  // We set capUser so that the first letter of the userName is in capital
  capUser = firstInCapital(user);
  // We open and mount the containers if we have the password and ask if we have not
  if (pass) {
    sprintf(cmd, "echo \"%s\" | sudo cryptsetup luksOpen /home/%s/cipher%s cipher%s && sudo mount /dev/mapper/cipher%s /home/%s/cipher ; echo \"%s\" | sudo cryptsetup luksOpen /home/shared/cipherShared cipherShared && sudo mount /dev/mapper/cipherShared /home/shared/cipher", pass, user, capUser, capUser, capUser, user, pass);
  } else {
    sprintf(cmd, "sudo cryptsetup luksOpen /home/%s/cipher%s cipher%s && sudo mount /dev/mapper/cipher%s /home/%s/cipher", user, capUser, capUser, capUser, user);
  }
  system(cmd);
  if (capUser)
    free(capUser);
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}

// Called whenever a session is closing
PAM_EXTERN int  pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char	*user;
  char	*capUser;
  char	cmd[BUFSIZE];
  int	retval;

  // We retrieve the userName
  retval = pam_get_user(pamh, (const char **)&user, "Username: ");
  if (retval != PAM_SUCCESS) {
    return (retval);
  }
  // We put the userName with a capitalized first letter in capUser
  capUser = firstInCapital(user);
  // We unmount and close the containers
  sprintf(cmd, "sudo umount /home/%s/cipher ; sudo cryptsetup luksClose cipher%s ; sudo umount /home/shared/cipher ; sudo cryptsetup luksClose cipherShared", user, capUser);
  system(cmd);
  if (capUser)
    free(capUser);
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}

// Function that add a key to a container whenever a password is changed
void	changeContainerPass(char *user, char *pass)
{
  char	cmd[BUFSIZE];
  char	*capUser;

  capUser = firstInCapital(user);
  sprintf(cmd, "echo -e \"Jesuis%s\n%s\n%s\" | sudo cryptsetup luksAddKey /home/%s/cipher%s", capUser, pass, pass, user, capUser);
  system(cmd);
  if (capUser)
    free(capUser);
}

// Called whenever the password is changed
PAM_EXTERN int  pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char		*pUsername;
  char		*pass;
  int		retval;

  // We get the userName
  if ((retval = pam_get_user(pamh, (const char **)&pUsername, "Username: ")) != PAM_SUCESS)
    return (retval);
  // We get the password
  if ((retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **)&pass)) != PAM_SUCESS)
    return (retval);
  if (pass)
    changeContainerPass(pUsername, pass);
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}
