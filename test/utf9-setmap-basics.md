# UTF9 set/map basics
```bash
$ ./pi0 '{1,2,3,4,5,6,7,8} 0 >i' 2>/dev/null; echo $?
1
$ ./pi0 '{1,2,3,4,5,6,7,8} m» 0 >i'
```


## Adding stuff
```bash
$ ./pi0 's∅                                         :out
         "foo" s∅ s+                                :out
         "foo" "bar" 2 »s "bif" % s+ "bif" % s+ s»  :out
         m∅                                         :out
         "a" "b" m∅ m+                              :out
         "bif" "foo" {"foo", "bar", "a", "b"} m+ m» :out'
{}
{"foo"}
{"bif","foo","bar"}
{}
{"b":"a"}
{"a":"b", "foo":"bif"}
```


## Union and intersection
```bash
$ ./pi0 '1 2 3 4 4 »s 1 3 5 7 4 »s s| :out
         1 2 3 4 4 »s 1 3 5 7 4 »s s& :out
         {"a","b","c","d"} m» {"a","A","e","f"} m» m| :out
         m∅ m∅                                     m| :out
         {"a","b"} {"c","d"}                       m| :out'
{3,7,4,5,2,1}
{3,1}
{"a":"A", "e":"f", "c":"d"}
{}
{"a":"b", "c":"d"}
```
