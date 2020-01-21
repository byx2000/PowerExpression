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
		cout << PowerExpression().setExpr("i = 1, j = 1, s = 0, while(i <= 100){while(j <= 200){if(i * j % 10 == 0){s += (i + j), j += 1, break}, j += 1}, if(i % 17 != 0){i += 1, continue}, i += 1}, s").setFuncManager(funcMgr).eval() << endl;
	}
	catch (ParseError& e)
	{
		cout << "Parse error: " << e.info() << endl;
	}
	catch (RuntimeError& e)
	{
		cout << "Runtime error: " << e.info() << endl;
	}

	/*string expr = "i = 0, s = 0, while (i <= 5){if (i == 3){break},s += i, i += 1 }, s";
	ExpressionLexer lexer(expr);

	Token token;
	while ((token = lexer.next()).type() != Token::End)
	{
		cout << token.toString() << endl;
	}*/


	system("pause");
	return 0;
}