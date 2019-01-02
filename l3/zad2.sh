list_procs() {
  for dir in /proc/[0-9]*; do
    line=$(grep "^Name:\|^State:\|^Pid:\|^PPid:\|^Uid:" $dir/status | awk '{print $2}')
    if [ -r $dir/fd ]
    then
      line="$line $(ls $dir/fd | wc -l)"
    else
      line="$line ???"
    fi
    echo $line 
  done
}

list_procs | column --table --table-columns NAME,STATE,PID,PPID,UID,FILES
