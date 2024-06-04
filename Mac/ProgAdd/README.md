# ProgAdd

---

Moves a give executable to /usr/local/bin/
This way, it can be found by the terminal and can be excecuted from any terminal.

## Installation
Run these commands to install progadd on your mac:
```bash
g++ ./progadd.cc -o progadd -std=c++17
./progadd ./progadd
```

Usage:
```bash
progadd ./myExecutable
```
now myExecutable can be called from every terminal like this
```bash
myExecutable
```