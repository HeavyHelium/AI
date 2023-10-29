#include <iostream>
#include <vector>
#include <cassert>
#include <climits>
#include <random>

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



struct Board {
    /*
    Queen column positions are fixed & invariant to later movements.
    Board configurations are represented as arrays of size n.
    */
    static inline const int FREE{-1}; 

    std::vector<int> queens;
    std::vector<int> row_cnts;
    std::vector<int> primary_cnts;
    std::vector<int> secondary_cnts;



    Board(int n=8)
        :queens(n, FREE),
         row_cnts(n),
         primary_cnts(2 * n - 1, 0),
         secondary_cnts(2 * n - 1, 0) {
        
        init_by_min_conf();
    }

    void place_queen(int queen_id, int row) {
        
        if(queens[queen_id] == FREE) {
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
        if(queens[queen_id] != FREE) {
            int row = queens[queen_id];

            --row_cnts[row];
            --primary_cnts[primary(queen_id, row)];
            --secondary_cnts[secondary(queen_id, row)];
            queens[queen_id] = FREE;
        }
    }

    int conflicts(int queen_id) const {
        return row_conflicts(queen_id) +
               primary_conflicts(queen_id) +
               secondary_conflicts(queen_id);
    }

    int row_conflicts(int queen_id) const {
        int row_id = queens[queen_id];
        return row_cnts[row_id] - 1;
    }


    int primary(int col, int row) const {
        return col - row + size() - 1;
    }

    int secondary(int col, int row) const {
        return col + row;
    }

    int primary_conflicts(int queen_id) const {
        return primary_cnts[primary(queen_id, 
                                    queens[queen_id])] - 1;
    }

    int secondary_conflicts(int queen_id) const {
        return secondary_cnts[secondary(queen_id, 
                                        queens[queen_id])] - 1;
    }


    int size() const {
        return queens.size();
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const Board& b) {
        for(int i = 0; i < b.size(); ++i) { // row-wise
            for(int j = 0; j < b.size(); ++j) { // column-wise
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
        for(int i = 0; i < size(); ++i) {
            place_min_conf(i);
        }
    }


    /// @brief placed the queen on column queen_id 
    /// on the row with min #conflicts 
    void place_min_conf(int queen_id) {

        std::vector<int> rows_min_conf;
        int min_conf = INT_MAX; // size() + 1 is sufficient
        int curr_conf{42};

        for(int row = 0; row < size(); ++row) {
            place_queen(queen_id, row);
            curr_conf = conflicts(queen_id);

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


};




int main() {

    int n = 4;

    for(int i = 1; i < 5; ++i) {
        Board b(n);
        std::cout << b << std::endl;
    }

    return 0;
}