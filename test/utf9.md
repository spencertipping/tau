# UTF9 tests
We'll use π₀ and lazytest to assert various behavior here.


## Vector basics
```bash
$ ./pi0 "i8'8 [] i8. i8'8 >>i8s :out"
7 6 5 4 3 2 1 0
$ ./pi0 "i8'8 [] i8. i8'8 >>i8s i8'1 i8s+ :out"
8 7 6 5 4 3 2 1
$ ./pi0 "i8'8 [] i8. i8'8 >>i8s i8'1 % i8s+ :out"
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


## Literals
```bash
$ ./pi0 '"foo" :out'
"foo"
$ ./pi0 '"foo\n" :cout'
foo
$ ./pi0 '("foo", "bar") :out'
("foo","bar")
$ ./pi0 '{"foo", "bar"} :out'
{"foo":"bar"}
```


## Casting
```bash
$ ./pi0 "i8'1 >i32 ?t :out"
'i32
$ ./pi0 "i8'100 >f64 ?t :out"
'f64
```
