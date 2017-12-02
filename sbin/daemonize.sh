#!/bin/bash

# if no args specified, show usage
usage="daemonize.sh --action=(start|stop|ping) --target=(master|agent) --host=hostname"

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

if [ $action = "" ]; then
  loge "$usage"
fi

if [ $target = "" ]; then
  loge "$usage"
fi

if [ $host = "" ]; then
  loge "$usage"
fi
    
# Show the login information
atthis="$reset@$bdgreen$USER@$host$reset:$bdblue$DTC_HOME"

# Export this host
export DTC_THIS_HOST=$host      

# Prepare log directory
mkdir -p $DTC_HOME/workspace/log
export DTC_LOG_FILE="$DTC_HOME/workspace/log/$USER@$host-$target.`date +%Y-%m-%d.%H:%M:%S`.log"
#logi "Log file $DTC_LOG_FILE"

# Prepare the PID folder (file recording the pid of running processes)
mkdir -p $DTC_HOME/workspace/pid
pid_file="$DTC_HOME/workspace/pid/$USER@$host-$target.pid"
#logi "PID file $pid_file"


# -------------------------------------------------------------------------------------------------
# Daemonizing programs
# -------------------------------------------------------------------------------------------------

# Procedure: start_daemon
# The procedure is used to daemonize a given target program.
start_daemon() {
  
  # Ignore daemonizing this program if it is already running.
  if [ -f $pid_file ]; then
    oldpid=`cat $pid_file`
    if ps -p $oldpid > /dev/null; then
      #logi "$target is already running (stop first for startover)"
      logi "$action $target $atthis $OK"  
      return 0 
    fi
  fi
  
  # Launch the target
  if [ ! -f $DTC_HOME/bin/$target ]; then
    loge "$DTC_HOME/bin/$target not found"
  fi
  #nohup $nohup_opt $DTC_HOME/bin/$target >> $DTC_LOG_FILE 2>&1 < /dev/null &
  nohup $nohup_opt $DTC_HOME/bin/$target &> $DTC_LOG_FILE &
  #nohup $nohup_opt sleep 60 >> $DTC_LOG_FILE 2>&1 < /dev/null &
  newpid=$!   

	# store the pid into a file 
  echo $newpid > $pid_file
  #sleep 1

  # Check process is still running 
  if ! ps -p $newpid > /dev/null; then
    loge "$action $target $atthis $FAILED"  
  else 
    logi "$action $target $atthis $OK"  
  fi

  #logi "Successfylly started $target (pid=$newpid)"
}

# start processing the action
case $action in

  # Start the target.
  (start)
    start_daemon $@
    ;;

  # Stop the target.
  (stop)
    if [ -f $pid_file ]; then
      target_pid=`cat $pid_file`
      if ps -p $target_pid > /dev/null; then
        #logi "Stopping $target (pid=$target_pid)"
        if kill $target_pid; then
          logi "$action $target $atthis $OK"  
        else
          loge "$action $target $atthis $FAILED"  
        fi
      else
        #logi "$target is already stopped"
        logi "$action $target $atthis $OK"  
      fi
      rm -f $pid_file
    else
      #logi "$target is already stopped"
      logi "$action $target $atthis $OK"  
    fi
    ;;
  
  # Ping the target.
  (ping)
    if [ -f $pid_file ]; then
      target_pid=`cat $pid_file`
      if ps -p $target_pid > /dev/null; then
        #logi "$target (pid=$target_pid) is running"
        logi "$target $atthis $RUNNING"  
      else
        #logi "$target is not running"
        logi "$target $atthis $STOPPED"  
      fi
    else
      #logi "$target is not running"
      logi "$target $atthis $STOPPED"  
    fi
    ;;
  
  # Unknown action.
  (*)
    loge "Unknown action '$action' in daemonizing target '$target'"
    ;;

esac



