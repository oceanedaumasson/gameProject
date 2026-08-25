# Warzone Engine

A C++17 implementation of a Risk-style strategy game engine, built around five
classic object-oriented design patterns: **Strategy**, **Observer**, **State**,
**Adapter**, and clear separation of concerns across map, player, card, order,
and command-processing subsystems.

> Originally developed as a team project for a university systems-design
> course (advanced C++ programming). This repo is a personal fork used
> to showcase my contributions.

## My contributions

- Designed the **Cards Deck/Hand** system 
- Implemented the **Main game loop** for gameplay (Reinforcement Phase, Issue Orders Phase, Orders Execution Phase)
- Implemented the **Strategy pattern's** for Aggressive and Neutral players

## Features

- Map loading and validation (connected graph, connected continents)
- Card / deck system with 5 card types (Bomb, Reinforcement, Blockade,
  Airlift, Diplomacy)
- Six order types with full validation and a battle simulation engine
  (attack/defend probability, territory conquest, card rewards)
- Command processor with console and file input (**Adapter** pattern)
- Turn-based game engine with a formal state machine (**State** pattern):
  Start → Map Loaded → Map Validated → Players Added → Assign Reinforcement →
  Issue Orders → Execute Orders → Win
- Five AI/human player strategies (**Strategy** pattern),
  swappable at runtime
- Tournament mode: automated multi-map, multi-strategy round robin
- Full game logging via the **Observer** pattern (`LogObserver` /
  `Subject` / `ILoggable`)

`demo` is a single menu-driven entry point that walks through the whole
engine. From the `build/` directory:

```
1. Map loading & validation
2. Cards: deck, draw, play
3. Orders: all 6 types + battle simulation
4. Player strategies (Strategy pattern)
5. Full game loop (bounded automatic run)
0. Exit
```

### Individual module drivers

Each subsystem also has its own standalone driver, useful for testing one
piece in isolation:

| Executable | Demonstrates |
|---|---|
| `mapDriver` | Map loading and validation, interactively |
| `cardsDriver` | Deck / hand / card play mechanics |
| `ordersDriver` | All 6 order types, validation, and execution |
| `playerStrategyDriver` | Each AI strategy individually, plus dynamic switching |
| `startupPhaseDriver` | Game startup command sequence |
| `mainGameLoopDriver` | Full reinforcement → orders → execution loop |
| `loggingObserverDriver` | Observer-pattern logging to `gamelog.txt` |

## Tech stack

C++17 · CMake · STL (no external dependencies)

## License
 
MIT — see [LICENSE](LICENSE).
