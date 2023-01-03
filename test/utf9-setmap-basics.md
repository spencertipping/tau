# UTF9 set/map basics
```bash
$ ./pi0 '{1,2,3,4,5,6,7,8} 0 >i' 2>/dev/null; echo $?
1
$ ./pi0 '{1,2,3,4,5,6,7,8} m» 0 >i'
```


## Adding stuff
```bash
$ ./pi0 's∅ :out _
         "foo" s∅ s+ :out _
         "foo" "bar" 2 »s "bif" % s+ "bif" % s+ s» :out _'
{}
{"foo"}
{"bif","foo","bar"}
$ ./pi0 'm∅ :out _
         "a" "b" m∅ m+ :out _
         "bif" "foo" {"foo", "bar", "a", "b"} m+ m» :out _'
{}
{"b":"a"}
{"a":"b", "foo":"bif"}
```
