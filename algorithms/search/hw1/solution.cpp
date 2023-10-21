#include <iostream>
#include <vector>
#include <queue>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <utility>

using board = std::vector<std::vector<int>>; // matrix representation

using dict = std::unordered_map<int, std::pair<int, int>>;


struct Move {
   int x{0};
   int y{0} ;
};



using string_dict = std::unordered_map<std::string, Move>;


std::istream& operator>>(std::istream& is,
                         board& b) { 
    for(int i = 0; i < b.size(); ++i) {
        for(int j = 0; j < b[i].size(); ++j) {
            is >> b[i][j];
        } 
    }
    return is;
}


std::ostream& operator<<(std::ostream& os,
                         const board& b) { 
    for(int i = 0; i < b.size(); ++i) {
        for(int j = 0; j < b[i].size(); ++j) {
            os << b[i][j] << " ";
        }
        std::cout << std::endl; 
    }
    return os;
}


/// @brief A Board represents a node
struct Board { 

    Board(const int dim, dict& target):
        tiles(std::vector<std::vector<int>>(dim)), 
        target(target) { 
        
        for(int i = 0; i < dim; ++i) {
            tiles[i] = std::vector<int>(dim, 0);
        }
    }

    int manhattan() const {
        int dist{0};
        int number{42}; // initial value does not matter

        for(int i = 0; i < size(); ++i) {
            for(int j = 0; j < size(); ++j) {
                number = tiles[i][j];
                if(number) {
                    dist += std::abs(i - target[number].first) +
                            std::abs(j - target[number].second); 
                }
            }
        }
        return dist;
    }

    bool allright() const {
        return manhattan() == 0;
    }

    int hamming() const { 
        int dist{0};
        int number{0};

        for(int i = 0; i < size(); ++i) {
            for(int j = 0; j < size(); ++j) {
                number = tiles[i][j];
                if(number) {
                    dist += i != target[number].first ||
                            j != target[number].second; 
                }
            }
        }
        return dist;
    }

    bool solvable() const {
        const int inv = num_inversions();
        const int dim = size();
        int blank_row = blank_coordinates().first;

        return (dim % 2 != 0 && inv % 2 == 0) ||
               (dim % 2 == 0 && (inv + blank_row) % 2 != 0);  
    }

    int size() const {
        return tiles.size();
    }

 
    board tiles;
    dict& target;
// private:
    int num_inversions() const { // bubble sort, the stupid way, ikr
                                 // the merge sort way is faster, yet 
                                 // takes longer to implement :D
        int inversions{0};
        const int dim = size();
        int x, zx, y, zy;

        for(int i = 0; i < dim * dim; ++i) {
            x = i / dim;
            y = i  % dim;

            if(tiles[x][y]) {
                for(int k = i + 1; k < dim * dim; ++k) {
                    zx = k / dim;
                    zy = k % dim;

                    if(tiles[zx][zy] && target[tiles[zx][zy]].first * dim +
                                        target[tiles[zx][zy]].second < 
                                        target[tiles[x][y]].first * dim +
                                        target[tiles[x][y]].second) {
                        ++inversions;
                    }
                }   
            }
        }

        return inversions;
    } 

    std::pair<int, int> blank_coordinates() const {
        int dim = size();
        for(int i = 0; i < dim; ++i) {
            for(int j = 0; j < dim; ++j) {
                if(not(tiles[i][j])) {
                    return std::pair{i, j};
                }
            }
        }
        throw std::runtime_error("Invalid state");
    }

// why inversed axes?

    static inline string_dict moves = { {"left", Move{0, 1}}, 
                                        {"right", Move{0, -1}}, 
                                        {"up", Move{1, 0}},
                                        {"down", Move{-1, 0}}, };


};


struct Target { 

    dict posistions;

    friend std::istream& operator>>(std::istream& is,
                                    Target& t) {

        int N{42};
        int zero_target_id{42}; // I in input example

        is >> N;
        int dim = Target::get_dim(N);

        is >> zero_target_id;
        zero_target_id = zero_target_id != -1 ? zero_target_id : N; 
        
        t.init_target(dim, zero_target_id);

        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, 
                                    const Target& t) { 
        for(auto& val_pos: t.posistions) {
            os << val_pos.first << ":(" 
               << val_pos.second.first << ", "
               << val_pos.second.second << ")\n";
        }

        return os << std::endl;
    }

    int size() const {
        return posistions.size();
    }

private:

    /// @brief create the target as a dictionary, 
    /// for linear heuristic metric calculation 
    void init_target(int dim, int zero_id) {
        
        const int len = dim * dim;

        if(zero_id < 0 || zero_id >= len) {
            throw std::domain_error("Target zero id is out of bounds!");
        }

        int x, y;

        for(int i = 0; i < zero_id; ++i) {
            x = i / dim;
            y = i % dim;
            posistions[x * dim + y + 1] = {x, y};
        }

        x = zero_id / dim;
        y = zero_id % dim;
        posistions[0] = { x, y };

        for(int i = zero_id + 1; i < len; ++i) {
            x = i / dim;
            y = i % dim;
            posistions[i] = {x, y};
        }

    }

    static int get_dim(const int n) {
        int root = std::sqrt(n + 1);
        if(root * root - 1 != n) {
            throw std::runtime_error("Invalid N. N + 1 must be an exact square.");
        }

        return root;
    }

};


struct Solution {
    Target& target;
    Board b;

    Solution(Target& t): target(t), 
                         b(std::sqrt(t.size()),
                           t.posistions) {
    } 

    friend std::istream& operator>>(std::istream& is, Solution& s) {
        return is >> s.b.tiles;
    }


};


int main() try {

    Target t;
    std::cin >> t;

    Solution s(t);
    std::cin >> s;
    std::cout << "Manhattan distance: " << s.b.manhattan()<< std::endl;
    std::cout << "Solvable: " << std::boolalpha << s.b.solvable() << std::endl;

    return 0;

} catch(const std::exception& e) { 

    std::cout << "The following error occurred: "
              << e.what() << std::endl; 
}