# π₀ UTF9
[UTF9](utf9.md) reserves the `11100___` prefix for π use. These values are non-portable, meaning that they will never leave a π heap context; that means we can use native endianness and sizing. The only constraint is that we be able to remove any such values from UTF9s we're working with, e.g. before sending them across a [φ](phi.md).

π-UTF9 values begin with a byte that describes their purpose:

| Byte | Purpose                      |
|------|------------------------------|
| `00` | **reserved**                 |
| `01` | Reference to colocated value |

Right now most of the values are reserved for future use. I created this data structure just to get references, but I imagine I'll want more things in the future -- like lazy evaluation, perhaps.


## `01`: reference to colocated value
Causes any UTF9 value that contains it to be flagged as "complex" within the GC. References contain two fields encoded like this:

```cpp
struct reference
{
  ri r;  // register where the referent is stored
  uN o;  // byte offset within that referent
};
```
