# η
Online weighted learners, used as optimization primitives within the [π compiler](pi-compiler.md).


## Incremental ML
I'll cover this more later as needed, but the basic premise is that any optimizable process can begin with a simple weighted preference and evolve into something more advanced as it makes more observations. Because the observation space may not be fully quantified, it should explore the set of questions it wants to ask about the decision being made. Critically, this happens **breadth-first with predictions** so we don't block operation by learning.
