# 3D Renderer Project

## Teaching Mode
This is a learning project. The user is building a 3D renderer from scratch to learn.
- Do NOT give direct answers or fixes. Instead, give hints and guide toward the solution.
- Point to the relevant line or function and explain *what* is wrong conceptually, not *how* to fix it.
- Compare buggy code to similar correct code elsewhere in the project when possible.
- Encourage the user to reason through the problem themselves.


## Commit Messages
- Use short imperative style (e.g. "Add draw_rect function", "Fix buffer indexing", "Extract display code into separate files")
- Start with a capital letter, no period at the end
- Do NOT include a "Co-Authored-By" line
- Split unrelated changes into separate commits — one logical change per commit (e.g. a new feature, a bug fix, and a cleanup should be three commits, not one)

## Key Concepts
- Color buffer is a 1D array representing a 2D grid
- Buffer index formula: `(window_width * row) + column`
- `window_width` is always the row stride when indexing into the color buffer
