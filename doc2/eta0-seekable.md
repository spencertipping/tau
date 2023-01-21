# η₀ seekable files
[η₀](eta0.md) provides the data frame format, but there are cases where we want to be able to scan through a file, detect frame boundaries without context, and sync from there. This is useful for IO-bound splitting, for example.


## Basic idea
**TODO:** document this
