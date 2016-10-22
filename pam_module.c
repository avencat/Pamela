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
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}

PAM_EXTERN int	pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  UNUSED(pamh);
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  UNUSED(pamh);
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

int		createRepos(char *user, char *pass, char *capUser)
{
  char		repo[256];
  char		cmd[BUFSIZE];
  uid_t		uid;
  gid_t		gid;
  struct passwd	*pwd;
  struct group	*grp;
  int		fd;

  if (!(pwd = getpwnam(user))) {
    perror("getpwnam failed:\n");
    return (-1);
  }
  uid = pwd->pw_uid;
  if (!(grp = getgrnam(user))) {
    gid = 0;
  } else {
    gid = grp->gr_gid;
  }
  sprintf(repo, "/home/%s/cipher", user);
  if (access(repo, 0)) {
    mkdir(repo, 0700);
    if (chown(repo, uid, gid) == -1) {
      perror("CHOWN FAILED IN CREATEREPOS!\n");
      return (-1);
    }
  }
  sprintf(repo, "/home/%s/cipher%s", user, capUser);
  if (access(repo, 0)) {
    if ((fd = open(repo, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH)) == -1) {
      perror("open failed :\n");
      return (-1);;
    }
    close(fd);
  }
  sprintf(cmd, "echo \"%s\" | sudo cryptsetup luksFormat /home/%s/cipher%s && sudo cryptsetup luksOpen /home/%s/cipher%s cipher%s && sudo mkfs.ext4 /dev/mapper/cipher%s", pass, user, capUser, user, capUser, capUser, capUser);
  return (0);
}

PAM_EXTERN int  pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char  *pass;
  char  *user;
  char	*capUser;
  char  cmd[BUFSIZE];

  pam_get_data(pamh, "pam_module_pass", (const void **)&pass);
  pam_get_data(pamh, "pam_module_user", (const void **)&user);
  capUser = firstInCapital(user);
  if (createRepos(user, pass, capUser) == -1) {
    if (capUser)
      free(capUser);
    return (PAM_SESSION_ERR);
  }
  sprintf(cmd, "echo \"%s\" | sudo cryptsetup luksOpen /home/%s/cipher%s cipher%s && sudo mount /dev/mapper/cipher%s /home/%s/cipher ; echo \"%s\" | sudo cryptsetup luksOpen /home/shared/cipherShared cipherShared && sudo mount /dev/mapper/cipherShared /home/shared/cipher", pass, user, capUser, capUser, capUser, user, pass);
  system(cmd);
  if (capUser)
    free(capUser);
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}

PAM_EXTERN int  pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char	*user;
  char	*capUser;
  char	cmd[BUFSIZE];

  pam_get_data(pamh, "pam_module_user", (const void **)&user);
  capUser = firstInCapital(user);
  sprintf(cmd, "sudo umount /home/%s/cipher ; sudo cryptsetup luksClose cipher%s ; sudo umount /home/shared/cipher ; sudo cryptsetup luksClose cipherShared", user, capUser);
  system(cmd);
  if (capUser)
    free(capUser);
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}

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

PAM_EXTERN int  pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
  char *pUsername;
  char		*pass;

  pam_get_user(pamh, (const char **)&pUsername, "Username: ");
  pam_get_item(pamh, PAM_AUTHTOK, (const void **)&pass);
  if (pass)
    changeContainerPass(pUsername, pass);
  UNUSED(flags);
  UNUSED(argc);
  UNUSED(argv);
  return (PAM_SUCCESS);
}
