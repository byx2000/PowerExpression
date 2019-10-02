#pragma once

#include <string>

class ParseError
{
public:
	ParseError(const std::string& info = "Unknown parse error") : _info(info) {}
	std::string info() const { return _info; }
private:
	std::string _info;
};

class RuntimeError
{
public:
	RuntimeError(const std::string& info = "Unknown runtime error") : _info(info) {}
	std::string info() const { return _info; }
private:
	std::string _info;
};