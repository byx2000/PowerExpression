#include "ExpressionLexer.h"

using namespace std;

string Token::typeDesc[18] = { "Integer", "Float", "ArithOp", "LogicalOp", "LeftParenthesis", "RightParenthesis", "LeftBracket", "RightBracket", "LeftBrace", "RightBrace", "Identifier", "Comma", "Semicolon", "Assign", "Keyword", "Conjunc", "End", "Unknown" };
string Token::keyword[4] = { "if", "else", "while", "for" };

string Token::toString() const
{
	string res = "<";
	res += _name;
	res += ", ";
	res += typeDesc[_type];
	res += ">";
	return res;
}

ExpressionLexer::ExpressionLexer(const string & expr)
{
	lex(expr);
}

bool ExpressionLexer::lex(const string & expr)
{
	tokenList.clear();

	size_t index = 0;
	while (index < expr.size())
	{
		//跳过空白字符
		while (index < expr.size() && IsCharBlank(expr[index])) { index++; }

		//读取完毕
		if (index == expr.size())
		{
			break;
		}
		//遇到数字
		else if (IsCharDigit(expr[index]))
		{
			int t = index;
			bool isFloat = false;

			//向后读取，直到遇到分隔符或小数点
			while (index < expr.size() && !IsCharSeparator(expr[index]))
			{
				//读到小数点
				if (expr[index] == '.')
				{
					isFloat = true;
					break;
				}
				index++;
			}

			//整数
			if (!isFloat)
			{
				string name = expr.substr(t, index - t);
				if (!IsStringInt(name))
				{
					throw ParseError("Illegal integer: " + name);
				}
				tokenList.push_back(Token(name, Token::Integer));
			}
			//小数
			else
			{
				while (index < expr.size() && !IsCharSeparator(expr[index])) { index++; }
				string name = expr.substr(t, index - t);
				if (!IsStringFloat(name))
				{
					throw ParseError("Illegal float: " + name);
				}
				tokenList.push_back(Token(name, Token::Float));
			}
			index--;
		}
		//小数点
		else if (expr[index] == '.')
		{
			int t = index;
			//向后读取，直到遇到分隔符
			while (index < expr.size() && !IsCharSeparator(expr[index])) { index++; }
			string name = expr.substr(t, index - t);
			//检查是否是合法的浮点数
			if (!IsStringFloat(name))
			{
				throw ParseError("Illegal float: " + name);
			}
			tokenList.push_back(Token(name, Token::Float));
			index--;
		}
		//算数运算符
		else if (expr[index] == '+' || expr[index] == '-' || expr[index] == '*' || expr[index] == '/' || expr[index] == '^' || expr[index] == '%')
		{
			//双斜杠注释
			if (expr[index] == '/' && index != expr.size() - 1 && expr[index + 1] == '/')
			{
				//忽略后面所有内容，直到空格
				while (index < expr.size() && expr[index] != '\n')	index++;
			}
			//算数赋值
			else if (index != expr.size() - 1 && expr[index + 1] == '=')
			{
				tokenList.push_back(Token(expr.substr(index, 2), Token::Assign));
				index++;
			}
			//运算符
			else
			{
				tokenList.push_back(Token(expr.substr(index, 1), Token::ArithOp));
			}
		}
		//小括号
		else if (expr[index] == '(')
		{
			tokenList.push_back(Token("(", Token::LeftParenthesis));
		}
		else if (expr[index] == ')')
		{
			tokenList.push_back(Token(")", Token::RightParenthesis));
		}
		//中括号
		else if (expr[index] == '[')
		{
			tokenList.push_back(Token("[", Token::LeftBracket));
		}
		else if (expr[index] == ']')
		{
			tokenList.push_back(Token("]", Token::RightBracket));
		}
		//大括号
		else if (expr[index] == '{')
		{
			tokenList.push_back(Token("{", Token::LeftBrace));
		}
		else if (expr[index] == '}')
		{
			tokenList.push_back(Token("}", Token::RightBrace));
		}
		//标识符开头
		else if (IsCharIdent(expr[index]))
		{
			int t = index;
			while (index < expr.size() && !IsCharSeparator(expr[index])) { index++; }
			string name = expr.substr(t, index - t);
			//检查是否是合法的标识符
			if (!IsStringIdent(name))
			{
				throw ParseError(string("Illegal indetifier: ") + name);
			}
			//关键字
			if (count(begin(Token::keyword), end(Token::keyword), name) != 0)
			{
				tokenList.push_back(Token(name, Token::Keyword));
			}
			//标识符
			else
			{
				tokenList.push_back(Token(name, Token::Identifier));
			}
			index--;
		}
		//逗号
		else if (expr[index] == ',')
		{
			tokenList.push_back(Token(",", Token::Comma));
		}
		//等号
		else if (expr[index] == '=')
		{
			if (index != expr.size() - 1 && expr[index + 1] == '=')
			{
				tokenList.push_back(Token("==", Token::LogicalOp));
				index++;
			}
			else
			{
				tokenList.push_back(Token("=", Token::Assign));
			}
		}
		//感叹号
		else if (expr[index] == '!')
		{
			if (index != expr.size() - 1 && expr[index + 1] == '=')
			{
				tokenList.push_back(Token("!=", Token::LogicalOp));
				index++;
			}
			else
			{
				tokenList.push_back(Token("!", Token::LogicalOp));
			}
		}
		//小于号
		else if (expr[index] == '<')
		{
			if (index != expr.size() - 1 && expr[index + 1] == '=')
			{
				tokenList.push_back(Token("<=", Token::LogicalOp));
				index++;
			}
			else
			{
				tokenList.push_back(Token("<", Token::LogicalOp));
			}
		}
		//大于号
		else if (expr[index] == '>')
		{
			if (index != expr.size() - 1 && expr[index + 1] == '=')
			{
				tokenList.push_back(Token(">=", Token::LogicalOp));
				index++;
			}
			else
			{
				tokenList.push_back(Token(">", Token::LogicalOp));
			}
		}
		//分号
		else if (expr[index] == ';')
		{
			tokenList.push_back(Token(";", Token::Semicolon));
		}
		//且
		else if (expr[index] == '&')
		{
			if (index == expr.size() - 1 || expr[index + 1] != '&')
			{
				throw ParseError("Single '&'.");
			}
			tokenList.push_back(Token("&&", Token::Conjunc));
			index++;
		}
		//或
		else if (expr[index] == '|')
		{
		if (index == expr.size() - 1 || expr[index + 1] != '|')
		{
			throw ParseError("Single '|'.");
		}
		tokenList.push_back(Token("||", Token::Conjunc));
		index++;
		}
		//未知字符
		else
		{
			throw ParseError(string("Illegal character: ") + expr[index]);
		}

		index++;
	}
	currentIndex = 0;
	return true;
}

Token ExpressionLexer::next()
{
	if (currentIndex < tokenList.size())
	{
		return tokenList[currentIndex++];
	}
	else
	{
		return Token("", Token::End);
	}
}

Token ExpressionLexer::peek()
{
	if (currentIndex < tokenList.size())
	{
		return tokenList[currentIndex];
	}
	else
	{
		return Token("", Token::End);
	}
}

void ExpressionLexer::back()
{
	currentIndex--;
	if (currentIndex < 0)
	{
		currentIndex = 0;
	}
}

void ExpressionLexer::read(const string & name)
{
	if (next().name() != name)
	{
		throw ParseError("\"" + name + "\"" + " expected");
	}
}

void ExpressionLexer::read(Token::Type type)
{
	if (next().type() != type)
	{
		throw ParseError(Token::typeDesc[type] + " expected");
	}
}

bool ExpressionLexer::IsStringInt(const string & s)
{
	if (s.size() == 0)
	{
		return false;
	}
	if (!IsCharDigit(s[0]) && s[0] != '-')
	{
		return false;
	}
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (!IsCharDigit(s[i]))
		{
			return false;
		}
	}
	return true;
}

bool ExpressionLexer::IsStringFloat(const string & s)
{
	if (s.size() == 0)
	{
		return false;
	}
	if (!IsCharDigit(s[0]) && s[0] != '-' && s[0] != '.')
	{
		return false;
	}

	bool flag = false;
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (s[i] == '.')
		{
			if (flag)
			{
				return false;
			}
			else
			{
				flag = true;
			}
		}
		else if (!IsCharDigit(s[i]))
		{
			return false;
		}
	}

	return true;
}

bool ExpressionLexer::IsStringBlank(const string & s)
{
	for (size_t i = 0; i < s.size(); ++i)
	{
		if (!IsCharBlank(s[i]))
		{
			return false;
		}
	}
	return true;
}

bool ExpressionLexer::IsStringIdent(const string & s)
{
	if (IsStringBlank(s))
	{
		return false;
	}

	if (!IsCharIdent(s[0]))
	{
		return false;
	}

	for (size_t i = 0; i < s.size(); ++i)
	{
		if (!IsCharIdent(s[i]) && !IsCharDigit(s[i]))
		{
			return false;
		}
	}

	return true;
}