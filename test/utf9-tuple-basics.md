# Tuple basics
```bash
$ ./pi0 '(1,2,3) (4,5,6) t++ [1 i32+] t* :out
         (1,2,3,4,5,6) [3 % i32%] t* [2 i32>] t% :out'
(5,6,7,2,3,4)
(1,0,1,0)
```


## Indexes
```bash
$ ./pi0 '0 >>t :data'
[0]	()
$ ./pi0 '[ :data :f :outn " " :cout i< :out _ ] '\''each   %=
         [|n| n= n [1 i32+] i32. n >>t |] '\''ntuple %=
         [ [|b n| n= : b= n ntuple b >i :data each |]
           10 % i32. _ ]
         4 % u8.'
```
