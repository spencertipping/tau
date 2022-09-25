# ρ structure, part 2
Picking up from the discussion and implied constraints in [part 1](rho-structure1.md), let's now try to figure out how ρ documents should work. It's a complicated and open-ended space, so this should be a lot of fun.

Iverson has some interesting presentation in the APL book: although he sometimes lists code in a conventional way, it's often embellished with control-flow arrows and ends up looking more like a graph than like a text document. This is semantically correct: control flow is poorly represented by text and better by arrows. `if`-body indentation and other layout is a symptom of this; we want visual presentation to reflect semantic structure.

I don't believe ρ needs to have an opinion about how semantics should be represented (π and ρ are split in part 1), but I do think it should provide ways to view code that align with semantic meaning.


## Input gestures
+ Mouse inputs (imprecise, but good at angle+magnitude)
+ Scroll inputs (imprecise, adds third and fourth dimensions to mouse)
+ Touch inputs (imprecise, but good at multi-selection)
+ Keyboard inputs (precise but awkward for spatial indication)

I'm going to stick to keyboard inputs for ρ. We can have mouse/touch too, but the editor should, like `vim`, be completely usable without those. Mouse/keyboard switching overhead is high enough that we should allow the user to focus on the keyboard.
