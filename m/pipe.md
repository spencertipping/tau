# Pipe
A pipe is a device used by a [leaf](leaf.md) to move data between two [cell](cell.md) [ports](port.md). It moves data unidirectionally and has a capacity that can be customized.

Pipes are almost always arranged into [chans](chan.md) by pairing two of them: one for each direction. In this way, directional capacities can be configured individually but the chan can be managed as a unit.

Pipes do not emit _α_ or _ω_; these are emitted by the cells being connected. (If pipes emitted them, then multiplexed RPCs would be unable to proxy _α_ and _ω_ correctly.) The first message sent down a pipe must be _α_ and the last message must be _ω_. A pipe will automatically be disconnected and its connecting ports closed if you send _ω_. Note that a chan's return pipe is _not_ closed automatically; this allows the half-open state.
