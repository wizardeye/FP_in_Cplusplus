#include <iostream>
#include <mutex>
#include <algorithm>
#include <random>
#include <vector>
#include <map>
#include <tuple>


//  list 6.1
template <typename F>
class lazy_val {
private:
    F m_computation;
    mutable bool m_cache_initialized;
    mutable decltype(m_computation()) m_cache;
    // mutable std::mutex m_cache_mutex;    //  for performance
    mutable std::once_flag m_value_flag;


public:
    lazy_val(F computation): m_computation(computation), m_cache_initialized(false) {}

    operator const decltype(m_computation())& () const {
        // std::unique_lock<std::mutex> lock{m_cache_mutex};

        // if (!m_cache_initialized) {
        //     m_cache = m_computation();
        //     m_cache_initialized = true;
        // }

        std::call_once(m_value_flag, [this](){
            m_cache = m_computation();
        });

        return m_cache;
    }
};


//  list 6.2
template < typename BeginIt
         , typename EndIt
         , typename MidIt
         >
void sort_until(BeginIt begin, MidIt mid, EndIt end)
{
    // The `std::nth_element` algorithm is a partial sorting algorithm.
    //
    // After the algorithm is executed, the element at the `mid` position
    // will be the same element which would be at that position if
    // the collection was fully sorted.
    std::nth_element(begin, mid, end);

    // It also partitions the collection to have all of the elements before
    // this new `mid` element are less than or equal to the elements after
    // the new `mid` element.

    // This means that the elements before `mid` are the elements that
    // would be before `mid` in the completely sorted collection.
    // We just need to sort them.
    std::sort(begin, mid);
}


//  list 6.3
unsigned int normalFib(unsigned int n) {
    std::cout << n << " th caclulation(normal)\n";
    return n == 0 ? 0
        : n == 1 ? 1
        : normalFib(n-1) + normalFib(n-2);
}


std::vector<unsigned int> cache{0,1};

unsigned int fib(unsigned int n) {
    if (cache.size() > n) {
        return cache[n];
    }
    else {
        const auto result = fib(n-1) + fib(n-2);
        cache.push_back(result);
        std::cout << n << " th caclulation(cache)\n";
        return result;
    }
}

class fib_cache {
public:
    fib_cache() : m_previous{0}, m_last{1}, m_size{2} {}

    std::size_t size() const {
        return m_size;
    }

    unsigned int operator[] (unsigned int n) const {
        return  n == m_size - 1 ? m_last :
                n == m_size - 2 ? m_previous :
                0;
    }

    void push_back(unsigned int value) {
        m_size++;
        m_previous = m_last;
        m_last = value;
    }


private:
    int m_previous;
    int m_last;
    int m_size;
};

unsigned int advanced_fib(unsigned int n) {
    auto cache = fib_cache();

    if (cache.size() > n) {
        return cache[n];
    }
    else {
        const auto result = fib(n-1) + fib(n-2);
        cache.push_back(result);

        return result;
    }
}

//  list 6.7
template <typename Result, typename... Args>
auto make_memoized(Result (*f)(Args...)) {
    std::map<std::tuple<Args...>, Result> cache;    //  (args : val)

    return [f, cache](Args... args) mutable -> Result {
        const auto args_tuple = std::make_tuple(args...);
        const auto cached = cache.find(args_tuple);

        if (cached == cache.end()) {
            auto result = f(args...);
            cache[args_tuple] = result;
            return result;
        }
        else {
            return cached->second;
        }
    };
}

template <typename F>
unsigned int fib2(F&& fibMemo, unsigned int n) {
    std::cout << n << " th caclulation(fib2)\n";
    return n == 0 ? 0
        :  n == 1 ? 1
        :  fibMemo(n-1) + fibMemo(n-2);
}


//  list 6.9
class null_param{};

template <class Sig, class F>
class memoize_helper;

template <class Result, class... Args, class F>
class memoize_helper<Result(Args...), F> {
public:
    template <typename Function>
    memoize_helper(Function&& f, null_param) : _f(f) {}
    memoize_helper(const memoize_helper& other) : _f(other._f) {}
    
    template <class... InnerArgs>
    Result operator()(InnerArgs&&... args) const {
        std::unique_lock<std::recursive_mutex> lock{_cache_mutex};
        const auto args_tuple = std::make_tuple(args...);
        const auto cached = _cache.find(args_tuple);    //  todo

        if (cached != _cache.end()) {
            return cached->second;
        }
        else {
            auto&& result = _f(*this, std::forward<InnerArgs>(args)...);
            _cache[args_tuple] = result;
            return result;
        }
    }

private:
    using function_type = F;
    using args_tuple_type = std::tuple<std::decay_t<Args>...>;
    function_type _f;
    mutable std::map<args_tuple_type, Result> _cache;
    mutable std::recursive_mutex _cache_mutex;
};

template <class Sig, class F>
memoize_helper<Sig, std::decay_t<F>>
make_memoized_r(F&& f) {
    return {std::forward<F>(f), null_param()};
}


//  list 6.10
template <typename... Strings>
class lazy_string_concat_helper;

template <typename LastString, typename... Strings>
class lazy_string_concat_helper<LastString, Strings...> {
private:
    LastString data;
    lazy_string_concat_helper<Strings...> tail;

public:
    lazy_string_concat_helper(LastString data, lazy_string_concat_helper<Strings...> tail)
    : data(data), tail(tail) {}

    int size() const {
        return data.size() + tail.size();
    }

    template <typename It>
    void save(It end) const {
        const auto begin = end - data.size();
        std::copy(data.cbegin(), data.cend(), begin);
        tail.save(begin);
    }

    operator std::string() const {
        std::string result(size(), '\0');
        save(result.end());
        return result;
    }

    lazy_string_concat_helper<std::string, LastString, Strings...>
    operator+(const std::string& other) const {
        return lazy_string_concat_helper<std::string, LastString, Strings...>(
            other, *this
        );
    }
};


template<>
class lazy_string_concat_helper<> {
public:
    lazy_string_concat_helper() {}
    int size() const { return 0; }

    template <typename It>
    void save(It) const {}

    lazy_string_concat_helper<std::string>
    operator+(const std::string& other) const {
        return lazy_string_concat_helper<std::string>(
            other, *this
        );
    }
};


//  tuple test
int sum(int a, int b, int c, int d) { return a + b + c + d;}



int main() {
    std::cout << "[6.1] lazy eval ex : cache\n";
    auto app1 = [](){        
        int ret = 42;
        std::cout << "big question ...\n";

        return ret;    
    };

    std::cout << "eager eval => app1()\n";
    auto ans1 = app1();
    std::cout << "answer is " << ans1 << '\n';

    lazy_val ev1(app1);
    std::cout << "\nlazt eval first run [caching] => app1()\n";
    std::cout << ev1 << std::endl;
    
    std::cout << "\nlazt eval second run [cached] => app1()\n";
    std::cout << ev1 << std::endl;

    std::cout << "\n[6.2] lazy eval for optimization ex : lazy quick sort\n";
    std::vector<int> xs { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(xs.begin(), xs.end(), g);

    std::cout << "Before sorting: ";
    for (const auto& x: xs) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    sort_until(xs.begin(), xs.begin() + 3, xs.end());

    std::cout << "After sorting: ";
    for (const auto& x: xs) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;

    std::cout << "\n[6.2.3] fib using optimized cache ex\n";
    std::cout << "fib(10) => " << advanced_fib(10) << '\n';

    std::cout << "\n[6.3.1] fib using memoized ex\n";
    auto fibMemo = make_memoized(normalFib);
    std::cout << "fib(10) = " << fibMemo(10) << "\n\n";

    auto fibMemo2 = make_memoized(normalFib);
    auto fibMemoRecur = fib2(fibMemo2, 10);
    std::cout << "fibMemoRecur(10) = " << fibMemoRecur << '\n';

    // auto fibMemo3 = make_memoized_r(normalFib);
    // auto fibMemoRecur3 = fibMemo3(normalFib, 10);
    // std::cout << "[memoized wrapper] fibMemoRecur(10) = " << fibMemoRecur3 << '\n';

    // auto fibMemo3 = make_memoized_r<unsigned int(unsigned int)>(
    //     [](auto& fib, unsigned int n) {
    //         std::cout << "calculating " << n << "!\n";
    //         return n == 0 ? 0
    //         : n == 1 ? 1
    //         : fib(n-1) + fib(n-2);
    //     }
    // );

    // std::cout << "[memoized wrapper] fibMemoRecur(10) = " << fibMemo3(normalFib, 10) << '\n';


    std::cout << "\n[6.4] concat strings ex\n";
    std::string name = "jane";
    std::string surname = "smith";

    lazy_string_concat_helper<> lazy_concat;
    const std::string fullname = lazy_concat + surname + ", " + name;
    std::cout << fullname << '\n';

    auto test = make_memoized(sum);
    std::cout << test(1,2,3,4) << '\n';

    return 0;
}