#!/bin/sh

CLEAN_FLAG=/tmp/.cleanmode

if [ -e ${CLEAN_FLAG} ];then
	cp /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override_ori
	sed -i 's/\(theme_\|\)bg_[0-9]\{1,\}/bg_13/g' /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override

	glib-compile-schemas /usr/share/glib-2.0/schemas

else
	if [ -e /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override_ori ];then

		cp /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override_ori /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override
		rm -rf /usr/share/glib-2.0/schemas/10_gooroom-configuration.gschema.override_ori

		glib-compile-schemas /usr/share/glib-2.0/schemas

	fi
	
	chmod +x /bin/su /usr/bin/sudo
fi


