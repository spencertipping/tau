# τ: programming in 𝓛 space
τ is a runtime system built around temporary steady states. If a bash command like `cat ... | grep ... | gzip > ...` defines a pipeline in a time-invariant way, then τ extends this by adding the ability to modify and extend the pipeline at runtime; that is, each part of the program is time-invariant but there are step changes. ([`ni`](https://github.com/spencertipping/ni) captures some of this, but ultimately falls short.)

Another way to think of it is that τ is to bash what the Laplace transform is to the Fourier transform: we can now describe systems that are locally time-invariant but which nonetheless change over time.


## Φ runtime system
A garbage-collected dataflow graph whose components ([γ](doc2/gamma.md)) are cooperatively multithreaded ([λ](doc2/lambda.md) and [Λ](doc2/Lambda.md)) and communicate with FIFO queues ([φ](doc2/phi.md) and [ζ](doc2/zeta.md)). All side-effects are contained as messages on these FIFOs, sent to a process boundary called [Φ](doc2/Phi.md).

We can define a GC for γs using the φ graph: a γ is live only when there's a φ-path to Φ.


## π scripting
[π](doc2/pi.md) is a small concatenative bytecode language used to implement individual λs. It also hosts the [Γ](doc2/Gamma.md) matrix compiler API, which is used to create Φ dataflow components.


## Components
+ [Φ boundary](doc2/Phi.md)
  + [γ matrix](doc2/gamma.md)
  + [λ vector](doc2/lambda.md)
  + [Λ scheduler](doc2/Lambda.md)

**TODO:** should φ be a port as it is in tau1, or should it be a FIFO like ζ is?
