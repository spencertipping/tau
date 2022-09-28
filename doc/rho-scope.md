# ρ scope
What is an editor for -- why not just use `vim` or `notepad`? There's a lot to it, but I'd sum it up like this:

1. Visual information density/relevance
2. Relevant edit operations (e.g. auto-indent, robust search/replace)
3. Append-primary version control, if your language supports that
4. Realtime OT resolution
5. Communicating the computer's interpretation to you


## OT≡intention
**An OT system will work well for a human iff it preserves their intentions while resolving merge conflicts.** Therefore, its quality is bounded above by the degree to which edit actions reflect human intentions, and the degree to which those intentions can be resolved across users.


## Core constraint
**A programming language is a basis shift from input actions to semantic structures.**

Because input actions can be graded by ergonomic effort, and structures jointly by performance, embedded entropy, and weak correctness, we can further specify that both input actions and semantic structures are members of an information geometry. Input action entropy is defined by human effort, structure entropy is some objective function over the attributes above.

Languages should provide a viable domain for OT, which implies that edit actions should correspond closely to both source code and semantic intentions. This roughly implies that the compiler's basis shift should be minimally complex, although it's more complicated in practice.


## Input stream
Let's go back to the `vim`/`notepad` world for a moment. What do these editors do for us that writing a file directly doesn't? A lot, but the most important thing is that they provide a sideband (editor keybindings) that we can use to multiplex our document updates. For example, I can backspace stuff or update something that already exists without retyping the whole document.

**Multiplexing reduces the entropy overhead of an edit from _O(n)_ to _O(1)_ or _O(log n)_, give or take.**

It makes sense why this would be important: humans' quantum of correctness is very low, often on the order of just a few characters at a time. To be more precise, there are several things happening, each of which should probably be considered separately. Working upwards from the UI, and naming these things "layers" as in "L1", "L2", etc:

1. Byte-level transcoding: the user types stuff
2. Small framing: the user mentally interacts with a phrase (like a line of text) and produces edits that modify it
3. Medium framing: the user mentally interacts with a page and solves for phrase-level changes (technical decomposition of a business objective)
4. Large framing: the user mentally interacts with a human-level objective and solves for modules/pages that realize that objective

Each of these steps is laden with errors, which means the user's time is split between authoring stuff and fixing the stuff they've authored. The "fixing stuff" piece is greatly compounded if they're working collaboratively with someone else because the human-synchronization protocol is just as error-laden as authoring, if not more so due to intrinsic defects like Dunning-Kruger, incomplete rational propagation, and ego.

Editor multiplexing allows for cheap error correction at L1 and often at L2. L3 and L4 are more or less a lost cause, not necessarily due to editor deficiencies. Most programming languages have no information channel to capture them (comments don't count because they're second-class citizens), so they tend to be fully erased by the time they've been migrated into code.

The basis shifts in L3 and L4 complicate the picture further, as they aren't at all straightforward. I might think, "I want to answer _x_ data question" and spend most of my time thinking not about the inference process, but about data engineering and how to serialize IO. In other words, the programming environment becomes exponentially less helpful as our problem domain scales upwards: `x += 5` is efficient, `for (auto x : xs) t += x` is acceptable, a function that sorts things requires a moderate amount of human hand-holding (choosing the algorithm), and a web store to sell T-shirts is completely opaque.

Clearly L3 and L4 are where editors and languages both fail right now. L1 and L2 are essentially solved problems. If we want an editor and environment to be better at multiplexing effort for L3 and L4, we need to find common eigenvectors between human objectives and semantic results. Then we need to have the editor and language generate L1 and L2-level edits in cases where they're obvious.

Or we can go the APL route and unify L2 and L3 with aggressive notation. Or both.


## L4 basis space
Before I dismiss this as being impossible, I want to at least explore the thought process.

Human objectives are specified in human language, which for simplicity let's say is English. So if we want a programming environment to meaningfully interact with L4, it needs to not only be able to parse and understand a problem description in English, but also to infer relevant context and understand the surrounding environment well enough to resolve ambiguity in constructive ways. Because an L4 software engineer would ask questions and accept requests for change, it should also ask questions and accept these requests.

Humans can do this intelligently because (1) we understand English, (2) we understand computers, and (3) we've often built up experience that allows us to entropy-grade the problem domain. Using the codec model, we have three matrices, each of which, ideally speaking, is complete and well-factored:

+ _code ↔ software_ (L1 and L2)
+ _software ↔ product_ (L3 and L4)
+ _product ↔ english_ (L4, roughly)

I'm using some terms loosely here, but hopefully the gist makes sense.

Quantifying "product" in a precise way is often considered to be difficult, but let's try it anyway. A product may be abstract, but only its observable qualities matter; and that means we can define it in terms of how the user will perceive it. Rather than saying "I want a button here", we can say "I want the user to perceive that they can do this thing at this point". Then the basis space shifts: given that perceptive intention, how should we encode it?

There's an elegance to this because humans are actually quite bad at solving this problem well. Many user interfaces are difficult to use, or allocate screen space poorly (especially on non-desktop devices), or are unusably slow, or interrupt the user with valuable but inopportune tutorials. Machine-optimizing this problem space is the million-dollar challenge.


### Idiom and opacity
Data science is perceived as being more complicated than software engineering, perhaps because its outputs are less viscerally observable. A usable product is easier to detect than a robust study, at least to many people. Or maybe it's just a younger field and we have fewer preferences about how it should work. I think the product space is less well-defined, as evidenced by the surplus of technically correct but ultimately unenlightening results.

Either way, L4 should be able to address products-as-math just as easily as it does products-as-UIs, which I think is straightforward if we consider an adversarial user: "does the output fail to _x_ defect" is a degree of observability and therefore a way to quantify the product.

Notably, **this does not imply that the computer can do math** -- nor does it imply that of humans who solve the same problem. Just as we can treat sorting algorithms as opaque pieces, we can do the same for study methodologies and UI design principles. It's good to have alternatives and causal knowledge, but it's also acceptable to use atomic, opaque ideas. In some cases these opaque idioms are optimal; we don't always have the time or context to discover the "right" solution.


### Human-relevance domains
UIs enable action, data science enables thought; but both interface with users in some quantifiable way.

Human relevance can be expressed as a vector function: a human has an objective vector and our goal is to realize that objective -- either by ourselves, or by rotating their capability vector towards their objective (e.g. shipping them a software product that does this). So if we can quantify their goals and capabilities, we can optimize software for them.

I don't think any of this is especially straightforward, but the fact that it isn't completely intractable is encouraging.


## L3 basis space
This is roughly where my original conception of π intents picked up: describe an intent and it would produce code via bridges to implement that intent. For example, map/reduce can be expressed as a series of these constraints. Most of the substance of map/reduce is reducible to function -- or more precisely, γ -- composition.

L3 doesn't involve human concerns much. There's a little bit of "know what good code is", but otherwise it's hands-off technical optimization: anything from _a → b_ is sufficient. Most of the business-case burden is on the user as they define the relevant boundary conditions.


## L2 basis space
π can be designed around L2 ergonomics, both by being concise and by enabling linear edits when possible. ρ can include bindings that are commonly used to select subexpressions -- but before I commit to this, I'd like to make sure it will be useful. I don't know what type of edits will be common for π code, especially if we have L3.

A simple litmus test would be, how easy is it for us to auto-arrange code in some semantically relevant way? For example, render as flowchart or outline control flow. Then we can define edit operations against that representation.


## L1 basis space
`vim`-style bindings, probably. I'd like to improve on the copy/paste situation in two ways:

1. All registers are visible and document-resident
2. Copy/paste is an atomic operation

(1) is for visibility, (2) is for OT. In particular, ρ needs to know where copy/pasted content comes from so we can transpose OT edits into the copy buffer and ultimately across the copy/paste operation itself.
