# Transform code
`ni` relies on Perl for record transformation, but I want τ to implement a custom bytecode that is better optimized for it. Ideally we get all of `ni` functionality with a more concise and targeted notation.

This is a natural fit for a stack language, so we'd use RPN and tight PEG-driven addressing.
