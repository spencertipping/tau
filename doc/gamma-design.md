# γ design
A discussion informing the basic principles of γ components at the API level.

γ is just a container for λs and φs. Its job is to manage all of the resources associated with these objects -- it's a lifecycle container.

γs don't have any intrinsic preference about how their λ or φs are used.


## φ allocation
Most of the topologies below have some fixed φs. These tend to be allocated first and into specific indexes: α = 0, β = 1, δ = 2, ε = 3, etc. τ always uses numeric offsets to refer to φs, just like FDs and TCP ports are numeric in UNIX.

Server φs use any unused φ index when connecting new clients.
