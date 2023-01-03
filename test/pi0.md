# π₀ tests
+ [Stdlib](pi0-stdlib.md)
+ [Return stack](pi0-return-stack.md)


## Function definitions
```bash
$ ./pi0 '[ "hi" :out ] '\''say-hi %=
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
$ ./pi0 '[ 1 i32+ ] '\''inc %=
         10 [ inc ] i32. 10 »t :out'
(10,9,8,7,6,5,4,3,2,1)
```

(Update: it was just an `osize`/`isize` mixup in `o9idx`.)


## Referential equivalence
```bash
$ ./pi0 '"foo" :     ≡
         "foo" "foo" ≡
         "foo" "foo" ==
         "foo" 3     == 4 »bs :out'
f t f t
```
