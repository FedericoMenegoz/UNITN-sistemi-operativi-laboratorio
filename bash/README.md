# BASH-amo le mani
These are some exercises I found online. \
At this link there you can find a super bash course where I got some of these exercises.\
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1045332.svg)](https://doi.org/10.5281/zenodo.1038525)

# [Seq](seq.sh)
Write a script `seq <start> <step> <end>`
- if one argument is provided `seq <end>` will print from 1 to `end`
- if two arguments are provided `seq <start> <end>` will print from `start` to `end`
- if three arguments are provided `seq <start> <step> <end>` will print from `start` to `end` every `step`

`seq 1 4 13` should output:
```
1
5
9
13
```

# [Create i Dir and j Files](create.sh)
Write a script `create.sh <i> <j> <file>`:
- create directories named 1, 2, ..., `i`
- use touch to put empty files named 1 till `j` in each of these directories
- print an error if a negative value is provided fo `i` or `j`
- if any of the files exist, the script should exit with an error
- provide help if  one of the args is -h, then exit the script
- if the third argument is a file, the script should copy this file to all locations instead of createing empty files with touch

# [Match 3 Pattern](match_patterns.sh)
Write a script `match_patterns.sh <path> <pat1> <pat2> <pat3>`:
- it should find the patterns provided in the file `path`
- it should display for each pattern the number of matches followed by the line numbers the matches did occur 
# [Monitor](ps_monitor.sh) 

Create a script `ps_monitor.sh <N>`:
- if no argument are passed it should show the processes of all users (including those without a controlling terminal) with information on the username and start time;
- if an integer (N) is passed, it should show the first N processes

__SUGGESTION__: use `ps` command

# [Lines Counter](lines_counter.sh)

Create a script that accept two args with the following sintax:\
`./lines_counter.sh <directory> [up|down]`\
The script need to show the __files__  present in the directory with the relative number of lines present in each file, sorted in _ascendig_ or _descending_ order.

__NOTES__: 
- check if first argument is a directory
- check if second argument is 'up' or 'down'

# [Backup](backup.sh)

Create a script as follow: \
`./backup.sh <name> <backupName>` \
\
If `name` is a __directory__:
1. make a directory in the current dir `name_backupName`
2. recursively copy the content of `name` into it

If `name` is a __file__ make 5 copies of it with the following sintax:\
`<name>*i<backupName>`

__NOTE__: consider only directories and files inside the current directory.