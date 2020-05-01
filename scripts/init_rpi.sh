#!/usr/bin/env bash

#!/usr/bin/env bash
if [ -z ${1+x} ];
then
 echo "Missing hostname parameter";
 echo "Usage init_rpi <hostname/ip-address>";
 exit
 fi
TARGET=$1

ssh ubuntu@${TARGET} 'mkdir -p ~/tmp'
#Install deps
scp ./install_rpi.sh ubuntu@${TARGET}:~/tmp/install_rpi.sh
ssh ubuntu@${TARGET} 'bash ~/tmp/install_rpi.sh'

# Copy models
bash ./copy_models.sh ${TARGET}

# Copy resources
bash ./copy_models.sh ${TARGET}
