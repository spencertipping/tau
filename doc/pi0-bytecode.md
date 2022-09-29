# π₀ bytecode
A relatively straightforward lookup table of functions, but I want to avoid the usual stack-shuffling prep for binary operations. Those can be addressed directly to local registers.


## Encoding
In a vacuum, I'd define the bytecode instruction schema like this (ignoring the obvious inefficiencies):

```cpp
struct insn
{
  u8      operator;
  u2      operands;
  u8      n_operands[operands == 3];
  u2      n_rets;
  operand xs[n_operands];
  operand ys[n_rets];
  u8      imm_size;
  u8      imm[imm_size];
};

union operand : u1  // tagged union
{
  struct stack {};  // top of stack, no additional info
  struct local
  {
    u1 erase;       // if true, erase after reading (for GC)
    u8 r;           // which local register
  };
};
```


### Packing the bytecode operator
Like [UTF9](utf9.md), bytecodes have two parts: one to select the operation and one to indicate argument/return placement. This is done using _bytecode containers_, which are encoding adapters that refer to a defined bytecode function and provide operand/return interfacing in the process. You can think of bytecode containers as being like calling conventions with customizable slots.

Each container dispatches on a single byte.
