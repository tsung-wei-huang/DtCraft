#!/bin/bash

# -------------------------------------------------------------------------------------------------
# Color setting.
# -------------------------------------------------------------------------------------------------
sbin=`dirname $0`

if [ $TERM != "xterm*" ]; then
  export TERM=xterm-256color
fi

bold=$(tput bold)
underline=$(tput sgr 0 1)
reset=$(tput sgr0)

purple=$(tput setaf 171)
red=$(tput setaf 1)
green=$(tput setaf 76)
cyan=$(tput setaf 6)
yellow=$(tput setaf 3)
blue=$(tput setaf 4)

bdblue=$bold$blue
bdgreen=$bold$green
bdred=$bold$red
bdcyan=$bold$cyan

FAILED=$bdred"[FAILED]"$reset
OK=$bdcyan"[OK]"$reset
RUNNING=$bdcyan"[RUNNING]"$reset
STOPPED=$bdcyan"[STOPPED]"$reset

logi() {
  #printf "✔ `date +%Y-%m-%d\ %H:%M:%S`] %s${reset}\n" "$@"
  printf "%s${reset}\n" "$@"
}

loge() {
  #printf "${bold}${red}✖ %5s `date +%Y-%m-%d\ %H:%M:%S`] %s${reset}\n" "$$" "$@"
  #printf "${bold}${red}✖ `date +%Y-%m-%d\ %H:%M:%S`] %s${reset}\n" "$@"
  printf "${bold}${red}%s${reset}\n" "$@"
  exit -1
}

logw() { 
  #printf "${bold}${yellow}➜ %5s `date +%Y-%m-%d\ %H:%M:%S`] %s${reset}\n" "$$" "$@"
  #printf "${bold}${yellow}➜ `date +%Y-%m-%d\ %H:%M:%S`] %s${reset}\n" "$@"
  printf "${bold}${yellow}%s${reset}\n" "$@"
}

logd() {
  #printf "${bold}${cyan}○ %5s `date +%Y-%m-%d\ %H:%M:%S`] %s${reset}\n" "$$" "$@"
  #printf "${bold}${cyan}○ `date +%Y-%m-%d\ %H:%M:%S`] %s${reset}\n" "$@"
  printf "${bold}${cyan}%s${reset}\n" "$@"
}

# Procedure: sync
# Synchronize the files/directories of destination with the source.
# $1 source 
# $2 destination
# $3 prologue 
sync() {
  rsync -azrtopg --delete --exclude ".*" -e ssh $1 $2
  if [ "$?" -ne "0" ]; then
    loge "Failed to synchronize $2"
  fi
}



# Procedure: read_hosts
# $1: host file
# $2: array
# $3: counter
read_hosts() {

  if [ ! -f $1 ]; then
    loge "Failed to read hosts ($1 not found)";
  fi 
  
  while read -r line; do
    
    # skip the comments
    { [[ "$line" =~ ^#.*$ ]] || [[ -z $line ]] ;} && continue
    
    # Parse the line
    read -a tokens <<< $line

    if [ ${#tokens[@]} == 3 ]; then
  
      # Extract the user/host/home
      eval $2["\$((\$3*3+0))"]=\${tokens[0]}
      eval $2["\$((\$3*3+1))"]=\${tokens[1]}
      eval $2["\$((\$3*3+2))"]=\${tokens[2]}
      eval $3=$(($3+1))

    else
      logw "Wrong line format '$line'"
    fi
    
  done < $1
}

# -------------------------------------------------------------------------------------------------
# Get directory variables
# -------------------------------------------------------------------------------------------------
sbin=`dirname $0`
sbin=`cd $sbin; pwd`

# Get the home 
export DTC_HOME=`cd $sbin/../; pwd`

# -------------------------------------------------------------------------------------------------
# Source the user-defined configuration file if given
# -------------------------------------------------------------------------------------------------
if [ -f $DTC_HOME/conf/conf.sh ]; then
  . $DTC_HOME/conf/conf.sh 
fi

# -------------------------------------------------------------------------------------------------
# Set the ssh options.
# -------------------------------------------------------------------------------------------------
ssh_opt="-n -x -o StrictHostKeyChecking=no"
 
# -------------------------------------------------------------------------------------------------
# Set the nohup options.
# -------------------------------------------------------------------------------------------------
nohup_opt="nice -n 0"

# -------------------------------------------------------------------------------------------------
# Read the masterlist
# -------------------------------------------------------------------------------------------------
declare -a masters

num_masters=0

read_hosts $DTC_HOME/conf/master masters num_masters

if [ $num_masters == 0 ]; then
  loge "No master found in $DTC_HOME/conf/master"
fi

export DTC_MASTER_HOST=${masters[0, 1]}

# -------------------------------------------------------------------------------------------------
# Read the agentlist
# -------------------------------------------------------------------------------------------------
declare -a agents

num_agents=0

read_hosts $DTC_HOME/conf/agents agents num_agents

if [ $num_agents == 0 ]; then
  loge "No agent found in $DTC_HOME/conf/agents"
fi


