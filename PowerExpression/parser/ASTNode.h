#pragma once

/*
	该头文件声明了抽象语法树的所有节点类型

	抽象语法树节点的基类为ASTNode类，包含toString和eval两个虚方法
	Expr类为ASTNode的包装类，目的是为了自动管理内存
*/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <map>
#include "Error.h"
#include "FunctionManager.h"

/*运行环境*/
class Environment
{
public:
	
	void clearVarTable() { varTable.clear(); }
	void clearArrTable() { arrTable.clear(); }
	void resetFuncMgr() { funcMgr = FunctionManager(); }
	void clear() { varTable.clear(); arrTable.clear(); }

	/*变量*/
	bool varExist(const std::string& varName) { return varTable.count(varName) != 0; }
	void setVarVal(const std::string& varName, double val) { varTable[varName] = val; }
	double getVarVal(const std::string& varName) { return varTable[varName]; }

	/*数组*/
	bool arrElemExist(const std::string& arrName, const std::vector<int>& index)
	{
		if (arrTable.count(arrName) == 0)
		{
			return false;
		}
		return arrTable[arrName].count(index) != 0;
	}
	void setArrElemVal(const std::string& arrName, const std::vector<int>& index, double val) { arrTable[arrName][index] = val; }
	double getArrElemVal(const std::string& arrName, const std::vector<int>& index) { return arrTable[arrName][index]; }

	/*函数*/
	void setFuncMgr(const FunctionManager& funcMgr) { this->funcMgr = funcMgr; }
	bool funcExist(const std::string& funcName) { return funcMgr.exist(funcName); }
	size_t getFuncParamCount(const std::string& funcName) { return funcMgr.paramCount(funcName); }
	double funcInvoke(const std::string& funcName, double p[]) { return funcMgr.invoke(funcName, p); }

private:
	std::unordered_map<std::string, double> varTable; //变量符号表
	std::unordered_map<std::string, std::map<std::vector<int>, double> > arrTable; //数组符号表
	FunctionManager funcMgr; //函数管理器
};

/*抽象语法数节点基类*/
class ASTNode
{
	friend class Expr;
protected:
	ASTNode() : use(1) {}

	virtual std::string toString() const = 0;
	virtual double eval(Environment& env) const = 0;
	virtual ~ASTNode() {};

	int use;
};

/*节点包装类*/
class Expr
{
public:
	Expr(); //空语句
	~Expr() { if (p != NULL && --p->use == 0)	delete p; }
	Expr(const Expr& expr) { p = expr.p; p->use++; } //复制构造函数
	Expr operator=(const Expr& expr); //赋值
	static Expr newEmpty();
	static Expr newNum(double val);
	static Expr newVar(const std::string& varName);
	static Expr newArrElem(const std::string& arrName, const std::vector<Expr>& index);
	static Expr newUnary(int type, const Expr& expr);
	static Expr newBinary(int type, const Expr& left, const Expr& right);
	static Expr newFunc(const std::string& funcName, const std::vector<Expr>& params);
	static Expr newComma(const std::vector<Expr>& subexpr);
	static Expr newVarAssign(const std::string& varName, const Expr& expr);
	static Expr newArrElemAssign(const std::string& arrName, const std::vector<Expr>& index, const Expr& val);
	static Expr newWhile(const Expr& cond, const Expr& body);
	static Expr newIf(const Expr& cond, const Expr& tExpr, const Expr& fExpr);
	static Expr newFor(const Expr& init, const Expr& cond, const Expr& update, const Expr& body);
	
	std::string toString() const { return p->toString(); }
	double eval(Environment& env) const { return p->eval(env); }

private:
	Expr(ASTNode* p) : p(p) {};
	ASTNode* p;
};

/*空语句*/
class EmptyNode : public ASTNode
{
	friend class Expr;

	EmptyNode() {};
	std::string toString() const { return ""; }
	double eval(Environment& env) const { return 0.0; }
};

/*浮点数*/
class NumNode : public ASTNode
{
	friend class Expr;

	NumNode(double val) : val(val) {}
	std::string toString() const;
	double eval(Environment& env) const { return val; }

	double val;
};

/*变量*/
class VarNode : public ASTNode
{
	friend class Expr;

	VarNode(const std::string& varName)
		: varName(varName) {}
	std::string toString() const { return varName; }
	double eval(Environment& env) const;

	std::string varName;
};

/*数组元素*/
class ArrElemNode : public ASTNode
{
	friend class Expr;
	
	ArrElemNode(const std::string& arrName, const std::vector<Expr>& index)
		: arrName(arrName), index(index) {}
	std::string toString() const;
	double eval(Environment& env) const;

	std::string arrName;
	std::vector<Expr> index;
};

/*一元运算符*/
class UnaryOpNode : public ASTNode
{
	friend class Expr;

public:
	enum Type { Plus, Minus, Not };

private:
	static std::string name[3];

	UnaryOpNode(Type type, const Expr& expr) : type(type), expr(expr) {}
	std::string toString() const;
	double eval(Environment& env) const;

	Type type;
	Expr expr;
};

/*二元运算符*/
class BinaryOpNode : public ASTNode
{
	friend class Expr;

public:
	enum Type { Add, Sub, Mul, Div, Pow, Rem, Equal, Unequal, LessThan, BiggerThan, LessThanEqual, BiggerThanEqual, And, Or };

private:
	static std::string name[14];

	BinaryOpNode(Type type, const Expr& left, const Expr& right) : type(type), left(left), right(right) {}
	std::string toString() const;
	double eval(Environment& env) const;

	Type type;
	Expr left;
	Expr right;
};

/*函数*/
class FuncNode : public ASTNode
{
	friend class Expr;

	FuncNode(const std::string& funcName, const std::vector<Expr>& params)
		: funcName(funcName), params(params) {}
	std::string toString() const;
	double eval(Environment& env) const;

	std::string funcName;
	std::vector<Expr> params;
};

/*逗号表达式*/
class CommaNode : public ASTNode
{
	friend class Expr;

	CommaNode(const std::vector<Expr>& subexpr) : subexpr(subexpr) {}
	std::string toString() const;
	double eval(Environment& env) const;

	std::vector<Expr> subexpr;
};

/*变量赋值*/
class VarAssignNode : public ASTNode
{
	friend class Expr;

	VarAssignNode(const std::string& varName, const Expr& expr)
		: varName(varName), expr(expr) {}
	std::string toString() const;
	double eval(Environment& env) const;

	std::string varName;
	Expr expr;
};

/*数组元素赋值*/
class ArrElemAssignNode : public ASTNode
{
	friend class Expr;

	ArrElemAssignNode(const std::string& arrName, const std::vector<Expr>& index, const Expr& val) :
		arrName(arrName), index(index), val(val) {}
	std::string toString() const;
	double eval(Environment& env) const;

	std::string arrName;
	std::vector<Expr> index;
	Expr val;
};

/*while循环*/
class WhileNode : public ASTNode
{
	friend class Expr;

	WhileNode(const Expr& cond, const Expr& body) : cond(cond), body(body) {}
	std::string toString() const;
	double eval(Environment& env) const;

	Expr cond;
	Expr body;
};

/*if语句*/
class IfNode : public ASTNode
{
	friend class Expr;

	IfNode(const Expr& cond, const Expr& tExpr, const Expr& fExpr)
		: cond(cond), tExpr(tExpr), fExpr(fExpr) {}
	std::string toString() const;
	double eval(Environment& env) const;

	Expr cond;
	Expr tExpr;
	Expr fExpr;
};

/*for循环*/
class ForNode : public ASTNode
{
	friend class Expr;

	ForNode(const Expr& init, const Expr& cond, const Expr& update, const Expr& body)
		: init(init), cond(cond), update(update), body(body) {}
	std::string toString() const;
	double eval(Environment& env) const;

	Expr init, cond, update;
	Expr body;
};