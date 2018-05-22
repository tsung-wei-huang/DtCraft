#!/bin/bash

usage="Usage: kernel.sh --action=[start|stop|ping]"

# if no args specified, show usage
if [ $# != 1 ]; then
  echo "$usage"
  exit
fi

# -------------------------------------------------------------------------------------------------
# Preprocessing
# -------------------------------------------------------------------------------------------------

# Get and enter the directory of the script
sbin=`dirname $0`
sbin=`cd $sbin; pwd`

# Source the utilities.
if [ ! -f $sbin/common.sh ]; then
  echo "$sbin/common.sh not found"
  exit -1
fi

. $sbin/common.sh

# Get the action
for i in $@
do
  case $i in
    --action=*)
      action="${i#*=}"
    ;;

    *)
      loge "Unknown option $1"
    ;;
  esac
done

if [ $action = "" ]; then
  loge "$usage"
fi

# -------------------------------------------------------------------------------------------------
# kernel call on master and agents
# -------------------------------------------------------------------------------------------------

# Procedure
# $1: hostlist
# $2: # of hosts to be called
# $3: target
kernel() {

  local target=$3

  for ((i=0; i<$2; i++)); do

    # Extract the user/host/home
    eval local user=\${$1["\$((\$i*3+0))"]}
    eval local host=\${$1["\$((\$i*3+1))"]}
    eval local home=\${$1["\$((\$i*3+2))"]}

    atpeer="$reset@$bdgreen$user@$host$reset:$bdblue$home"

    # Initialize the command based on actions.
    case $action in
      
      # (start|stop|ping)
      #
      # Connect to the remote and synchronize the conf folder, then launch the command
      # to daemonize the target.
      #
      start|stop|ping)
        # (TODO: ideally we only have to synchronize conf folder) - now for debugging purpose.
        rsync_src=$DTC_HOME/conf/
        rsync_dst=$user@$host:$home/conf/
        ssh_cmd="sudo ${home}/sbin/daemonize.sh --action=$action --target=$target --host=$host"
      ;;

      # Unknown options.
      *)
        loge "Unknown action $action"
      ;;
    esac
    
    # Task of data synchronization and ssh command execution.
    task() {

      if [[ ! -z $rsync_src && ! -z $rsync_dst ]]; then
        rsync -ar --delete --exclude=".*" -e ssh --rsync-path="mkdir -p $home && rsync" \
            $rsync_src $rsync_dst || loge "rsync conf/ $FAILED"
      fi
       
      if [[ ! -z $ssh_cmd ]]; then
        ssh $ssh_opt $user@$host "$ssh_cmd" || loge "$ssh_cmd $FAILED"
      fi
    }
    
    # run the task
    task &

  done
  
  # wait until all background jobs finished.
  wait
}

kernel masters 1 dtc-master
kernel agents num_agents dtc-agent

