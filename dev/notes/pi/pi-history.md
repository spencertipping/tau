# π history
The π language is explicitly designed with history management in mind: the source code has version control built in. It presents as an append-only record of actions that are incrementally indexed for performance. In this sense it borrows from Smalltalk and other image-oriented development platforms.

+ Easy diff view: how do we do this? Union/intersection in tree space?
+ Reconstruct history → `reflog` with full history


## Version control as a compression format
It's no coincidence that git uses an LZ77-style delta strategy to compact its files. History tends to repeat itself, particularly when you have multiple branches.
