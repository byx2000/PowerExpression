#pragma once

#include <cmath>
#include <ctime>
#include <algorithm>
#include <string>
#include <unordered_map>

/*数学函数原型：返回值为double，参数为double数组*/
typedef double(*MathFunc)(double p[]);

/*
	函数管理器：管理自定义函数集合

	对外接口：
	FunctionManager(): 构造默认函数管理器，支持常用数学函数

	bool add(const string& name, const MathFunc funcAddr, int cParam): 添加自定义函数
		name: 自定义函数名，不可与已有的函数冲突
		funcAddr: 自定义函数指针
		cParam: 自定义函数参数个数
		返回值: 添加成功返回true，失败返回false

	bool remove(const string& name): 删除自定义函数，可删除自定义函数
		name: 函数的名称
		返回值： 删除成功返回true，失败返回false

	bool exist(const string& name) const: 判断函数是否存在
		name: 函数的名称
		返回值： 存在返回true，不存在返回false

	size_t paramCount(const std::string& name) const: 返回指定函数的参数

	double invoke(const string& name, double p[]): 调用函数
		name: 函数的名称
		p: 参数列表
*/
class FunctionManager
{
public:
	FunctionManager();
	bool add(const std::string& name, const MathFunc funcAddr, int cParam);
	bool remove(const std::string& name);
	bool exist(const std::string& name) const { return funcTable.count(name) != 0; };
	size_t paramCount(const std::string& name) const;
	double invoke(const std::string& name, double p[]) const;

private:
	class FuncInfo
	{
	public:
		FuncInfo() : funcAddr(NULL), cParam(0) {}
		FuncInfo(MathFunc funcAddr, int cParam) : funcAddr(funcAddr), cParam(cParam) {}
		MathFunc funcAddr;
		int cParam;
	};

	mutable std::unordered_map<std::string, FuncInfo> funcTable;
};