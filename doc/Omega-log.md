# Ω log file
Log files just frame values with length prefixes so they can be played back sequentially, or loaded if the offset is known. Each value's length must be representable as a `u32`, which is large enough for most applications; it's rare to store very large values.

Internally, the log file is straightforward; it just prepends a `u32b` to each value:

```
size1:u32b value1[size1]... size2:u32b value2[size2]... ...
```
