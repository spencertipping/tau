# φ
φ is an endpoint-view of a pair of bidirectional [ζ](zeta.md). All φs carry [UTF9](utf9.md) values in practice.


## Pipe ζs
All established connections are ζs, which are created by linking two φs to one another directly. You can write data to a disconnected φ; if you do, the write will block until a ζ is created to receive it. (`<<` has this blocking behavior, `<<=` will insta-fail if ζ is disconnected.)


## Server φs
Any φ can be created as a server, which acts much like a UNIX socket in that it accepts connections eagerly and emits φ IDs as its IO. These φs arrive fully connected and ready for IO.

Server φs cannot be used for regular IO -- nothing can maintain a connection to them. Any attempted connection will instantly be routed to a new φ.
