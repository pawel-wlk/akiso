if [ $# -ne 2 ]; then
  echo "USAGE: sh zad4.sh [URL] [time interval in seconds]"
  exit 1
fi
if ! [[ $2 =~ ^[0-9]+$ ]]; then
  echo incorrect second argument >&2
  exit 1
fi

url=$1
interval=$2

dirname=$(echo $url | sed 's/\/\|\.//g')

path=/home/wilkosz/websitetracker/$dirname
mkdir $path 2>/dev/null
git init $path >/dev/null
touch $path/current

while [ true ]; do
  if ! [ "$(curl -s -I -f $url)" ];then
    echo site is down >&2
    exit 1
  fi

  links -dump $url > $path/new
  if [ "$(diff $path/new $path/current)" ]; then
    diff $path/new $path/current | zenity --text-info --title="$url got changed!" 
    mv $path/new $path/current
    git --git-dir=$path/.git --work-tree=$path add $path/current > /dev/null
    git --git-dir=$path/.git --work-tree=$path commit -m 'new revision' > /dev/null
  fi

  sleep $interval
done
