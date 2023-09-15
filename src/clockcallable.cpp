#include <chrono>

#include "clockcallable.hpp"

int ClockCallable::arity() {return 0;};

Value* ClockCallable::call(Interpreter* interpreter, std::vector<Value*> arguments) { 

    namespace sc = std::chrono;
    auto time = sc::system_clock::now();
    auto since_epoch = time.time_since_epoch();
    auto millis = sc::duration_cast<sc::milliseconds>(since_epoch);
    long now = millis.count() ;
    double now_s = now / 1000;

    return new Value(now_s);
};

std::string ClockCallable::toString() {return "<native fn>";};

ClockCallable::ClockCallable() {};