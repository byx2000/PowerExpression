#pragma once

#include <cmath>
#include <ctime>
#include <algorithm>
#include <string>
#include <unordered_map>

/*��ѧ����ԭ�ͣ�����ֵΪdouble������Ϊdouble����*/
typedef double(*MathFunc)(double p[]);

/*
	�����������������Զ��庯������

	����ӿڣ�
	FunctionManager(): ����Ĭ�Ϻ�����������֧�ֳ�����ѧ����

	bool add(const string& name, const MathFunc funcAddr, int cParam): ����Զ��庯��
		name: �Զ��庯���������������еĺ�����ͻ
		funcAddr: �Զ��庯��ָ��
		cParam: �Զ��庯����������
		����ֵ: ��ӳɹ�����true��ʧ�ܷ���false

	bool remove(const string& name): ɾ���Զ��庯������ɾ���Զ��庯��
		name: ����������
		����ֵ�� ɾ���ɹ�����true��ʧ�ܷ���false

	bool exist(const string& name) const: �жϺ����Ƿ����
		name: ����������
		����ֵ�� ���ڷ���true�������ڷ���false

	size_t paramCount(const std::string& name) const: ����ָ�������Ĳ���

	double invoke(const string& name, double p[]): ���ú���
		name: ����������
		p: �����б�
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