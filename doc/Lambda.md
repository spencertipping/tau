# Λ thread manager
Λ manages and schedules [λ](lambda.md) threads. There is exactly one Λ per [τ](tau.md) process.


## λ lifecycle
Creating a λ schedules a function to be executed, but the λ will not be started immediately -- nor will it necessarily be the first thing to happen once the caller invokes Λ.

If a λ is destroyed, it is immediately deallocated and will never be scheduled again. λs cannot schedule exit/destructor code or prevent their own destruction.
