# [Exercise 1](ps_monitor.sh) 

Create a script `ps_monitor.sh <N>`:
- if no argument are passed it should show the processes of all users (including those without a controlling terminal) with information on the username and start time;
- if an integer (N) is passed, it should show the first N processes

__SUGGESTION__: use `ps` command

# [Exercise 2](lines_counter.sh)

Create a script that accept two args with the following sintax:\
`./lines_counter.sh <directory> [up|down]`\
The script need to show the __files__  present in the directory with the relative number of lines present in each file, sorted in _ascendig_ or _descending_ order.

__NOTES__: 
- check if first argument is a directory
- check if second argument is 'up' or 'down'

# [Exercise 3](backup.sh)

Create a script as follow: \
`./backup.sh <name> <backupName>` \
\
If `name` is a __directory__:
1. make a directory in the current dir `name_backupName`
2. recursively copy the content of `name` into it

If `name` is a __file__ make 5 copies of it with the following sintax:\
`<name>*i<backupName>`

__NOTE__: consider only directories and files inside the current directory.