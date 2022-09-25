# ρ structure, part 2
Picking up from the discussion and implied constraints in [part 1](rho-structure1.md), let's now try to figure out how ρ documents should work. It's a complicated and open-ended space, so this should be a lot of fun.

Iverson has some interesting presentation in the APL book: although he sometimes lists code in a conventional way, it's often embellished with control-flow arrows and ends up looking more like a graph than like a text document. This is semantically correct: control flow is poorly represented by text and better by arrows. `if`-body indentation and other layout is a symptom of this; we want visual presentation to reflect semantic structure.

I don't believe ρ needs to have an opinion about how semantics should be represented, but I do think it should provide ways to view code that align with semantic meaning.
