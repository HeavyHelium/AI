#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cmath>
#include <random>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <iomanip>


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


double gen_prob() {
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
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

    double max_cost() const {
        int temp = PLANE_LIMIT * PLANE_LIMIT;
        return 2 * temp * (size() - 1); 
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
    double fitness{0};
    std::vector<int> path;

    Individual(const std::vector<int>& path,
               const Coordinates& c)
        : path(path) {

        fitness = calc_fitness(c);


    }

    Individual(const Coordinates& c) {
        rand_init(c.size());
        fitness = calc_fitness(c);
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

    /// @brief inversion mutation
    void mutate_inv() {
        int i = gen_number(size() - 1);
        int j = gen_number(size() - 1);

        if(i > j) {
            std::swap(i, j);
        }        
    
        std::reverse(path.begin() + i, path.begin() + j + 1);    
    }

    ///@bried two point crossover, assumes i <= j

    static Individual child1_twopoint(const Individual& i1,
                                      const Individual& i2, 
                                      const Coordinates& c,
                                      const int i, 
                                      const int j) {

        // std::cout << i << ", " << j << std::endl;

        if(i == j) {
            return child1_onepoint(i1, i2, c, i);
        }
        
        std::vector<int> res_path(i1.size(), 0);

        for(int k = i; k < j; ++k) {
            res_path[k] = i1.path[k];
        }

        int read = j;
        int write = read;

        while(write != i) {
            if(read == i1.path.size()) {
                read = 0;
                continue;
            }
            if(write == i1.path.size()) {
                write = 0;
                continue;
            }

            if(std::find(i1.path.begin() + i, 
                         i1.path.begin() + j, 
                         i2.path[read]) == i1.path.begin() + j) {
                res_path[write++] = i2.path[read++];
            } else {
                ++read;
            }
        }

        // std::cout << "parents: \n" << i1 << "\n" << i2 << "\n";

        Individual ch1 = Individual(res_path, c);
        // std::cout << "rp: " << res_path << std::endl;
        ch1.mutate_inv();

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
        
        std::vector<int> res_path(i1.size(), 0);
        const auto end = i1.path.begin() + point;

        for(int i = 0; i < i1.path.size(); ++i) {
            res_path[i] = i1.path[i];
        }


        int write = point;
        int read = point;

        while(write < i1.path.size()) {
            if(read == i1.path.size()) {
                read = 0;
            }

            if(std::find(i1.path.begin(),
                         i1.path.begin() + point, 
                         i2.path[read]) == i1.path.begin() + point) {
                res_path[write++] = i2.path[read++];
                
            } else {
                ++read;
            }
        }

        Individual ch(res_path, c);
        ch.mutate_inv();

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
        return fitness > other.fitness;
    }

    bool operator<(const Individual& other) const {
        return other > *this;
    }


    friend std::ostream& operator<<(std::ostream& os,
                                    const Individual& id) {
        for(int i = 0; i < id.path.size(); ++i) {
            os << id.path[i] << " ";
        }
        os << '\n';
        os << "fitness: " << id.fitness;
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

    double calc_fitness(const Coordinates& c) {
        if(path.size() < 2) {
            return 0;
        }

        double res{0};
        

        for(int i = 0; i < path.size() - 1; ++i) {
            res += c[path[i]].distance(c[path[i + 1]]);
        }



        return c.max_cost() - res;
    }

};

struct Population: public std::vector<Individual> {
    void rand_init(const int size, const Coordinates& c) {
        for(int i = 0; i < size; ++i) {
            (*this).push_back(Individual(c));
        }
    }

    void sort() {
        std::sort(this->begin(), this->end(), 
                  [](const Individual& i1,
                     const Individual& i2) {
                     return i1.fitness < i2.fitness; }); // sort the population by fitness
    }

    void sort_rev() {
        std::sort(this->begin(), this->end(), 
                  [](const Individual& i1,
                     const Individual& i2) {
                     return i1.fitness > i2.fitness; }); // sort the population by fitness
    }
};

struct Solution {

    static const inline double SELECTION_PROP = 0.5;
    
    Coordinates c;
    Population p;
    
    const int max_iter;
    const int path_len;
    const int population_size;

    Crossover crsvr;
    
    Solution(const int population_size, 
             const int max_iter, 
             const int vertex_cnt, 
             Crossover crsvr = Crossover::TWO_POINT)
        : max_iter(max_iter),
          path_len(vertex_cnt),
          population_size(population_size), 
          crsvr(crsvr),
          c(vertex_cnt) { // random initialization of coordiantes
        

        p.rand_init(population_size, c);
        // std::cout << c << std::endl;            
    }

    Solution(const int population_size, 
             const int max_iter, 
             const int vertex_cnt, 
             const Coordinates& c)
        : max_iter(max_iter),
          path_len(vertex_cnt),
          population_size(population_size), c(c) {
        
        p.rand_init(population_size, c);
    }


    double sum_fitness() const {
        return std::accumulate(p.begin(), p.end(), 0.0,
                               [](double a, const Individual& i){return a + i.fitness;});
    }

    /// @brief Roulette Wheel Selection
    std::vector<int> selection() {
        int id{-1};
        const double sum = sum_fitness();
       
        double prob_prev_cuml{0};
        double prob_temp{0};


        int selection_cnt = SELECTION_PROP * population_size;
        if(selection_cnt % 2 != 0) {
            ++selection_cnt;
        }

        p.sort();

        std::vector<double> probs(population_size);
        
        std::transform(p.begin(), p.end(), probs.begin(), [&sum, &prob_prev_cuml]
                                                          (const Individual& i) { 
                                                            prob_prev_cuml += i.fitness / sum;
                                                            return prob_prev_cuml; // CDF calculation 
                                                           }); 

        std::vector<int> selected;
        int cnt = 0;

        while(cnt < selection_cnt) {

            prob_temp = gen_prob();
            for(int i = 0; i < p.size(); ++i) {

                id = 0;

                while(prob_temp > probs[id] && prob_temp < probs.back()) { 
                    ++id;
                }
            
                selected.push_back(id);
                ++cnt;
            }
        }
    


        return selected;
    }

    void crossover() {
        std::vector<int> parents = selection();
        int parent_id1{-1};
        int parent_id2{-1};
        int point1, point2;

        for(int i = 0; i < parents.size() / 2; ++i) {
            
        
            parent_id1 = i;
            parent_id2 = i + parents.size() / 2;

            point1 = gen_number(path_len);
            point2 = gen_number(path_len - 1);
            if(point1 > point2) {
                std::swap(point1, point2);
            }

            Individual child1 = Individual::child1_twopoint(p[parent_id1], 
                                                            p[parent_id2],
                                                            c, point1, point2);

            

            Individual child2 = Individual::child2_twopoint(p[parent_id1], 
                                                            p[parent_id2], 
                                                            c, point1, point2);

            p.push_back(child1);
            p.push_back(child2);
        }

        merge_generations();   
    }

    void merge_generations() {
        p.sort_rev();
        p.erase(p.begin() + population_size, p.end());
    }



    void solve() {

        int mod = max_iter / 10;

        for(int i = 0; i < max_iter; ++i) {
            crossover();
            if(i % mod == 0 || i == max_iter - 1) {
              std::cout << "Epoch " << i + 1 << ": " << p[0] << std::endl;

            }
        } 
    }

};



int main() {
    // std::vector<Coordinate> cs{ {5, 3}, {1, 4},
    //                             {8, 8}, {8, 7},
    //                             {9, 7}, {1, 1}, 
    //                             {2, 0}, {7, 9}, 
    //                             {8, 7}, {10, 5} };

    
    // Solution s(5, 100, 10, cs);
    // s.solve();

    const int max_iter = 1000;

    int N;
    std::cin >> N;

    Solution s(N / 2 + 1, max_iter, N);
    s.solve();

    // s.crossover();
    // assert(s.p.size() == s.population_size);

    // s.crossover();
    // std::cout << s.p << std::endl;
     
    // std::cout << "Child 2\n";
    // std::cout << ch2;


    return 0;
}
