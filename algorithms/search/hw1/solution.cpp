#include <iostream>
#include <vector>
#include <queue>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <climits>

using board = std::vector<std::vector<int>>; // matrix representation

using dict = std::unordered_map<int, std::pair<int, int>>; // to save goal state
using string_dict = std::unordered_map<std::string, std::string>;


struct Move {
   int x{0};
   int y{0} ;

   Move reverse() const {
        return Move{x ? -x : x, 
                    y ? -y : y};
   }
};



using string_move_dict = std::unordered_map<std::string, Move>;


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

std::ostream& operator<<(std::ostream& os, 
                         const std::vector<std::string>& v) {
    for(const std::string& elem: v) {
        os << elem << '\n';
    }
    return os << std::endl;
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

    bool allright() const { // reached goal state
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
        int blank_row = zero_id / dim;

        return (dim % 2 != 0 && inv % 2 == 0) ||
               (dim % 2 == 0 && (inv + blank_row) % 2 != 0);  
    }

    int size() const {
        return tiles.size();
    }

    bool apply_move(const Move& m) {
        const int dim = size();
        int zx = zero_id / dim;
        int zy = zero_id % dim;

        int x, y;
        x = zx + m.x;
        y = zy + m.y;

        if(within_limits(x) && within_limits(y)) {
            std::swap(tiles[zx][zy], 
                      tiles[x][y]);
            zero_id = x * dim + y; // update zero pos
            return true; 
        }

        return false;
    }

    bool within_limits(int id) const {
        return (id >= 0 && id < size()); 
    }
 
    board tiles;
    int zero_id{0};
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

    std::pair<int, int> blank_coordinates() {
        int dim = size();
        for(int i = 0; i < dim; ++i) {
            for(int j = 0; j < dim; ++j) {
                if(not(tiles[i][j])) {
                    zero_id = i * dim + j;
                    return std::pair{i, j};
                }
            }
        }
        throw std::runtime_error("Invalid state");
    }


    /// @brief  with respect to the blank position
    static inline string_move_dict moves = { {"left", Move{0, 1}}, 
                                             {"right", Move{0, -1}}, 
                                             {"up", Move{1, 0}},
                                             {"down", Move{-1, 0}}, };
    
    static inline string_dict opposite_name {{"left", "right"},
                                             {"right", "left"}, 
                                             {"up", "down"}, 
                                             {"down", "up"},};


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
        zero_target_id = zero_target_id != -1 ? zero_target_id : N; // set to bottom right if -1
        
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

    static inline const int FOUND = -1;

    Target& target;
    Board b;
    std::vector<std::string> path;

    /// @brief  performs depth-limited search
    /// @param g the cost function, level-wise
    /// @param threshold determines the termination condition
    /// @return  FOUND if the goal state is reached and INT_MAX otherwise
    int search(int g, int threshold) {
        int man = b.manhattan();
        int f = g + b.manhattan();
        if(f > threshold) {
            return f;
        }

        if(b.allright()) {
            std::cout << '\n' << g << std::endl;
            std::cout << path << std::endl;
            return FOUND;
        }


        int min = INT_MAX;
        int temp{42};


        for(const auto& name_move: Board::moves) {
            if((not(path.empty()) &&
                path.back() != Board::opposite_name[name_move.first]) || path.empty()) {
            
                if(b.apply_move(name_move.second)) {

                    path.push_back(name_move.first);
                    temp = search(g + 1, threshold);

                    if(temp == FOUND) {
                        return FOUND;
                    } 
                    if(temp < min) {
                        min = temp;
                    }

                    b.apply_move(name_move.second.reverse());    
                    path.pop_back();
                }
                
            }
        }

        return min;
    }

    void idastar() {
        int threshold = b.manhattan(); // the heuristic value of root
        int temp{42};

        while(42) {
            temp = search(0, threshold);
            if(temp == FOUND) {
                break;
            } else {
                threshold = temp;
            }
        }
    }



    Solution(Target& t): target(t), 
                         b(std::sqrt(t.size()),
                           t.posistions) {
    } 

    friend std::istream& operator>>(std::istream& is,
                                    Solution& s) {
        is >> s.b.tiles;
        s.b.blank_coordinates();
        
        if(not(s.b.solvable())) {
            throw(std::runtime_error("Board cannot be solved!"));
        }

        return is;
    }


};


int main() try {

    Target t;
    std::cin >> t;

    Solution s(t);
    std::cin >> s;

    s.idastar();

    return 0;

} catch(const std::exception& e) { 

    std::cout << "The following error occurred: "
              << e.what() << std::endl; 
}