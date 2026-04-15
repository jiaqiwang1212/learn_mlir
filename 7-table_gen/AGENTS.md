<!-- Parent: ../AGENTS.md -->
<!-- Generated: 2026-04-14 | Updated: 2026-04-14 -->

# 7-table_gen

## Purpose
Chapter 7: Interactive TableGen syntax exploration. Unlike other chapters, this one is a Jupyter notebook (`learn_td.ipynb`) that walks through TableGen language constructs — `def`, `class`, `multiclass`, `foreach`, `let` blocks — outside of any build system.

## Key Files

| File | Description |
|------|-------------|
| `learn_td.ipynb` | Jupyter notebook covering TableGen syntax and patterns |
| `readme.md` | Chapter notes and references |

## For AI Agents

### Working In This Directory
- No CMake build; this chapter is standalone notebook exploration.
- To run the notebook: `uv run jupyter lab learn_td.ipynb` (or `uvx jupyter lab`).
- TableGen syntax learned here underpins all `.td` files in every other chapter.

### Common Patterns
- `def MyDef : ParentClass<args> { ... }` — defining a record
- `class MyClass<list<dag> fields> { ... }` — parameterized class
- `multiclass` — generates multiple defs from one template

## Dependencies

### External
- Jupyter (for notebook execution)
- `llvm-tblgen` (to test TableGen snippets, optional)

<!-- MANUAL: -->
