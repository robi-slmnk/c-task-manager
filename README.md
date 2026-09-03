# c-task-manager

A terminal task manager for Linux, written from scratch in C with [ncurses](https://invisible-island.net/ncurses/).

It reads process information straight out of the kernel's `/proc` filesystem — no external libraries beyond ncurses — and shows a live, scrollable list of every running process, with system CPU usage and the ability to kill a selected process.

The project started as a way to understand how processes are actually represented inside the operating system, and how a tool like `top` or `htop` gets its numbers.

---

## Features

- **Live process list** — refreshed once per second, built by walking `/proc` and parsing `/proc/<pid>/status` for the process name and resident memory (`VmRSS`).
- **System CPU usage** — computed from the delta between two consecutive samples of `/proc/stat`, so the figure reflects the last refresh interval rather than the machine's uptime average.
- **Sorted by memory footprint** — the heaviest processes stay at the top.
- **Keyboard navigation** — arrow keys move a highlighted selection; the view scrolls automatically once the selection reaches the edge of the terminal.
- **Pause / resume** — freeze the display to read it without rows shifting under the cursor.
- **Process termination** — send a kill signal to the selected process without leaving the interface.
- **Custom dynamic storage** — the process list lives in a hand-rolled growable array that doubles its capacity on demand, with explicit cleanup on exit.

---

## Requirements

- Linux (the program depends on the `/proc` filesystem)
- `clang` — or `gcc`, if you change the compiler in the `Makefile`
- `ncurses` development headers

Install ncurses:

```bash
# Debian / Ubuntu
sudo apt install libncurses-dev clang make

# Fedora
sudo dnf install ncurses-devel clang make

# Arch
sudo pacman -S ncurses clang make
```

---

## Build and run

```bash
git clone https://github.com/robi-slmnk/c-task-manager.git
cd c-task-manager
make
```

`make` compiles the sources and launches the task manager immediately. To only build the binary:

```bash
make opt1
./task_manager
```

Killing a process you do not own requires elevated privileges — run with `sudo` if you need that.

---

## Controls

| Key | Action |
| --- | --- |
| <kbd>↑</kbd> / <kbd>↓</kbd> | Move the selection (the view scrolls at the edges) |
| <kbd>P</kbd> | Pause / resume the refresh cycle |
| <kbd>K</kbd> | Kill the selected process |
| <kbd>Q</kbd> | Quit |

Keys are case-insensitive.

---

## Interface

```
Processor usage : 4.271000
Press P or p for process pause   Press K or k for process termination   Press Q or q to close the task manager

Process ID        Name                     Memory
1                 systemd                  12984 kB
842               Xorg                     98220 kB
1337              firefox                  512480 kB
...
```

The selected row is drawn in reverse video.

---

## Project structure

| File | Responsibility |
| --- | --- |
| `main.c` | Initialises the ncurses screen (no echo, hidden cursor, keypad input, 1 s input timeout) and hands control to the main loop |
| `process_core.c` / `.h` | The core: scans `/proc`, parses each process's status file, samples `/proc/stat`, renders the table and handles key input |
| `mem_manager.c` / `.h` | Allocation, growth and teardown of the process array, plus the comparison function used for sorting |
| `rstrings.c` / `.h` | Small string helpers — digit checks (used to tell PID directories apart from the rest of `/proc`) and whitespace trimming |
| `Makefile` | Builds the four translation units and links against `-lncurses` |

### Data model

```c
typedef struct {
    char  PID[64];
    char  pName[64];
    char  memory_kbb[64];
    float cpu_usage;
} Process;

typedef struct {
    Process *list;
    int      process_count;
    int      mem_cap;
} process_manager;
```

`process_manager` owns the array; `mem_cap` is doubled by `processReallocation()` whenever the scan finds more processes than the current capacity holds.

---

## How it works

1. **Discovery.** Every entry in `/proc` whose name is entirely digits is a process. Each one gives a path like `/proc/1337/status`.
2. **Parsing.** That file is read line by line; the `Name:` and `VmRSS:` fields are extracted and trimmed of leading whitespace and the trailing newline. A process with no `VmRSS` line (kernel threads) is reported as `0 kB`.
3. **CPU.** The first line of `/proc/stat` aggregates jiffies across all cores. Summing every field gives total time, and the idle + iowait fields give idle time; comparing two consecutive samples yields usage for the interval that just elapsed.
4. **Sorting.** `qsort` orders the list by the memory string — longer strings first, then lexicographically — which puts the largest values on top.
5. **Rendering.** The table is redrawn to fit the current terminal height, offset by the scroll position, with the selected row inverted.
6. **Input.** `getch()` runs on a timeout, so the loop refreshes on its own when no key is pressed. Pausing flips the timeout negative, which makes `getch()` block until the next keystroke.

---

## Known limitations

These are honest rough edges rather than hidden bugs — the project is still in progress:

- CPU usage is reported **system-wide**, not per process; the `cpu_usage` field in `Process` is reserved for that and not yet populated.
- Termination sends `SIGKILL` (signal 9) directly, so the target gets no chance to shut down cleanly.
- Sorting compares the memory values as strings, which works because they share a unit but is not a true numeric comparison.
- Process names longer than the fixed 64-byte buffers are truncated.
- The list is rebuilt from scratch on every refresh rather than diffed.

---

## Roadmap

- [ ] Per-process CPU usage from `/proc/<pid>/stat`
- [ ] Configurable sort column (memory, CPU, PID, name)
- [ ] Search / filter by process name
- [ ] `SIGTERM` before `SIGKILL`, with a confirmation prompt
- [ ] Command line, user and thread count columns
- [ ] Configurable refresh interval

---

## License

No license has been chosen yet. Until one is added, all rights are reserved — open an issue if you would like to use the code.
