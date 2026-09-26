# movhex

Pathfinding engine in C (gnu11) for dynamic hexagonal grids. 

Built for strict CPU and memory constraints. Scored full marks for performance and memory efficiency - Algorithms and Data Structures 2024/2025 @ PoliMi.

### Problem Overview

- Offset-coordinate hex grid (R x C) with 6-way adjacency
- Cell exit costs (0 = blocked, 1-100)
- Up to 5 directional air routes (teleports) per cell
- Dynamic operations: `init`, `change_cost` (radial decay), `toggle_air_route`, `travel_cost`
- Full spec: [`docs/specifications.pdf`](docs/specifications.pdf)

### Implementation

No external dependencies.

- **Hot loop:** No malloc/free during pathfinding. Neighbors are written to a static array.
- **Priority queue:** Custom binary min-heap, O(log V). Lazy deletion (skips visited nodes on pop).
- **Hex distance:** Uses offset coords with `y & 1`. No conversion to cube. Returns O(1) when the map is unmodified, otherwise runs Dijkstra.
- **Route cache:** Hash table (open addressing + quadratic probing) for `travel_cost`. Same query = O(1).
- **Air routes:** Per-cell linked lists. Checked together with physical neighbors.

### Build & Run
```
make movhex
./movhex < test/input.txt
# CFLAGS = -Wall -Werror -std=gnu11 -O2 -ggdb -lm
```

### Testing

Deterministic tests:
```
for f in test/*.txt; do ./movhex < "$f" | diff - "$f.result" && echo "PASS $f"; done
```
### Profiling

Memory safety:
```
valgrind --leak-check=full --track-origins=yes ./movhex < test/large.txt
```

CPU / Call Graph:
```
valgrind --tool=callgrind --callgrind-out-file=/tmp/callgrind.out ./movhex < test/large.txt
kcachegrind /tmp/callgrind.out
```

Cache misses:
```
valgrind --tool=cachegrind --cachegrind-out-file=/tmp/cachegrind.out ./movhex < test/large.txt
```

### Tech Stack
C (gnu11), Make, Valgrind, GDB
