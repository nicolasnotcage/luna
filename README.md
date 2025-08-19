# Luna - Chess Engine

A comprehensive chess engine and game development project implementing a bitboard-based chess engine with UCI protocol extensions and an integrated SDL-based game interface.

## Project Overview

ChessLab consists of three primary components that work together to create a complete chess playing experience:

### 🎯 **ChessRules** 
Core chess logic library containing:
- Bitboard-based board representation
- Move generation and validation
- Chess rule enforcement (castling, en passant, check detection)
- Position management and FEN parsing
- Zobrist hashing for position identification

### 🧠 **ChessEngine (Luna)**
UCI-compatible chess engine featuring:
- Alpha-beta search with various optimizations
- Transposition tables with Zobrist hashing for position caching and search optimization
- Position evaluation functions
- Standard UCI protocol implementation
- Custom UCI extensions for enhanced debugging and rule customization
- Performance testing (perft) validation
- Builds as `luna.exe` in the ChessEngine directory

### 🎮 **GameEngine**
SDL3-based game interface providing:
- Visual chess board and piece rendering
- Player input handling and move validation
- Windows process spawning with anonymous pipes for UCI-compliant engine communication
- Integration with chess engine for computer opponents
- Scene-based architecture supporting multiple game modes
- Audio and visual effects using FMOD
- Configurable engine path support (via config file or command line)

## Building the Project

### Prerequisites
- CMake 3.16 or higher
- Visual Studio 2022 (Windows) or equivalent C++17 compiler
- SDL3 and FMOD libraries (included in `lib/` directory)

### Build Instructions

1. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

2. **Generate project files:**
   ```bash
   cmake -G "Visual Studio 17 2022" -A x64 ..
   ```

3. **Build all components:**
   ```bash
   cmake --build . --config Debug
   ```

   For release builds:
   ```bash
   cmake --build . --config Release
   ```

### Build Outputs

After building, you'll find the following executables:

- **`luna.exe`** - Built in `build/src/ChessEngine/Debug/` (or `Release/`)
  - Standalone UCI chess engine
  - Can be used with any UCI-compatible chess GUI
  
- **`game_engine.exe`** - Built in `build/src/GameEngine/Debug/` (or `Release/`)
  - Interactive chess game with SDL3-based GUI
  - Communicates with chess engines via UCI protocol
  
- **`chess_rules`** - Static library linked into other components

## Configuration

### Engine Path Configuration

The game engine can locate the chess engine in two ways:

1. **Config file** (`resources/config.txt`):
   ```
   engine_path = build/src/ChessEngine/Debug/luna.exe
   ```

2. **Command line arguments** (takes precedence over config file):
   ```bash
   # Using --engine flag
   ./game_engine.exe --engine path/to/your/engine.exe
   
   # Using --engine= syntax
   ./game_engine.exe --engine=path/to/your/engine.exe
   
   # Show help
   ./game_engine.exe --help
   ```

Command line arguments will update the config file with the specified engine path.

## Project Structure

```
ChessLab/
├── src/
│   ├── ChessRules/           # Core chess logic library
│   │   ├── include/          # Bitboard, position, move generation
│   │   └── src/              # Implementation files
│   ├── ChessEngine/          # UCI chess engine (Luna)
│   │   ├── include/          # Engine, search, evaluation, TT, Zobrist
│   │   └── src/              # Implementation files
│   └── GameEngine/           # SDL game interface
│       ├── chess_game/       # Chess-specific game logic
│       ├── platform/         # Core engine systems
│       ├── system/           # Process management, config
│       └── graph/            # Scene graph and rendering
├── lib/                      # External libraries (SDL3, FMOD)
├── resources/                # Game assets and configuration
│   ├── config.txt            # Engine path and game settings
│   ├── audio/                # Sound effects and music
│   └── images/               # Textures and sprites
└── include/                  # Third-party headers (SDL3, FMOD, STB)
```

## Key Features

### Advanced Chess Engine
- **Transposition Tables**: Caches previously evaluated positions using Zobrist hashing for significant performance improvements
- **Zobrist Hashing**: Efficient position identification and comparison
- **Alpha-Beta Search**: Optimized search algorithm with pruning
- **UCI Protocol**: Full compatibility with standard chess GUIs

### Process Management
- **Anonymous Pipes**: Secure communication between game engine and chess engine processes
- **Cross-Platform Process Spawning**: Windows-native process creation and management
- **Engine Flexibility**: Support for any UCI-compliant chess engine

### Game Interface
- **SDL3 Integration**: Modern graphics and input handling
- **Scene Management**: Modular game state architecture
- **Audio System**: FMOD-powered sound effects and background music
- **Visual Polish**: Smooth animations and responsive UI

## Usage

### Chess Engine (UCI Mode)
```bash
./luna.exe
# Communicate via standard UCI commands:
# uci, isready, position, go, quit
```

### Game Interface
```bash
./game_engine.exe
# Interactive chess game with visual interface
# Optionally specify engine path:
./game_engine.exe --engine path/to/engine.exe
```

### Using with External Engines
The game engine can work with any UCI-compliant chess engine:
```bash
./game_engine.exe --engine stockfish.exe
./game_engine.exe --engine komodo.exe
```

## Development

This project demonstrates modern C++ practices and advanced chess programming techniques:

- **Modern C++ Architecture**: Clean separation of concerns with modular design
- **Bitboard Algorithms**: Efficient board representation and move generation
- **UCI Protocol Implementation**: Standard chess engine communication
- **Process Management**: Inter-process communication with anonymous pipes
- **Hash Table Optimization**: Transposition tables with Zobrist hashing
- **Game Engine Integration**: Real-time rendering and user interaction
- **Cross-Platform Development**: Windows-focused with portable design patterns

## License

Academic project - Johns Hopkins University
