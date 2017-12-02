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
        ssh_cmd="cd ${home}/sbin; ./daemonize.sh --action=$action --target=$target --host=$host"
      ;;

      # remove_logs
      # remove all log files
      remove_logs)
        logi "remove $atpeer/workspace/log/*"  
        ssh_cmd="rm -rf ${home}/workspace/log/*"
      ;;

      # list
      # list all master and agents
      list)
        logi "$target $atpeer"
      ;;
      
      # bootstrap
      #
      # Synchronize all remote dtc projects with the local machine (this)
      #
      #bootstrap)

      #  logi "$action $target $atpeer"
      #  rsync_src=$DTC_HOME/
      #  rsync_dst=$user@$host:$home/
      #  ssh_cmd="cd ${home}; \
      #           ./configure &> /dev/null; make clean &> /dev/null; make &> /dev/null"
      #;;

      # Unknown options.
      *)
        loge "Unknown action $action"
      ;;
    esac
    
    # Task of data synchronization and ssh command execution.
    task() {

      if [[ ! -z $rsync_src && ! -z $rsync_dst ]]; then
        rsync -ar --delete --exclude=".*" -e ssh --rsync-path="mkdir -p $home && rsync" \
            $rsync_src $rsync_dst || loge "rsync $atpeer $FAILED"
      fi
       
      if [[ ! -z $ssh_cmd ]]; then
        ssh $ssh_opt $user@$host $ssh_cmd || loge "$ssh_cmd $atpeer $FAILED"
      fi
    }
    
    # Invoke the task in background (for parallel processing).
    task &

  done
  
  # wait until all background jobs finished.
  wait
}

kernel masters 1 master
kernel agents num_agents agent

