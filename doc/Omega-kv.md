# Ω `k→v` log files
Simple enough: we just append values to the end and occasionally rewrite the whole file to remove deleted values. Atomic updates also append; non-atomic updates can be in-place if size permits.

The log file should provide replay, so each `k→v` pair is framed like so:

```
len:u32b klen:u16b key... value...
```

That's a total of 6 bytes of overhead per k/v pair, which is kind of a lot but it allows us to recover fully from just the log, which will be important for failure scenarios.
