from typing import List, Tuple, Optional
from dataclasses import dataclass


@dataclass 
class Move:
    x: int = -1
    y: int = -1

    def read(self) -> None:

        while True:
            inp = input()
            if inp != '\n':
                tokens: List[str] = inp.split()
                
                if len(tokens) != 2:
                    print("A move consists of exactly 2 ints. Input move again.")
                    continue

                try:
                    self.x = int(tokens[0])
                    self.y = int(tokens[1])
                except ValueError:
                    print("Move coordinates must be ints. Input move again.")
                    continue

                if self._in_range():
                    break
                else:
                    print("Move coordinates must be in range[0, 2]. Input move again.")

    def _in_range(self) -> bool:
        return self.x < 3 and self.x >= 0 and\
               self.y < 3 and self.y >= 0 
        


class Board:
    PLAYERS: Tuple["str", "str"] = "X", "O"
    LIMIT: int = 10

    def __init__(self, 
                 player1: str = PLAYERS[0], 
                 computer: str = PLAYERS[0]) -> None:

        if player1 not in Board.PLAYERS:
            raise RuntimeError("Player symbol must be one of {Board.PLAYERS[0]}, \
                                                             {Board.PLAYERS[1]}")

        self.board: List[List[str]] = [["_" for _ in range(0, 3)] for _ in range(0, 3)]
        self.turn: str = player1
        self.computer: str = computer

    def play(self) -> None:
        print("Let the game begin")
        print(self)
        while True:
            winner: Optional[str] = self.terminal()
            
            if not winner:
                m: Move = Move()
                self.make_move()
                print(self)
            
            elif winner == "":
                return "Draw :("
            else:
                return f"{winner} won!"
                        


    def make_move(self) -> None:
        # assert(self.turn != self.computer)
        m: Move = Move()
        m.read()
        
        if self.board[m.x][m.y] != "_":
            print("Invalid move, spot is taken! Input again.")
            self.make_move()
        else:
            self.board[m.x][m.y] = self.turn
            self.turn = Board.PLAYERS[1] if self.turn == Board.PLAYERS[0] else Board.PLAYERS[0]
    
        


    def terminal(self) -> Optional[str]: # "" for draw, None for non-terminal state
        for i in range(0, 3):
            if self.board[i][0] != "_" and all(elem == self.board[i][0] 
                                               for elem in self.board[i]):
                return self.board[i][0]
        
        for j in range(0, 3):
            if self.board[0][j] != "_" and all(elem == self.board[0][j] 
                                               for elem in (self.board[k][j]
                                                            for k in range(1, 3))):
                return self.board[0][j]

        if self.board[0][0] != "_" and all(self.board[i][i] == self.board[0][0]
                                           for i in range(1, 3)):
            return self.board[0][0]

        if self.board[0][2] != "_" and all(self.board[i][2 - i] == self.board[0][2]
                                           for i in range(1, 3)):
            return self.board[0][0]

        return "" if all("_" not in row for row in self.board) else None

    def __str__(self) -> str:
        return "\n".join(str(row) for row in self.board)

    def eval(self, winner: str, depth: int) -> int:
        if winner == self.computer:
            return Board.LIMIT - depth
        if winner == "":
            return 0
        return depth - Board.LIMIT

if __name__ == "__main__":
    b: Board = Board()
    res: str = b.play()
    print(res)