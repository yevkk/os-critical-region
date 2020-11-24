### Benchmark-results
N = 10000000

#### `std::atomic`

| Threads | Time(ms) |
|---------|----------|
|       2 |       76 |
|       3 |      383 |
|       4 |      601 |

#### `std::mutex`

| Threads | Time(ms) |
|---------|----------|
|       2 |      165 |
|       3 |      610 |
|       4 |     1083 |

#### `lab::SpinLock`

| Threads | Time(ms) |
|---------|----------|
|       2 |       94 |
|       3 |      782 |
|       4 |     1310 |

#### `lab::DekkerLock`

| Threads | Time(ms) |
|---------|----------|
|       2 |       53 |

#### `lab::ImprovedBakeryLock`

| Threads | Time(ms) |
|---------|----------|
|       2 |      134 |
|       3 |     1670 |
|       4 |     3022 |
