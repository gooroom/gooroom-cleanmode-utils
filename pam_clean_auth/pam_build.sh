#!/bin/bash

gcc -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include  -fPIC -fno-stack-protector -c pam_clean_auth.c

sudo ld -x --shared -o /lib/x86_64-linux-gnu/security/pam_clean_auth.so	pam_clean_auth.o

rm pam_clean_auth.o

sudo chmod 644 /lib/x86_64-linux-gnu/security/pam_clean_auth.so
