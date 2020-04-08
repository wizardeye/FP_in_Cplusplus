

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>
#include <fstream>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <map>


//  4.1
class GreaterThan {
public:
    GreaterThan(int value): _value{value} {}
    bool operator()(int arg) const {
        return arg > _value;
    }

private:
    int _value;
};


//  4.3
template <typename F, typename T>
class PartialApplication {
public:
    PartialApplication(F fn, T secondArg): _fn{fn}, _secondArg{secondArg} {}

    template <typename A>
    auto operator()(A&& firstArg) const {
            return _fn(std::forward<A>(firstArg), _secondArg);
        }

private:
    F _fn;
    T _secondArg;
};


//  4.4
template <typename F, typename T>
PartialApplication<F, T> bind2nd(F&& fn, T&& secondArg) {
    return PartialApplication<F, T>(std::forward<F>(fn), std::forward<T>(secondArg));
}


//  4.10
enum class NameFormat {
    NameOnly, FullName
};

struct Person {
    std::string name;
    std::string surName;

    void print(std::ostream& out, NameFormat format) const {
        if (format == NameFormat::NameOnly) {
        out << "Person::print ==> " << name << '\n';
        }
        else if (format == NameFormat::FullName) {
            out << "Person::print ==> " << name << ' ' << surName << '\n';
        }
    }
};

void Print_Person(const Person& person, std::ostream& out, NameFormat format) {
    if (format == NameFormat::NameOnly) {
        out << person.name << '\n';
    }
    else if (format == NameFormat::FullName) {
        out << person.name << ' ' << person.surName << '\n';
    }
}


//  4.12 currying
bool greater(double first, double second) {
    return first > second;
}

auto greater_curried(double first) {
    return [first](double second){
        return first > second;
    };
}


//  make_curried
template <typename Function, typename ...CapturedArgs>
class curried {
public:
    curried(Function function, CapturedArgs... args)
        : m_function(function)
        , m_captured(std::make_tuple(args...))
    {
    }

    curried(Function function, std::tuple<CapturedArgs...> args)
        : m_function(function)
        , m_captured(args)
    {
    }

    template <typename ...NewArgs>
    auto operator()(NewArgs&& ...args) const
    {
        auto new_args = std::make_tuple(std::forward<NewArgs>(args)...);
        auto all_args = std::tuple_cat(m_captured, std::move(new_args));

        if constexpr(std::is_invocable_v<Function, CapturedArgs...,
                                         NewArgs...>) {
            return std::apply(m_function, all_args);

        } else {
            return curried<Function, CapturedArgs..., NewArgs...>(
                m_function, all_args);
        }
    }

private:
    Function m_function;
    std::tuple<CapturedArgs...> m_captured;
};


// Needed for pre-C++17 compilers
template <typename Function>
curried<Function> make_curried(Function &&f)
{
    return curried<Function>(std::forward<Function>(f));
}



class callable_test {
public:
    template <typename T1, typename T2, typename T3>
    auto operator() (T1 x, T2 y, T3 z) const
    {
        return x + y + z;
    }

    template <typename T1, typename T2>
    auto operator() (T1 x, T2 y) const
    {
        return x + y;
    }

};

//  4.13
auto Curried_Print_Person(const Person& person) {
    return [&](std::ostream& out) {
        return [&](NameFormat format) {
            Print_Person(person, out, format);
        };
    };
};


//  4.14
class Result {
public:
    Result(const std::string& s): _result{s} {}
    std::string getResult() { return _result; }

private:
    std::string _result;
};

class Connection {};
class Session {};

Result query(Connection& connection, Session& session, const std::string& table_name, const std::string& filter) {
    return Result("DB query result => ok");
}


//  4.3
std::vector<std::string> getWords(const std::string& text) {
    std::ifstream ifs{text};
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

template <typename T>                                       //  word frequency count
std::unordered_map<T, unsigned int> count_occurences(const std::vector<T>& items) {
    std::unordered_map<T, unsigned int> ms;
    for (auto item : items) {
        ms[item]++;
    }

    return ms;
}

template <typename C, typename P1, typename P2>             //  reverse word-frequency pair
std::vector<std::pair<P2, P1>> reverse_pairs(const C& collection) {
    std::vector<std::pair<P2, P1>> vs;
    vs.reserve(collection.size());

    for (auto c : collection) {
        P1 k = c.first;
        P2 v = c.second;
        vs.push_back(std::pair(v, k));
    }

    return vs;
}

template <typename C>                                       //  sort by fequency
std::vector<C>& sort_by_frequency(std::vector<C>& vs) {
    std::sort(std::begin(vs), std::end(vs), 
        [](const C& c1, const C& c2){ 
            if (c1.first == c2.first) {
                return c1.second < c2.second;
            }
            else {
                return c1.first > c2.first;
            }
         });

    // std::sort(std::begin(vs), std::end(vs), std::less<>());

    return vs;
}

template <typename C>                                       //  print
void print_pairs(const C& collection) {
    int i = 0;
    for (const auto& c : collection) {
        std::cout << i << ") " << "freq: " << c.first << ", word: " << c.second << '\n';
        i++;
    }
}


//  4.4
void to_upper(std::string& s) {
    std::transform(std::begin(s), std::end(s), std::begin(s), toupper);
}

void pointer_to_upper(std::string* s) {
    if (s) to_upper(*s);
}

void vector_to_upper(std::vector<std::string>& vs) {
    for (auto& s : vs) {
        to_upper(s);
    }
}

void map_to_upper(std::map<int, std::string>& ms) {
    for (auto& s : ms) {
        to_upper(s.second);
    }
}


//  4.17
template <typename Function>
auto pointer_lift(Function f) {
    return [f](auto* item){
        if (item) { f(*item); }
    };
}


template <typename Function>
auto collection_lift(Function f) {
    return [f](auto& items) {
        for (auto& item : items) {
            f(item);
        }
    };
}


int main()
{
    std::cout << "[ex 4.1]\n";
    GreaterThan gt_42{42};
    std::cout << "1 is greater than 42? ==> " << std::boolalpha << gt_42(1) << '\n';
    std::cout << "50 is greater than 42? ==> " << std::boolalpha << gt_42(50) << '\n';

    std::cout << "\n[ex 4.3]\n";
    using Fn = std::function<bool(int, int)>;
    // auto greater = [](int first, int second){ return first > second; };
    auto pa_gt_42 = PartialApplication<Fn, int>(std::greater<int>(), 42);
    std::cout << "[PA] 1 is greater than 42? ==> " << std::boolalpha << pa_gt_42(1) << '\n';
    std::cout << "[PA] 50 is greater than 42? ==> " << std::boolalpha << pa_gt_42(50) << '\n';
    
    std::cout << "\n[ex 4.4]\n";
    auto bind2nd_gt_42 = bind2nd(std::greater<int>(), 42);
    std::cout << "[bind2nd] 1 is greater than 42? ==> " << std::boolalpha << bind2nd_gt_42(1) << '\n';
    std::cout << "[bind2nd] 50 is greater than 42? ==> " << std::boolalpha << bind2nd_gt_42(50) << '\n';

    std::cout << "\n[ex 4.5]\n";
    std::vector<int> xs{3,9,6,8,2,10,1,15};
    std::stable_partition(std::begin(xs), std::end(xs), bind2nd(std::greater<int>(), 6));
    for (auto x : xs) {
        std::cout << x << ", ";
    }
    std::cout << std::endl;

    std::cout << "\n[ex 4.6]\n";
    std::vector<double> degrees{0,30,45,60};
    std::vector<double> radians(degrees.size());
    std::transform(std::begin(degrees), std::end(degrees), std::begin(radians),
        bind2nd(std::multiplies<double>(), M_PI/180));
    for (auto r : radians) {
        std::cout << r << ", ";
    }
    std::cout << std::endl;

    std::cout << "\n[ex 4.7 ~ 4.8]\n";
    auto bound = std::bind(std::greater<int>(), 6, 10);
    std::cout << "[bind] 8 is 6 ~ 10? ==> " << std::boolalpha << bound(8) << std::endl;

    auto is_greater_than_10 = std::bind(std::greater<int>(), std::placeholders::_1, 10);
    std::cout << "[placeholders::_1] 8 > 10?  ==> " << std::boolalpha << is_greater_than_10(8) << std::endl;

    auto is_less_than_10 = std::bind(std::greater<int>(), 10, std::placeholders::_1);
    std::cout << "[placeholders::_1] 8 < 10?  ==> " << std::boolalpha << is_less_than_10(8) << std::endl;

    std::cout << "\n[ex 4.9]\n";
    std::vector<int> scores(xs);
    std::sort(std::begin(scores), std::end(scores), std::bind(std::greater<int>(), std::placeholders::_2, std::placeholders::_1));
    for (auto s : scores) {
        std::cout << s << ", ";
    }
    std::cout << std::endl;

    std::cout << "\n[ex 4.10]\n";
    std::vector<Person> ps {
        {"Peter", "idsfk skdjfksf"},
        {"Nartha", "anna dsfsdf"},
        {"Jane", "ostin sadfsd"},
        {"David", "dsfsf asdfasdf"},
        {"Rose", "mammmdsfm asdfsad"},
        {"Tom", "demarco"}
    };
    
    std::for_each(std::cbegin(ps), std::cend(ps), 
        std::bind(Print_Person, std::placeholders::_1, std::ref(std::cout), NameFormat::NameOnly));
    
    std::ofstream file{"test.txt"};
    std::for_each(std::cbegin(ps), std::cend(ps), 
        std::bind(Print_Person, std::placeholders::_1, std::ref(file), NameFormat::FullName));
    
    std::cout << "\n[ex 4.11]\n";
    std::for_each(std::cbegin(ps), std::cend(ps),
        std::bind(&Person::print, std::placeholders::_1, std::ref(std::cout), NameFormat::NameOnly));

    auto is_greater_than_10_using_lambda = [](double value) {
        return std::greater<double>()(value, 10);
    };
    std::cout << "[labmda] 8 > 10?  ==> " << std::boolalpha << is_greater_than_10_using_lambda(8) << std::endl;

    auto is_less_than_10_using_lambda = [](double value){
        return std::greater<double>()(10, value);
    };
    std::cout << "[labmda] 8 < 10?  ==> " << std::boolalpha << is_less_than_10_using_lambda(8) << std::endl;

    std::sort(std::begin(scores), std::end(scores), [](double v1, double v2){
        return std::greater<double>()(v1, v2);
    });
    std::cout << "[labmda] scores dec ordering\n";
    for (auto s : scores) {
        std::cout << s << ", ";
    }
    std::cout << std::endl;

    std::for_each(std::cbegin(ps), std::cend(ps), [](const Person& person){
        Print_Person(person, std::cout, NameFormat::FullName);
    });
    
    std::ofstream file2{"test2.txt"};
    std::for_each(std::cbegin(ps), std::cend(ps), [&file2](const Person& person){
        Print_Person(person, file2, NameFormat::NameOnly);
    });


    std::cout << "\n[ex 4.12]\n";
    std::cout << "2 > 3 => " << greater(2, 3) << '\n';
    std::cout << "[curried] 2 > 3 => " << greater_curried(2)(3) << '\n';
    std::cout << '\n';

    std::cout << "\n[ex 4.13]\n";
    Curried_Print_Person(ps[0])(std::cout)(NameFormat::FullName);
    
    //  todo    why not? fail to compile "make_curried"
    // auto Curried_Print_Person2 = make_curried(Print_Person);
    // Curried_Print_Person2(ps[1])(std::cout)(NameFormat::FullName);
    

    std::cout << "[ex 4.14]\n";
    auto table = "Movies";
    auto filter = "Name = \"Sintel\"";
    Connection localConnection, remoteConnection;
    Session session;

    auto fullQueryResults = query(localConnection, session, table, filter);
    std::cout << "full query request... " << fullQueryResults.getResult() << '\n';

    auto curriedQuery = make_curried(query);    //  apply curring

    auto localQuery = curriedQuery(localConnection);
    
    auto localQueryResults = localQuery(session, table, filter);
    std::cout << "local query request... " << localQueryResults.getResult() << '\n';

    auto moviesQuery = curriedQuery(localConnection, session, table);
    auto moviesQueryResults = moviesQuery(filter);
    std::cout << "movies query request... " << moviesQueryResults.getResult() << '\n';

    //  4.3
    std::cout << "\n[chap 4.3] word frequency print.\n";
    auto words = getWords("helloworld.cpp");
    auto counts = count_occurences<std::string>(words);
    auto reversePairs = reverse_pairs<std::unordered_map<std::string, unsigned int>, std::string, unsigned int >(counts);
    auto sortedPairs = sort_by_frequency<std::pair<unsigned int, std::string>>(reversePairs);
    print_pairs(sortedPairs);

    //  4.16
    std::cout << "\n[list 4.16] to_upper function for string collection.\n";
    std::string str{"pointer to upper\n"};
    pointer_to_upper(&str);
    std::cout << str << '\n';
    
    std::vector<std::string> liftVs{"test", "vector to upper\n"};
    vector_to_upper(liftVs);
    std::for_each(std::begin(liftVs), std::end(liftVs), 
        [](const auto& s){std::cout << s << '\n';
    });

    std::map<int, std::string> liftMs{{1, "test"}, {2, "map to upper\n"}};
    map_to_upper(liftMs);
    std::for_each(std::begin(liftMs), std::end(liftMs), 
        [](const auto& s){std::cout << s.first << "] " << s.second << '\n';
    });

    //  4.17
    std::cout << "\n[list 4.17] function lifting\n";
    std::string str2{"pointer to upper by generic lift.\n"};
    
    // pointer_lift<std::function<void(std::string&)>>(to_upper)(&str2);
    pointer_lift(to_upper)(&str2);
    std::cout << str2;
    
    std::vector<std::string> vs2{"vector", "to", "upper", "by", "generic", "lift"};
    collection_lift(to_upper)(vs2);
    std::for_each(std::begin(vs2), std::end(vs2), [](auto v){std::cout << v << ", ";});

    return 0;
}