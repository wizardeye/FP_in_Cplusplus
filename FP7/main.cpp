#include <iostream>
#include <mutex>
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <tuple>
#include <string>
#include <functional>
#include <fstream>
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

std::vector<std::string> read_text2(const std::string& fileName) {
    std::ifstream ifs{fileName};
    if (ifs.fail() || ifs.eof()) {
        std::cout << "word file open fail.\n";
        return {};
    }

    std::vector<std::string> words;
    std::string word;

    while (ifs >> word) {
        words.push_back(word);
    }

    return words;
}


//  list 7.4
template <typename Range>
void write_top_10(const Range& xs) {
    auto items = ranges::views::zip(xs, ranges::views::ints(1, ranges::unreachable))
    | ranges::views::transform([](const auto& pair){ return std::to_string(pair.second) + " " + pair.first;})
    | ranges::views::take(10);
    
    for (const auto& item : items) {
        std::cout << item << std::endl;
    }
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
    
    
    //  7-4
    std::cout << "\n[ex 7-4] unreachable sentinel\n";
    
    std::vector<std::string> xs {
        "x-ray", "for higher", "double target", "prince of prince", "killing machine", "I am a boy. you are a girl?",
        "Gandi", "kamasutra", "zoo zoo", "chu webs", "bagging tackle", "carry on me", "boxer", "millionare boy", "god father"
    };
    
    write_top_10(ranges::views::all(xs));
    
    
    //  7-5
    std::cout << "\n[ex 7-5 ~ 7-6] word count\n";
    auto string_to_lower = [](const std::string& s){
        return s | ranges::views::transform(tolower);
    };
    
    auto string_only_alnum = [](const std::string& s){
        auto ret = s | ranges::views::filter(isalnum);
        return std::string(s);
    };
    
    auto is_empty_string = [](const std::string& s){return s.empty();};
    
    std::cout << "(input your word)\n";
    
    std::vector<std::string> xs2 {
        "x-ray", "for higher", "double target", "prince of prince", "killing machine", "I am a boy. you are a girl?",
        "Gandi", "kamasutra", "zoo zoo", "chu webs", "bagging tackle", "carry On ME", "bOxer", "millionare boy", "god father",
        "Gandi", "kamasutra", "zoo zoo", "chu webs", "bagging tackle", "cARry On me", "boxer", "mIllionAre Boy", "god father",
        "Gandi", "kamasutra", "zoo zoo", "CHU weBs", "CHU weBs", "CHU weBs"
    };
    
//    std::vector<std::string> words2 = ranges::istream_range<std::string>(std::cin)
    std::vector<std::string> words2 = xs2
    | ranges::views::transform(string_to_lower)
    | ranges::views::transform(string_only_alnum)
//    | ranges::views::remove_if(&std::string::empty);
    | ranges::views::remove_if(is_empty_string);

    
    words2 |= ranges::actions::sort;    //  for sorting, convert ranges to vector
    
    const auto results = words2
    | ranges::views::group_by(std::equal_to<>())
    | ranges::views::transform([](const auto& group){
        const auto begin = std::begin(group);
        const auto end = std::end(group);
        const auto count = ranges::distance(begin, end);
        const auto word = *begin;
        
        return std::make_pair(count, word);
    })
    | ranges::to_vector | ranges::actions::sort;
    
    for (auto value : results
         | ranges::views::reverse
         | ranges::views::take(10)) {
        std::cout << value.first << " " << value.second << std::endl;
    }
    
    
    return 0;
}
