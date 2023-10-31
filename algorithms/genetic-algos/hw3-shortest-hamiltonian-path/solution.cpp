#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cmath>
#include <random>
#include <algorithm>


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

    Individual(const std::vector<int>& path,
               const Coordinates& c)
        : path(path) {
        
        fitness = Individual::calc_fitness(path, c);
    }

    Individual(const Coordinates& c) {
        rand_init(c.size());
        fitness = Individual::calc_fitness(path, c);
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

    static double calc_fitness(const std::vector<int>& path,
                               const Coordinates& c) {
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
    c.rand_init(10);
    std::cout << c;

    Population p;
    p.rand_init(10, c);
    std::cout << p;

    return 0;
}