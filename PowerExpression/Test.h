#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include  "parser\\PowerExpression.h"

using namespace std;

void TestExpressionLexer()
{
	ifstream fexpr("test\\TestExpressionLexer.txt");
	ifstream ftoken("test\\TestExpressionLexer_r.txt");

	string expr;
	while (getline(fexpr, expr))
	{
		if (expr.size() == 0)
		{
			continue;
		}

		ExpressionLexer lexer;

		try
		{
			lexer.lex(expr);
		}
		catch (ParseError& e)
		{
			cout << "expr failed: " << expr << endl;
			cout << "Exception occured:" << endl;
			cout << e.info() << endl;
			return;
		}

		vector<Token> tokenList = lexer.getTokenList();
		for (size_t i = 0; i < tokenList.size(); ++i)
		{
			string name, type;
			ftoken >> name >> type;
			if (name != tokenList[i].name() || type != Token::typeDesc[tokenList[i].type()])
			{
				cout << "expr failed: " << expr << endl;
				cout << name << endl;
				cout << tokenList[i].name() << endl;
				cout << type << endl;
				cout << Token::typeDesc[tokenList[i].type()] << endl;
				return;
			}
		}
	}

	cout << "All test cases passed!" << endl;
}

void TestExpressionLexer_E()
{
	ifstream fexpr("test\\TestExpressionLexer_e.txt");

	string expr;
	while (getline(fexpr, expr))
	{
		if (expr.size() == 0)
		{
			continue;
		}

		bool exceptionOccur = false;
		try
		{
			ExpressionLexer lexer(expr);
		}
		catch (ParseError&)
		{
			exceptionOccur = true;
		}

		if (!exceptionOccur)
		{
			cout << "expr failed: " << expr << endl;
			return;
		}
	}

	cout << "All test cases passed!" << endl;
}

void TestAST()
{
	ifstream fexpr("test\\TestAST.txt");
	ifstream fans("test\\TestAST_r.txt");

	string expr;
	PowerExpression pe;
	while (getline(fexpr, expr))
	{
		if (expr.size() == 0)
		{
			continue;
		}

		

		try
		{
			string res = pe.setExpr(expr).toString();
			string ans;
			fans >> ans;

			if (res != ans)
			{
				cout << "expr failed: " << expr << endl;
				cout << res << endl;
				cout << ans << endl;
				return;
			}
		}
		catch (ParseError& e)
		{
			cout << "expr failed: " << expr << endl;
			cout << e.info() << endl;
			return;
		}
	}

	cout << "All test cases passed!" << endl;
}

void TestAST_E()
{
	ifstream fexpr("test\\TestAST_e.txt");

	string expr;
	PowerExpression pe;
	while (getline(fexpr, expr))
	{
		if (expr.size() == 0)
		{
			continue;
		}

		bool flag = false;
		try
		{
			pe.setExpr(expr).toString();
		}
		catch (ParseError&)
		{
			flag = true;
		}
		catch (RuntimeError&)
		{
			flag = true;
		}

		if (!flag)
		{
			cout << "expr failed: " << expr << endl;
			return;
		}
	}

	cout << "All test cases passed!" << endl;
}

void TestASTEval()
{
	ifstream fexpr("test\\TestASTEval.txt");
	ifstream fans("test\\TestASTEval_r.txt");

	string expr;
	PowerExpression pe;
	while (getline(fexpr, expr))
	{
		if (expr.size() == 0)
		{
			continue;
		}

		try
		{
			double res = pe.setExpr(expr).eval();
			stringstream ss;
			ss << fixed << setprecision(3) << res;
			string s = ss.str();
			if (s == "-0.000")
			{
				s = "0.000";
			}
			string ans;
			fans >> ans;

			if (s != ans)
			{
				cout << "expr failed: " << expr << endl;
				cout << s << endl;
				cout << ans << endl;
				return;
			}
		}
		catch (ParseError& e)
		{
			cout << "expr failed: " << expr << endl;
			cout << e.info() << endl;
			return;
		}
		catch (RuntimeError& e)
		{
			cout << "expr failed: " << expr << endl;
			cout << e.info() << endl;
			return;
		}
	}

	cout << "All test cases passed!" << endl;
}

void TestASTEval_E()
{
	ifstream fexpr("test\\TestASTEval_e.txt");

	string expr;
	PowerExpression pe;
	while (getline(fexpr, expr))
	{
		if (expr.size() == 0)
		{
			continue;
		}

		bool flag = false;
		try
		{
			double res = pe.setExpr(expr).eval();
		}
		catch (ParseError&)
		{
			flag = true;
		}
		catch (RuntimeError&)
		{
			flag = true;
		}

		if (!flag)
		{
			cout << "expr failed: " << expr << endl;
			return;
		}
	}

	cout << "All test cases passed!" << endl;
}