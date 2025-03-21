# η
Online weighted learners, used to inform decisions such as those made by the [π₁ inference system](pi1-intent.md).


## Incremental ML
I'll cover this more later as needed, but the basic premise is that any optimizable process can begin with a simple weighted preference and evolve into something more advanced as it makes more observations. Because the observation space may not be fully quantified, it should explore the set of questions it wants to ask about the decision being made. Critically, this happens **breadth-first with predictions** so we don't block operation by learning.

In order to make this work, η needs to track net entropy along different query strategies and explore the best performers. It can locally deviate from this contour to find better global optima.
