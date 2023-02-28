# η heap/container encoding
η data has a few properties:

1. It's contiguous, up to η₀ nonlinear references
2. A tag bit indicates whether it or any descendant contains a [η₀](eta0.md) reference
3. Its size is knowable in constant time
4. Its type is encoded as an integer, where `0` is reserved for η₀
5. Data is stored native-endian, sizes are stored fixed-endian
6. Data can be transparently compressed with `zstd`

Specifications:

+ [η₀ spec](eta0.md) for the bit-level format and container-level attributes
+ [η₁ spec](eta1.md) for the user-level API

**TODO:** design Asqi dataflow and make sure we have enough support here


## API
**TODO:** design the API more proactively; current `η0o{}` shenanigans are suboptimal, both from a design and an implementation perspective

**TODO:** `std::get<>` destructuring

**NOTE:** η₁ casting is also suboptimal; although it's fine to have different views, we shouldn't have to explicitly cast. Instead, a unified η should do this for us and allow for type-aware iteration.

**NOTE:** we need the ability to store native pointers (in η₀ references) with untyped destructor function references; that means two adjacent `void*`s, most likely, as well as [ξ](xi.md) destructors


## Primitive user types
| Code | Description | Spec                       |
|------|-------------|----------------------------|
| 1    | `signal`    | [ζ signal](zeta-signal.md) |
| 2    | `symbol`    | [η symbol](eta-symbol.md)  |
| 3    | `bytes`     |                            |
| 4    | `utf8`      |                            |
| 5    | `int/be`    | Big-endian signed int      |
| 6    | `uint/be`   | Big-endian unsigned int    |
| 7    | `float/be`  | Big-endian float           |
| 8    | `boolean`   |                            |


## Container types
| Code | Description | Format                  |
|------|-------------|-------------------------|
| 9    | `tuple`     | `x1 x2 ... xn`          |
| 10   | `set`       | `x1 x2 ... xn`          |
| 11   | `map`       | `k1 v1 k2 v2 ... kn vn` |

**NOTE:** maps are guaranteed to have an even number of elements; otherwise they fail `η₀bc` and are η₁-invalid.

**TODO:** add a dedicated type for file descriptors so we can close them automatically if any are left behind in ξs. Otherwise we leak FDs if a consumer closes its input.


## Vector types
**TODO:** as needed
