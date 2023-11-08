# N Queens

On an **NxN** chess board place **N** queens so that no two queens share the same row, column or diagonal.  
Use the **_MinConflicts_** algorithm to solve the problem.

($\forall$ n $\in$ $\mathbb{N}$)[n $\neq$ 2 $\land$ n $\neq$ 3 $\iff$ n-queens has a solution]

## How to run

- Compile

```sh
g++ -std=c++17 src/solution.cpp -o a.out
```

- Run with only solution output in board format

```sh
./a.out
```

- Run with time execution estimate and solution output in board format

```sh
./a.out time
```

- Run with solution output in test format

```sh
./a.out test
```

## IO

- Input: **N**

* Output
  - **_In all cases, if the board does not have a solution, the solution output is -1;_**
    - The board format is such that we denote a queen with '\*' and an empty cell with '\_'.
    - The test format is [i_0, ..., i_n-1], where i_j is the row position of the queen.

\***_All of this is under the assumption that each queen's column position is fixed at initialization and invariantly no two queens have the same column position._**
