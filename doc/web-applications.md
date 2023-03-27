# Web applications in τ
τ represents a steady-state factoring of your application, so the first thing to do is to define the obvious static components. The server probably has an HTTP endpoint, user-session thing, and database connection; the client has the DOM, event stream, and server connection. So broadly we have this general architecture:

```
+-client------------------+
|           ___           |
| DOM ←--- |   |          |
|          | ψ | ↔ server |
| events → |___|          |
|                         |
+-------------------------+

+-server----------------------------------+
|                                         |
|                   | ↔ ψ ↔ |             |
| HTTP ↔ user demux | ↔ ψ ↔ | DB mux ↔ DB |
|                   | ↔ ψ ↔ |             |
|                                         |
+-----------------------------------------+
```
