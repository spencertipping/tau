# φ
φ is an endpoint-view of a pair of bidirectional [ζ](zeta.md). All φs carry [UTF9](utf9.md) values in practice.


## Pipe ζs
All established connections are ζs, which are created by linking two φs to one another directly. You can write data to a disconnected φ; if you do, the write will block until a ζ is created to receive it.


## Server φs
φ objects are never servers, unlike UNIX FDs which can be. Instead, servers are implemented at the γ level by marking FDs as "redirect on connection". These server connections are handled by [γ₀](gamma0.md).
