#include <iostream>
#include <Windows.h>
#include "Test.h"

using namespace std;

double Square(double p[]) { return p[0] * p[0]; }

int main()
{
	TestExpressionLexer();
	TestExpressionLexer_E();
	TestAST();
	TestAST_E();
	TestASTEval();
	TestASTEval_E();
	cout << endl;

	FunctionManager funcMgr;
	funcMgr.add("square", Square, 1);

	try
	{
		cout << PowerExpression().setExpr("1+square(3)").setFuncManager(funcMgr).eval() << endl;
	}
	catch (ParseError& e)
	{
		cout << "Parse error: " << e.info() << endl;
	}
	catch (RuntimeError& e)
	{
		cout << "Runtime error: " << e.info() << endl;
	}


	system("pause");
	return 0;
}