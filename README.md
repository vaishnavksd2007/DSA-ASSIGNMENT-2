# Assignment 2 – Question 12: Railway Booking IDs – Collision Resolution

**Subject:** PCCST303 – Data Structures and Algorithms

## Files
| File | Content |
|---|---|
| `hashing.c` | Source code (C) – Linear, Quadratic, Double Hashing |
| `input.txt` | Input data: `23 43 13 33 53 63 73` |
| `output.txt` | Program output (insertion trace, final tables, search probes) |
| `README.md` | Trace tables, complexity analysis, comparison table, conclusion |

Run: `gcc -o hashing hashing.c && ./hashing`

## Design choice
* Table size **m = 10**, primary hash `h1(k) = k mod 10`.
* Secondary hash for double hashing `h2(k) = 7 − (k mod 7)` (7 is prime and < m, never 0).
* Every key ends in 3, so **all 7 keys hash to index 3**. This is the worst case for primary clustering and shows the real difference between the techniques.
* Probe sequences: Linear `(h1+i) mod m`, Quadratic `(h1+i²) mod m`, Double `(h1+i·h2) mod m`.

## (a) Insertion trace and final tables

| Key | h1 | h2 | Linear (positions tried) | Quadratic | Double |
|---|---|---|---|---|---|
| 23 | 3 | 5 | 3 (1) | 3 (1) | 3 (1) |
| 43 | 3 | 6 | 3,4 (2) | 3,4 (2) | 3,9 (2) |
| 13 | 3 | 1 | 3,4,5 (3) | 3,4,7 (3) | 3,4 (2) |
| 33 | 3 | 2 | 3,4,5,6 (4) | 3,4,7,2 (4) | 3,5 (2) |
| 53 | 3 | 3 | 3..7 (5) | 3,4,7,2,9 (5) | 3,6 (2) |
| 63 | 3 | 7 | 3..8 (6) | 3,4,7,2,9,8 (6) | 3,0 (2) |
| 73 | 3 | 4 | 3..9 (7) | **fails** – 10 probes (3,4,7,2,9,8,9,2,7,4) | 3,7 (2) |
| **Total probes** | | | **28** | **31** (73 not inserted) | **13** |

**Final tables**

| Index | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
|---|---|---|---|---|---|---|---|---|---|---|
| Linear | – | – | – | 23 | 43 | 13 | 33 | 53 | 63 | 73 |
| Quadratic | – | – | 33 | 23 | 43 | – | – | 13 | 63 | 53 |
| Double | 63 | – | – | 23 | 13 | 33 | 53 | 73 | – | 43 |

Note: quadratic probing with m = 10 (not prime) only reaches offsets {0,1,4,5,6,9} from index 3, i.e. slots 3,4,7,8,9,2. Once these six are full, key 73 cannot be placed even though 4 slots (0,1,5,6) are empty.

## (b) Search trace (probes)

| Key | Type | Linear | Quadratic | Double |
|---|---|---|---|---|
| 23 | exists | 1 | 1 | 1 |
| 43 | exists | 2 | 2 | 2 |
| 73 | exists (Quadratic: not stored) | 7 | 10 (not found) | 2 |
| 83 | absent | 8 | 10 | 6 |
| 99 | absent | 2 | 2 | 3 |
| 5 | absent | 6 | 1 | 4 |
| **Total** | | **26** | **26** | **18** |
| **Average** | | 4.33 | 4.33 | 3.00 |

Unsuccessful searches stop at the first empty slot, or after m probes.

## (c) Load factor and analysis

**Load factor** α = n/m = 7/10 = **0.70** (0.60 for quadratic, since only 6 keys were stored).

Expected probes (uniform hashing assumption):

| Method | Successful | Unsuccessful |
|---|---|---|
| Linear | ½(1 + 1/(1−α)) | ½(1 + 1/(1−α)²) |
| Quadratic | ≈ −ln(1−α)/α | ≈ 1/(1−α) |
| Double | (1/α)·ln(1/(1−α)) | 1/(1−α) |

At α = 0.7: Linear ≈ 2.2 / 6.1, Quadratic/Double ≈ 1.7 / 3.3. Our data is far worse for linear (successful avg for stored keys = (1+2+...+7)/7 = 4) because all keys collide at one index – an adversarial, not uniform, input. Double hashing stayed close to theory since `h2` gave each key a different step.

**Effect of load factor:** as α → 1, linear probing degrades fastest (primary clustering: the block 3–9 grew with every insertion); quadratic removes primary clustering but suffers secondary clustering and may fail to find a slot when α is high or m is not prime (as seen with 73); double hashing is least sensitive to α because different keys follow different probe paths.

**Time complexity**

| Operation | Average (α constant) | Worst case |
|---|---|---|
| Insert / Search – all three | O(1) | O(n) |

**Space complexity:** O(m) for the table, O(1) extra for each method (double hashing needs one extra hash function, no extra memory).

## Comparison table

| Criterion | Linear | Quadratic | Double |
|---|---|---|---|
| Keys stored (of 7) | 7 | 6 | 7 |
| Insertion probes | 28 | 31 | 13 |
| Search probes (6 searches) | 26 | 26 | 18 |
| Clustering | Primary | Secondary | Minimal |
| Can fail below α = 1? | No | Yes (m not prime) | No (if m prime, h2 coprime with m) |
| Cache locality | Best | Medium | Poorest |
| Implementation | Simplest | Simple | Needs 2nd hash |

## Final conclusion
For this booking-ID set, where every ID maps to the same primary slot, **double hashing is the most suitable technique**: it stored all keys with the fewest insertion probes (13 vs 28 and 31) and the fewest search probes (18 vs 26 and 26). Linear probing works but builds one long cluster, making later operations O(n)-like. Quadratic probing failed to insert key 73 at only α = 0.6, so it is unsafe with m = 10; it would need a prime table size (e.g. 11). For a real system, use a prime table size, keep α below about 0.7 and resize when exceeded.
