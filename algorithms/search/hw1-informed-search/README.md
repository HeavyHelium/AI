# N puzzle

- Board of N blocks and 1 free spot
- Input
  - N - the number of blocks with numbers, i.e. 8, 15, 24
  - I - the index of the position of zero in the goal state

[Better problem definition & explanation](https://www.cs.princeton.edu/courses/archive/spring18/cos226/assignments/8puzzle/index.html)

- Solution implements the **_IDA\*_** minimal path cost search algorithm

**_!NB!_** The tests outputs might be different depending on your order of moves(i.e. first you try right or left, etc.)

- The output should be in the format\:

```
<number of steps takes to solve the puzzle, -1 if no solution exists>
<step_1>
.....
<step_K>

```

Where step_i is the ith step in the solution, step_i $\in$ {left, right, down, up}.

## Running

- Compile the program

```sh
g++ solution.cpp -std=c++17 -o a.out
```

- For testing

```sh
cd tests
python3 runner.py

```
