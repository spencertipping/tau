# Concept mapping experiment
Does anything interesting happen if we use words-within-documents as a linkage basis within a 2.5D world of information? I'm interested to determine whether documents are written with enough locality to visualize to something meaningful.

There are two possible versions of this experiment:

1. Markdown files (human text)
2. Code files (computer text)

I think we can ignore code for now. I want to see what we get with markdown.


## Initial coordinates
For visual clarity, let's say we have anchor points from which traversal occurs along angle-quantized trajectories. Some trajectories may be multistep, and they form a Huffman-style code; common words like "and" and "the" will have short paths, less-common words will have longer paths.

We'll use some rules for path construction, for example the path can change direction at most three times. That will create longer paths quickly, which we want.

Three first steps:

1. Gather a set of markdown files and compute word frequencies
2. Use those frequencies to assign a trajectory per word
3. Generate a map


## Markdown sources and word frequencies
**TODO**
