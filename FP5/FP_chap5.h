#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <list>
#include <cassert>
#include <array>
#include <memory>

class Movie {
public:
    Movie(std::string name) : _name{name} {}

    double average_score() {
        return std::accumulate(std::begin(_scores), std::end(_scores), 0,
            [](auto s1, auto s2){return s1 + s2;}) / _scores.size();
    }

    void addScore(int score) { _scores.push_back(score); }


private:
    std::string _name;
    std::list<int> _scores;
    std::size_t _scores_size;   //  tightly bind with _scores
};


double max(const std::vector<double>& numbers) {
    assert(!numbers.empty());
    auto result = std::max_element(std::cbegin(numbers), std::cend(numbers));
    // std::cerr << "Maximum is: " << *result << '\n';  //  impure statement

    return *result;
}


//  Person
class Person {
public:
    Person withName(const std::string& name) const & {
        Person result(*this);
        result._name = name;

        return result;
    }

    Person withName(const std::string& name) && {
        Person result(std::move(*this));
        result._name = name;

        return result;
    }


private:
    std::string _name;
};




int main()
{
    Movie movie{"gone with the wind"};
    movie.addScore(98);
    movie.addScore(88);
    movie.addScore(95);
    movie.addScore(94);
    movie.addScore(72);
    
    std::cout << "[ex5.1]\n";
    std::cout << "movie's average score: "  << movie.average_score() << '\n';

    std::cout << "\n[ex5.2]\n";
    auto sum_max = max({1}) + max({1,2}) + max({1,2,3});
    std::cout << "max({1}) + max({1,2}) + max({1,2,3}): " << sum_max << '\n';

    return 0; 
}