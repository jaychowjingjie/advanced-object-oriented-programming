#ifndef UTILITIES_H
#define UTILITIES_H
#include <exception>

class Error : public std::exception {
public:
	Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
        {return msg;}
private:
	const char* msg;
};

/* add any of your own declarations here */
const double REFUEL_MIN = .005;

// length used by model and view to represent names
const int SHORTEN_NAME_LENGTH = 2;

#endif
