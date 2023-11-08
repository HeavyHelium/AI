#include <iostream>
#include <vector>
#include <cassert>
#include <climits>
#include <random>
#include <chrono>
#include <stdexcept>
#include <iomanip>

std::mt19937 rng(std::random_device{}()); // Set Mersenne twister


template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    for(int i = 0; i < v.size(); ++i) {
        os << v[i] << '\n';
    }
    return os << std::endl;
}

int gen_number(int n) {
    return std::uniform_int_distribution<int>(0, n)(rng);
}

enum class Initialization {
    RAND,
    MIN_CONFL,
};


struct Board {
    /*
    Queen column positions are fixed & invariant to later movements.
    Board configurations are represented as arrays of size n.
    */
    static inline const int FREE{-1}; 
    static inline const int K{2};

    Board(int n=8)
        :queens(n, Board::FREE),
         row_cnts(n),
         primary_cnts(2 * n - 1, 0),
         secondary_cnts(2 * n - 1, 0),
         N(n) {
        
        if(n == 2 || n == 3) {
            throw std::runtime_error("-1"); // message modified to adhere to requirements
        }
    }

    int queen_max_conf() {
        std::vector<int> col_max_conf;
        int max_conf = INT_MIN;
        int curr_conf;

        for(int i = 0; i < N; ++i) {
            curr_conf = conflicts(i, queens[i]);

            if(curr_conf > max_conf) {

                col_max_conf.clear();
                col_max_conf.push_back(i);
                max_conf = curr_conf;

            } else if(curr_conf == max_conf) {
                col_max_conf.push_back(i);
            }
        }

        if(max_conf == 0) {
            solved = true;
        }

        int max_col_id = gen_number(col_max_conf.size() - 1);
        return col_max_conf[max_col_id];
    }

    void place_queen(int queen_id, int row) {
        
        if(queens[queen_id] == Board::FREE) {
            ++row_cnts[row];
            ++primary_cnts[primary(queen_id, row)];
            ++secondary_cnts[secondary(queen_id, row)];
            queens[queen_id] = row;
        
        } else {
            remove_queen(queen_id);
            place_queen(queen_id, row);
        }
    }

    void remove_queen(int queen_id) {
        if(queens[queen_id] != Board::FREE) {
            int row = queens[queen_id];

            --row_cnts[row];
            --primary_cnts[primary(queen_id, row)];
            --secondary_cnts[secondary(queen_id, row)];
            queens[queen_id] = Board::FREE;
        }
    }

    int conflicts(int queen_id, int row) const {

        int total = row_cnts[row] +
                    primary_cnts[primary(queen_id, row)] +
                    secondary_cnts[(secondary(queen_id, row))];
        
        return row == queens[queen_id] ? total - 3 : total; 
    }



    int primary(int col, int row) const {
        return col - row + N - 1;
    }

    int secondary(int col, int row) const {
        return col + row;
    }

    void solve(Initialization init) {
        int iter, col;
        //int been_there_done_that = 0;

        int limit = Board::K * N;


        while(not(solved)) {

            if(init == Initialization::RAND) {
                rand_init();
            } else {
                init_by_min_conf();
            }
            iter = 0;
            while(iter++ <= limit) {
                col = queen_max_conf(); // this raises the solved flag
                if(not(solved)) {
                    place_min_conf(col);
                }

                if(solved) {
                    //std::cout << "Took " << been_there_done_that * Board::K * n + iter << " steps\n";
                    break;
                }
            }
            //++been_there_done_that;
        }

        //std::cout << "Solution found!" << std::endl;

    }

    void print_test_format() const {
        std::cout << "[";
        if(N) {
            std::cout << queens[0];
        }
        for(int i = 1; i < N; ++i) {
            std::cout << ", " << queens[i];
        }
        std::cout << "]" << std::endl;
    }


    friend std::ostream& operator<<(std::ostream& os,
                                    const Board& b) {
        for(int i = 0; i < b.N; ++i) {
            for(int j = 0; j < b.N; ++j) {
                if(b.queens[j] == i) {
                    os << "* ";
                } else {
                    os << "_ ";
                }
            }
            os << '\n';
        }
        return os << std::endl;
    }

private:
    void init_by_min_conf() { 
        for(int i = 0; i < N; ++i) {
            place_min_conf(i);
        }
    }

    void rand_init() {
        int row;
        const int n = N;
        for(int i = 0; i < n; ++i) {
            row = gen_number(n - 1);
            place_queen(i, row);
        }
    }


    /// @brief places the queen on column queen_id 
    /// on the row with min #conflicts 
    void place_min_conf(int queen_id) {

        std::vector<int> rows_min_conf;
        int min_conf = INT_MAX;
        int curr_conf;

        for(int row = 0; row < N; ++row) {
            //place_queen(queen_id, row);
            curr_conf = conflicts(queen_id, row);

            if(curr_conf < min_conf) {
                rows_min_conf.clear();
                rows_min_conf.push_back(row);
                min_conf = curr_conf;

            } else if(curr_conf == min_conf) {
                rows_min_conf.push_back(row);
            }
        }

        int min_row_id = gen_number(rows_min_conf.size() - 1);

        place_queen(queen_id, rows_min_conf[min_row_id]);
    }


    std::vector<int> queens;
    std::vector<int> row_cnts;
    std::vector<int> primary_cnts;
    std::vector<int> secondary_cnts;
    const int N;
    bool solved = false;

};



struct ArgParser {

    ArgParser(int argc, char** args) {
         if(argc > 2) {
            throw std::runtime_error(usage);
         } 

         if(argc == 2) { // one arg
            if(args[1] == std::string("time")) {
                time = true;
            } else if(args[1] == std::string("test")) {
                test = true;
            } else {
                throw std::runtime_error(usage);
            }
            assert(time ^ test);
         }

    }

    
    bool time{false};
    bool test{false};

    static inline const std::string usage{"Usage: <program name> <optional: time> <optional: test>"};
};





int main(int argc, char** argv) try {

    ArgParser ap(argc, argv);
    Initialization init = Initialization::RAND;

    int n;
    std::cin >> n;

    Board b(n);

    if(ap.test) {
    
        b.solve(init);
        b.print_test_format();
    
    } else if(ap.time) { 
        
        auto start = std::chrono::high_resolution_clock::now();
        b.solve(init);
        auto stop = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);

        if(ap.time) {
            std::cout << "Execution time: " << 
                         std::fixed << std::setprecision(2) << 
                         duration.count() << "s" << std::endl;
        }

    } else {
        b.solve(init);
    }

    if(n < 101) {
        std::cout << b;
    }

    return 0;

} catch(std::exception& e) {
    std::cout << e.what() << std::endl;
}