# π Linux ABI
## Filesystem
+ `"name" mod F:`: `open()` to create a file (`mod` is UNIX mode), return fd or `-errno`
+ `"name" F_`: close file, return 0 or errno
+ `"name" F?`: stat file, return map of stat fields (or none)
+ `"name" mod F|`: make fifo


## General
+ `E?`: `strerror()` for an errno

**TODO:** embed errno into `none` type


## Processes
+ `X:`: `fork`, return pid
+ `X?`: `waitpid` **TODO**
+ `env args "command" X=`: `execvpe`
+ `args "command" X/=`: `execvp`
+ `pid sig X_`: `kill`


## FIFOs
+ `P:`: create pipe, return `w r` FDs (or `none errno`)


## Sockets
+ `S:`: create a socket **TODO**


## φ
+ `fd F>`: read from file (yielding)
+ `x fd F<`: write to file (yielding)


## TODO
+ FD/φ IO
+ X11/GL
