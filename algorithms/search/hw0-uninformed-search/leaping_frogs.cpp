/*
There are 2 * n frogs and only one free spot
The frogs are positioned in a manner of the kind: 
>>..> _ <<...<, where < denotes a frog looking to the left,
> denotes a frog looking to the right

The number of left-looking frogs is equal to the number of right-looking frogs

A frog cannot change its outlook

The goal state is a configuration of the kind:
<<..< _ >>..>
*/

#include <iostream>
#include <vector>
#include <chrono>
#include <stdexcept>


struct Solution { 

    Solution(int n = 2)
        :board(2 * n + 1, '_'), 
         empty_space{n} { 
        
        for(int i = 0; i < n; ++i) { 
            board[i] = 'L';
        }
        board[empty_space] = '_';
        for(int i = n + 1; i < board.size(); ++i) { 
            board[i] = 'R';
        }
        
    }

    void solve() { 
        std::vector<std::string> path;
        solve_rec(empty_space);
    }


    void solve_rec(int empty_id) {
        if(all_right()) { 
            found = true;
            path.push_back(board);

            return;
        }

        for(int move: moves) { 
            int new_empty = empty_id + move;

            if(new_empty >= 0 && new_empty < board.size()) { 

                if(move > 0 && board[new_empty] == 'R' || // ensure validity
                   move < 0 && board[new_empty] == 'L') { 

                    std::swap(board[new_empty], 
                              board[empty_id]);
                    solve_rec(new_empty);
                    
                    std::swap(board[new_empty], // backtrack
                              board[empty_id]);

                    if(found) { 
                        path.push_back(board);
                        return; // once we find the first solution, we return
                    }

                }
            }

        }
    }


    friend std::ostream& operator<<(std::ostream& os,
                                    const Solution& s) { 
        for(auto iter = s.path.end() - 1; iter >= s.path.begin(); --iter) {
            os << *iter << '\n';
        }

        return os << std::endl;
    }


private:

    bool all_right() const {  
        int n = N(); 
        for(int i = 0; i < n; ++i) { 
            if(board[i] != 'R') { 
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
    std::string board;
    std::vector<std::string> path;

    static inline const std::vector<int> moves{-1, 1, 2, -2};
};

struct ArgParser {

    ArgParser(char** args) {
        int iter{0};

        while(*(++args)) {
            ++iter;

            if(iter > 1) {
                throw std::runtime_error("Wrong argument count!");
            } else if(*args != std::string("t")) {

                std::cout << *args << std::endl;
                throw std::runtime_error(usage);

            } else {
                time = true;
            }
        } 
    }

    
    bool time{false};

    static inline const std::string usage{"Usage: <program name> <optional: t>"};
};


int main(int argc, char** argv) try { 

    ArgParser ap(argv);
    
    int n;
    std::cin >> n;
    std::cout << std::endl;

    Solution s(n);
    

    if(ap.time) {

        auto start = std::chrono::high_resolution_clock::now();
        s.solve();
        auto stop = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);

        std::cout << "Execution time: " << duration.count() << "s" << std::endl;
        
    } else {
        s.solve();
    }

    std::cout << s;

    return 0;

} catch(const std::exception& e) {

    std::cerr << e.what() << "\n";

    return -1;
}