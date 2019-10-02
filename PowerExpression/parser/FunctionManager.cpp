#include "FunctionManager.h"

using namespace std;

/*内置函数*/

static double Pi(double p[]) { return 3.14159; }
static double E(double p[]) { return 2.71828; }
static double Sin(double p[]) { return sin(p[0]); }
static double Cos(double p[]) { return cos(p[0]); }
static double Tan(double p[]) { return tan(p[0]); }
static double Arcsin(double p[]) { return asin(p[0]); }
static double Arccos(double p[]) { return acos(p[0]); }
static double Arctan(double p[]) { return atan(p[0]); }
static double Ln(double p[]) { return log(p[0]); }
static double Log(double p[]) { return log(p[1]) / log(p[0]); }
static double Exp(double p[]) { return exp(p[0]); }
static double Sinh(double p[]) { return sinh(p[0]); }
static double Cosh(double p[]) { return cosh(p[0]); }
static double Tanh(double p[]) { return tanh(p[0]); }
static double Arcsinh(double p[]) { return asinh(p[0]); }
static double Arccosh(double p[]) { return acosh(p[0]); }
static double Arctanh(double p[]) { return atanh(p[0]); }
static double Max(double p[]) { return max(p[0], p[1]); }
static double Min(double p[]) { return min(p[0], p[1]); }
static double Abs(double p[]) { return fabs(p[0]); }
static double Pow(double p[]) { return pow(p[0], p[1]); }
static double Sqrt(double p[]) { return sqrt(p[0]); }
static double Int(double p[]) { return (int)p[0]; }
static double Floor(double p[]) { return floor(p[0]); }
static double Ceil(double p[]) { return ceil(p[0]); }
static double Rand(double p[]) { return rand(); }

FunctionManager::FunctionManager()
{
	srand((unsigned)time(NULL));

	//添加内置函数
	add("pi", Pi, 0);
	add("e", E, 0);
	add("sin", Sin, 1);
	add("cos", Cos, 1);
	add("tan", Tan, 1);
	add("arcsin", Arcsin, 1);
	add("arccos", Arccos, 1);
	add("arctan", Arctan, 1);
	add("ln", Ln, 1);
	add("log", Log, 2);
	add("exp", Exp, 1);
	add("sinh", Sinh, 1);
	add("cosh", Cosh, 1);
	add("tanh", Tanh, 1);
	add("arcsinh", Arcsinh, 1);
	add("arccosh", Arccosh, 1);
	add("arctanh", Arctanh, 1);
	add("max", Max, 2);
	add("min", Min, 2);
	add("abs", Abs, 1);
	add("pow", Pow, 2);
	add("sqrt", Sqrt, 1);
	add("int", Int, 1);
	add("floor", Floor, 1);
	add("ceil", Ceil, 1);
	add("rand", Rand, 0);
}

bool FunctionManager::add(const string & name, const MathFunc funcAddr, int cParam)
{
	if (funcTable.count(name) != 0 || cParam < 0)
	{
		return false;
	}
	funcTable[name] = FuncInfo(funcAddr, cParam);
	return true;
}

bool FunctionManager::remove(const string & name)
{
	if (funcTable.count(name) == 0)
	{
		return false;
	}
	funcTable.erase(name);
	return true;
}

size_t FunctionManager::paramCount(const string & name) const
{
	return funcTable[name].cParam;
}

double FunctionManager::invoke(const string & name, double p[]) const
{
	if (funcTable.count(name) == 0)
	{
		return NAN;
	}
	return funcTable[name].funcAddr(p);
}