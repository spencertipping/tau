# Casting
```bash
$ ./pi "i8'1 >i32 ?t :out"
'i32
$ ./pi "i8'100 >f64 ?t :out"
'f64
```


## Vector construction
```bash
$ ./pi '
  (1,2,3) >i8s  : ?t 2 »t :out
  (1,2,3) >i16s : ?t 2 »t :out
  (1,2,3) >i32s : ?t 2 »t :out
  (1,2,3) >i64s : ?t 2 »t :out
  (t,f,t) >bs   : ?t 2 »t :out
'
('i8,1 2 3)
('i16,1 2 3)
('i32,1 2 3)
('i64,1 2 3)
('b,t f t)
```
