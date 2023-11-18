from typing import List, Tuple, Optional
from dataclasses import dataclass


def y_n_input() -> bool:
    outputs: Tuple(str, str) = ('y', 'n')
    tokens: List[str] = []

    while(True):
        res: str = input()
        tokens = res.split()
        
        if len(tokens) != 1 or tokens[0] not in outputs:
            print("Please respond with [y/n]")
        else:
            return True if tokens[0] == outputs[0] else False

@dataclass
class GameConfig:
    first_player: str = ""
    computer: str = ""
    opponent: str = ""

    def read(self) -> None:
        print("Would you like to take the first turn? [y/n]")
        first: bool = y_n_input()

        print(f"Would you like to play with {Board.PLAYERS[0]}? [y/n]")
        res = y_n_input()

        self.opponent: str = Board.PLAYERS[0] if res else Board.PLAYERS[1]
        self.computer: str = Board.PLAYERS[1] if res else Board.PLAYERS[0]
        self.first_player = self.opponent if first else self.computer
    


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
    FREE_SPOT: str = "_"
    DRAW: str = ""
    LIMIT: int = 10
    N_INF = - LIMIT - 1
    P_INF = LIMIT + 1

    def __init__(self, conf: GameConfig) -> None:

        self.board: List[List[str]] = [[Board.FREE_SPOT 
                                        for _ in range(0, 3)] 
                                        for _ in range(0, 3)]
       
        self.turn: str = conf.first_player
        self.computer: str = conf.computer
        self.opponent: str = conf.opponent

        assert(self.computer != self.opponent)

    def maximizer(self, alpha: int, 
                  beta: int, depth: int = 0) -> int:
        winner: Optional[str] = self.terminal()

        """
        Computer virtual move
        """
        if winner != None:
            return self.eval(winner, depth)
        
        best: int = Board.N_INF # alpha candidate

        for i in range(0, 3):
            for j in range(0, 3):

                if self.board[i][j] == Board.FREE_SPOT:
                    
                    self.board[i][j] = self.computer

                    best = max(best, self.minimizer(alpha, beta, depth + 1))
                    self.board[i][j] = Board.FREE_SPOT # backtrack

                    if best >= beta:
                        return best 
                    
                    alpha = max(alpha, best)
        
        return best

    def minimizer(self, alpha: int,
                  beta: int, depth: int = 0) -> int:
        """
        Opponent virtual move
        """
        winner: Optional[str] = self.terminal()

        if winner != None:
            return self.eval(winner, depth)
        
        best: int = Board.P_INF# beta candidate

        for i in range(0, 3):
            for j in range(0, 3):
                if self.board[i][j] == Board.FREE_SPOT:
                    
                    self.board[i][j] = self.opponent

                    best = min(best, self.maximizer(alpha, beta, depth + 1))
                    self.board[i][j] = Board.FREE_SPOT # backtrack

                    if alpha >= best:
                        return best
                    
                    beta = min(best, beta)

        return best    

    def play(self) -> None:
        
        print(self, end="\n\n")

        winner: Optional[str] = self.terminal()

        if winner != None:
            print("End of game. " + 
                  ("Nobody " if winner == Board.DRAW else winner) + " won!")
            return 

        if self.turn == self.computer:
            self.computer_make_move()
        else:
            self.opponent_make_move()
        
        self.next_turn()
        self.play()


    def opponent_make_move(self) -> None:
        assert(self.computer != self.turn)
        m: Move = Move()
        m.read()

        if self.board[m.x][m.y] != Board.FREE_SPOT:
            print("Invalid move, spot is taken! Input again.", end="\n\n")
            self.opponent_make_move()
        else:
            self.make_move(m)

    def next_turn(self) -> None:
        self.turn = Board.PLAYERS[1] if self.turn == Board.PLAYERS[0] else Board.PLAYERS[0]
    
    def make_move(self, m: Move) -> None:
        self.board[m.x][m.y] = self.turn

    def computer_make_move(self) -> None:
        """
        Computer maximizes opponent's mins
        """
        assert self.turn == self.computer
        best_move: Move = Move()

        best_res: int = Board.N_INF
        val: int = Board.N_INF

        for i in range(0, 3):
            for j in range(0, 3):
                if self.board[i][j] == Board.FREE_SPOT:
                    
                    self.board[i][j] = self.computer
                    
                    val = self.minimizer(Board.N_INF, Board.P_INF)
                    if val > best_res:
                        best_res = val
                        best_move.x = i
                        best_move.y = j

                    self.board[i][j] = Board.FREE_SPOT

        self.make_move(best_move)

    def terminal(self) -> Optional[str]: # "" for draw, None for non-terminal state
        for i in range(0, 3):
            if self.board[i][0] != Board.FREE_SPOT\
               and all(elem == self.board[i][0] for elem in self.board[i]):
               
                return self.board[i][0]
        
        for j in range(0, 3):
            if self.board[0][j] != Board.FREE_SPOT \
               and all(elem == self.board[0][j] \
                       for elem in (self.board[k][j] for k in range(1, 3))):
        
                return self.board[0][j]

        if self.board[0][0] != Board.FREE_SPOT \
           and all(self.board[i][i] == self.board[0][0] for i in range(1, 3)):
            
            return self.board[0][0]

        if self.board[0][2] != Board.FREE_SPOT \
           and all(self.board[i][2 - i] == self.board[0][2] for i in range(1, 3)):
           
            return self.board[0][2]

        return Board.DRAW if all(Board.FREE_SPOT not in \
                                 row for row in self.board) else None


    def __str__(self) -> str:
        return "\n".join(" ".join(row) for row in self.board)

    def eval(self, winner: str, depth: int) -> int:
        if winner == self.computer:
            return Board.LIMIT - depth
        if winner == Board.DRAW:
            return 0
        return depth - Board.LIMIT




if __name__ == "__main__":

    conf: GameConfig = GameConfig()
    conf.read()

    b: Board = Board(conf)
    b.play()