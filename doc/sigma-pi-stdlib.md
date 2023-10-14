# σ/π stdlib
See [π](pi.md) for an overview of the language structure. This page describes the functions/operators that σ defines for π.


## Examples
```bash
$ diff <($sigma 'n10') <(seq 0 9)
$ diff <($sigma 'n10p[x+1]') <(seq 10)
$ diff <($sigma ':foo [n10p[x+1]] :bar [n20] $foo') <(seq 10)
$ $sigma 'n1pακιτωΩρθ'
α κ ι τ ω Ω ρ θ
$ $sigma 'i5N p@>@; px>>|'
(2) (3) (4)
$ $sigma 'n1p@k("a" 1 "b" 2)'
"a" "b"
$ $sigma 'n1p@v("a" 1 "b" 2)'
1 2
```


## TODO: write up operators
