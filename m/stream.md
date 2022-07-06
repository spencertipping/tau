# Streams
Streams are invariant assertions made about dataflow. For example, if a stream connects `A` to `B`, we know that those are the endpoints and that data will move only when both sides are ready.

Importantly, _streams are not objects_ -- although they may be implemented by objects. As such, _ι_ and _κ_ are informational markers rather than commands.

Functionally, streams are a metaphor used by [operators](operator.md) to request dataflow operations from the [fabric](fabric.md).


## Stream state space
Every operator-bound stream has the following progression of states:

_α → [ι → ... → κ]* → ω_

That is, _α_ indicates beginning-of-data, _ι_ indicates "stream is temporarily operable", _κ_ "stream is temporarily blocked", and _ω_ "stream is permanently inoperable", i.e. EOF/closed.
