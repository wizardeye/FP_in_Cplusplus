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
#include <memory>
#include <assert.h>
#include <variant>
#include <range/v3/all.hpp>


class socket_t {
public:
    socket_t() {std::cout << "socket create. and open socket.\n";}
    ~socket_t() {std::cout << "socket delete. close socket.\n";}
};

class state_t {
protected:
    state_t(int type) : type(type) {}
    
public:
    virtual ~state_t() {}
    int type;
};


class init_t : public state_t {
public:
    enum { id = 0 };
    init_t() : state_t{id} {
        std::cout << "init\n";
    }
};


class running_t : public state_t {
public:
    enum { id = 1 };
    running_t() : state_t{id} {
        std::cout << "running\n";
    }
    
    void read() { _count++; }
    
    unsigned count() const {
        return _count;
    }
    
private:
    unsigned _count = 0;
    socket_t _web_page;
};


class finished_t : public state_t {
public:
    enum { id = 2 };
    finished_t(unsigned count) : state_t{id}, _count{count} {
        std::cout << "finished\n";
    }
    
    unsigned count() const {
        return _count;
    }
    
private:
    unsigned _count;
};


class program_t {
public:
    program_t(): _state(std::make_unique<init_t>()) {}  //  not exist null state
    
    void run() {
        assert(_state->type == init_t::id);
        _state = std::make_unique<running_t>();
        
        auto state = static_cast<running_t*>(_state.get());
        
        for (int i = 0; i < 10; i++) {
            state->read();
        }
    }
    
    
    void counting_finished() {
        assert(_state->type == running_t::id);
        
        auto state = static_cast<running_t*>(_state.get());
        _state = std::make_unique<finished_t>(state->count());  //  todo    leak?
        std::cout << "count: " << state->count() << std::endl;
    }
    
private:
    std::unique_ptr<state_t> _state;
};


//  using variant

class init_var_t {
public:
    init_var_t() {std::cout << "init\n";}
};
        
class running_var_t {
public:
    running_var_t() {std::cout << "running\n";

    unsigned count() const { return _count; }
    
    void read() { _count++; }
    
private:
    unsigned _count = 0;
    socket_t _web_page;
};


class finished_var_t {
public:
    finished_var_t(unsigned count) : _count{count} {
        std::cout << "finished\n";
    }
        
    unsigned count() const {
        return _count;
    }
        
private:
    unsigned _count;
};
        

class program_var_t {
public:
    program_var_t() : _state(init_var_t()) {}
        
    void run() {
        auto* state = std::get_if<init_var_t>(&_state);
        assert(state != nullptr);
        
        _state = running_var_t();
        
        for (int i = 0; i < 10; i++) {
            std::get<running_var_t>(_state).read();
        }
    }
        
    void counting_finished() {
        auto* state = std::get_if<running_var_t>(&_state);
        assert(state != nullptr);
        
        _state = finished_var_t(state->count());
        std::cout << "count: " << std::get<finished_var_t>(_state).count() << std::endl;
    }
        
private:
    std::variant<init_var_t, running_var_t, finished_var_t> _state;
};
        
        

int main()
{
    program_t p;
    p.run();
    p.counting_finished();
        
    std::cout << "\n===apply variant\n";
        
    program_var_t varP;
    varP.run();
    varP.counting_finished();
    
    return 0;
}
