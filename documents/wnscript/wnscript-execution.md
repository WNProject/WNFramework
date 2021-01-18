# Execution Environments

WNScript currently has several modes of compilation and execution.

| Mode                | Description                                                                                                                                                                            |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| llvm-jit-engine     | In this mode, WNScripts can be directly executed from C/C++ code. The script is compiled on first use, and a function pointer is obtainable from the scripting engine.                 |
| c-generation        | Instead of running any scripts, WNScripts can be translated into equivalent C code.                                                                                                    |
| c-engine (planned)  | A set of files created from the c-generation step, after having been compiled into a shared library (.so/.dll) file can be loaded and run using the same machinery as llvm-jit-engine. |
| c-program (planned) | Same as c-generation but additional C code is emitted to allow the resulting script to be run as a sandalone executable.                                                               |

> **Note** this is going to expand in the future.
