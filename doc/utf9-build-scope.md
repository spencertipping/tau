# UTF9 build scope
Objects that are valid only within the native build that created them. The first byte indicates the object subtype, following bytes carry the data.

+ `00`: reserved
+ `01`: `struct`


## `struct`
This is a way to pass around `struct` instances that are encoded with native-endianness and alignment. You can `Sc<type*>(i9)` to decode them.

By default, `struct` includes no RTTI; it's up to you to make sure you know what type of struct you're decoding. The only sanity check is that the struct size is matched against the UTF9 size.
