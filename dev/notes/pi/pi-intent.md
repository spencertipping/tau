# π intent
π intents are similar to types, but they don't have a direct correspondence to the way values are encoded; almost all values in π programs are encoded in UTF9, which includes RTTI. (Note that this still leaves room for optimization-by-specialization, which the intent system achieves.)

An intent is an expectation about a value or a function, which often happens to look a lot like a type. For example, `i64` means you intend for a value to be a signed 64-bit int. `φ utf8` is a φ carrying UTF-8 strings. `human` is a bidirectional φ corresponding to a human using some user interface. `server` is a machine that can host Φs, and which is probably intended to run large computations.

**The goal of the intent system is to choose concrete representations, which is the same as choosing function variants.**


## Entropy basis shifts
If matrices shift linear bases, then distributions shift entropy bases. And just as machine-coding effort is measured in bits, human-coding effort is also measurable -- and we can refer to that as "entropy" as well. In practice it's derived from ergonomic properties, but the principle is the same.

So a π intent basis shift amounts to a transcoder: decoding human inputs along the ergonomic entropy distribution and encoding along the conditionally-informed output probability distribution (where conditions incorporate whatever intent constraints that may apply).


## Environmental intent
Many data-processing workflows defy useful typing because the data may have exceptional cases or its schema may be poorly-specified. The simplest ways to work with such data are (1) to ignore types altogether, or (2) to examine the data and infer a type structure.

Intent is more open-ended in that it is free to capture more nebulous aspects of the data, for instance mean and range as well as the type. So if you're processing a series of JSON records some of whose fields are polymorphic, you might end up with a data intent like this:

```
('json,
  { 'observations: 24,
    'encoding:     'compact },
  { 'id:     ('int,   { 'density:    1,
                        'min:        10001,
                        'unique:     t
                        'uniqueness: {'entropy: 14.311} }),
    'author: ('utf8,  { 'density:    1,
                        'charset:    {'alnum, "_[]"},
                        'l2size:     (...),
                        'unique:     f,
                        'uniqueness: {'μ: 8.95} }),
    'posts:  ('array, { 'density:    0.889,
                        'l2size:     (3,8,5,4,2,1,1),
                        'contents:   ...,
                         ... }),
    ...})
```

This data-guided profile informs validity, optimization choices, and other things a normal type system would have difficulty with.

Because intents must not involve side effects, there are instead two ways you can apply an inference system to real-world data:

1. Include a subset of records into your source code as a quoted resource
2. Run an analysis separately and include the result into your source code

(2) removes provenance from the resulting intent information, which is useful if the inference process consumes far more data than it produces -- e.g. when you need precision.


## Data semantics
If `φ i64` is a φ of ints, then `φs i64` might be many φs of ints. While useful for work distribution, `φs` by itself isn't sufficient to understand more subtle aspects like how the data might be sharded or ordered. We want an intent that describes enough for us to know how each φ needs to be handled.

**Q:** is `φs` an intent or a variable? If a variable, how do we constrain it (or do we, beyond C++ template-style duck typing)?


## Compilation
Intents of the form _a → b_ are compositional structures that can be compiled. For example, `int → int` is ambiguous, `a → a + 1` less so. If we join those constraints, we get `{int → int, a → a + 1}`, which uniquely selects a function -- in this case `int+` applied to `1`. `1` is itself an intent, encoded as `('lit, 1)`.

This continuity between "type-like things" and "value-like things" is important: although many functional languages are designed such that their value and type spaces are somewhat orthogonal, the compiler is helpful only when inferring types. π is different in that both are considered unknowns that the compiler is allowed to infer. Because this often results in ambiguity, the compiler is required to minimize the global (conditionally-aware) entropy of the result.

Compilation is executed by [the parser](pi-parsing.md), whose purpose is to shift the entropy basis from user input to compiled output.
