# γ state
We need to represent a few things:

1. Individual components
2. Edits to those components
3. Pipelines of multiple components
4. Connection slots for pipelines

(4) isn't explicit; it's a property of components that are eigenreactive -- and therefore create new γs that slot into those eigenvector positions.

(1) and (2) form the core component API, which creates and configures γs that are stored on the stack. (3) allows polysemantic connections between these components; it deals with "which φs connect to which other φs".

Each pipeline, as a γ component, can have its own "control φ" that is used to signal that it should be destroyed. The control φ can also emit performance/diagnostic information and enable debugging.
