#!/bin/bash

usage="Usage: submit.sh --master=IP <binary> [args]"

# -------------------------------------------------------------------------------------------------
# Preprocessing
# -------------------------------------------------------------------------------------------------

# Get and enter the directory of the script
sbin=`dirname $0`
sbin=`cd $sbin; pwd`

# Get the home 
export DTC_HOME=`cd $sbin/../; pwd`

# Extract the options
for i in "$@"
do
  # --master=IP 
  case $i in
    --master=*)
      master=${i#*=}
    shift
    ;;

    *)
    ;;
  esac
done

# Now $1 is the the binary name and $#/$@ is its argc/argv
if [ "$master" != "" ]; then
  export DTC_MASTER_HOST=${master}
fi

export DTC_THIS_HOST=`hostname`
export DTC_EXECUTION_MODE="submit"
export DTC_SUBMIT_FILE=$1
export DTC_SUBMIT_ARGV=$@

# Launch the submit
exec $@
