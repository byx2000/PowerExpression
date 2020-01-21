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

		//whileѭ��
		if (word == "while")
		{
			//��ȡ������
			lexer.read("(");
			//��ȡ�������ʽ
			Expr cond = parseExpr();
			//��ȡ������
			lexer.read(")");
			//��ȡ�������
			lexer.read("{");
			//ѭ���岻Ϊ��
			Expr body;
			if (lexer.peek().name() != "}")
			{
				//��ȡѭ������ʽ
				body = parseExpr();
			}
			//��ȡ�Ҵ�����
			lexer.read("}");
			//����while���ʽ�ڵ�
			return Expr::newWhile(cond, body);
		}
		//if���
		else if (word == "if")
		{
			//��ȡ������
			lexer.read("(");
			//��ȡ�������ʽ
			Expr cond = parseExpr();
			//��ȡ������
			lexer.read(")");
			//��ȡ�������
			lexer.read("{");
			//����ʽ��Ϊ��
			Expr tExpr;
			if (lexer.peek().name() != "}")
			{
				//��ȡ����ʽ
				tExpr = parseExpr();
			}
			//��ȡ�Ҵ�����
			lexer.read("}");
			Expr fExpr;
			//��else�Ӿ�
			if (lexer.peek().name() == "else")
			{
				lexer.next();
				//��ȡ�������
				lexer.read("{");
				//�ٱ��ʽ��Ϊ��
				if (lexer.peek().name() != "}")
				{
					//��ȡ�ٱ��ʽ
					fExpr = parseExpr();
				}
				//��ȡ�Ҵ�����
				lexer.read("}");
			}
			//����if���ڵ�
			return Expr::newIf(cond, tExpr, fExpr);
		}
		//forѭ��
		else if (word == "for")
		{
			//��ȡ������
			lexer.read("(");

			Expr init = Expr::newNum(1.0);
			//��ʼ���ʽ��Ϊ��
			if (lexer.peek().name() != ";")
			{
				//������ʼ���ʽ
				init = parseExpr();
				//��ȡ�ֺ�
				lexer.read(";");
			}
			//��ʼ���ʽΪ��
			else
			{
				lexer.next();
			}

			Expr cond = Expr::newNum(1.0);
			//�������ʽ��Ϊ��
			if (lexer.peek().name() != ";")
			{
				//�����������ʽ
				cond = parseExpr();
				//��ȡ�ֺ�
				lexer.read(";");
			}
			//�������ʽΪ��
			else
			{
				lexer.next();
			}
			
			Expr update = Expr::newNum(1.0);
			//���±��ʽ��Ϊ��
			if (lexer.peek().name() != ")")
			{
				//�������±��ʽ
				update = parseExpr();
				//��ȡ������
				lexer.read(")");
			}
			//���±��ʽΪ��
			else
			{
				lexer.next();
			}
			//��ȡ�������
			lexer.read("{");
			//ѭ���岻Ϊ��
			Expr body;
			if (lexer.peek().name() != "}")
			{
				//��ȡ��������ʽ
				body = parseExpr();
			}
			//��ȡ�Ҵ�����
			lexer.read("}");
			//����forѭ���ڵ�
			return Expr::newFor(init, cond, update, body);
		}
		//break���
		else if (word == "break")
		{
			return Expr::newBreak();
		}
		//continue���
		else if (word == "continue")
		{
		return Expr::newContinue();
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

	//��ֵ���򺬱����ı��ʽ
	if (token.type() == Token::Identifier && !env.funcExist(token.name()))
	{
		size_t cur = lexer.getCurrentIndex();
		//��ȡ������
		string varName = lexer.next().name();
		//������ֵ
		if (lexer.peek().type() == Token::Assign)
		{
			//��ȡ��ֵ����
			string op = lexer.next().name();
			//���츳ֵ�ڵ�
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
		//����Ԫ�ظ�ֵ������Ԫ�صı��ʽ
		else if (lexer.peek().name() == "[")
		{
			/*lexer.next();
			//��ȡ�±���ʽ
			Expr index = parseExpr();
			//��ȡ�ҷ�����
			lexer.read("]");*/
			vector<Expr> v;
			while (lexer.peek().name() == "[")
			{
				lexer.next();
				//��ȡ�±���ʽ
				v.push_back(parseExpr());
				//��ȡ�ҷ�����
				lexer.read("]");
			}

			//����Ԫ�ظ�ֵ
			if (lexer.peek().type() == Token::Assign)
			{
				
				//v.push_back(index);

				//��ȡ��ֵ����
				string op = lexer.next().name();
				//���츳ֵ�ڵ�
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
			//������Ԫ�صı��ʽ
			else
			{
				//�ָ�������
				lexer.setCurrentIndex(cur);
				//��������
			}
		}
		//�������ı��ʽ
		else
		{
			//�ָ�������
			lexer.setCurrentIndex(cur);
			//��������
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
	//����ǰ��������
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
	//����ǰ��������
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
	//�����������ı��ʽ
	else if (name == "(")
	{
		Expr res = parseExpr();
		lexer.read(")");
		return res;
	}
	//����
	else if (name == "-")
	{
		return Expr::newUnary(UnaryOpNode::Minus, parseElem());
	}
	//����
	else if (name == "+")
	{
		return Expr::newUnary(UnaryOpNode::Plus, parseElem());
	}
	//�߼��������
	else if (name == "!")
	{
		return Expr::newUnary(UnaryOpNode::Not, parseElem());
	}
	//�������û����������Ԫ��
	else if (type == Token::Identifier)
	{
		if (lexer.peek().name() == "(") //��������
		{
			lexer.next();
			//�����б�
			vector<Expr> params;
			//�����в���
			if (lexer.peek().name() != ")")
			{
				//��ȡ��һ������
				params.push_back(parseSubexpr());
				//��ȡʣ�µĲ���
				while (lexer.peek().name() != ")")
				{
					lexer.read(",");
					params.push_back(parseSubexpr());
				}
			}
			//��ȡ������
			lexer.read(")");
			return Expr::newFunc(name, params);
		}
		else //����������Ԫ��
		{
			//����Ԫ��
			if (lexer.peek().name() == "[")
			{
				vector<Expr> v;
				while (lexer.peek().name() == "[")
				{
					lexer.next();
					//��ȡ�±���ʽ
					v.push_back(parseExpr());
					//��ȡ�ҷ�����
					lexer.read("]");
				}
				//��������Ԫ�ؽڵ�
				return Expr::newArrElem(name, v);
			}
			//����
			else
			{
				//��������ڵ�
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