// One small leap for a frog, one giant leap for the CS student(maybe?) :D

/*
There are 2 * n frogs and only one free spot
The frogs are positioned in a manner of the kind: 
>>..> _ <<...<, where < denotes a frog looking to the left, <
> denotes a frog looking to the right

The number of left-looking frogs is equal to the number of right-looking frogs

A frog cannot change its outlook(So don't be one, please)

The goal state is a configuration of the kind:
<<..< _ >>..>
*/

#include <iostream>
#include <vector>


struct Solution { 

    Solution(int n = 2)
        :board(2 * n + 1), 
         empty_space{n} { 
        
        for(int i = 0; i < n; ++i) { 
            board[i] = '>';
        }
        board[empty_space] = '_';
        for(int i = n + 1; i < board.size(); ++i) { 
            board[i] = '<';
        }
        
    }

    void solve() { 
        solve_rec(empty_space);
    }


    void solve_rec(int empty_id) { 
        if(all_right()) { 
            found = true;
            std::cout << "Found solution: ";
            std::cout << *this;
            return;
        }

        for(int move: moves) { 
            int new_hole = empty_id + move;

            if(new_hole >=0 && new_hole < board.size()) { 

                if(move > 0 && board[new_hole] == '<' || // ensure validity
                   move < 0 && board[new_hole] == '>') { 

                    std::swap(board[new_hole], 
                              board[empty_id]);
                    solve_rec(new_hole);
                    
                    std::swap(board[new_hole], // backtrack
                              board[empty_id]);

                    if(found) { 
                        std::cout << *this;
                        return;
                    }

                }
            }

        }
    }


    friend std::ostream& operator<<(std::ostream& os,
                                    const Solution& s) { 
        for(char elem: s.board) { 
            os << elem;
        }

        return os << '\n';
    }


//private:

    bool all_right() const {  // well, all right right, right
        int n = N(); 
        for(int i = 0; i < n; ++i) { 
            if(board[i] != '<') { 
                return false;
            }
        }

        if(board[n] != '_') return false;

        return true;
    }

    int N() const { 
        return (board.size() - 1) / 2;
    }

    bool found = false;
    int empty_space = -1;
    std::vector<char> board;

    static inline const std::vector<int> moves{-2, -1, 1, 2};
};






int main() { 
    Solution s;

    s.solve_rec(s.empty_space);


    return 0;
}