# Tau
+ [ζ](doc/zeta.md)
+ [φ](doc/phi.md) and [Φ](doc/Phi.md)
+ [γ](doc/gamma.md) and [Γ](doc/Gamma.md)
  + [γ design](doc/gamma-design.md)
  + [γ authoring](doc/gamma-authoring.md)
+ [λ](doc/lambda.md)
+ [utf9](doc/utf9.md)
+ [security](doc/security.md)


## Development setup
On Ubuntu 22.04:

```sh
$ sudo apt install -y \
       docker.io \
       build-essential \
       libboost-fiber-dev libboost-context-dev libmsgpack-dev \
       libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev \
       libxcb-xinput-dev \
       g++-multilib-i686-linux-gnu \
       libc6-dbg{,:i386} \
       libstdc++6-12-dbg{,:i386}
```
