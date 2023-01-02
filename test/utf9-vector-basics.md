# Vector basics
```bash
$ ./pi0 "i8'8 [] i8. i8'8 >>i8s :out
         i8'8 [] i8. i8'8 >>i8s i8'1 i8s+ :out
         i8'8 [] i8. i8'8 >>i8s i8'1 % i8s+ :out"
7 6 5 4 3 2 1 0
8 7 6 5 4 3 2 1
8 7 6 5 4 3 2 1
$ ./pi0 "i8'8 []       i8. i8'8 >>i8s
         i8'8 [_ i8'4] i8. i8'8 >>i8s i8s> :out"
f f f f t t t t
$ ./pi0 "i8'24 [|n| n= n [] i8. n >>i8s i8'4 i8s> :out _] i8."
b[]
t
t t
t t t
t t t t
f t t t t
f f t t t t
f f f t t t t
f f f f t t t t
f f f f f t t t t
f f f f f f t t t t
f f f f f f f t t t t
f f f f f f f f t t t t
f f f f f f f f f t t t t
f f f f f f f f f f t t t t
f f f f f f f f f f f t t t t
f f f f f f f f f f f f t t t t
f f f f f f f f f f f f f t t t t
f f f f f f f f f f f f f f t t t t
f f f f f f f f f f f f f f f t t t t
f f f f f f f f f f f f f f f f t t t t
f f f f f f f f f f f f f f f f f t t t t
f f f f f f f f f f f f f f f f f f t t t t
f f f f f f f f f f f f f f f f f f f t t t t
```
