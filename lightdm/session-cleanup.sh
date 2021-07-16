#!/bin/sh

CLEAN_FLAG=/tmp/.cleanmode
NORMAL_FLAG=/tmp/.normal

if [ -e ${CLEAN_FLAG} ];then
	EMPTY_FOLDER=$(echo "/tmp/.cleanmode/*" | cut -f4 -d'/')
	for C_entry in $CLEAN_FLAG/*
	do
		USER=$(echo ${C_entry} | cut -f4 -d'/')
	done
	
	PWENT=$(getent passwd ${USER}) || {
		exit 1	
	}

elif [ -e ${NORMAL_FLAG} ];then
	for N_entry in $NORMAL_FLAG/*
	do
		USER=$(echo ${N_entry} | cut -f4 -d'/')
	done
else
	exit 1
fi

if [ ${USER} != "root" ];then
	if [ -x /bin/loginctl ] || [ -x /usr/bin/loginctl ];then
		loginctl terminate-user ${USER} >/dev/null || true

	else
		while ps h -u ${USER} >/dev/null
		do
			killall -9 -u ${USER} || true
			sleep 0.2;
		done
	fi

	PID=`ps -aux|awk -v UID=${USER} '$1 == UID'|grep "lightdm --session"|awk '{print $2}'`

	RESIDUAL_PROCESS=`ps -aux|awk -v UID=${USER} '$1 == UID'|grep "lightdm --session"|wc -l`
	
	if [ ${RESIDUAL_PROCESS} -ge 1 ];then
		kill -9 ${PID}
	fi
fi


if [ -e ${CLEAN_FLAG} ];then
	PWENT=$(getent passwd ${USER}) || {
		exit 1
	}

	USER_HOME=$(echo ${PWENT} | cut -f6 -d:)

	umount ${USER_HOME} || umount -l ${USER_HOME} || ture 
	rm -rf ${USER_HOME}
	rm -rf ${CLEAN_FLAG}

	usermod -d /home/${USER} ${USER}

	chmod +x /bin/su /usr/bin/sudo
	
	cp /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override_ori /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override
	rm -rf /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override_ori
	glib-compile-schemas /usr/share/glib-2.0/schemas

	USER_UID=$(echo ${PWENT} | cut -f3 -d:)

	find /tmp -mindepth 1 -maxdepth 1 -uid ${USER_UID} -print0 | xargs -0 rm -rf || true 

	for media_dir in /run/media/${USER} /media/${USER};do
		if [ -d ${media_dir} ];then
			for dir in $(find ${media_dir} -mindepth 1 -maxdepth 1);do
				umount ${dir} || true
			done

			rmdir ${media_dir} || true
		fi
	done
else
	rm -rf ${NORMAL_FLAG}
fi
