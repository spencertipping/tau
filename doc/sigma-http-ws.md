# σ HTTP/WS server
σ defines three components that are often used together:

+ `T`: TCP socket server Γ
+ `H`: HTTP/WS Ψ₄
+ `A`: session authentication and routing Γ

The basic idea is to create a pipeline like this: `T3[HA...]`, where `T3` creates a TCP server on port 3000, and each inbound connection is parsed as HTTP and then authenticated, routing it to the appropriate destination(s) and ultimately terminating into an internal τ server connection, e.g. `&db` or `&broadcast`.


## TCP server structure
A TCP server listens on a port and, for each inbound connection, constructs a [Ξ](Xi.md) whose top duplex is a read/write [ψ](psi.md) for the new connection socket handle. Sending _ω_ leftwards to the socket, or ending its input, will `shutdown(SHUT_WR)`; sending _Ω_ will `shutdown(SHUT_RDWR)` and close the FD. `shutdown(SHUT_RD)` happens if you close the rightward output from the ψ -- i.e. you stop reading from it.

**TODO:** verify all of the TCP handshake details here


## HTTP/WS Ψ₄ codec
`H` is responsible for two things: transcoding a single HTTP session between the left-hand byte stream and the right-hand structured encoding, and closing any connection that exhibits traits of exploitation, e.g. slowloris or other malformed data. The `H` contract is that data you get from it will be well-formed and, as far as HTTP is concerned, reasonably non-malicious (i.e. headers below some size, valid HTTP method, that type of thing).

**The HTTP/WS server does not handle routing or authentication.** It simply parses the HTTP/WS protocol on a request-by-request basis and closes the connection if anything sufficiently unexpected takes place.

**TODO:** document data format


## Session authentication and routing
σ ties authentication and routing together because they ultimately are parts of the same problem. For example, a `/login` endpoint is required in order to establish user identity and acquire a session cookie, which is then required in order to access authenticated APIs.

`A` right-terminates a single HTTP session, replies to login-related requests, and forwards the rest with the user's identity or `null` prepended to the η. **That same authentication header must be present on replies; otherwise nothing will be sent.** This confirms that the reply was intended for the connected user.

`A` supports OAuth2 for some providers.

**TODO:** define the authentication backend API
