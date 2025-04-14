![Logo](https://github.com/realCheesyQuesadilla/seppuku/raw/main/samurai.png)

# seppuku - Process Kill Library

A lightweight, flexible library for adding timeout capabilities to any Linux process using LD_PRELOAD.

## Overview

`seppuku.so` is a dynamic library that terminates a process after a specified amount of time. It can be used with any program without modifying its code, making it particularly useful for:

- Adding timeouts to programs that don't natively support them
- Enforcing resource limits on long-running processes
- Preventing runaway processes in automated environments
- Creating timeout wrappers for testing and benchmarking

## Installation

### Requirements

- Linux operating system
- GCC compiler
- POSIX threads library (pthread)

### Building

Clone the repository and build the library:

```bash
git clone https://github.com/realCheesyQuesadilla/seppuku.git
cd seppuku
make
```

## Usage

Basic usage:

```bash
LD_PRELOAD=./seppuku.so TIME=<seconds> <command>
```

Examples:

```bash
LD_PRELOAD=./seppuku.so TIME=5 nc -lvp 4444

LD_PRELOAD=./seppuku.so nc -lvp 4444
```

## Features

- **Zero Modification**: Works with any executable without changing its code
- **Minimal Overhead**: Lightweight implementation with negligible performance impact
- **Flexible Timeouts**: Set any timeout duration in seconds
- **Clean Termination**: Uses SIGTERM for graceful process termination

### Integration with Scripts

You can easily integrate the timeout library into shell scripts:

```bash
#!/bin/bash

# Set the path to seppuku.so
TIMEOUT_LIB="/path/to/seppuku.so"

# Function to run a command with seppuku
run_with_timeout() {
    local timeout_seconds="$1"
    shift
    LD_PRELOAD="$TIMEOUT_LIB" TIME="$timeout_seconds" "$@"
}

# Example usage
run_with_timeout 5 ./long_running_task
```

## How It Works

The library uses the LD_PRELOAD mechanism to inject itself into the target process. When loaded, it:

1. Reads the `TIME` environment variable
     NOTE: If no `TIME` variable exists, it will default to time specified in code.
3. Creates a background timer thread
4. After the specified timeout, sends SIGTERM to terminate the process


This approach ensures that the timeout works regardless of what the target process is doing, even if it's stuck in an infinite loop or blocked I/O operation.

## Limitations

- Will not work with statically linked executables
- May not work with SUID binaries (for security reasons)
- Cannot timeout specific parts of a program, only the entire process

## Future Goals  

I plan to attend functionality to delete binary passed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.

