// One small leap for a frog, one giant leap for the CS student(maybe?) :D

/*
There are 2 * n frogs and only one free spot
The frogs are positioned in a manner of the kind: 
>>..> _ <<...<, where < denotes a frog looking to the left,
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

        /*
        Solve for the symmetric case, so we don't have to reverse the print path
        */
        
        for(int i = 0; i < n; ++i) { 
            board[i] = '<';
        }
        board[empty_space] = '_';
        for(int i = n + 1; i < board.size(); ++i) { 
            board[i] = '>';
        }
        
    }

    void solve() { 
        solve_rec(empty_space);
        std::cout << "Goal state reached." << std::endl;
    }


    void solve_rec(int empty_id) { 
        if(all_right()) { 
            found = true;
            std::cout << "Starting...\n"; // actually ending
            std::cout << *this;
            return;
        }

        for(int move: moves) { 
            int new_empty = empty_id + move;

            if(new_empty >=0 && new_empty < board.size()) { 

                if(move > 0 && board[new_empty] == '>' || // ensure validity
                   move < 0 && board[new_empty] == '<') { 

                    std::swap(board[new_empty], 
                              board[empty_id]);
                    solve_rec(new_empty);
                    
                    std::swap(board[new_empty], // backtrack
                              board[empty_id]);

                    if(found) { 
                        std::cout << *this;
                        return; // once we find the first solution, we return
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


private:

    bool all_right() const {  // well, all right left, all left right
        int n = N(); 
        for(int i = 0; i < n; ++i) { 
            if(board[i] != '>') { 
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
    s.solve();

    return 0;
}