#!/bin/bash

gcc -fPIC -fno-stack-protector -W -Wall -Wextra -Werror -c pam_module.c

sudo ld -x --shared -o /lib/security/pam_module.so pam_module.o

rm pam_module.o
