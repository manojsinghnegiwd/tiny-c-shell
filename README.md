# TinyShell

A simple shell implementation in C, designed as a learning project to understand the fundamentals of shell programming and system calls.

## Overview

TinyShell is a minimalistic shell implementation that demonstrates core shell functionalities including:

- Command execution
- Basic built-in commands
- Process management
- Input/output redirection
- Pipeline support
- Signal handling

## Features (Planned)

- [x] Basic command execution
- [x] Built-in commands (cd, pwd, exit, printenv, print_history)
- [x] Environment variable support (shell variable assignment and retrieval)
- [x] Command history with up/down arrow keys
- [ ] Input/output redirection (>, <, >>)
- [ ] Pipeline support (|)
- [ ] Background process execution (&)
- [ ] Signal handling (Ctrl+C, Ctrl+Z)
- [ ] Tab completion

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./tinyshell
```

### Available Commands

- Standard UNIX commands (ls, cat, echo, etc.)
- `cd <directory>` - Change directory
- `pwd` - Print working directory
- `exit` - Exit the shell
- `printenv` - Print all shell variables
- `print_history` - Display command history
- `VARIABLE=value` - Set shell variable
- Use ↑/↓ arrow keys to navigate command history
- Backspace to delete characters

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details. 