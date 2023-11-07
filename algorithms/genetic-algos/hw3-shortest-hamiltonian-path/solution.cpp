#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cmath>
#include <random>
#include <algorithm>
#include <cassert>


std::mt19937 rng(std::random_device{}()); // Set Mersenne Twister

/// Utility functions
template<typename T>
std::ostream& operator<<(std::ostream& os,
                         const std::vector<T>& v) {

    for(int i = 0; i < v.size(); ++i) {
        os << v[i] << "\n";
    }

    return os << std::endl;
}

int gen_number(int n) {
    return std::uniform_int_distribution<int>(0, n)(rng);
}
/// End of utility functions


enum class Crossover {
    ONE_POINT,
    TWO_POINT,
};




struct Coordinate {
    int x{-1};
    int y{-1};

    double distance(const Coordinate& other) const {
        int d{0};

        return std::pow((x - other.x), 2) +
               std::pow((y - other.y), 2);
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const Coordinate& c) {
        return os << "(" << c.x << ", " << c.y << ")"; 
    }
}; 


struct Coordinates: public std::vector<Coordinate> {
    static inline const int PLANE_LIMIT = 10;

    Coordinates(const std::vector<Coordinate>& vec) {
        for(const Coordinate& c: vec) {
            this->push_back(c);
        }
    }
    Coordinates(const int n) {
        rand_init(n);
    }

private:
    void rand_init(const int n) {
        resize(n);

        for(int i = 0; i < n; ++i) {
            ((*this)[i]).x = gen_number(PLANE_LIMIT);
            ((*this)[i]).y = gen_number(PLANE_LIMIT);
        }
    }

};






struct Individual {
    double unfitness{0};
    std::vector<int> path;

    Individual(std::vector<int> path,
               const Coordinates& c)
        : path(path) {
        
        unfitness = calc_unfitness(c);
    }

    Individual(const Coordinates& c) {
        rand_init(c.size());
        unfitness = calc_unfitness(c);
    }

    /// @brief 
    /// @return the length of the path 
    int size() const {
        return path.size();
    }

    /// @brief random swap mutation 
    void mutate() {
        const int limit = size() - 1;
        int id1 = gen_number(limit);
        int id2 = gen_number(limit);

        std::swap(path[id1], path[id2]);
    } 

    ///@bried two point crossover, assumes i <= j

    static Individual child1_twopoint(const Individual& i1,
                                      const Individual& i2, 
                                      const Coordinates& c,
                                      const int i, 
                                      const int j) {
        
        std::vector<int> res_path(i1.size());
        
        const auto chunk_begin = i1.path.begin() + i;
        const auto chunk_end = i1.path.begin() + j + 1;
        
        std::copy(chunk_begin, // copy [i, j] chuck of parent 1
                  chunk_end, 
                  res_path.begin() + i);

        int read = j + 1;
        int write = read;

        while(write != i) {
            if(write == i1.size()) {
                write = 0;
            } else if(read == i1.size()) {
                read = 0;
            } else {
                if(std::find(chunk_begin,
                             chunk_end, i2.path[read]) == chunk_end) {
                    res_path[write++] = i2.path[read++];
                } else {
                    ++read;
                }
            }
            
        }
        Individual ch1 = Individual(res_path, c);
        ch1.mutate();

        return ch1;
    } 

    static Individual child2_twopoint(const Individual& i1, 
                                      const Individual& i2, 
                                      const Coordinates& c, 
                                      const int i, 
                                      const int j) {
        return child1_twopoint(i2, i1, c, i, j);
    }



    /// @brief one point crossover
    static Individual child1_onepoint(const Individual& i1, 
                                      const Individual& i2,
                                      const Coordinates& c,
                                      const int point) {
        
        std::vector<int> res_path(i1.size());
        auto end = i1.path.begin() + point;
        
        std::copy(i1.path.begin(),
                  end,
                  res_path.begin());
        
        int curr = point;

        for(int elem: i2.path) {
            if(std::find(i1.path.begin(), end, elem) == end) {
                res_path[curr++] = elem;
            }
        }
        
        Individual ch = Individual(res_path, c);
        ch.mutate();

        return ch;
    }
    /// @brief one point crossover 
    static Individual child2_onepoint(const Individual& i1, 
                                      const Individual& i2, 
                                      const Coordinates& c,
                                      const int point) {
    
        return child1_onepoint(i2, i1, c, point);
    }

    bool operator>(const Individual& other) const { // for the min heap
        return unfitness > other.unfitness;
    }

    bool operator<(const Individual& other) const {
        return *this > other;
    }


    friend std::ostream& operator<<(std::ostream& os,
                                    const Individual& id) {
        for(int i = 0; i < id.path.size(); ++i) {
            os << id.path[i] << " ";
        }
        os << '\n';
        os << "unfitness: " << id.unfitness;
        return os << '\n';
    }

private:
    
    void rand_init(const int size) {
        path.resize(size);
        for(int i = 0; i < size; ++i) {
            path[i] = i;
        }
        std::random_shuffle(path.begin(), path.end());
    }

    double calc_unfitness(const Coordinates& c) {
        if(path.size() < 2) {
            return 0;
        }

        double res{0};

        for(int i = 0; i < path.size() - 1; ++i) {
            res += c[path[i]].distance(c[path[i + 1]]);
        }

        return res;
    }

};

// struct Population: public std::vector<Individual> {
//     void rand_init(const int size, const Coordinates& c) {
//         for(int i = 0; i < size; ++i) {
//             (*this).push_back(Individual(c));
//         }
//     }
// };

struct Solution {
    
    std::priority_queue<Individual> parents;
    std::priority_queue<Individual> next_gen;
    Coordinates c;
    const int max_iter;
    const int path_len;
    const int population_size;
    Crossover crsvr;
    
    Solution(const int population_size, 
             const int max_iter, 
             const int vertex_cnt, 
             Crossover crsvr)
        : max_iter(max_iter),
          path_len(vertex_cnt),
          population_size(population_size), 
          crsvr(crsvr),
          c(vertex_cnt) {

        for(int i = 0; i < population_size; ++i) {
            parents.push(Individual(c));
        }
        // std::cout << parents.top() << std::endl;
    }

    Solution(const int population_size, 
             const int max_iter, 
             const int vertex_cnt, 
             Crossover crsvr, 
             const Coordinates& c)
        : max_iter(max_iter),
          path_len(vertex_cnt),
          population_size(population_size),
          crsvr(crsvr), c(c) {
        
        for(int i = 0; i < population_size; ++i) {
           parents.push(Individual(c));
        }
    }

    void two_point() {
        assert(crsvr == Crossover::TWO_POINT);
        int temp1, temp2;
        int init_size = parents.size();

        while(parents.size() > init_size / 2) {
            Individual p1 = parents.top();
            parents.pop();
            Individual p2 = parents.top();
            parents.pop();

            temp1 = gen_number(path_len - 1);
            temp2 = gen_number(path_len - 1);

            if(temp1 > temp2) {
                std::swap(temp1, temp2);
            }

            next_gen.push(p1);
            next_gen.push(p2);
            next_gen.push(Individual::child1_twopoint(p1, p2, c, temp1, temp2));
            next_gen.push(Individual::child2_twopoint(p1, p2, c, temp1, temp2));
        }
        parents = next_gen;
        next_gen = std::priority_queue<Individual>{}; 
    }

    void one_point() {
        assert(crsvr == Crossover::ONE_POINT);
        int temp;
        int init_size = parents.size();

        while(parents.size() > init_size / 2) {
            Individual p1 = parents.top();
            parents.pop();
            Individual p2 = parents.top();
            parents.pop();

            temp = gen_number(path_len);

            next_gen.push(p1);
            next_gen.push(p2);
            next_gen.push(Individual::child1_onepoint(p1, p2, c, temp));
            next_gen.push(Individual::child2_onepoint(p1, p2, c, temp));
        }
        parents = next_gen;
        next_gen = std::priority_queue<Individual>{}; 
    }


    void crossover() {
        int temp1, temp2;
        int init_size = parents.size();

        if(crsvr == Crossover::ONE_POINT) {
            one_point();
        } else { // twopoint
            two_point();
        }

    }

    const Individual& get_fittest() const {
        return parents.top();
    }


    void solve() {

        int mod = max_iter / 10;

        for(int i = 0; i < max_iter; ++i) {
            crossover();
            const Individual& fittest = get_fittest();
            if(i % mod == 0 || i == max_iter - 1) {
                std::cout << "Epoch " << i + 1 << ": " << fittest << std::endl;
            }
        } 
    }

};



int main() {
    std::vector<Coordinate> cs{ {5, 3}, {1, 4},
                                {8, 8}, {8, 7},
                                {9, 7}, {1, 1}, 
                                {2, 0}, {7, 9}, 
                                {8, 7}, {10, 5} };
    Coordinates c(cs);

    //std::cout << c << std::endl;
    //c.rand_init(5);
    // std::cout << c;

    // Population p;
    // p.rand_init(10, c);
    // std::cout << p;

    // int cross_point = 2;

    // int i = 2;
    // int j = 4;

    // Individual i1({1, 2, 3, 4, 5, 6, 7}, c);
    // Individual i2({7, 6, 2, 4, 3, 5, 1}, c);

    // Individual ch1 = Individual::child1_twopoint(i1, i2, c, i, j);
    // Individual ch2 = Individual::child2_twopoint(i1, i2, c, i, j);

    // std::cout << ch1;
    // std::cout << ch2;
    // std::cout << "Cross point " << cross_point << std::endl;

    // std::cout << "Child 1\n";
    // std::cout << ch1;
 
 
    int n;
    std::cin >> n;
    Solution s(5, 10000, n, Crossover::TWO_POINT, c);
    std::cout << "For coordinates: \n" << s.c << std::endl;
    s.solve();


    
     
    // std::cout << "Child 2\n";
    // std::cout << ch2;


    return 0;
}


/*

0_, 1, 2, 3_,  4, 5, 6_, 7, 8, 9_ // 10 iterations
10 / 3 => chunk size 3 

0_, 1, 2_, 3, 4_, 5 // 6 iters
6 / 3 -> chunk size 2 => 4 times 

iters = q * times + remainder, remainder \in [0, ..., times - 1]



*/