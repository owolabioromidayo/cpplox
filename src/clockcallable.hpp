#ifndef CLOCK_CALLABLE_H_ 
#define CLOCK_CALLABLE_H_ 

#include "types.hpp"
#include "environment.hpp"


class ClockCallable : public LoxCallable {
public:
    int arity();

    Value* call(Interpreter* interpreter, std::vector<Value*> arguments);

    std::string toString();

    ClockCallable();
};

#endif //CLOCK_CALLABLE_H_ 