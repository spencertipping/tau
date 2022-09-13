# π history
The π language is explicitly designed with history management in mind: the source code has version control built in. It presents as an append-only record of actions that are incrementally indexed for performance. In this sense it borrows from Smalltalk and other image-oriented development platforms.

**TODO:** define how this works

+ `stream > obj` edit-fusion structure
+ Log keystrokes: that's history
+ Keystrokes → parser combinator type things, which form actions
  + Just a prefix tree, right?
+ Snapshot `obj` sometimes
+ Easy diff view: how do we do this? Union/intersection in tree space?
+ Reconstruct history → `reflog` with full history
