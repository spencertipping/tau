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
$ ./pi0 "0 1 2 :data"
[0]	2
[1]	1
[2]	0
$ ./pi0 "0 ?t 127 ?t 128 ?t 32767 ?t :data"
[0]	'i16
[1]	'i16
[2]	'i8
[3]	'i8
$ ./pi0 "32768 ?t 2147483647 ?t :data"
[0]	'i32
[1]	'i32
$ ./pi0 "10 [] i16. 10 >>i32s :out"
9 8 7 6 5 4 3 2 1 0
$ ./pi0 "10 [] i16. 10 >>c32s :out"
(9,0) (8,0) (7,0) (6,0) (5,0) (4,0) (3,0) (2,0) (1,0) (0,0)
```


## Casting
```bash
$ ./pi0 "i8'1 >i32 ?t :out"
'i32
$ ./pi0 "i8'100 >f64 ?t :out"
'f64
```
