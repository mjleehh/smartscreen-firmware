#!/bin/sh
#

cat sdkconfig | grep -i password | grep -v '=""'
if [ $? -eq 0 ] ; then
   echo "looks like you are trying to commit a password in the config"
   echo "commit rejected!"
   exit 1
fi

cat sdkconfig | grep -i ssid | grep -v '=""'
if [ $? -eq 0 ] ; then
   echo "looks like you are trying to commit wifi credentials in the config!"
   echo "commit rejected!"
   exit 1
fi

exit 0


