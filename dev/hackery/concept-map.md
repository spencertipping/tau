# Concept mapping experiment
Does anything interesting happen if we use words-within-documents as a linkage basis within a 2.5D world of information? I'm interested to determine whether documents are written with enough locality to visualize to something meaningful.

There are two possible versions of this experiment:

1. Markdown files (human text)
2. Code files (computer text)

I think we can ignore code for now. I want to see what we get with markdown.


## Initial coordinates
For visual clarity, let's say we have anchor points from which traversal occurs along angle-quantized trajectories. Some trajectories may be multistep, and they form a Huffman-style code; common words like "and" and "the" will have short paths, less-common words will have longer paths.

**NOTE:** this isn't quite right; words move you _towards_ something, not _away from_ something -- unless we're looking at it in `word2vec` terms.

We'll use some rules for path construction, for example the path can change direction at most twice. That will create longer paths quickly, which we want. We also can't have any path doubling back on itself, as this creates shortcuts. Maybe we just want displacements, not paths as such.

Three first steps:

1. Gather a set of markdown files and compute word frequencies
2. Use those frequencies to assign a trajectory per word
3. Generate a map


## Markdown sources and word frequencies
```sh
$ ni e[find ~ -xdev -name '*.md'] \
     rp'!/node_modules|site-packages|\.cache/' \
     FDp'r F_(0..$_) for 0..FM' F^:/rp'-d "$_/.git"' \
     gcO_
281  /home/spencertipping/r/cycles
124  /home/spencertipping/r/caterwaullabs/gitarc
101  /home/spencertipping/r/public/caterwaul
83   /home/spencertipping/.bash
71   /home/spencertipping/r/public/ni
71   /home/spencertipping/.bash/ni
56   /home/spencertipping/r/caterwaullabs/oafley-dadbot
45   /home/spencertipping/r/profiles/cooking/.config/google-chrome
42   /home/spencertipping/r/caterwaullabs/splunge
29   /home/spencertipping/r/foobarindustrial/robotics
25   /home/spencertipping/r/clients/predictionmachine/punoqun
21   /home/spencertipping/.emacs.d
19   /home/spencertipping/r/public/www
19   /home/spencertipping/r/internal/resources
18   /home/spencertipping/r/profiles/asqi/.config/google-chrome
```

`cycles` it is. Yet another reason this repo can't be made public. Filtering out the `transformers` submodule:

```sh
$ ni e[find ~/r/cycles -xdev -name '*.md'] rp'!/transformers/' wcl
182
```

Awesome, still more than enough. Let's make the word count, both locally and for all words. Having both will help us place our project within a larger frame of reference.

```sh
$ ni e[find ~/r/cycles -xdev -name '*.md'] rp'!/transformers/' \
     \<plc FWZ1gcxzz\>concept-map-wc

se0.128$ ni /data/the-pile/mystic.the-eye.eu/public/AI/pile/train \
            r/zst$/fx64[%f : i%f \<D:text plc FWZ1Ux\>] \
            \<\#g,sgA zz:/data/the-pile/wc \
            riB/usr/share/dict/words zz\>/data/the-pile/wc-wamerican
```
