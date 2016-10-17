# Pamela
Security project during my studies at Epitech.
The purpose of this project is to create a pam module to open some cipher directories/partitions when a user open his session.

## Add a new user {user}
sudo adduser {user}

## Download cryptsetup
sudo apt-get install cryptsetup

## Add a new cipher of {number} MiB directory for {user}
### Creation of a file that will be converted into cipher container
dd if=/dev/urandom bs=1M count={number} of=/home/{user}/cipher{user}

#### Be careful below, the password that you give to the container MUST be the same as your user session password, indeed, PAM just pass your session password to the container at the opening of your session !

### Creation of a cipher container
sudo cryptsetup luksFormat /home/{user}/cipher{user}

### Opening of the cipher container
sudo cryptsetup luksOpen /home/{user}/cipher{user} cipher{user}

### Format the container on Ext4
sudo mkfs.ext4 /dev/mapper/cipher{user}

### Create the mount point
mkdir /home/{user}/cipher

## Modify the /etc/fstab file
### Add this line to your /etc/fstab file :
/dev/mapper/cipher{user}  /home/{user}/cipher ext4  defaults,noauto 0 0

## Download libpam-mount
### Installation
sudo apt-get install libpam-mount

## Edit the /etc/security/pam_mount.conf.xml
### Add this line at the end of the file before the last tag </pam-mount>
<volume user="{user}" mountpoint="/home/{user}/cipher" path="/home/{user}/cipher{user}" fstype="crypt" />

### If you want to add a container for all the users (a Shared container), modify this line as follow :
<volume user="*" mountpoint="/home/{user}/cipher" path="/home/{user}/cipher{user}" fstype="crypt" />

### And add the password of each of your users in the keys of the container with the command :
sudo cryptsetup luksAddKey /home/{user}/cipher{user}
