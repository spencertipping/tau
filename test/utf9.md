# UTF9 tests
We'll use π₀ and lazytest to assert various behavior here.

```bash
$ ./pi0 "i8'8 [] i8. i8'8 >>i8s :out"
7 6 5 4 3 2 1 0
$ ./pi0 "i8'8 [] i8. i8'8 >>i8s i8'1 i8s+ :out"
8 7 6 5 4 3 2 1
$ ./pi0 "i8'8 [] i8. i8'8 >>i8s i8'1 % i8s+ :out"
8 7 6 5 4 3 2 1
$ ./pi0 "i8'24 [|n| n= n [] i8. n >>i8s i8'4 i8s> :out _] i8."

```
