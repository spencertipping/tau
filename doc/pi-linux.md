# π Linux ABI
## Filesystem
+ `"name" mod F:`: `open()` to create a file (`mod` is UNIX mode), return fd or `-errno`
+ `"name" F_`: unlink file, return 0 on success
+ `"name" F?`: stat file, return map of stat fields (or none)
+ `"name" mod F|`: make fifo
+ `fd


## General
+ `E?`: `strerror()` for an errno


## Processes
+ `P:`: `fork`, return pid
+ `P?`: `waitpid` **TODO**
+ `env args "command" P=`: `execvpe`
+ `args "command" P/=`: `execvp`
+ `pid sig P_`: `kill`


## TODO
+ FIFOs
+ Sockets
+ FD/φ IO
+ X11/GL
