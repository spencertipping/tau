# π₀ bytecode
Exactly like [Joy](https://en.wikipedia.org/wiki/Joy_(programming_language)): we have a very cheap evaluation function that simply proceeds through a list, executing each element in turn. Local scopes are available by pushing/popping them. You can refer upwards to parent scopes if you want to.

There are three stacks: data, return, and frame.
