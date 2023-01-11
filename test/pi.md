# π tests
+ [Stdlib](pi-stdlib.md)
+ [Return stack](pi-return-stack.md)


## Function definitions
```bash
$ ./pi '[ "hi" :out ] '\''say-hi %=
        say-hi say-hi :data
        [|n| n= say-hi] 4 % i32.'
"hi"
"hi"
"hi"
"hi"
"hi"
"hi"
```

I ran into a bug involving global functions, so let's test those:

```bash
$ ./pi '[ 1 i32+ ] '\''inc %=
        10 [ inc ] i32. 10 »t :out'
(10,9,8,7,6,5,4,3,2,1)
```

(Update: it was just an `osize`/`isize` mixup in `o9idx`.)


## Referential equivalence
```bash
$ ./pi '"foo" :     ≡
        "foo" "foo" ≡
        "foo" "foo" ==
        "foo" 3     == 4 »bs :out'
f t f t
```


## Stack virtualization
```bash
$ ./pi '1 2 3 (, _, _ _) :out :out :out :out'
(3,2,1)
3
2
1
```
