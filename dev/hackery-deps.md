# `hackery/` dependencies
To get everything in `dev/hackery`, this might be a start (but no promises):

```sh
$ ./build deps
$ sudo dpkg --add-architecture i386
$ sudo apt install -y \
       build-essential \
       libboost-context-dev \
       libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev \
       libxcb-xinput-dev \
       g++-multilib-i686-linux-gnu \
       libc6-dbg{,:i386} \
       libstdc++6-12-dbg{,:i386} \
       libpango1.0-dev libcairo2-dev \
       libzstd-dev libcurl4-openssl-dev
```
