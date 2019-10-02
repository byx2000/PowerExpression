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
		//�����հ��ַ�
		while (index < expr.size() && IsCharBlank(expr[index])) { index++; }

		//��ȡ���
		if (index == expr.size())
		{
			break;
		}
		//��������
		else if (IsCharDigit(expr[index]))
		{
			int t = index;
			bool isFloat = false;

			//����ȡ��ֱ�������ָ�����С����
			while (index < expr.size() && !IsCharSeparator(expr[index]))
			{
				//����С����
				if (expr[index] == '.')
				{
					isFloat = true;
					break;
				}
				index++;
			}

			//����
			if (!isFloat)
			{
				string name = expr.substr(t, index - t);
				if (!IsStringInt(name))
				{
					throw ParseError("Illegal integer: " + name);
				}
				tokenList.push_back(Token(name, Token::Integer));
			}
			//С��
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
		//С����
		else if (expr[index] == '.')
		{
			int t = index;
			//����ȡ��ֱ�������ָ���
			while (index < expr.size() && !IsCharSeparator(expr[index])) { index++; }
			string name = expr.substr(t, index - t);
			//����Ƿ��ǺϷ��ĸ�����
			if (!IsStringFloat(name))
			{
				throw ParseError("Illegal float: " + name);
			}
			tokenList.push_back(Token(name, Token::Float));
			index--;
		}
		//���������
		else if (expr[index] == '+' || expr[index] == '-' || expr[index] == '*' || expr[index] == '/' || expr[index] == '^' || expr[index] == '%')
		{
			//˫б��ע��
			if (expr[index] == '/' && index != expr.size() - 1 && expr[index + 1] == '/')
			{
				//���Ժ����������ݣ�ֱ���ո�
				while (index < expr.size() && expr[index] != '\n')	index++;
			}
			//������ֵ
			else if (index != expr.size() - 1 && expr[index + 1] == '=')
			{
				tokenList.push_back(Token(expr.substr(index, 2), Token::Assign));
				index++;
			}
			//�����
			else
			{
				tokenList.push_back(Token(expr.substr(index, 1), Token::ArithOp));
			}
		}
		//С����
		else if (expr[index] == '(')
		{
			tokenList.push_back(Token("(", Token::LeftParenthesis));
		}
		else if (expr[index] == ')')
		{
			tokenList.push_back(Token(")", Token::RightParenthesis));
		}
		//������
		else if (expr[index] == '[')
		{
			tokenList.push_back(Token("[", Token::LeftBracket));
		}
		else if (expr[index] == ']')
		{
			tokenList.push_back(Token("]", Token::RightBracket));
		}
		//������
		else if (expr[index] == '{')
		{
			tokenList.push_back(Token("{", Token::LeftBrace));
		}
		else if (expr[index] == '}')
		{
			tokenList.push_back(Token("}", Token::RightBrace));
		}
		//��ʶ����ͷ
		else if (IsCharIdent(expr[index]))
		{
			int t = index;
			while (index < expr.size() && !IsCharSeparator(expr[index])) { index++; }
			string name = expr.substr(t, index - t);
			//����Ƿ��ǺϷ��ı�ʶ��
			if (!IsStringIdent(name))
			{
				throw ParseError(string("Illegal indetifier: ") + name);
			}
			//�ؼ���
			if (count(begin(Token::keyword), end(Token::keyword), name) != 0)
			{
				tokenList.push_back(Token(name, Token::Keyword));
			}
			//��ʶ��
			else
			{
				tokenList.push_back(Token(name, Token::Identifier));
			}
			index--;
		}
		//����
		else if (expr[index] == ',')
		{
			tokenList.push_back(Token(",", Token::Comma));
		}
		//�Ⱥ�
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
		//��̾��
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
		//С�ں�
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
		//���ں�
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
		//�ֺ�
		else if (expr[index] == ';')
		{
			tokenList.push_back(Token(";", Token::Semicolon));
		}
		//��
		else if (expr[index] == '&')
		{
			if (index == expr.size() - 1 || expr[index + 1] != '&')
			{
				throw ParseError("Single '&'.");
			}
			tokenList.push_back(Token("&&", Token::Conjunc));
			index++;
		}
		//��
		else if (expr[index] == '|')
		{
		if (index == expr.size() - 1 || expr[index + 1] != '|')
		{
			throw ParseError("Single '|'.");
		}
		tokenList.push_back(Token("||", Token::Conjunc));
		index++;
		}
		//δ֪�ַ�
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