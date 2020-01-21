#pragma once

/*
	��ͷ�ļ������˳����﷨�������нڵ�����

	�����﷨���ڵ�Ļ���ΪASTNode�࣬����toString��eval�����鷽��
	Expr��ΪASTNode�İ�װ�࣬Ŀ����Ϊ���Զ������ڴ�
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

/*���л���*/
class Environment
{
public:
	
	void clearVarTable() { varTable.clear(); }
	void clearArrTable() { arrTable.clear(); }
	void resetFuncMgr() { funcMgr = FunctionManager(); }
	void clear() { varTable.clear(); arrTable.clear(); }

	/*����*/
	bool varExist(const std::string& varName) { return varTable.count(varName) != 0; }
	void setVarVal(const std::string& varName, double val) { varTable[varName] = val; }
	double getVarVal(const std::string& varName) { return varTable[varName]; }

	/*����*/
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

	/*����*/
	void setFuncMgr(const FunctionManager& funcMgr) { this->funcMgr = funcMgr; }
	bool funcExist(const std::string& funcName) { return funcMgr.exist(funcName); }
	size_t getFuncParamCount(const std::string& funcName) { return funcMgr.paramCount(funcName); }
	double funcInvoke(const std::string& funcName, double p[]) { return funcMgr.invoke(funcName, p); }

private:
	std::unordered_map<std::string, double> varTable; //�������ű�
	std::unordered_map<std::string, std::map<std::vector<int>, double> > arrTable; //������ű�
	FunctionManager funcMgr; //����������
};

/*�����﷨���ڵ����*/
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

/*�ڵ��װ��*/
class Expr
{
public:
	Expr(); //�����
	~Expr() { if (p != NULL && --p->use == 0)	delete p; }
	Expr(const Expr& expr) { p = expr.p; p->use++; } //���ƹ��캯��
	Expr operator=(const Expr& expr); //��ֵ
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
	static Expr newBreak();
	static Expr newContinue();
	
	std::string toString() const { return p->toString(); }
	double eval(Environment& env) const { return p->eval(env); }

private:
	Expr(ASTNode* p) : p(p) {};
	ASTNode* p;
};

/*�����*/
class EmptyNode : public ASTNode
{
	friend class Expr;

	EmptyNode() {};
	std::string toString() const { return ""; }
	double eval(Environment& env) const { return 0.0; }
};

/*������*/
class NumNode : public ASTNode
{
	friend class Expr;

	NumNode(double val) : val(val) {}
	std::string toString() const;
	double eval(Environment& env) const { return val; }

	double val;
};

/*����*/
class VarNode : public ASTNode
{
	friend class Expr;

	VarNode(const std::string& varName)
		: varName(varName) {}
	std::string toString() const { return varName; }
	double eval(Environment& env) const;

	std::string varName;
};

/*����Ԫ��*/
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

/*һԪ�����*/
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

/*��Ԫ�����*/
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

/*����*/
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

/*���ű��ʽ*/
class CommaNode : public ASTNode
{
	friend class Expr;

	CommaNode(const std::vector<Expr>& subexpr) : subexpr(subexpr) {}
	std::string toString() const;
	double eval(Environment& env) const;

	std::vector<Expr> subexpr;
};

/*������ֵ*/
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

/*����Ԫ�ظ�ֵ*/
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

/*whileѭ��*/
class WhileNode : public ASTNode
{
	friend class Expr;

	WhileNode(const Expr& cond, const Expr& body) : cond(cond), body(body) {}
	std::string toString() const;
	double eval(Environment& env) const;

	Expr cond;
	Expr body;
};

/*if���*/
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

/*forѭ��*/
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

/*break���*/
class BreakNode : public ASTNode
{
	friend class Expr;

	BreakNode() {}
	std::string toString() const { return "break"; }
	double eval(Environment& env) const
	{
		throw std::string("break");
		return 0;
	}
};

/*continue���*/
class ContinueNode : public ASTNode
{
	friend class Expr;

	ContinueNode() {}
	std::string toString() const { return "continue"; }
	double eval(Environment& env) const
	{
		throw std::string("continue");
		return 0;
	}
};