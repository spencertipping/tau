# σ HTTP/WS server
σ defines three components that are often used together:

+ `T`: TCP socket server Γ
+ `H`: HTTP/WS Ψ₄
+ `A`: session authentication and routing Γ

The basic idea is to create a pipeline like this: `T3[HA...]`, where `T3` creates a TCP server on port 3000, and each inbound connection is parsed as HTTP and then authenticated, routing it to the appropriate destination(s) and ultimately terminating into an internal τ server connection, e.g. `&db` or `&broadcast`.


## TCP server structure
A TCP server on a port creates a [Ξ](Xi.md) with a read/write [ψ](psi.md) for each incoming connection. Sending _ω_ to the socket will `shutdown(SHUT_WR)` while sending _Ω_ will `shutdown(SHUT_RDWR)` and close the FD. Closing the rightward output from the ψ results in `shutdown(SHUT_RD)`.


## HTTP/WS Ψ₄ codec
`H` performs two tasks: converting a single HTTP session between byte stream and structured encoding, and terminating any suspicious connections (e.g., slowloris, malformed data). `H` ensures that data it provides is well-formed and reasonably non-malicious in the context of HTTP (e.g., headers below a certain size, valid HTTP method, websocket frames below a certain size).

**The HTTP/WS server does not handle routing or authentication.** It simply parses the HTTP/WS protocol on a request-by-request basis and closes the connection if anything sufficiently unexpected takes place.

**TODO:** document data format


## Session authentication and routing
σ ties authentication and routing together because they ultimately are parts of the same problem. For example, a `/login` endpoint is required in order to establish user identity and acquire a session cookie, which is then required in order to access authenticated APIs.

`A` right-terminates a single HTTP session, replies to login-related requests, and forwards the rest with the user's identity or `null` prepended to the η. **That same authentication header must be present on replies; otherwise nothing will be sent.** This confirms that the reply was intended for the connected user, preventing data leaks from topology errors.

`A` supports OAuth2 for some providers.

**TODO:** define the authentication backend API


## Tests
The following request from Joyce's Chrome crashes the server:

```
GET /8a3040aa6b8f70d0bfff71d2b3802768485b88d6f4bf9f90b35f016d92c15983 HTTP/1.1
x-forwarded-host: ijen.35.duck:3000
x-forwarded-proto: http
x-forwarded-port: 3000
x-forwarded-for: 10.35.1.75
cookie: rl_anonymous_id=RudderEncrypt%3AU2FsdGVkX1%2B1bIu%2Fb4YwCojCkDeNL%2FWGNkg8p4RS0ShBxk%2F8EOcHVZrTCiHE3KV%2B%2B6QIdYKs%2FlYlSpAsjGqzQA%3D%3D; rl_group_id=RudderEncrypt%3AU2FsdGVkX19tVzrspteCSIkgVICtxfUEv7SEbrfLdrk%3D; rl_group_trait=RudderEncrypt%3AU2FsdGVkX1%2B2OLq5k0BZTUh49iZqA51dk7zdoVrFj00%3D; rl_user_id=RudderEncrypt%3AU2FsdGVkX1%2FPcNHPVO8FbQjormehKfwdfkA%2Bi%2FT8IIWmyHxAiSAEj%2F3RrDA85a7w; rl_trait=RudderEncrypt%3AU2FsdGVkX19sR02HlC3IQlnCyCD388ZTrcrWDulvetw%3D
accept-language: en-US,en;q=0.9,zh-CN;q=0.8,zh;q=0.7
accept-encoding: gzip, deflate
referer: http://ijen.35.duck:3000/
accept: */*
user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36
connection: close
host: localhost:3001


```


```bash
$ $sigma 'n1p"GET /8a3040aa6b8f70d0bfff71d2b3802768485b88d6f4bf9f90b35f016d92c15983 HTTP/1.1
x-forwarded-host: ijen.35.duck:3000
x-forwarded-proto: http
x-forwarded-port: 3000
x-forwarded-for: 10.35.1.75
cook" "ie: rl_anonymous_id=RudderEncrypt%3AU2FsdGVkX1%2B1bIu%2Fb4YwCojCkDeNL%2FWGNkg8p4RS0ShBxk%2F8EOcHVZrTCiHE3KV%2B%2B6QIdYKs%2FlYlSpAsjGqzQA%3D%3D; rl_group_id=RudderEncrypt%3AU2FsdGVkX19tVzrspteCSIkgVICtxfUEv7SEbrfLdrk%3D; rl_group_trait=RudderEncrypt%3AU2FsdGVkX1%2B2OLq5k0BZTUh49iZqA51dk7zdoVrFj00%3D; rl_user_id=RudderEncrypt%3AU2FsdGVkX1%2FPcNHPVO8FbQjormehKfwdfkA%2Bi%2FT8IIWmyHxAiSAEj%2F3RrDA85a7w; rl_trait=RudderEncrypt%3AU2FsdGVkX19sR02HlC3IQlnCyCD388ZTrcrWDulvetw%3D
accept-language: en-US,en;q=0.9,zh-CN;q=0.8,zh;q=0.7
accep" "t-encoding: gzip, deflate
referer: http://ijen.35.duck:3000/
accept: */*
user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36
connection: close
host: localhost:3001

"||- H'
"GET" "/8a3040aa6b8f70d0bfff71d2b3802768485b88d6f4bf9f90b35f016d92c15983" x-forwarded-host: "ijen.35.duck:3000" x-forwarded-proto: "http" x-forwarded-port: "3000" x-forwarded-for: "10.35.1.75" cookie: "rl_anonymous_id=RudderEncrypt%3AU2FsdGVkX1%2B1bIu%2Fb4YwCojCkDeNL%2FWGNkg8p4RS0ShBxk%2F8EOcHVZrTCiHE3KV%2B%2B6QIdYKs%2FlYlSpAsjGqzQA%3D%3D; rl_group_id=RudderEncrypt%3AU2FsdGVkX19tVzrspteCSIkgVICtxfUEv7SEbrfLdrk%3D; rl_group_trait=RudderEncrypt%3AU2FsdGVkX1%2B2OLq5k0BZTUh49iZqA51dk7zdoVrFj00%3D; rl_user_id=RudderEncrypt%3AU2FsdGVkX1%2FPcNHPVO8FbQjormehKfwdfkA%2Bi%2FT8IIWmyHxAiSAEj%2F3RrDA85a7w; rl_trait=RudderEncrypt%3AU2FsdGVkX19sR02HlC3IQlnCyCD388ZTrcrWDulvetw%3D" accept-language: "en-US,en;q=0.9,zh-CN;q=0.8,zh;q=0.7" accept-encoding: "gzip, deflate" referer: "http://ijen.35.duck:3000/" accept: "*/*" user-agent: "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36" connection: "close" host: "localhost:3001"
```

The following request segfaulted the regex parser, so I rewrote the HTTP library to use non-regex:

```bash
$ $sigma 'n1p"GET /a/[%22z%22,1,%220e0e8738fc604d971a7f4eb2ae7bb7f4a0b123a927237ee47cf10c7076988c84%22,1,%221093f230274c8640344f0a88a75d06d2728eb4772d6013e7a8ce8a59bfc0dc3f%22,1,%22550dd28143afe6c1ba44d37b323302d25fdeb29f1c7b98909da3a14997cee1e1%22,1,%22f40d12b3ea8854160e2bf97372705f426283ba1ef2a3cbe2e415b7c5990039f2%22,1,%22d82988cdec7f273ebde9a8c232a3cda9e518a5a6c219549f2762951b0d0fff10%22,1,%aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1111111] HTTP/1.1\r\n\r\n" H'
"GET" "/a/[%22z%22,1,%220e0e8738fc604d971a7f4eb2ae7bb7f4a0b123a927237ee47cf10c7076988c84%22,1,%221093f230274c8640344f0a88a75d06d2728eb4772d6013e7a8ce8a59bfc0dc3f%22,1,%22550dd28143afe6c1ba44d37b323302d25fdeb29f1c7b98909da3a14997cee1e1%22,1,%22f40d12b3ea8854160e2bf97372705f426283ba1ef2a3cbe2e415b7c5990039f2%22,1,%22d82988cdec7f273ebde9a8c232a3cda9e518a5a6c219549f2762951b0d0fff10%22,1,%aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1111111]"
```
