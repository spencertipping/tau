# ρ structure
ρ documents are built by replaying edits, each of which is a UTF9 value in a streaming timeline. Edits correspond to individual X11/input events; they are considered to be completely atomic from the user's perspective. Documents are allowed to incorporate author/time information and other metadata into the assembled structure. It's UTF9, after all, and UTF9 is flexible.

Because a document is a UTF9 data structure, it can also be snapshotted to avoid replay overhead. In this way UTF9 documents behave like git repositories.

Documents also need not be linear. One "document" may be many logical trees or regions of content, possibly with links between them. Each piece can be branched and reassembled arbitrarily; edit streams are first-class objects that can be transformed with [π](pi.md) functions.


## ρ/π split
π -- or more precisely, ∏ -- is likely to change over time. ρ should not. It's fine for users to write different ρ objects over time, but the relationship between specific edit actions and ρ results is never allowed to change in a way that breaks history. That means ρ actions, like `vim` keybindings, are designed independently of π semantics; and by extension, so are ρ documents.

So while we might design ρ documents with π in mind, they're ultimately a distinct idea.


## Selection and focus
Text is simple: it's always arranged in a line/grid and you can focus on a small region of it. People work around this by having multiple editor windows and/or split screens. All of that is fine but not especially ideal. Its shortcomings become evident when you search for something that occurs several times: you have to page through the matches rather than seeing them all at once. You can't fold the content around multiple focal points because you get only one focal point. (`vim` further kludges around this by offering location history.)

ρ should allow you to maintain multiple simultaneous edit/focal points that can be folded to be visible at once. This carries some important implications, most importantly that **ρ space is flexible.** You don't specify rigid coordinates when you construct a document; instead, you specify relative positioning: _a_ is above _b_, that type of thing. These invariants remain true even when the document is folded, just as code folding preserves the document sequence.

Having multiple selection/foci also solves the atomic copy/paste problem, which matters in a collaborative context. You can use one cursor to select the source content and atomically transfer that content to another cursor in a single edit operation. This also solves the "where did my clipboard come from" problem: we can assume clipboard contents have no in-document origin (and therefore are OT-verbatim), whereas all atomic content transfers are tracked to the document and can be commuted.


## Inside/outside
Text documents have no containers. Syntax models on top of these documents manage to create them, but the documents themselves have no such concept; and that's important for two reasons:

1. Deep nesting is difficult to parse, visually speaking
2. Containers often have too much structure, which defies human intuition (see [invalid states](#invalid-states) below)

Lisp tries to add uniform containment to text and suffers from (1). Without indentation, it is very difficult to parse more than a couple of parentheses in a row, let alone track the number of layers you're still inside after a close paren. The problem is that for a human observer, containment is a space-partitioning strategy that, when nested, creates a lot of small, disjoint spaces. If you have a large box and create a slightly smaller box inside it, you get a large square (good) and a large ring (not good). Create one more layer and you have two rings that are hard to tell apart.

ρ never relies on "inside/outside", nor can ρ elements contain others. The only exception to this rule is for containers that change the schema of data inside vs outside; for example, maybe you insert a plain-text document into a UTF9 world. Then the text document has row/column edit behavior, whereas the rest of the ρ document is more flexible.


## Invalid states
I've used `paredit` with Lisp and have mixed feelings about the experience. `paredit` is designed to maintain S-expression structure and provides a series of operations like "move last thing out of this list", "absorb prior thing into this list", or "swap two elements". This is useful because these are common operations.

It's also problematic for a few reasons. First, `paredit` suffers from the inside/outside issue when using a single-point cursor to describe the thing you want to select. Which list are you manipulating, and which item within that list? The edit cursor does a poor job communicating this to the user because it was never meant to.

Second, `paredit` behaves badly when your document is malformed, as it often is if you do things like commenting out a line with a close-paren on it, or using editor-level copy/paste. In other words, its assumption that the document is always valid is wrong; and when it is, `paredit` can jump your cursor far away from where you want it and/or mangle your text in various minor but annoying ways.

Third, `paredit` sometimes makes it difficult to apply edits that move the document through invalid states. This can be seen either as a problem with `paredit`'s action set (too limited), or as a conflict between what the editor offers and what `paredit` wants you to do. In any case, we humans have enough intuition built up around our syntactic-edit models that we want to apply these edits without a parser rejecting them.

**Humans do not think in parser terms when editing text; we think in text terms, because that's what's in front of us.**

To emphasize this point further, we use elaborate conventions to bridge the gap between structure and presentation -- indentation styles, where to put braces, etc. All of this happens within the null space of the parser, so clearly it doesn't matter to the machine. But to us it's the difference between code we can maintain and code we can't. Part of that equation is laying out code to be editor-friendly; it's more about presentation than ergonomics, but ergonomics do matter.

To the extent that text editors are successful, it's probably because they give us the feeling of being able to move text just like we would move normal objects; and when an editor like `paredit` doesn't allow this, it feels unnecessarily restrictive. Editors very much reflect our spatial intuition; and for that reason, syntactic validity is a distant secondary consideration. (Also, editors provide a lot of composable, low-level operations that are not meant to be understood as atomic intentions. This creates many problems for OT, and ρ should avoid it.)

Another way to look at it is that editor operations are so low-level that an editor has little hope of understanding our intentions, and when the editor jumps the gun and tries to make assumptions it usually makes more of a mess than it would have had it waited until those intentions were clearer. Maybe there's a local-vs-global optimum element to it.


## Path linearity
...or equivalently, continuation perplexity. How easy is it for a human to anticipate where the "next" thing will be? In ASCII text, very simple; it's a grid -- although proportional fonts complicate this in minor ways.

Programming languages also complicate the picture by adding a semantic layer with its own continuation model that has very little to do with text. As a result, "go to next execution point" is provided by no editor at all despite being a cognitively straightforward thing to want. Live debugging is present in almost no IDEs even though it would be enormously valuable.

Path linearity matters from a UI perspective because it's how people tend to select things. Though as an aside, wouldn't it be _freaking awesome_ if you could select-by-execution-trace? Or "select until `x == 10`" or any other runtime thing. That would be amazing.

But back to the point, when you click-drag to select stuff you're implying a region, and that region is linear in nature because point → point through time forms a possibly-squiggly line. I'm not going to call this a strength of text editors; the squiggliness of the line gets quantized out, so the whole process feels imprecise and awkward. But there are other ways to do it: you can hold down shift and select, or in `vim` you can just move in visual mode. A very poor man's bounding box for text.

Single-selection makes everything simple, but it's also limiting. You can't do the obvious thing of renaming a variable because there's no way to select-by-scope. As a result of that and non-atomic edits, we wind up with the absurdity that my variable-rename can conflict with your out-of-band insertion of code, and a human has to waste time to sort that out.

The problems created by linear paths and selections are so grievous that I'm happy to dispense with linearity as a constraint. It's a good thing to have, but precise, parallel selection is much more important; that's where OT resilience comes from. It also paves the way to unexplored territory like execution-oriented syntax traversal.

For example: "let _x = 10_, _y = 20_. select errors." _That's_ the selection I want. Or "grab _x_ and all dependent statements, and put them into this other function." Or "select slow things". The possibilities are enormous.
