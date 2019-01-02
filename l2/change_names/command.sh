#!/bin/sh

# change all files
# ls | sed 's/\(.*\)/"\1"/' | sed 'p;s/\(.*\)/\L\1/' | xargs -n2 mv -n 

# don't change directories
find . -type f | sed 's/\(.*\)/"\1"/' | sed 'p;s/\(.*\)/\L\1/' | xargs -n2 mv -n 

