# Ω database
[η](eta.md) values normally live in memory; Ω gives you a way to easily save them to disk. Ω is a transactional key/value store that uses a hashed ordering to get _O(lg lg n)_ average-case lookup time against packed files. File operations are externally atomic: if process _A_ is writing into a database and process _B_ is reading from it, _B_ will see each transaction entirely or not at all.

Transactions are serialized by the writer, of which there can be only one at a time.

+ [Ω design](Omega-design.md)
