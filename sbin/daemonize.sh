#!/usr/bin/env bash

# if no args specified, show usage
usage="daemonize.sh --action=(start|stop|ping) --target=(dtc-master|dtc-agent) --host=hostname"

if [ $# -le 2 ]; then
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

# Extract the options
for i in "$@"
do
  case $i in
    --action=*)
      action="${i#*=}"
    ;;
    --target=*)
      target="${i#*=}"
    ;;
    --host=*)
      host="${i#*=}"
    ;;

    *)
      loge "Unknown option $1"
    ;;
  esac
done

if [ $action = "" ] || [ $target = "" ] || [ $host = "" ] ; then
  loge "$usage"
fi
    
# Show the login information
atthis="$reset@$bdgreen$USER@$host$reset:$bdblue$DTC_HOME"

# Export this host
export DTC_THIS_HOST=$host      

# Prepare log directory
mkdir -p $DTC_HOME/workspace/log
export DTC_LOG_FILE="$DTC_HOME/workspace/log/$target@$host.log"

# -------------------------------------------------------------------------------------------------
# Daemonize the program
# -------------------------------------------------------------------------------------------------

# Procedure: start_daemon
# The procedure is used to daemonize a given target program.
start_daemon() {
  
  logi "start $target @$host"
  
  # Ignore daemonizing this program if it is already running.
  if pgrep $target > /dev/null; then
    logi "$target is already running @$host"
    return 0
  fi

  # Launch the target
  if [ ! -f $DTC_HOME/bin/$target ]; then
    loge "$DTC_HOME/bin/$target not found"
  fi
  
  # Increate the fd limit
  ulimit -n 8192

  #nohup $nohup_opt $DTC_HOME/bin/$target >> $DTC_LOG_FILE 2>&1 < /dev/null &
  nohup $nohup_opt $DTC_HOME/bin/$target &> $DTC_LOG_FILE &
}

# Procedure: ping_daemon
ping_daemon() {
  if pgrep $target > /dev/null; then
    logi "$target is running @$host"
  else
    logi "$target is stopped @$host"
  fi
}

# procedure: stop_daemon
stop_daemon() {
  logi "stop $target @$host"
  if pgrep $target > /dev/null; then 
    pkill $target; 
  fi
}

# start processing the action
case $action in

  # Start the target.
  (start)
    start_daemon $@
    ;;

  # Stop the target.
  (stop)
    stop_daemon $@
    ;;
  
  # Ping the target.
  (ping)
    ping_daemon $@
    ;;
  
  # Unknown action.
  (*)
    loge "Unknown action '$action' in daemonizing target '$target'"
    ;;

esac



