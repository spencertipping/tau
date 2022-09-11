# Tau
APL + Erlang + bash + Joy: a Laplace transform for control flow, where most code is data.

I will probably never have time to document this properly.

+ [ζ](doc/zeta.md)
+ [φ](doc/phi.md) and [Φ](doc/Phi.md)
+ [γ](doc/gamma.md) and [Γ](doc/Gamma.md)
  + [Ξ](doc/Xi.md)
  + [γ design](doc/gamma-design.md)
  + [γ authoring](doc/gamma-authoring.md)
  + [γ topologies](doc/gamma-topologies.md)
+ [Λ](doc/Lambda.md)
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
       libstdc++6-12-dbg{,:i386} \
       libpango1.0-dev libcairo2-dev
```

This repo cannot be cloned on case-insensitive filesystems.

**TODO:** dockerize build process


## Next steps
1. γ definition DSL (C++ side)
2. γ₀ → γ UTF9 API
3. Φ trunk multiplexing
4. γ₀ ED25519 + AES secure channels
5. Γ routing
