# Tuple basics
```bash
$ ./pi '(1,2,3) (4,5,6) t++ [1 i32+] t* :out
        (1,2,3,4,5,6) [3 % i32%] t* [2 i32>] t% :out'
(5,6,7,2,3,4)
(1,0,1,0)
```


## Indexes
```bash
$ ./pi '0 »t : ?S :data'
[0]	2
[1]	()
$ ./pi '[ :f : :outn " " :cout i< :out ] '\''each   %=
        [|n| n= n [1 i32+] i32. n »t |]  '\''ntuple %=
        [ [|b n| n= : b= n ntuple b >i each |]
          10 % i32. _ ]
        4 % u8.'
i0() u8[]
i0(1) 0 2
i0(2,1) 0 2 1 8
i0(3,2,1) 0 2 1 8 2 14
i0(4,3,2,1) 0 2 1 8 2 14 3 20
i0(5,4,3,2,1) 0 2 1 8 2 14 3 20 4 26
i0(6,5,4,3,2,1) 0 2 1 8 2 14 3 20 4 26 5 32
i0(7,6,5,4,3,2,1) 0 2 1 8 2 14 3 20 4 26 5 32 6 38
i0(8,7,6,5,4,3,2,1) 0 2 1 8 2 14 3 20 4 26 5 32 6 38 7 44
i0(9,8,7,6,5,4,3,2,1) 0 2 1 8 2 14 3 20 4 26 5 32 6 38 7 44 8 50
i1() u8[]
i1(1) u8[]
i1(2,1) 0 2
i1(3,2,1) 0 2
i1(4,3,2,1) 0 2 2 14
i1(5,4,3,2,1) 0 2 2 14
i1(6,5,4,3,2,1) 0 2 2 14 4 26
i1(7,6,5,4,3,2,1) 0 2 2 14 4 26
i1(8,7,6,5,4,3,2,1) 0 2 2 14 4 26 6 38
i1(9,8,7,6,5,4,3,2,1) 0 2 2 14 4 26 6 38
i2() u8[]
i2(1) u8[]
i2(2,1) u8[]
i2(3,2,1) u8[]
i2(4,3,2,1) 0 2
i2(5,4,3,2,1) 0 2
i2(6,5,4,3,2,1) 0 2
i2(7,6,5,4,3,2,1) 0 2
i2(8,7,6,5,4,3,2,1) 0 2 4 26
i2(9,8,7,6,5,4,3,2,1) 0 2 4 26
i3() u8[]
i3(1) u8[]
i3(2,1) u8[]
i3(3,2,1) u8[]
i3(4,3,2,1) u8[]
i3(5,4,3,2,1) u8[]
i3(6,5,4,3,2,1) u8[]
i3(7,6,5,4,3,2,1) u8[]
i3(8,7,6,5,4,3,2,1) 0 2
i3(9,8,7,6,5,4,3,2,1) 0 2
```

We also need to make sure that indexed and non-indexed collections behave identically with respect to lookups.

```bash
$ ./pi '
  [|n| n= n [] i32. n »t  |] '\''nt %=  5 nt :out
  [|n| n= n [] i32. n »s  |] '\''ns %=  5 ns :out
  [nt : [7 i32+] t* % ^m m»] '\''nm %=  5 nm :out
  [|n b t T s S m M| b= n=
    n nt   t=  n ns   s=  n nm   m=
    t b >i T=  s b >i S=  m b >i M=
    n 10 i32+
      [ : : t t@ % T t@ == [] [("FAIL", "t", n, b, ) :out] ?.
        : : s s? % S s? == [] [("FAIL", "s", n, b, ) :out] ?.
        : : m m@ % M m@ == [] [("FAIL", "m", n, b, ) :out] ?. _ ] i32.
  |] '\''test %=
  100 [|n| n= 5 [n % test] i8. |] i32.
'
(4,3,2,1,0)
{1,3,2,4,0}
{1:8, 3:10, 2:9, 4:11, 0:7}
```

Debugging workbench for the above:

```sh
$ ./pi '
  [|n| n= n [] i32. n »t |]     '\''ntuple %=
  [|n| n= n [] i32. n »s |]     '\''nset   %=
  [ ntuple : [7 i32+] t* % ^m ] '\''nmap   %=

  [|n b x t T s S m M| >i32 x= b= n=
    n ntuple t=  n nset s=  n nmap m=
    t b >i   T=  s b >i S=  m b >i M=

    # ("t", T i<, T i>) :out
    # ("s", S i<, S i>) :out
    ("m", M i<, M i>) :out

    ( # x t t@, x T t@,
      # x s s?, x S s?,
     x m m@, x M m@) :out
  |] '\''test-debug %=

  15 0 24 test-debug
  15 1 24 test-debug
  15 2 24 test-debug
'
```
