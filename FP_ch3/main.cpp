#include <iostream>
#include <mutex>
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <tuple>
#include <functional>
#include <range/v3/all.hpp>


template <typename Collection>
void print_item(Collection c) {
    std::for_each(std::begin(c), std::end(c), [](auto item){
        std::cout << item << '\n';
    });
}


enum class SEX {
    MALE, FEMALE
};

struct Person {
    SEX sex;
    std::string name;
    std::string teamName;
};


std::string team_for_person(Person p) {
    return p.teamName;
}

std::string name(Person p) {
    return p.name;
}

//  list 7.1
template <typename Persons, typename F>
void group_by_team(Persons& persons, F team_for_person, const std::vector<std::string>& teams) {
    auto begin = std::begin(persons);
    const auto end = std::end(persons);

    for (const auto& team : teams) {
        begin = std::partition(begin, end, [&](const auto& person){
            return team == team_for_person(person);
        });
    }
}


std::vector<std::string> read_text() {
    return {
      "phone", "jaskfjk", "dshiq", "unicorn", "monster", "truck", "pizza", "cheeze", "coke", "side", "west",
      "phone", "jaskfjk", "dshiq", "unicorn", "monster", "truck", "pizza", "cheeze", "coke", "side", "west"
    };
}



int main() {
    //  intro
    std::cout << "[ex intro]\n";

    std::vector<Person> ps = {
        Person{SEX::FEMALE, "twice", "B-team"},
        Person{SEX::MALE, "mc sniper", "A-team"},
        Person{SEX::FEMALE, "Madona", "C-team"},
        Person{SEX::MALE, "earth boy", "A-team"},
        Person{SEX::FEMALE, "Chanee So", "B-team"},
        Person{SEX::FEMALE, "shakra", "B-team"},
        Person{SEX::MALE, "MJ", "A-team"}
    };

    std::vector<std::string> ts = {
        "A-team", "B-team", "C-team"
    };

    std::cout << "==============================\n";
    std::cout << "(no-grouping)\n\n";
    std::for_each(std::begin(ps), std::end(ps), [](Person p){
        std::cout << p.name << "'team : " << p.teamName << '\n';
    });

    group_by_team<std::vector<Person>, std::function<std::string(Person)>>(ps, team_for_person, ts);

    std::cout << "\n==============================\n";
    std::cout << "(grouping by team name)\n\n";
    std::for_each(std::begin(ps), std::end(ps), [ts](Person p){
        std::cout << p.name << "'team : " << p.teamName << '\n';
    });

    
    //  7-1
    std::cout << "\n[ex 7-1] filter & take\n";
    auto is_female = [](const Person& p){
        return p.sex == SEX::FEMALE;
    };

    std::vector<std::string> names = ps
    | ranges::views::filter(is_female)
    | ranges::views::transform(name);

    std::cout << "(female name filtering)\n\n";
    print_item(names);
    
    
    std::vector<std::string> namesTake3 = ps
    | ranges::views::filter(is_female)
    | ranges::views::transform(name)
    | ranges::views::take(3);
    
    std::cout << "\n\n(female name filtering & take (3)\n\n";
    print_item(namesTake3);
    
    
    //  7-3
    std::cout << "\n[ex 7-3] sort & unique\n";
    
    std::vector<std::string> words = read_text()
    | ranges::actions::sort
    | ranges::actions::unique;
    
    print_item(words);
    
    std::vector<std::string> wordsLval = read_text();
    
    std::cout << "\n==============================\n";
    std::cout << "(actions using L-val)\n\n";
    
    wordsLval |= ranges::actions::sort | ranges::actions::unique;
    
    print_item(wordsLval);
    
    
    
    
    
    
    return 0;
}
