# Tau
APL + Erlang + bash + Joy + Lisp + FRP + probabilistic Prolog: a Laplace transform for control flow, where most code is data.

Design notes:

+ [φ](doc/phi.md) and [Φ](doc/Phi.md)
  + [ζ](doc/zeta.md)
  + [Λ](doc/Lambda.md)
  + [UTF9](doc/utf9.md)
+ [γ](doc/gamma.md)
  + [γ design](doc/gamma-design.md)
  + [γ authoring](doc/gamma-authoring.md)
  + [γ topologies](doc/gamma-topologies.md)
+ [γ₀](doc/gamma0.md) and [Γ](doc/Gamma.md)
  + [Ξ](doc/Xi.md)
  + [ξ](doc/xi.md)
+ [π](doc/pi.md)
  + [π₀](doc/pi0.md)
  + [π₁](doc/pi1.md)
  + [π₂](doc/pi2.md)
  + [η](doc/eta.md)

Production design:

+ [Runtime security](doc/security.md)


## Development setup
**Warning:** this repo requires a case-sensitive filesystem; it cannot be cloned correctly on case-insensitive MacOS or Windows.

On Ubuntu 22.04:

```sh
$ sudo dpkg --add-architecture i386
$ sudo apt update
$ sudo apt install -y docker.io  # for emsdk builds
$ sudo apt install -y \
       build-essential \
       libboost-fiber-dev libboost-context-dev \
       libmsgpack-dev \
       libxcb1-dev libx11-dev libx11-xcb-dev libgl-dev \
       libxcb-xinput-dev \
       g++-multilib-i686-linux-gnu \
       libc6-dbg{,:i386} \
       libstdc++6-12-dbg{,:i386} \
       libpango1.0-dev libcairo2-dev
```

**TODO:** dockerize build process


## Priority queue
Narrowly:

+ π₀ γ API
  + γ/λ interfacing
  + ξ API
+ π₀ GC finalizers + natives
+ π₀ define-function (likely just using frame variables)
+ π₀ REPL
  + π₀/Φ integration
  + Read-and-execute source
+ UTF9 indexed-structure support
+ WASM Φ
+ Websocket φ
+ π₁ stdlib
  + π₁ native function intent declarations
  + π₁ intent execution

Broadly:

1. [ξ](doc/xi.md)
2. Φ trunk multiplexing
3. γ₀ ED25519 + AES secure channels
4. [π](doc/pi.md)
5. Γ routing


### Fall 2022 deliverables
Ordered by descending priority, give or take. Time estimates are 80% CI.

**Total:** 49.75 days, out of 60.4 days left this year. >80% chance it all gets done, assuming no major setbacks.

If we drop data science, we're down to 34.75 days and a >95% chance the remainder is done.

+ **Done so far:** 3.25/49.75 days
+ **Dropped:** 0.25 days (I canceled these tasks as being unnecessary)
+ **Over/under:** 0.25 days ahead of schedule


### Execution log
+ 2022.1102: -1.75 = 48 (0.75 ahead)
+ 2022.1104: -1.5 = 46.5 (0.25 ahead)


### Next-sprint plan
**Total:** 17 days; goal = 2022.1120

+ [ ] **Primary:** browser-enabled multiway chat = webapp core
  + [ ] Native websocket server -- later we'll also want JSON/REST API
  + [ ] WASM/websocket Φ
  + [ ] Basic DOM -- textbox in → div/pre out
  + [ ] WebGL proof of concept (small mirror of linux/xframe stuff)
+ [ ] **Background:** GL enhancements
  + [ ] Define the operation space more precisely
  + [ ] Z-order blending
  + [ ] Arc rendering
  + [ ] GL images
+ [ ] **Background:** π₀
  + [ ] **Think:** ξ API design
  + [ ] **Think:** γ API design
  + [ ] Generative tests
  + [ ] Performance tests

**TODO:** more of these as I make progress on the ones above


### Fall 2022 list
+ [ ] **15 days:** Web stuff
  + [ ] **3 days:** WASM/websocket Φ
  + [ ] **1 day:** Multiplexer
  + [ ] **2 days:** HTTP server
  + [ ] **2 days:** WebGL rendering
  + [ ] **2 days:** DOM interop
  + [ ] **1 day:** Fragment-shader arc rendering
  + [ ] **1 day:** Scenegraph Z-order blending
  + [ ] **2 days:** GL image → texture support
  + [ ] **1 day:** π₀ image IO?
+ [ ] **17.75 days:** Core runtime
  + [ ] **2 days:** π₀ γ API (**NOTE:** some design-lead time required)
  + [ ] **2 days:** π₀ ξ API (**NOTE:** some design-lead time required)
  + [ ] **3.5 days:** π₀ stress/security testing
    + [ ] **0.5 days:** GC
    + [ ] **2 days:** Randomized tests
    + [ ] **1 day:** Performance tests
  + [ ] **5.25 days:** UTF9
    + [ ] **2 days:** Indexed collections
    + [x] **0.25 days:** Quoted maps
    + [x] **0.5 days:** Allocate bits for bitvector size
    + [x] **0.25 days:** `verify()` detailed return value
    + [x] **0.5 days:** Fix/verify int cast semantics
    + [x] **1.5 days:** Fix/verify `i9` cast semantics generally (optional, probably)
  + [ ] **2 days:** `libsodium` encrypted-Φ/Γ (optional)
  + [ ] **3 days:** π₀ REPL (optional)
+ [ ] **2 days:** ϝ components
  + [ ] **2 days:** Reactive
    + [ ] **1 day:** Union
    + [ ] **1 day:** Rate-limited register
+ [ ] **15 days:** Data science (lower priority)
  + [ ] **1 day:** UTF9 frame format
  + [ ] **0.5 days:** File IO γ
  + [ ] **1.5 days:** JSON ↔ UTF9 (point accessors + whole-object conversion)
  + [ ] **3 days:** Offline bulk sort
  + [ ] **2 days:** Online streaming sort
  + [ ] **2 days:** Shell command γ
  + [ ] **4 days:** Distributed map/reduce
  + [ ] **1 day:** FFT
+ Stretch goals
  + π₁ compiler
  + Unified Γ routing
  + π₂
