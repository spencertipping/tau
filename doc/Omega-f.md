# Ωf
A short header followed by a bare η in a file, updated with a full rewrite and atomic `rename`. The header is updated when a new file has been written, at which point interested readers can reopen the file to get the new value.

```
Ωf\0      # 4-byte magic
rev:u32b  # revision count, incremented on complete update (write + rename)
η...      # data
```

This file does not use locks. When replaced, `rev` in the _original_ file is incremented to signal to readers that new data is available. It doesn't matter whether the update to `rev` is atomic: any change means that the file should be reopened.

If multiple writers attempt to replace a file, the last one wins; atomicity is provided by the OS.
