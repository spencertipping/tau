# π scripting
π is a series of [φ](phi.md) parsers that allow [η](eta.md)-transforming functions to be defined from string inputs, ultimately making it possible to write many τ programs without compiling code.


## π program structure
A π program is just a C++ lambda of the form `F<πv(πi&)>`, where `πi` is a structure that stores several elements:

+ `ξi`: current input
+ `ξo`: current output
+ `M<St, πv>`: bound variables

The return value is the evaluated value of the expression, after side effects against `πi`.
