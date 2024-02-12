# TUNIT

Tunit is a really simple and lightweight testing library written in C.
It is contained in only *one* file, `tunit.h`


![hey](https://github.com/Kaporos/tunit/assets/34960023/20ba9906-fdc2-4142-b180-a0634152154e)


## Features

- Basic assert between numbers
- Tests suites
- Pass values to tests


## Usage

Currently this project is still a WIP, please take a look at `main.c` which contains a demo of the actual state of the lib.


To use it, just add `tunit.h` in your project directory and put this in your main.c:

```c
#define TUNIT_IMPLEMENTATION
#include "tunit.h"
```

