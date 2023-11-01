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

const int PLANE_LIMIT = 100;

struct Coordinates: public std::vector<Coordinate> {
    static inline const int PLANE_LIMIT = 100;    

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
    const Coordinates& c;

    Individual(std::vector<int> path,
               const Coordinates& c)
        : path(path),
          c(c) {
        
        fitness = calc_fitness();
    }

    Individual(const Coordinates& c): c(c) {
        rand_init(c.size());
        fitness = calc_fitness();
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

    /// @brief one point crossover
    static Individual child1(const Individual& i1, 
                             const Individual& i2,
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
        
        
        return Individual(res_path, i1.c);
    }
    /// @brief one point crossover 
    static Individual child2(const Individual& i1, 
                             const Individual& i2, 
                             const int point) {
    
        return child1(i2, i1, point);
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

    double calc_fitness() {
        if(path.size() < 2) {
            return 0;
        }

        int res{0};

        for(int i = 0; i < path.size() - 1; ++i) {
            res += c[path[i]].distance(c[path[i + 1]]);
        }

        return res;
    }

};

struct Population: public std::vector<Individual> {
    void rand_init(const int size, Coordinates& c) {
        for(int i = 0; i < size; ++i) {
            (*this).push_back(Individual(c));
        }
    }
};



int main() {
    Coordinates c;
    c.rand_init(5);
    std::cout << c;

    Population p;
    p.rand_init(10, c);
    std::cout << p;

    int cross_point = 2;

    Individual i1({1, 2, 3, 4, 5}, c);
    Individual i2({5, 4, 3, 2, 1}, c);

    Individual ch1 = Individual::child1(i1, i2, cross_point);
    Individual ch2 = Individual::child2(i1, i2, cross_point);

    std::cout << i1;
    std::cout << i2;
    std::cout << "Cross point " << cross_point << std::endl;

    std::cout << "Child 1\n";
    std::cout << ch1;

     
    std::cout << "Child 2\n";
    std::cout << ch2;

     


    return 0;
}