# _Γ ∷ Ξ → Ξ_
Γs transform [Ξs](Xi.md) and can be combined associatively like matrices.

Some Γs manipulate Ξ state, while others transform [ξ](xi.md) duplexes by creating [ψ](psi.md) processor elements. Common ψ topologies are factored into the [Ψ](Psi.md) library of Γs.

```bash
$ bin/gamma-fast
0
0
55
500500
50005000
110
```

Some fork tests:

```bash
$ diff <(bin/sigma-fast 'n4 P2px+1 M?>_') <(seq 1 4)
$ diff <(bin/sigma-fast 'n4 P2px+1 P2px+1 M?>_') <(seq 2 5)
$ diff <(bin/sigma-fast 'n4 P4px+1 P4px+1 P2px+1 M?>_') <(seq 3 6)
$ diff <(bin/sigma-fast 'n4000 P4px+1 P4px+1 P2px+1 M?>_') <(seq 3 4002)
```

And some fork/exec tests:

```bash
$ diff <(bin/sigma-fast 'n0x["/bin/seq" "100"] x["/bin/cat"] >F1') <(seq 100)
$ diff <(bin/sigma-fast 'n0x["/bin/seq" "100"] x["/bin/cat"] x["/bin/cat"] >F1') <(seq 100)
```
