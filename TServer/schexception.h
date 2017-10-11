#pragma once

#include <string>
#include <exception>

class SchException : public std::exception
{
public:
	SchException(const char* pMessage) : msg_(pMessage) { }
	SchException(std::string const &message) : msg_(message) { }
	//virtual char const *what() const noexcept{ return msg_.c_str(); }

private:
	std::string msg_;
};