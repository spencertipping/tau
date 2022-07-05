# Streams
Streams are invariant assertions made about dataflow. For example, if a stream connects `A` to `B`, we know that those are the endpoints and that data will move only when both sides are ready.

Importantly, _streams are not objects_ -- although they may be implemented by objects. As such, _ι_ and _κ_ are informational markers rather than commands.

Functionally, streams are a metaphor used by operators to request dataflow operations from the [fabric](fabric.md).


## Stream state space
