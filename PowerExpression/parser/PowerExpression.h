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
	PowerExpression: ���ʽ��������

	����ӿ�:
	PowerExpression(): Ĭ�Ϲ��캯��
		�������ʽ����

	PowerExpression(const string& expr, const FunctionManager &funcManager = FunctionManager()): ���캯��
		expr: ���ʽ�ַ���
		funcManager: ����������(����ָ��������Ĭ�Ϻ���������)

	PowerExpression& setExpr(const string& expr): ���ñ��ʽ�ַ���
		expr: ���ʽ�ַ���
		����ֵ: ��������

	PowerExpression& setFuncManager(const FunctionManager &funcManager): ���ú���������
		funcManager: ����������
		����ֵ: ��������

	string toString(): ��ȡ�����﷨�����ַ�����ʾ�������﷨�������׳��쳣

	double eval(): �Ա��ʽ��ֵ�������﷨���������ʱ�������׳��쳣
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
	std::string expr; //���ʽ�ַ���
	ExpressionLexer lexer; //���ʽ������
	Expr root; //�����﷨���ĸ��ڵ�

	bool exprUpdate, funcUpdate; //�ڲ�״̬����
	
	//���л���
	Environment env;

	void lex() { lexer.lex(expr); } //�ʷ�����
	void buildAST(); //���������﷨��
	void update(); //���·���

	//�ݹ��½��﷨����
	Expr parseExpr();
	Expr parseSubexpr();
	Expr parseLogicalExpr();
	Expr parseCmpexpr();
	Expr parseTerm();
	Expr parseFactor();
	Expr parseElem();
};