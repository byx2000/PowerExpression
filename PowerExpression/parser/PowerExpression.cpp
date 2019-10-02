#include "PowerExpression.h"

using namespace std;

PowerExpression& PowerExpression::setExpr(const string & expr)
{
	this->expr = expr;
	exprUpdate = true;
	return *this;
}

PowerExpression& PowerExpression::setFuncManager(const FunctionManager & funcManager)
{
	env.setFuncMgr(funcManager);
	funcUpdate = true;
	return *this;
}

void PowerExpression::buildAST()
{
	if (expr.empty())
	{
		throw ParseError("The expression is empty.");
	}

	root = parseExpr();
	if (lexer.peek().type() != Token::End)
	{
		throw ParseError("Unexpected end of expression.");
	}
}

void PowerExpression::update()
{
	if (exprUpdate)
	{
		lex();
		buildAST();
		exprUpdate = false;
	}
	else if (funcUpdate)
	{
		buildAST();
		funcUpdate = false;
	}
}

string PowerExpression::toString()
{
	update();
	return root.toString();
}

double PowerExpression::eval()
{
	update();
	env.clear();
	return root.eval(env);
}

Expr PowerExpression::parseExpr()
{
	vector<Expr> subexpr;
	subexpr.push_back(parseSubexpr());

	while (lexer.peek().name() == ",")
	{
		lexer.next();
		size_t index = lexer.getCurrentIndex();
		try
		{
			subexpr.push_back(parseSubexpr());
		}
		catch (...)
		{
			lexer.setCurrentIndex(index);
			break;
		}
	}

	if (subexpr.size() == 1)
	{
		return subexpr[0];
	}
	else
	{
		return Expr::newComma(subexpr);
	}
}

Expr PowerExpression::parseSubexpr()
{
	Expr res = parseLogicalExpr();

	while (lexer.peek().name() == "&&" || lexer.peek().name() == "||")
	{
		string op = lexer.next().name();
		if (op == "&&")
		{
			res = Expr::newBinary(BinaryOpNode::And, res, parseLogicalExpr());
		}
		else
		{
			res = Expr::newBinary(BinaryOpNode::Or, res, parseLogicalExpr());
		}
	}

	return res;
}

Expr PowerExpression::parseLogicalExpr()
{
	if (lexer.peek().type() == Token::Keyword)
	{
		string word = lexer.next().name();

		//while循环
		if (word == "while")
		{
			//读取左括号
			lexer.read("(");
			//获取条件表达式
			Expr cond = parseExpr();
			//读取右括号
			lexer.read(")");
			//读取左大括号
			lexer.read("{");
			//循环体不为空
			Expr body;
			if (lexer.peek().name() != "}")
			{
				//获取循环体表达式
				body = parseExpr();
			}
			//读取右大括号
			lexer.read("}");
			//构造while表达式节点
			return Expr::newWhile(cond, body);
		}
		//if语句
		else if (word == "if")
		{
			//读取左括号
			lexer.read("(");
			//获取条件表达式
			Expr cond = parseExpr();
			//读取右括号
			lexer.read(")");
			//读取左大括号
			lexer.read("{");
			//真表达式不为空
			Expr tExpr;
			if (lexer.peek().name() != "}")
			{
				//获取真表达式
				tExpr = parseExpr();
			}
			//读取右大括号
			lexer.read("}");
			Expr fExpr;
			//有else子句
			if (lexer.peek().name() == "else")
			{
				lexer.next();
				//读取左大括号
				lexer.read("{");
				//假表达式不为空
				if (lexer.peek().name() != "}")
				{
					//获取假表达式
					fExpr = parseExpr();
				}
				//读取右大括号
				lexer.read("}");
			}
			//构造if语句节点
			return Expr::newIf(cond, tExpr, fExpr);
		}
		//for循环
		else if (word == "for")
		{
			//读取左括号
			lexer.read("(");

			Expr init = Expr::newNum(1.0);
			//初始表达式不为空
			if (lexer.peek().name() != ";")
			{
				//解析初始表达式
				init = parseExpr();
				//读取分号
				lexer.read(";");
			}
			//初始表达式为空
			else
			{
				lexer.next();
			}

			Expr cond = Expr::newNum(1.0);
			//条件表达式不为空
			if (lexer.peek().name() != ";")
			{
				//解析条件表达式
				cond = parseExpr();
				//读取分号
				lexer.read(";");
			}
			//条件表达式为空
			else
			{
				lexer.next();
			}
			
			Expr update = Expr::newNum(1.0);
			//更新表达式不为空
			if (lexer.peek().name() != ")")
			{
				//解析更新表达式
				update = parseExpr();
				//读取右括号
				lexer.read(")");
			}
			//更新表达式为空
			else
			{
				lexer.next();
			}
			//读取左大括号
			lexer.read("{");
			//循环体不为空
			Expr body;
			if (lexer.peek().name() != "}")
			{
				//读取函数体表达式
				body = parseExpr();
			}
			//读取右大括号
			lexer.read("}");
			//构造for循环节点
			return Expr::newFor(init, cond, update, body);
		}
	}

	Expr res = parseCmpexpr();
	while (lexer.peek().type() == Token::LogicalOp && lexer.peek().name() != "!")
	{
		string op = lexer.next().name();
		if (op == "==")
		{
			res = Expr::newBinary(BinaryOpNode::Equal, res, parseCmpexpr());
		}
		else if (op == "!=")
		{
			res = Expr::newBinary(BinaryOpNode::Unequal, res, parseCmpexpr());
		}
		else if (op == ">")
		{
			res = Expr::newBinary(BinaryOpNode::BiggerThan, res, parseCmpexpr());
		}
		else if (op == ">=")
		{
			res = Expr::newBinary(BinaryOpNode::BiggerThanEqual, res, parseCmpexpr());
		}
		else if (op == "<")
		{
			res = Expr::newBinary(BinaryOpNode::LessThan, res, parseCmpexpr());
		}
		else if (op == "<=")
		{
			res = Expr::newBinary(BinaryOpNode::LessThanEqual, res, parseCmpexpr());
		}
	}
	return res;
}

Expr PowerExpression::parseCmpexpr()
{
	Token token = lexer.peek();

	//赋值语句或含变量的表达式
	if (token.type() == Token::Identifier && !env.funcExist(token.name()))
	{
		size_t cur = lexer.getCurrentIndex();
		//获取变量名
		string varName = lexer.next().name();
		//变量赋值
		if (lexer.peek().type() == Token::Assign)
		{
			//读取赋值符号
			string op = lexer.next().name();
			//构造赋值节点
			if (op == "=")
			{
				return Expr::newVarAssign(varName, parseSubexpr());
			}
			else if (op == "+=")
			{
				return Expr::newVarAssign(varName, Expr::newBinary(BinaryOpNode::Add, Expr::newVar(varName), parseSubexpr()));
			}
			else if (op == "-=")
			{
				return Expr::newVarAssign(varName, Expr::newBinary(BinaryOpNode::Sub, Expr::newVar(varName), parseSubexpr()));
			}
			else if (op == "*=")
			{
				return Expr::newVarAssign(varName, Expr::newBinary(BinaryOpNode::Mul, Expr::newVar(varName), parseSubexpr()));
			}
			else if (op == "/=")
			{
				return Expr::newVarAssign(varName, Expr::newBinary(BinaryOpNode::Div, Expr::newVar(varName), parseSubexpr()));
			}
			else if (op == "^=")
			{
				return Expr::newVarAssign(varName, Expr::newBinary(BinaryOpNode::Pow, Expr::newVar(varName), parseSubexpr()));
			}
			else if (op == "%=")
			{
				return Expr::newVarAssign(varName, Expr::newBinary(BinaryOpNode::Rem, Expr::newVar(varName), parseSubexpr()));
			}
		}
		//数组元素赋值或含数组元素的表达式
		else if (lexer.peek().name() == "[")
		{
			/*lexer.next();
			//读取下标表达式
			Expr index = parseExpr();
			//读取右方括号
			lexer.read("]");*/
			vector<Expr> v;
			while (lexer.peek().name() == "[")
			{
				lexer.next();
				//读取下标表达式
				v.push_back(parseExpr());
				//读取右方括号
				lexer.read("]");
			}

			//数组元素赋值
			if (lexer.peek().type() == Token::Assign)
			{
				
				//v.push_back(index);

				//读取赋值符号
				string op = lexer.next().name();
				//构造赋值节点
				if (op == "=")
				{
					return Expr::newArrElemAssign(varName, v, parseSubexpr());
				}
				else if (op == "+=")
				{
					return Expr::newArrElemAssign(varName, v, Expr::newBinary(BinaryOpNode::Add, Expr::newArrElem(varName, v), parseSubexpr()));
				}
				else if (op == "-=")
				{
					return Expr::newArrElemAssign(varName, v, Expr::newBinary(BinaryOpNode::Sub, Expr::newArrElem(varName, v), parseSubexpr()));
				}
				else if (op == "*=")
				{
					return Expr::newArrElemAssign(varName, v, Expr::newBinary(BinaryOpNode::Mul, Expr::newArrElem(varName, v), parseSubexpr()));
				}
				else if (op == "/=")
				{
					return Expr::newArrElemAssign(varName, v, Expr::newBinary(BinaryOpNode::Div, Expr::newArrElem(varName, v), parseSubexpr()));
				}
				else if (op == "^=")
				{
					return Expr::newArrElemAssign(varName, v, Expr::newBinary(BinaryOpNode::Pow, Expr::newArrElem(varName, v), parseSubexpr()));
				}
				else if (op == "%=")
				{
					return Expr::newArrElemAssign(varName, v, Expr::newBinary(BinaryOpNode::Rem, Expr::newArrElem(varName, v), parseSubexpr()));
				}
			}
			//含数组元素的表达式
			else
			{
				//恢复单词流
				lexer.setCurrentIndex(cur);
				//继续分析
			}
		}
		//含变量的表达式
		else
		{
			//恢复单词流
			lexer.setCurrentIndex(cur);
			//继续分析
		}
	}

	Expr res = parseTerm();

	while (lexer.peek().name() == "+" || lexer.peek().name() == "-")
	{
		string op = lexer.next().name();
		Expr right = parseTerm();
		if (op == "+")
		{
			res = Expr::newBinary(BinaryOpNode::Add, res, right);
		}
		else
		{
			res = Expr::newBinary(BinaryOpNode::Sub, res, right);
		}
	}

	return res;
}

Expr PowerExpression::parseTerm()
{
	//处理前置正负号
	if (lexer.peek().name() == "-" || lexer.peek().name() == "+")
	{
		int sign = 1;
		while (lexer.peek().name() == "-" || lexer.peek().name() == "+")
		{
			string op = lexer.next().name();
			if (op == "-")
			{
				sign *= -1;
			}
		}
		if (sign == -1)
		{
			return Expr::newUnary(UnaryOpNode::Minus, parseTerm());
		}
		else
		{
			return parseTerm();
		}
	}

	Expr res = parseFactor();

	while (lexer.peek().name() == "*" || lexer.peek().name() == "/" || lexer.peek().name() == "%")
	{
		string op = lexer.next().name();
		Expr right = parseFactor();
		if (op == "*")
		{
			res = Expr::newBinary(BinaryOpNode::Mul, res, right);
		}
		else if (op == "/")
		{
			res = Expr::newBinary(BinaryOpNode::Div, res, right);
		}
		else
		{
			res = Expr::newBinary(BinaryOpNode::Rem, res, right);
		}
	}

	return res;
}

Expr PowerExpression::parseFactor()
{
	//处理前置正负号
	if (lexer.peek().name() == "-" || lexer.peek().name() == "+")
	{
		int sign = 1;
		while (lexer.peek().name() == "-" || lexer.peek().name() == "+")
		{
			string op = lexer.next().name();
			if (op == "-")
			{
				sign *= -1;
			}
		}
		if (sign == -1)
		{
			return Expr::newUnary(UnaryOpNode::Minus, parseFactor());
		}
		else
		{
			return parseFactor();
		}
	}

	Expr res = parseElem();

	while (lexer.peek().name() == "^")
	{
		lexer.next();
		Expr right = parseElem();
		res = Expr::newBinary(BinaryOpNode::Pow, res, right);
	}

	return res;
}

Expr PowerExpression::parseElem()
{
	Token token = lexer.next();
	string name = token.name();
	Token::Type type = token.type();

	if (type == Token::Integer || type == Token::Float) //num
	{
		return Expr::newNum(atof(name.c_str()));
	}
	//括号括起来的表达式
	else if (name == "(")
	{
		Expr res = parseExpr();
		lexer.read(")");
		return res;
	}
	//负号
	else if (name == "-")
	{
		return Expr::newUnary(UnaryOpNode::Minus, parseElem());
	}
	//正号
	else if (name == "+")
	{
		return Expr::newUnary(UnaryOpNode::Plus, parseElem());
	}
	//逻辑非运算符
	else if (name == "!")
	{
		return Expr::newUnary(UnaryOpNode::Not, parseElem());
	}
	//函数调用或变量或数组元素
	else if (type == Token::Identifier)
	{
		if (lexer.peek().name() == "(") //函数调用
		{
			lexer.next();
			//参数列表
			vector<Expr> params;
			//函数有参数
			if (lexer.peek().name() != ")")
			{
				//读取第一个参数
				params.push_back(parseSubexpr());
				//读取剩下的参数
				while (lexer.peek().name() != ")")
				{
					lexer.read(",");
					params.push_back(parseSubexpr());
				}
			}
			//读取右括号
			lexer.read(")");
			return Expr::newFunc(name, params);
		}
		else //变量或数组元素
		{
			//数组元素
			if (lexer.peek().name() == "[")
			{
				vector<Expr> v;
				while (lexer.peek().name() == "[")
				{
					lexer.next();
					//读取下标表达式
					v.push_back(parseExpr());
					//读取右方括号
					lexer.read("]");
				}
				//构造数组元素节点
				return Expr::newArrElem(name, v);
			}
			//变量
			else
			{
				//构造变量节点
				return Expr::newVar(name);
			}
		}
	}
	else if (type == Token::End)
	{
		throw ParseError("Unexpected end of expression.");
	}
	else
	{
		throw ParseError("Unexpected token: " + name);
	}
}