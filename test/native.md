# Native tests
## ζ
```bash
$ ./zeta
t write-closed
received 1 2 3 5 8 13 21 34
received "foo"
received ("foo","bar","bif",1,(2,3),3,4.5,"bar",6.7,"abc",ω,τ⁵,θ²0.5,(5,8.9),"bif")
received ("foo","bar","bif",1,(2,3),3,4.5,"bar",6.7,"abc",ω,τ⁵,θ²0.5,(5,8.9),"bif")
received i9[Φ:0:9] = ("foo","bar","bif",1,(2,3),3,4.5,"bar",6.7,"abc",ω,τ⁵,θ²0.5,(5,8.9),"bif")
received ("foo","bar")
received {"foo":"bar", "bif":1, (2,3):3, 4.5:"bar", 6.7:"abc", ω:τ⁵, θ²0.5:(5,8.9), "bif":}
received {"foo","bar","bif",1,(2,3),3,4.5,"bar",6.7,"abc",ω,τ⁵,θ²0.5,(5,8.9),"bif"}
t is all good
(0)
(0,1)
(0,1,2)
(0,1,2,3)
(0,1,2,3,4)
(0,1,2,3,4,5)
(0,1,2,3,4,5,6)
(0,1,2,3,4,5,6,7)
(0,1,2,3,4,5,6,7,8)
(0,1,2,3,4,5,6,7,8,9)
(1,2,3,4,b"ABCDabcd")
("foo","bar",t,b"ABCDa")
(1.2,3.4,"foo",5,6,b"A")
```


## π GC
```bash
$ ./pi-gc | perl -npe 's/^0x.*?([+:])/$1/'
pre-GC heap size = 103
: "new value for a"
: ("foo",π₀r:(1,2,3),π₀r:(1,2,3),"bar")
: (1,2,3)
: (t,f,π₀r:("foo",π₀r:(1,2,3),π₀r:(1,2,3),"bar"),π₀r:(1,2,3),π₀r:("foo",π₀r:(1,2,3),π₀r:(1,2,3),"bar"))
: t
post-GC heap size = 81
+17: "new value for a"
+34: ("foo",(1,2,3),π₀r:(1,2,3),"bar")
+11: (1,2,3)
+64: (t,f,("foo",(1,2,3),π₀r:(1,2,3),"bar"),π₀r:(1,2,3),π₀r:("foo",(1,2,3),π₀r:(1,2,3),"bar"))
+3: t
simple GC OK
4
3
```
