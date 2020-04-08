#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <numeric>

//  todo: need to range-v3 library

bool is_not_space(char c) {
    return c != ' ';
}

std::string trim_left(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), is_not_space));

    return s;
}

std::string trim_right(std::string s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());

    return s;
}

std::string trim(std::string s) {
    return trim_left(trim_right(std::move(s)));
}


enum class SEX {
    MALE, FEMALE
};

struct Person {
    SEX sex;
    std::string name;
};

bool is_female(const Person& person) {
    return person.sex == SEX::FEMALE;
}

bool is_not_female(const Person& person) {
    return !is_female(person);
}

std::string name(const Person& person) {
    return person.name;
}_LIBCPP_AVAILABILITY_THROW_BAD_OPTIONAL_ACCESS


// template <typename FilterFunction>
// std::vector<std::string> names_for(const std::vector<Person>& ps, FilterFunction filter) {
//     std::vector<std::string> results;
//     for (const Person& person : ps) {
//         if (filter(person)) {
//             results.push_back(name(person));
//         }
//     }
//     return results;
// }

std::vector<std::string> prepend(std::string head, std::vector<std::string> tail) {
    //  todo: need to optimize
    // std::reverse(std::begin(tail), std::end(tail));
    // tail.push_back(head);
    // std::reverse(std::begin(tail), std::end(tail));
    // 
    // return tail;

    std::vector<std::string> prependTail{head};
    std::copy(std::begin(tail), std::end(tail), std::back_inserter(prependTail));

    return prependTail;
}

std::vector<Person> tails(const std::vector<Person>& vs) {
    return std::move(std::vector<Person>{vs.begin() + 1, vs.end()});
}

//  loop
template <typename FilterFunction>
std::vector<std::string> names_for(const std::vector<Person>& people, FilterFunction filter) {
    if (people.empty()) {
        return {};
    }
    else {
        const auto head = people.front();
        const auto processed_tail = names_for(tails(people), filter);

        if (filter(head)) {
            return prepend(name(head), processed_tail);
        }
        else {
            return processed_tail;
        }
    }
}

//  todo : folding
// template <typename FilterFunction>
// std::vector<std::string> append_name_if(std::vector<std::string> previously_collected, const Person& pesron) {
//     if (filter(person))l {
//         previously_collected.push_back(name(person));
//     }
//     return previously_collected;
// }

// template <typename FilterFunction>
// std::vector<std::string> names_for(const std::vector<Person>& people, FilterFunction filter) {
//     auto append_name_if = [=](std::vector<std::string> previously_collected, const Person& person) {
//         if (filter(person)) {
//             previously_collected.push_back(name(person));
//         }
//         return previously_collected;
//     };

//     return std::accumulate(people.cbegin(), people.cend(), std::vector<std::string>{}, append_name_if);
// }

// template <typename FilterFunction, typename Iterator>
// std::vector<std::string> names_for(Iterator people_begin, Iterator people_end, FilterFunction filter) {
//     if (people_begin == people_end) {
//         return {};
//     }
//     else {
//         const auto head = *people_begin;
//         const auto processed_tail = names_for(people_begin + 1, people_end, filter);

//         if (filter(head)) {
//             return prepend(name(head), processed_tail);
//         }
//         else {
//             return processed_tail;
//         }
//     }
// }

template <typename FilterFunction, typename Iterator>
std::vector<std::string> names_for_helper(
    Iterator people_begin, Iterator people_end, FilterFunction filter,
    std::vector<std::string> previously_collected
) {
    if (people_begin == people_end) {
        return previously_collected;
    }
    else {
        const auto head = *people_begin;
        
        if (filter(head)) {
            previously_collected.push_back(name(head));
        }

        return names_for_helper(
            people_begin + 1, people_end, filter, std::move(previously_collected)
        );
    }
}

template <typename FilterFunction, typename Iterator>
std::vector<std::string> names_for(Iterator people_begin, Iterator people_end, FilterFunction filter) {
    return names_for_helper(people_begin, people_end, filter, {});
}


int main()
{
    //  trim
    std::cout << trim("     can I can?   ") << "TRIM\n\n";

    //  partitioning
    std::vector<Person> ps {
        Person{SEX::MALE, "Peter"},
        Person{SEX::FEMALE, "Martha"},
        Person{SEX::FEMALE, "Jane"},
        Person{SEX::MALE, "David"},
        Person{SEX::FEMALE, "Rose"},
        Person{SEX::MALE, "Tom"}
    };

    auto peopleNamePrint = [](std::vector<Person> ps){
        for (auto p : ps) {
        std::cout << p.name << ", ";
        }
        std::cout << std::endl << std::endl;
    };

    std::cout << "people list\n";
    peopleNamePrint(ps);

    std::vector<Person> ps2{ps};
    std::cout << "people list : female partioning\n";
    std::partition(ps2.begin(), ps2.end(), is_female);
    peopleNamePrint(ps2);

    std::vector<Person> ps3{ps};
    std::cout << "people list : male erase-remove\n";
    ps3.erase(
        std::remove_if(ps3.begin(), ps3.end(), is_not_female),
        ps3.end()
    );
    peopleNamePrint(ps3);

    std::vector<Person> females;
    std::cout << "name list : female copy => get only female's name\n";
    std::copy_if(ps.cbegin(), ps.cend(), std::back_inserter(females), is_female);
    std::vector<std::string> names(females.size());
    std::transform(females.cbegin(), females.cend(), names.begin(), name);
    for (auto name : names) {
        std::cout << name << ", ";
    }
    std::cout << std::endl << std::endl;

    std::vector<Person> seperated(ps.size());
    std::cout << "name list : seperate\n";
    const auto last = std::copy_if(ps.cbegin(), ps.cend(), seperated.begin(), is_female);
    std::copy_if(ps.cbegin(), ps.cend(), last, is_not_female);
    peopleNamePrint(seperated);

    // std::cout << "recursive filtered name using loop\n";
    // for (auto name : names_for(ps, is_female)) {
    //     std::cout << name << ", ";
    // }
    // std::cout << std::endl << std::endl;

    std::cout << "recursive filtered name using recursion\n";
    for (auto name : names_for(std::begin(ps), std::end(ps), is_female)) {
        std::cout << name << ", ";
    }
    std::cout << std::endl << std::endl;

    //  folding
    std::cout << "recursive filtered name using folding\n";
    for (auto name : names_for(ps, is_female)) {
        std::cout << name << ", ";
    }
    std::cout << std::endl << std::endl;

    return 0;
}