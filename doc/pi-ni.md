# π ← `ni`
[`ni`](https://github.com/spencertipping/ni), or more precisely Perl, is very good at streaming data processing, and I want π to inherit its best qualities. I suspect APL is equally good if not better, but it's apples-and-oranges because Perl is optimized for unstructured text.


## `ni` for IO
`ni` crushes the IO game: `.` = `ls`, `\<` = `xargs cat`, `zz\>` = `| zstd >`, `+[...]` = `{ _ ; ...; } |`, etc. All of this moves system-level resources like files and directories into easy reach from in-core code. Another way to see it is that `ni` begins by decisively breaking down the barrier between "in-core" and "out-of-core". IO is assumed, so it's ergonomic.


## Quasiquoting
`r()` and Perl's list-flattening behavior conspire to provide powerful quasiquotation in TSV-space. This makes `r` behave in a number of different ways depending on how it's used:

+ Transpose-into-row
  + `/foo/g` (stream out) vs `r /foo/g` (gather into tuple)
  + `r a, b, c + 1, d`: stream `a, b, c+1, d` vertically, then collect
  + `r a, a + 1`: expand on `a`
+ `r ... for ...`: atomize with specified granularity
+ `r map ...` (collect) vs `map r, ...` (stream)

Most of these boil down to either transposing the data or, when used in conjunction with `a b c` destructuring, working as a poor-man's vectorized transformation setup. Here's an example of the difference:

```
r a, b, c + 1, d  ← destructure + quasiquote
$_ + 0 0 1 0      ← vectorized transform
$_ + c(1)         ← sparse vector
```

**NOTE:** if we want vectorized transforms like this, `0` **must** operate as a true identity, even across values of different types. That is, `x + 0` must always be strictly equal to `x`, including type. (So no `+0` coerces-to-number shenanigans.)


### Functions
Ideally we generalize the above transformation structure to functions, not just a single operator. Then we could combine operations of different types within the same stream of modifications.

```
r a + 1, substr(b, 5), c =~ /^(\d+)/  ← destructure
$_ : +1 ↓5 /^(\d+)/                   ← vectorized fns
```


## Streaming/π handoff
I recently wrote this in `ni`:

```sh
$ ni . r/xml/ W\< p'r a, /"([^"]+(?:\.tar|\.zip))"/' rB \
     p'join"", "https://archive.org/download/",
               a =~ s/_files.xml//r =~ s/^..//r,
               "/" . b' e[xargs wget -c]
```

Breaking it down:

+ List all XML files
+ `cat` each with the filename prepended to each line
+ Cut the lines down to just those involving `/"(<stuff>.(?:tar|zip))"/`, selecting just that text from those lines (and keeping the original filenames as field `a`)
+ Build URLs by splicing strings: some are constant, others are not
  + Transform the filenames by removing `_files.xml` suffix and `./` prefix (enhanced `basename`)
  + Splice in constants around/between transformed-`a` and `b`

This is a typical simple `ni` workflow: each record is processed independently, so we have no vertical aggregation. What would it look like in π?


### Independent streaming in π
```
*.xml ^[k↓2↑!/_files.xml/ /"([^"]+(?:\.tar|\.zip))"/<]
      ¨/+["https://archive.org/download" a '/ b]
      ë[wget -c]
```

Note that function composition is implied here: `↓2↑5` would be an anonymous function since the rightmost operator is underapplied.

**TODO:** figure out how we delineate operands; the above `[]` is serviceable, but kinda clunky

+ `*.xml` resolves to a shell wildcard, so you get a stream of matching filenames
+ `^` begins a zip; each side is a function that starts on the right
  + Left side:
    + `↑!/_files.xml/` takes until the start of `/_files.xml/`
    + `↓2` drops the leading `./` (which wouldn't be here, but I've got it for parity with `ni`)
    + `k` streams that infinitely
  + Right side:
    + `<` reads the file
    + `/"(...)"/` streams matches from lines, implicitly collapsing non-matches
+ `¨/+` sums each thing, in this case by joining strings
  + `/+` by itself would sum the whole stream; `¨` makes it itemize
+ `ë[...]` = `e[xargs ...]`, borrowing the `¨` notation from APL


### Syntactic disambiguation
`+` and `*` are conveniently illegal at the beginning of a regex, so we can always assume that `/+` is a reduction whereas `/f` begins a regex. More generally, I think we can attempt the functional parse first, then fall back to regex interpretation if that fails.

I should also note that π source isn't strictly text, so we probably have some options. We may be able to fuse `/+` together into a sort of ligature, for example. Same for `/<`, which might check for total ordering but which is also something you might write as a regex. The [compiler](pi-compiler.md) may be able to choose the right alternative in realtime by looking at the type of data being transformed.
