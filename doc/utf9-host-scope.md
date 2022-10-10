# UTF9 host scope
Objects that are valid only within the running host that created them. The first byte indicates the object subtype, following bytes carry the data.

+ `00`: reserved
+ `01`: path
+ `02`: PID
