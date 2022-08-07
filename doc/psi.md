# Ψ
ζ-local IO is managed by Ψ, which tracks all internal connections and keeps λ states synchronized with ζ flow blocking.


## ψ states
A γ creating ψs will observe them in several states:

+ _disconnected:_ `γ.ψc()` called, but nothing has been connected yet
+ _connected:_ a φ exists, containing two ζs to connect this ψ to another ψ elsewhere: φ sends _α_ to both sides
+ _half-close pending:_ this side has closed its writing ζ, which may contain unread data
+ _half-closed:_ this side has closed its writing ζ and the other side has read it fully (culminating in a φ-generated _ω_ terminator)
+ _close-pending:_ both sides have closed their writing ζs, which may contain unread data
+ _closed:_ both side have closed ζ and read all pending data, including the _ω_ terminators

Note that _closed_ is semantically similar to _disconnected,_ in that the underlying φ is gone, but ψs cannot be reused; so you need to `ψx` and then `ψc` to create a new connection.
