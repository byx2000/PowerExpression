#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include "Error.h"

class Token
{
public:
	enum Type { Integer, Float, ArithOp, LogicalOp, LeftParenthesis, RightParenthesis, LeftBracket, RightBracket, LeftBrace, RightBrace, Identifier, Comma, Semicolon, Assign, Keyword, Conjunc, End, Unknown };
	static std::string typeDesc[18];
	static std::string keyword[4];

	Token(const std::string& name = "", Type type = Unknown) : _name(name), _type(type) {}

	std::string name() const { return _name; }
	Type type() const { return _type; }

	std::string toString() const;

private:
	std::string _name;
	Type _type;
};

class ExpressionLexer
{
public:
	ExpressionLexer(const std::string& expr = "");

	bool lex(const std::string& expr);
	std::vector<Token> getTokenList() const { return tokenList; }
	Token next();
	Token peek();
	void back();
	size_t getCurrentIndex() { return currentIndex; }
	void setCurrentIndex(size_t index) { currentIndex = index; }
	void read(const std::string& name);
	void read(Token::Type type);

private:
	std::vector<Token> tokenList; //单词列表
	size_t currentIndex; //下一个单词索引

	//字符串处理函数
	static inline bool IsCharBlank(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
	static inline bool IsCharOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%'; }
	static inline bool IsCharDigit(char c) { return c >= '0' && c <= '9'; }
	static inline bool IsCharLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
	static inline bool IsCharIdent(char c) { return IsCharLetter(c) || c == '_'; }
	static inline bool IsCharSeparator(char c) { return IsCharBlank(c) || IsCharOperator(c) || c == '(' || c == ')' || c == '{' || c == '}' || c == ',' || c == '=' || c == '!' || c == '<' || c == '>' || c == ';' || c == '&' || c == '|' || c == '[' || c == ']'; }
	static bool IsStringInt(const std::string& s);
	static bool IsStringFloat(const std::string& s);
	static bool IsStringBlank(const std::string& s);
	static bool IsStringIdent(const std::string& s);
};