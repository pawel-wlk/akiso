download=0
upload=0
tests_no=0

println () {
  echo -e "$@                "
}
add_unit () {
  working=$1
  if [  $working -gt 1000000000 ]; then
    working=$(printf %.2f $(echo "$working/1000000000" | bc -l))
    working="$working GB/s"
  elif [ $working -gt 1000000 ]; then
    working=$(printf %.2f $(echo "$working/1000000" | bc -l))
    working="$working MB/s"
  elif [ $working -gt 1000 ]; then
    working=$(printf %.2f $( echo "$working/1000" | bc -l))
    working="$working KB/s"
  else
    working=$(printf %.2f $working)
    working="$working B/s"
  fi

  println $working
}

prev_down=$(cat /proc/net/dev | grep wlp3s0 | awk '{print $2}')
prev_up=$(cat /proc/net/dev | grep wlp3s0 | awk '{print $10}')

net_status () {
  download=$(cat /proc/net/dev | grep wlp3s0 | awk '{print $2}')
  upload=$(cat /proc/net/dev | grep wlp3s0 | awk '{print $10}')

  down_ps=$(( download - prev_down ))
  up_ps=$(( upload - prev_up ))

  prev_down=$download
  prev_up=$upload

  avg_down=$(( (avg_down*tests_no + down_ps)/(tests_no+1) ))
  avg_up=$(( (avg_up*tests_no + up_ps)/(tests_no+1) ))
  tests_no=$(( tests_no+1 ))
  
  println Network 🔄:
  println Download ⬇️ : $(add_unit $down_ps)
  println Upload ⬆️ : $(add_unit $up_ps)
  println Average Download ⬇️ : $(add_unit $avg_down)
  println Average Upload ⬆️ : $(add_unit $avg_up)
}

find_uptime() {
  uptime=$(cat /proc/uptime | awk '{print $1}')
  
  uptime=$( printf %.0f $uptime )

  day=$(( uptime/(24*3600) ))
  uptime=$(( uptime%(24*3600) ))
  hour=$(( uptime/3600 ))
  uptime=$(( uptime%3600 ))
  minute=$(( uptime/60 ))
  uptime=$(( uptime%60 ))
  second=$uptime

  if [ $day -eq 1 ]
  then
    day="$day day"
  else
    day="$day days"
  fi

  if [ $hour -eq 1 ]
  then
    hour="$hour hour"
  else
    hour="$hour hours"
  fi

  if [ $minute -eq 1 ]
  then
    minute="$minute minute"
  else
    minute="$minute minutes"
  fi

  if [ $second -eq 1 ]
  then
    second="$second second"
  else
    second="$second seconds"
  fi


  println Uptime ⌚: $day, $hour, $minute and $second
}

find_battery_status() {
  energy_full=$(cat /sys/class/power_supply/BAT0/uevent | grep "ENERGY_FULL=" | sed 's/^.*=//g')
  energy_now=$(cat /sys/class/power_supply/BAT0/uevent | grep "ENERGY_NOW" | sed 's/^.*=//g')

  percentage=$(echo "$energy_now/$energy_full*100" | bc -l)

  println "Battery 🔋: $(printf %.2f $percentage)%"
}

find_avg_load() {
  load=$(cat /proc/loadavg | awk '{print $1}')

  println "System load 💪: $load"
  [ ${#load_his[*]} -eq 26 ] && load_his=("${load_his[@]:1}")
  load_his[${#load_his[*]}]=$load
  graph ${load_his[@]}
}

graph() {
  width=55
  height=15

  #scale input 
  bars=()
  for el in $@
  do
    bars[${#bars[*]}]=$(printf %0.f $(echo "($el/4)*13" | bc -l))
  done
  
  #draw top border
  echo -n ┏
  for col in $(seq 1 $((width-2)))
  do
    echo -n ━
  done
  echo -n  ┓
  echo

  for row in $(seq 1 $height)
  do
    # begin line
    echo -n ┃

    for col in $(seq 2 $((width-1)))
    do
      #bars
      col_num=$(( col/2 ))
      if [ $((col%2)) -eq 1 ] && [ "$(($height-$row))" -lt "${bars[$(($col_num-1))]}" ] 2>/dev/null
      then
        echo -n -e "\e[38;5;88m█\e[0m"
      else
        echo -n " " 
      fi
    done

    # endline
    echo -n ┃
    echo
  done


  #draw bottom border
  echo -n ┗
  for col in $(seq 1 $((width-2)))
  do
    echo -n ━
  done
  echo -n  ┛
}


trap "tput cnorm; clear; exit 1" SIGINT
tput civis
clear
while [ true ]
do
  tput cup 0 0
  net_status
  find_uptime
  find_battery_status
  find_avg_load
  sleep 1
done
