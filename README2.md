# τ: programming in 𝓛 space
τ is a runtime system built around temporary steady states. If a bash command like `cat ... | grep ... | gzip > ...` defines a pipeline in a time-invariant way, then τ extends this by adding the ability to modify and extend the pipeline at runtime; that is, each part of the program is time-invariant but there are step changes. ([`ni`](https://github.com/spencertipping/ni) captures some of this, but ultimately falls short.)

Another way to think of it is that τ is to `ni` what the Laplace transform is to the Fourier transform: we can now describe systems that are locally time-invariant but which nonetheless change over time.


## Φ runtime system
A garbage-collected dataflow graph whose components ([γ](doc2/gamma.md)) are cooperatively multithreaded ([λ](doc2/lambda.md) and [Λ](doc2/Lambda.md)) and communicate with FIFO queues ([ζ](doc2/zeta.md), a vector of which is called [φ](doc2/phi.md)). All side-effects are contained as messages on these FIFOs, sent to a process boundary called [Φ](doc2/Phi.md).

We can define a GC for γs using the φ graph: a γ is live only when there's a ζ-path to Φ.


## π scripting
[π](doc2/pi.md) is a small concatenative bytecode language used to implement individual λs. It also hosts the [Γ](doc2/Gamma.md) matrix compiler API, which is used to create Φ dataflow components.

π manipulates [η](doc2/eta.md) values; [η₀](doc2/eta0.md) is the container format, inside which types such as η₁, η₂, etc are allocated. η is platform-independent with a few exceptions; platform-locked values are tagged so that they cannot be sent elsewhere without first being converted.


## Components
+ [Φ boundary](doc2/Phi.md)
  + [γ matrix](doc2/gamma.md)
  + [λ vector](doc2/lambda.md)
  + [Λ scheduler](doc2/Lambda.md)
+ [φ vector](doc2/phi.md)
  + [ζ channel](doc2/zeta.md)
  + [ζ signal](doc2/zeta-signal.md)
+ [η encoding](doc2/eta.md)
  + [η₀ container](doc2/eta0.md)
  + [η₁ API](doc2/eta1.md)
  + [η symbol](doc2/eta-symbol.md)
