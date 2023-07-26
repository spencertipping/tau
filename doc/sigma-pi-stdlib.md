# σ/π stdlib
See [π](pi.md) for an overview of the language structure. This page describes the functions/operators that σ defines for π.


## Examples
```bash
$ diff <(bin/sigma-fast 'n10M?>_') <(seq 0 9)
$ diff <(bin/sigma-fast 'n10p[x+1]M?>_') <(seq 10)
$ diff <(bin/sigma-fast ':foo [n10p[x+1]] :bar [n20] $foo M?>_') <(seq 10)
$ bin/sigma-fast 'n1pακιτωΩρθ;M?>_'
α κ ι τ ω Ω ρ θ
$ bin/sigma-fast 'i5N p@>@; p>>|x; M?>_'
(2) (3) (4)
```


## TODO: write up operators
