#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <unordered_map>
#include "ExpressionLexer.h"
#include "FunctionManager.h"
#include "ASTNode.h"

/*
	PowerExpression: 表达式运算引擎

	对外接口:
	PowerExpression(): 默认构造函数
		创建表达式引擎

	PowerExpression(const string& expr, const FunctionManager &funcManager = FunctionManager()): 构造函数
		expr: 表达式字符串
		funcManager: 函数管理器(若不指定，则用默认函数管理器)

	PowerExpression& setExpr(const string& expr): 设置表达式字符串
		expr: 表达式字符串
		返回值: 自身引用

	PowerExpression& setFuncManager(const FunctionManager &funcManager): 设置函数管理器
		funcManager: 函数管理器
		返回值: 自身引用

	string toString(): 获取抽象语法树的字符串表示，若有语法错误则抛出异常

	double eval(): 对表达式求值，若有语法错误或运行时错误则抛出异常
*/
class PowerExpression
{
public:
	PowerExpression() 
		: exprUpdate(true), funcUpdate(true)  {}
	PowerExpression(const std::string& expr, const FunctionManager &funcManager = FunctionManager())
		: expr(expr), exprUpdate(true), funcUpdate(true) {}
	PowerExpression& setExpr(const std::string& expr);
	PowerExpression& setFuncManager(const FunctionManager &funcManager);
	std::string toString();
	double eval();

private:
	std::string expr; //表达式字符串
	ExpressionLexer lexer; //表达式单词流
	Expr root; //抽象语法树的根节点

	bool exprUpdate, funcUpdate; //内部状态变量
	
	//运行环境
	Environment env;

	void lex() { lexer.lex(expr); } //词法分析
	void buildAST(); //构建抽象语法树
	void update(); //重新分析

	//递归下降语法分析
	Expr parseExpr();
	Expr parseSubexpr();
	Expr parseLogicalExpr();
	Expr parseCmpexpr();
	Expr parseTerm();
	Expr parseFactor();
	Expr parseElem();
};