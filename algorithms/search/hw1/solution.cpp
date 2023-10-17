#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <unordered_map>
#include <utility>

using board = std::vector<std::vector<int>>; // matrix representation

using dict = std::unordered_map<int, std::pair<int, int>>;


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


struct Board { 

    Board(board b):
        tiles(b) { 
    }

    Board(const int dim = 0):
    tiles(std::vector<std::vector<int>>(dim)) { 
        for(int i = 0; i < dim; ++i) {
            tiles.push_back(std::vector<int>(dim, 0));
        }
    }

    int manhattan(dict& target) const {
        int dist{0};
        int number{0};

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

    int hamming(dict& target) const { 
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

    int size() const {
        return tiles.size();
    }

 
    board tiles;
    

};


struct Solution { 

    Board b;
    dict target;
    int target_zero{0};



    void set_zero(const int val) { 
        if(val < 0 || val >= b.size() * b.size()) {
            throw std::domain_error("Invalid zero id");
        } else {
            target_zero = val;
        }
    }

    friend std::istream& operator>>(std::istream& is,
                                   Solution& s) {

        int N{0};
        int I{0};

        is >> N;                                
        is >> I;

        s.b = board(get_dim(N));

        s.set_zero(I == -1 ? s.b.size() * s.b.size() - 1 : I);

        for(int i = 0; i < s.b.size(); ++i) {
           s.b.tiles[i].resize(s.b.size()); 
        }

        s.init_target();

        is >> s.b.tiles;

        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, 
                                    const Solution& s) { 
        os << s.b.tiles << std::endl;
        os << "Target zero id: " << s.target_zero << std::endl;

        return os;

    }

private:

    void init_target() {
        int linear = 1;
        const int dim = b.size();

        const int zx = target_zero / dim;
        const int zy = target_zero % dim;

        target[0] = std::pair{ zx, zy };

        for(int i = 0; i <= zx; ++i) {
            for(int j = 0; (j < dim && i < zx) ||
                           (i == zx && j < zy); 
                           ++j) {
                target[linear++] = std::pair{ i, j };
            }
        }
        for(int j = zy + 1; j < dim; ++j) {
            target[linear++] = std::pair{ zx, zy };
        }

        for(int i = zx  + 1; i < dim; ++i) {
            for(int j = 0; j < dim; ++j) {
                target[linear++] = std::pair{i, j};
            }
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



int main() {

    Solution s;
    std::cin >> s;
    std::cout << s;
    std::cout << "..................." << std::endl;
    for(auto p: s.target) {
        std::cout << p.first << ", " << p.second.first << " " << p.second.second << std::endl;
    }
    std::cout << "..................." << std::endl;

    std::cout << "Distances: " << std::endl;
    std::cout << s.b.hamming(s.target) << std::endl;
    std::cout << s.b.manhattan(s.target) << std::endl;


    return 0;
}