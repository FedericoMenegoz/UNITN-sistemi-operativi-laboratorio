# [Exercise 1](char_counter/)

Create a C program that, using UNIX system calls, provides an interface of the type:

`./char_counter.out F N C`

where:

- `F` represents the absolute name of a file
- `N` represents an integer
- `C` represents a character

The initial process `P0` must create `N` child processes (`P1, P2, ..., PN`). 

Each child process `Pi` (where `i = 1, ..., N`) must read a different part of the file `F`. Specifically, if `L` is the length of the file `F`, each child will read a fraction of `L/N` characters from the file `F` according to the following criteria:

- `P1` will read the first fraction
- `P2` will read the second fraction
- ...
- `PN` will read the last fraction

Each process `Pi` will thus read a fraction of `F` with the purpose of calculating the number of occurrences of the character `C` in the part of the file examined. At the end of the scan, `Pi` will communicate to the parent process the number of occurrences of `C` found in the assigned fraction of the file.

The parent process `P0`, once it has obtained the results from all the children, will print the total number of occurrences of `C` in the file `F` and terminate.