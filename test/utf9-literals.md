# Literals
```bash
$ ./pi0 '"foo" :out "foo\n" :cout ("foo", "bar") :out {"foo", "bar"} :out'
"foo"
foo
("foo","bar")
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

We should also be able to write tuples and maps in fully-packed format:

```bash
$ ./pi0 '(1,2,3) :out {1,2,3,4} :out'
(1,2,3)
{1:2, 3:4}
```
