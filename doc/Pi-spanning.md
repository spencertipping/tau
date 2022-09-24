# ∏ spanning
∏ is allowed to write code that you don't mention. Many languages do ad-hoc versions of this, for instance to coerce `int` to `float`. ∏ generalizes the process such that any two spannable intents can be bridged with a minimum-entropy function that ∏ will create for you. The keys here are _spannable_ and _minimum entropy._


## Span existence
A span from _a_ to _b_ exists iff _∃{f₁, f₂, ...}_ with _f₁ ∘ f₂ ∘ ... ∷ a → b_. Checking this is not always trivial because functions can transform intents arbitrarily. It's also not always intractable because ∏ can use graph search algorithms that follow a uniform η frontier, which should minimize the expected time to find a span. Once the first span is found, ∏ can conduct bounded searches to try to find shorter ones.


## Minimum entropy
Not entirely straightforward because η can be defined with context. For example, suppose _f ∷ a → b_ and _g ∷ b → c_. Let's suppose there are a total of 32 functions that map _a → b_, so _f_ has a nominal five bits of entropy. _g_, on the other hand, maps a smaller space: just two bits if we know _b_, ten bits otherwise. Then choosing _f_ causes _g_ to be more probable, even though _g_'s entropy otherwise might be quite high.

We can avoid this ambiguity by treating the endpoint selection as the process that generates entropy: rather than specifying _f_ and _g_, we allow those to be inferred from sufficiently specific _a_, _b_, and _c_. This also lets ∏ avoid overfitting because it can measure the entropy it adds to the end points relative to the amount of entropy discovered by deriving bridge functions.
