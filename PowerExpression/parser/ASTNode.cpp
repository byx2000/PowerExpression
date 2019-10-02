#include "ASTNode.h"

using namespace std;

//判断浮点数相等
static bool equal(double a, double b) { return fabs(a - b) <= 1e-6; }

string NumNode::toString() const
{
	stringstream ss;
	ss << val;
	return ss.str();
}

Expr::Expr()
{
	p = new EmptyNode();;
}

Expr Expr::operator=(const Expr & expr)
{
	if (expr.p != NULL)
	{
		expr.p->use++;
	}
	
	if (p != NULL && --p->use == 0)
	{
		delete p;
	}
	p = expr.p;
	return *this;
}

Expr Expr::newEmpty() { return Expr(new EmptyNode()); }

Expr Expr::newNum(double val) { return Expr(new NumNode(val)); }

Expr Expr::newVar(const std::string & varName) { return Expr(new VarNode(varName)); }

Expr Expr::newArrElem(const std::string & arrName, const std::vector<Expr> & index) { return Expr(new ArrElemNode(arrName, index)); }

Expr Expr::newUnary(int type, const Expr & expr) { return Expr(new UnaryOpNode((UnaryOpNode::Type)type, expr)); }

Expr Expr::newBinary(int type, const Expr & left, const Expr & right) { return Expr(new BinaryOpNode((BinaryOpNode::Type)type, left, right)); }

Expr Expr::newFunc(const std::string & funcName, const std::vector<Expr>& params) { return Expr(new FuncNode(funcName, params)); }

Expr Expr::newComma(const std::vector<Expr>& subexpr) { return Expr(new CommaNode(subexpr)); }

Expr Expr::newVarAssign(const std::string & varName, const Expr & expr) { return Expr(new VarAssignNode(varName, expr)); }

Expr Expr::newArrElemAssign(const std::string & arrName, const std::vector<Expr> & index, const Expr & val) { return Expr(new ArrElemAssignNode(arrName, index, val)); }

Expr Expr::newWhile(const Expr & cond, const Expr & body) { return Expr(new WhileNode(cond, body)); }

Expr Expr::newIf(const Expr & cond, const Expr & tExpr, const Expr & fExpr) { return Expr(new IfNode(cond, tExpr, fExpr)); }

Expr Expr::newFor(const Expr & init, const Expr & cond, const Expr & update, const Expr & body) { return Expr(new ForNode(init, cond, update, body)); }

string BinaryOpNode::name[14] = { "+", "-", "*", "/", "^", "%", "==", "!=", "<", ">", "<=", ">=", "&&", "||" };

string BinaryOpNode::toString() const
{
	string res = name[type];
	res += "(";
	res += left.toString();
	res += ",";
	res += right.toString();
	res += ")";
	return res;
}

double BinaryOpNode::eval(Environment& env) const
{
	if (type == Add)
	{
		return left.eval(env) + right.eval(env);
	}
	else if (type == Sub)
	{
		return left.eval(env) - right.eval(env);
	}
	else if (type == Mul)
	{
		return left.eval(env) * right.eval(env);
	}
	else if (type == Div)
	{
		return left.eval(env) / right.eval(env);
	}
	else if (type == Pow)
	{
		return pow(left.eval(env), right.eval(env));
	}
	else if (type == Equal)
	{
		return (double)(equal(left.eval(env), right.eval(env)));
	}
	else if (type == Unequal)
	{
		return (double)(!equal(left.eval(env), right.eval(env)));
	}
	else if (type == Rem)
	{
		return (double)((int)left.eval(env) % (int)right.eval(env));
	}
	else if (type == LessThan)
	{
		return (double)(left.eval(env) < right.eval(env));
	}
	else if (type == BiggerThan)
	{
		return (double)(left.eval(env) > right.eval(env));
	}
	else if (type == LessThanEqual)
	{
		return (double)(left.eval(env) <= right.eval(env));
	}
	else if (type == BiggerThanEqual)
	{
		return (double)(left.eval(env) >= right.eval(env));
	}
	else if (type == And)
	{
		return (double)((!equal(left.eval(env), 0.0)) && (!equal(right.eval(env), 0.0)));
	}
	else if (type == Or)
	{
		return (double)((!equal(left.eval(env), 0.0)) || (!equal(right.eval(env), 0.0)));
	}
	else
	{
		return NAN;
	}
}

string UnaryOpNode::name[3] = { "+", "-", "!" };

string UnaryOpNode::toString() const
{
	string res = name[type];
	res += "(";
	res += expr.toString();
	res += ")";
	return res;
}

double UnaryOpNode::eval(Environment& env) const
{
	if (type == Plus)
	{
		return expr.eval(env);
	}
	else if (type == Minus)
	{
		return -expr.eval(env);
	}
	else if (type == Not)
	{
		double val = expr.eval(env);
		if (equal(val, 0.0))
		{
			return 1.0;
		}
		else
		{
			return 0.0;
		}
	}
	else
	{
		return NAN;
	}
}

string FuncNode::toString() const
{
	string res = funcName;
	res += "(";
	if (params.size() > 0)
	{
		res += params[0].toString();
		for (size_t i = 1; i < params.size(); ++i)
		{
			res += ",";
			res += params[i].toString();
		}
	}
	res += ")";
	return res;
}

double FuncNode::eval(Environment& env) const
{
	//函数不存在
	if (!env.funcExist(funcName))
	{
		throw RuntimeError("Function \"" + funcName + "\" is not exist.");
	}
	//函数参数个数不匹配
	else if (env.getFuncParamCount(funcName) != params.size())
	{
		throw RuntimeError(funcName + ": the number of params is incurrect.");
	}

	double* p = new double[params.size()];
	for (size_t i = 0; i < params.size(); ++i)
	{
		p[i] = params[i].eval(env);
	}
	double res = env.funcInvoke(funcName, p);
	delete[] p;
	return res;
}

string CommaNode::toString() const
{
	string res = ",(";
	if (subexpr.size() > 0)
	{
		res += subexpr[0].toString();
		for (size_t i = 1; i < subexpr.size(); ++i)
		{
			res += ",";
			res += subexpr[i].toString();
		}
	}
	res += ")";
	return res;
}

double CommaNode::eval(Environment& env) const
{
	double res = 0.0;
	for (size_t i = 0; i < subexpr.size(); ++i)
	{
		res = subexpr[i].eval(env);
	}
	return res;
}

string VarAssignNode::toString() const
{
	string res = "=(";
	res += varName;
	res += ",";
	res += expr.toString();
	res += ")";
	return res;
}

double VarAssignNode::eval(Environment& env) const
{
	double val = expr.eval(env);
	env.setVarVal(varName, val);
	return val;
}

double VarNode::eval(Environment& env) const
{
	if (!env.varExist(varName))
	{
		throw RuntimeError("Undefine variable: " + varName);
	}
	return env.getVarVal(varName);
}

string WhileNode::toString() const
{
	string res = "while(";
	res += cond.toString();
	res += ",";
	res += body.toString();
	res += ")";
	return res;
}

double WhileNode::eval(Environment& env) const
{
	double res = 0.0;
	while (!equal(cond.eval(env), 0.0))
	{
		res = body.eval(env);
	}
	return res;
}

string IfNode::toString() const
{
	string res = "if(";
	res += cond.toString();
	res += ",";
	res += tExpr.toString();
	res += ",";
	res += fExpr.toString();
	res += ")";
	return res;
}

double IfNode::eval(Environment& env) const
{
	if (!equal(cond.eval(env), 0.0))
	{
		return tExpr.eval(env);
	}
	else
	{
		return fExpr.eval(env);
	}
}

string ForNode::toString() const
{
	string res = "for(";
	res += init.toString();
	res += ",";
	res += cond.toString();
	res += ",";
	res += update.toString();
	res += ",";
	res += body.toString();
	res += ")";
	return res;
}

double ForNode::eval(Environment& env) const
{
	double res = 0.0;
	for (init.eval(env); !equal(cond.eval(env), 0.0); update.eval(env))
	{
		res = body.eval(env);
	}
	return res;
}

string ArrElemNode::toString() const
{
	string res = "arr(";
	res += arrName;
	for (size_t i = 0; i < index.size(); ++i)
	{
		res += ",";
		res += index[i].toString();
	}
	res += ")";
	return res;
}

double ArrElemNode::eval(Environment& env) const
{
	vector<int> v(index.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		v[i] = (int)index[i].eval(env);
	}

	if (!env.arrElemExist(arrName, v))
	{
		string info("Undefine array element: ");
		info += arrName;
		for (size_t i = 0; i < v.size(); ++i)
		{
			info += "[";
			info += to_string(v[i]);
			info += "]";
		}
		throw RuntimeError(info);
	}
	return env.getArrElemVal(arrName, v);
}

string ArrElemAssignNode::toString() const
{
	string res = "=(";
	res += arrName;
	for (size_t i = 0; i < index.size(); ++i)
	{
		res += ",";
		res += index[i].toString();
	}
	res += ")";
	return res;
}

double ArrElemAssignNode::eval(Environment& env) const
{
	vector<int> v(index.size());
	for (size_t i = 0; i < index.size(); ++i)
	{
		v[i] = (int)index[i].eval(env);
	}
	double x = val.eval(env);
	env.setArrElemVal(arrName, v, x);
	return x;
}