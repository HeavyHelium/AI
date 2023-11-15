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
#include <unordered_map>
#include <functional>
#include <cstddef>



std::mt19937 rng(std::random_device{}()); // Set Mersenne Twister


/// Utility functions

bool approx_equal(const double d1,
                  const double d2, 
                  const double epsilon=1e-2) {
    return std::abs(d1 - d2) < epsilon;
}

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
    double x{-1};
    double y{-1};

    double distance(const Coordinate& other) const {
        return std::sqrt(std::pow((x - other.x), 2) +
                         std::pow((y - other.y), 2));
    }

    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    } 

    friend std::ostream& operator<<(std::ostream& os,
                                    const Coordinate& c) {
        return os << "(" << c.x << ", " << c.y << ")"; 
    }
}; 



struct Coordinates: public std::vector<Coordinate> {
    static inline const int PLANE_LIMIT = 900;

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
        return std::sqrt(2 * temp) * (size() - 1); 
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
    static const inline double MUTATION_PROB = 0.8;

    double unfitness{0};
    std::vector<int> path;

    Individual(const std::vector<int>& path,
               const Coordinates& c)
        : path(path) {

        calc_unfitness(c);
    }

    Individual(const Coordinates& c) {
        rand_init(c.size());
        calc_unfitness(c);
    }

    int size() const {
        return path.size();
    }

    /// @brief random swap mutation 
    void mutate_swap(const Coordinates& c) {
        const int limit = size() - 1;
        int id1 = gen_number(limit);
        int id2 = gen_number(limit);

        std::swap(path[id1], path[id2]);
        calc_unfitness(c);
    } 

    /// @brief inversion mutation
    void mutate_inv(const Coordinates& c) {
        int i = gen_number(size() - 1);
        int j = gen_number(size() - 1);

        if(i > j) {
            std::swap(i, j);
        }        
    
        std::reverse(path.begin() + i, path.begin() + j + 1);    

        calc_unfitness(c);
    }

    ///@bried two point crossover
    static Individual child1_twopoint(const Individual& i1,
                                      const Individual& i2, 
                                      const Coordinates& c,
                                      const int i, 
                                      const int j) {

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

        return Individual(res_path, c);
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

        return Individual(res_path, c);
    }
    /// @brief one point crossover 
    static Individual child2_onepoint(const Individual& i1, 
                                      const Individual& i2, 
                                      const Coordinates& c,
                                      const int point) {
    
        return child1_onepoint(i2, i1, c, point);
    }


    friend std::ostream& operator<<(std::ostream& os,
                                    const Individual& id) {
        for(int i = 0; i < id.path.size(); ++i) {
            os << id.path[i] << " ";
        }
        os << '\n';
        os << "unfitness: " << id.unfitness;
        return os << std::endl;
    }

private:
    
    void rand_init(const int size) {
        path.resize(size);
        for(int i = 0; i < size; ++i) {
            path[i] = i;
        }
        std::random_shuffle(path.begin(), path.end());
    }

    void calc_unfitness(const Coordinates& c) {
        if(path.size() < 2) {
            unfitness = 0;
            return;
        }

        double res{0};

        for(int i = 0; i < path.size() - 1; ++i) {
            res += c[path[i]].distance(c[path[i + 1]]);
        }

        unfitness = res;
    }

};

struct Population: public std::vector<Individual> {
    void rand_init(const int size, const Coordinates& c) {
        for(int i = 0; i < size; ++i) {
            (*this).push_back(Individual(c));
        }
    }

    void sort(int beg, int end) {
        std::sort(this->begin() + beg, this->begin() + end, 
                  [](const Individual& i1,
                     const Individual& i2) {
                     return i1.unfitness < i2.unfitness; }); // sort the population by fitness
    }
};

struct Solution {
    static const inline double ELITISM_PROP = 0.3;
    static const int STOP_CNT = 5;
    
    Coordinates c;
    
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
          crsvr(crsvr), c(vertex_cnt) { // random initialization of coordiantes

        p.rand_init(population_size, c);
    }

    Solution(const int population_size, 
             const int max_iter, 
             const Coordinates& c, 
             Crossover crsvr = Crossover::TWO_POINT)
        : max_iter(max_iter),
          path_len(c.size()),
          population_size(population_size), 
          c(c), crsvr(crsvr) {
        
        p.rand_init(population_size, c);
    }


    const Population& population() const {
        return p;
    }

    void solve() {
        const int mod = max_iter / 10;
        // int reps = 0;
        // double prev = INT_MIN;

        for(int i = 0; i < max_iter /*&& reps < Solution::STOP_CNT*/; ++i) {
            crossover();
            if(i % mod == 0 || 
               i == max_iter - 1 /*||
               reps == Solution::STOP_CNT - 1*/) {
              
              std::cout << "Epoch " << i + 1 << //": " << p[0] << std::endl;
                           ": " << p[0].unfitness << '\n';
            }
            // if(approx_equal(prev, p[0].unfitness)) {
            //     ++reps;
            // } else {
            //     reps = 1;
            //     prev = p[0].unfitness;
            // }
        }
        std::cout << std::endl; 
    }

private:
        double sum_fitness() const {
        const double max_cost = c.max_cost();
        return std::accumulate(p.begin(), 
                               p.end(), 0.0, 
                               [&max_cost](double a, const Individual& i) { 
                                           return a + max_cost - i.unfitness;});
    }

    /// @brief Roulette Wheel Selection
    std::vector<int> selection() {
        int id{-1};
        const double sum = sum_fitness();
        const double max_cost = c.max_cost();
       
        double prob_prev_cuml{0};
        double prob_temp{0};

        int sel_cnt = selection_cnt();
        
        p.sort(0, population_size);

        std::vector<double> probs(population_size);
        std::transform(p.begin(), p.end(), probs.begin(), 
                       [&sum, &prob_prev_cuml, &max_cost](const Individual& i) { 
                                                            prob_prev_cuml += (max_cost - i.unfitness) / sum;
                                                            return prob_prev_cuml;}); // CDF calculation 
        std::vector<int> selected;
        int cnt = 0;

        while(cnt < sel_cnt) {
            prob_temp = gen_prob();
            id = 0;
            while(prob_temp > probs[id] && prob_temp < probs.back()) { 
                ++id;
            }
            selected.push_back(id);
            ++cnt;
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


            if(crsvr == Crossover::ONE_POINT) {
                   Individual child1 = Individual::child1_onepoint(p[parent_id1], 
                                                                   p[parent_id2],
                                                                   c, point1);   

                    Individual child2 = Individual::child2_onepoint(p[parent_id1], 
                                                                    p[parent_id2], 
                                                                    c, point1);
                    p.push_back(child1);
                    p.push_back(child2);
            } else {
                point2 = gen_number(path_len);

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
            
        }

        mutate_children();
        form_new_generation();   
    }

    void mutate_children() {
        for(auto iter = p.begin() + population_size; iter < p.end(); ++iter) {
            if(gen_prob() < Individual::MUTATION_PROB) {
                iter->mutate_inv(c);
            }
        }
    }

    void form_new_generation() {
        int remove_cnt = selection_cnt();
        int parents_rem = population_size - remove_cnt;

        assert(p.size() == population_size + remove_cnt);

        p.sort(0, population_size); // remove unfittest remove_cnt parents
        
        p.erase(p.begin() + population_size - remove_cnt, 
                p.begin() + population_size);

        p.sort(0, population_size);

        assert(p.size() == population_size);
    }


    int selection_cnt() const {
        int temp = population_size * (1 - Solution::ELITISM_PROP);
        return temp % 2 == 0 ? temp : temp + 1; 
    }

    Population p;
};


int main(int argc, char** argv) {
    const int max_iter = 1000;

    if(argc > 2 || (argc == 2 && argv[1] != std::string("test"))) {
        std::cout << "Usage: <program name> <optional: test>" << std::endl;
        return -1;
    } else if(argc == 2) {
        std::vector<Coordinate> cs{ {500.000190032, 499.999714054}, // translated so as to be non-negative
                                    {883.4580000000001, 499.999391244},
                                    {472.9794, 217.24200000000002}, 
                                    {835.751, 230.423}, 
                                    {569.4331, 253.22}, 
                                    {668.521, 531.4012}, 
                                    {820.35, 339.1}, 
                                    {679.933, 181.969}, 
                                    {992.671, 368.437}, 
                                    {612.198, 389.43899999999996}, 
                                    {806.3199999999999, 391.90999999999997}, 
                                    {717.343, 52.911}, };


        std::vector<std::string> cities{"Aberystwyth", "Brighton",
                                        "Edinburgh", "Exeter",
                                        "Glasgow", "Inverness",
                                        "Liverpool", "London",
                                        "Newcastle", "Nottingham",
                                        "Oxford", "Stratford"};

        Solution s(110, 15000, cs, Crossover::ONE_POINT); // one point better escapes local minimum
        s.solve();
        for(int i = 0; i < cs.size(); ++i) {
            std::cout << cities[((s.population()[0]).path)[i]] << std::endl;
        }

    } else {
        int N;
        std::cin >> N;

        Solution s(10 * N, max_iter, N); // default twopoint crossover
        s.solve();
    }


    return 0;
}
