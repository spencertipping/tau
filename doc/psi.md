# Ψ
ζ-local IO is managed by Ψ, which tracks all internal connections and keeps λ states synchronized with ζ flow blocking.


## ψ states
A γ creating ψs will observe them in several states, roughly corresponding to the state symbols that the reader would receive:

+ _α:_ `γ.ψc()` called, but nothing has been connected yet
+ _ι:_ a φ exists, containing two ζs to connect this ψ to another ψ elsewhere: φ sends _α_ to both sides

From there, the two constituent ζs can each progress through these states independently of the other:

+ _ι:_ default state, same as above
+ _κ:_ the writer has closed the ζ, reader can still consume queued data
+ _ω:_ the reader has consumed all data; the next symbol is the finalizing _ω_

We denote the ψ state with two symbols, the first for the read-ζ and the second for the write-ζ. That gives us the following set:

+ `αα`
+ `ιι`
+ `ικ`
+ `ιω`
+ `κι`
+ `κκ`
+ `κω`
+ `ωι`
+ `ωκ`
+ `ωω`
