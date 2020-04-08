#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <initializer_list>


enum class SEX {
    MALE, FEMALE
};

struct Person {
    SEX sex;
    std::string name;
    int age;    //  todo: age --> method impl
    std::string teamName;
};

class CPerson {
public:
    CPerson(SEX sex, std::string name, int age, std::string teamName):
        _sex{sex}, _name{name}, _age{age}, _teamName{teamName} {}

    SEX getSex() {return _sex;}
    std::string getName() {return _name;}
    std::string getTeamName() {return _teamName;}
    
    //  const, non-const overloading
    int getAge() const {return _age;}
    // int getAge() {return _age;}

private:
    SEX _sex;
    std::string _name;
    std::string _teamName;
    int _age = 0;
};


struct Car {
    std::string model;
    int age;
};

int ask() { return 42; }

typedef decltype(ask)* function_ptr;

class convertable_to_function_ptr {
public:
    operator function_ptr() const {
        return ask;
    }
};

bool older_than_42(const Person& person) {
    return person.age > 42;
}

class older_than {
public:
    older_than(int limit) : _limit{limit} {}

    bool operator() (const Person& person) const {
        return person.age > _limit;
    }

private:
    int _limit;
};


template <typename T>
class older_than2 {
public:
    older_than2(int limit) : _limit{limit} {}

    bool operator() (const T& object) const {
        return object.age > _limit;
    }

private:
    int _limit;
};


class older_than3 {
public:
    older_than3(int limit) : _limit{limit} {}

    template <typename T>
    bool operator() (T&& object) const {
        return std::forward<T>(object).age > _limit;
    }

private:
    int _limit;
};

//  todo    do not support const
class older_than3_using_class {
public:
    older_than3_using_class(int limit) : _limit{limit} {}

    template <typename T>
    bool operator() (T&& object) const {    //  todo: const object
        // return std::forward<T>(object).getAge() > _limit;
        // return std::forward<decltype(object)>(object).getAge() > _limit;
        return object.getAge() > _limit;
    }

private:
    int _limit;
};


class Company {
public:
    std::string team_name_for (const Person& employee) const {
        return "";
    }

    int count_team_members (const std::string& team_name) const {
        return std::count_if (_employees.cbegin(), _employees.cend(),
            [this, &team_name](const Person& employee){
                return team_name_for (employee) == team_name;
            });
    }

private:
    std::vector<Person> _employees;
};

//  3.1.1   perfect forwarding
template <typename Object, typename Function>
decltype(auto) call_on_object_by_val(Object object, Function function) {
    return function(object);
}

//  todo
template <typename Object, typename Function>
decltype(auto) call_on_object_by_ref(Object&& object, Function function) {
    return function(std::forward<Object>(object));
}


int main()
{
    //  3.1.1
    auto fnTwiceByVal = [](std::vector<int> vs){ 
        std::vector<int> ret(vs.size());
        std::transform(std::begin(vs), std::end(vs), std::begin(ret), [](int i){ return i*2; });

        return ret;
    };

    auto fnTwiceByLeftRef = [](std::vector<int>& vs){ 
        for (auto &v : vs) {
            v = v * 2;
        }

        return vs;
    };

    auto fnTwiceByRightRef = [](std::vector<int>&& vs){ 
        for (auto &&v : vs) {
            v = v * 2;
        }

        return vs;
    };

    auto fnByConstLeftRef = [](const std::vector<int>& vs){ 
        std::vector<int> temp;

        for (auto &v : vs) {
            temp.push_back(v);
        }

        return temp;
    };

    auto fnTwiceByConstRightRef = [](const std::vector<int>&& vs){ 
        std::vector<int> temp;

        for (auto &&v : vs) {
            temp.push_back(v);
        }

        return temp
        ;
    };

    std::cout << "[Perfect forwarding ex]\n";

    std::vector<int> object{1,2,3,4,5};
    std::cout << "original source : " << std::for_each(std::begin(object), std::end(object), [](int i){std::cout << i << ", ";}) << std::endl;
    
    //  pass by value
    auto ret = call_on_object_by_val<std::vector<int>>(object, fnTwiceByVal);
    std::cout << "forwarding by object value : " 
        << std::for_each(std::begin(ret), std::end(ret), [](int i){std::cout << i << ", ";}) << std::endl;

    //  pass by reference
    std::cout << "original source : " << std::for_each(std::begin(object), std::end(object), [](int i){std::cout << i << ", ";}) << std::endl;
    std::vector<int> object2(object);
    auto ret2 = call_on_object_by_ref<std::vector<int>>(object2, fnTwiceByLeftRef);
    std::cout << "forwarding by object 'Left' reference : "
        << std::for_each(std::begin(ret2), std::end(ret2), [](int i){std::cout << i << ", ";}) << std::endl;

    std::cout << "original source : " << std::for_each(std::begin(object), std::end(object), [](int i){std::cout << i << ", ";}) << std::endl;
    auto ret3 = call_on_object2<std::vector<int>>(std::move(object), fnTwiceByConstLeftRef);
    std::cout << "forwarding by object 'Const Left' reference : "
        << std::for_each(std::begin(ret3), std::end(ret3), [](int i){std::cout << i << ", ";}) << std::endl;
    
    std::cout << "original source : " << std::for_each(std::begin(object), std::end(object), [](int i){std::cout << i << ", ";}) << std::endl;
    std::vector<int> object3(object);
    auto ret4 = call_on_object_by_ref<std::vector<int>>(std::move(object3), fnTwiceByRightRef);
    std::cout << "forwarding by object 'Right' reference : "
        << std::for_each(std::begin(ret4), std::end(ret4), [](int i){std::cout << i << ", ";}) << std::endl;

    std::cout << "original source : " << std::for_each(std::begin(object), std::end(object), [](int i){std::cout << i << ", ";}) << std::endl;
    auto ret5 = call_on_object2<std::vector<int>>(std::move(object), fnTwiceByConstRightRef);
    std::cout << "forwarding by object 'Const Right' reference : "
        << std::for_each(std::begin(ret5), std::end(ret5), [](int i){std::cout << i << ", ";}) << std::endl;


    auto ask_ptr = &ask;
    std::cout << ask_ptr() << '\n';

    auto& ask_ref = ask;
    std::cout << ask_ref() << '\n';

    convertable_to_function_ptr ask_wrapper;
    std::cout << ask_wrapper() << '\n';

    std::vector<Person> ps {
        Person{SEX::MALE, "Peter", 56, "Mars"},
        Person{SEX::FEMALE, "Martha", 24, "Moon"},
        Person{SEX::FEMALE, "Jane", 52, "Mars"},
        Person{SEX::MALE, "David", 84, "Earth"},
        Person{SEX::MALE, "Tom", 32, "Mars"},
        Person{SEX::FEMALE, "Rose", 31, "Moon"}
    };

    std::vector<CPerson> cps {
        CPerson(SEX::MALE, "Peter", 56, "Mars"),
        CPerson(SEX::FEMALE, "Martha", 24, "Moon"),
        CPerson(SEX::FEMALE, "Jane", 52, "Mars"),
        CPerson(SEX::MALE, "David", 84, "Earth"),
        CPerson(SEX::MALE, "Tom", 32, "Mars"),
        CPerson(SEX::FEMALE, "Rose", 31, "Moon")
    };


    std::cout << "how many people older than 42? => " 
            << std::count_if(ps.cbegin(), ps.cend(), older_than_42) << " number.\n";

    std::cout << "how many people older than 16? [fn obj] => "
            << std::count_if(ps.cbegin(), ps.cend(), older_than(16)) << " numbers.\n";

    std::cout << "how many people older than 42? [generic fn obj] => " 
            << std::count_if(ps.cbegin(), ps.cend(), older_than2<Person>(42)) << " numbers.\n";

    std::vector<Car> cs {
        Car{"Santafe", 5},
        Car{"BMW s300", 10},
        Car{"Sonata", 12}
    };

    std::cout << "how many cars older than 6? [generic fn obj] => " 
            << std::count_if(cs.cbegin(), cs.cend(), older_than2<Car>(6)) << " numbers.\n";

    older_than3 predicate(6);
    std::cout << "how many people older than 6? [generic fn obj#2] => " 
            << std::count_if(ps.cbegin(), ps.cend(), predicate) << " numbers.\n";

    std::cout << "how many cars older than 6? [generic fn obj#2] => " 
            << std::count_if(cs.cbegin(), cs.cend(), predicate) << " numbers.\n";

    //  todo Person Struct -> Class test 
    //  !! do not support const
    older_than3_using_class predicate_using_class(6);
    std::cout << "[Using Class] how many people older than 6? [generic fn obj#2] => " 
            << std::count_if(cps.cbegin(), cps.cend(), predicate_using_class) << " numbers.\n";

    // auto predicate_using_lambda = [limit=6](auto&& object) {
    //     return object.getAge() > limit;
    // };
    // std::cout << "[Using Lambda] how many people older than 6? [generic fn obj#2] => " 
    //         << std::count_if(cps.cbegin(), cps.cend(), predicate_using_lambda) << " numbers.\n";


    auto predicate2 = [limit = 6](auto&& object) {
        return object.age > limit;
    };

    std::cout << "how many cars older than 6? [!!!1!!!!generic fn obj#2] => " 
            << std::count_if(cs.cbegin(), cs.cend(), predicate2) << " numbers.\n";



    return 0;
}
