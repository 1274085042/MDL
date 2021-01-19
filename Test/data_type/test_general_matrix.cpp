//#include "scalar.h"
#include "cpu_matrix.h"
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;

void TestMatrix1()
{
	cout << "Test general matrix case 1..." << endl;
	static_assert(IsMatrix<Matrix<CheckElement, CheckDevice>>, "Test Error");
	static_assert(IsMatrix<Matrix<CheckElement, CheckDevice>&>, "Test Error");
	static_assert(IsMatrix<Matrix<CheckElement, CheckDevice>&&>, "Test Error");
	static_assert(IsMatrix<const Matrix<CheckElement, CheckDevice>&>, "Test Error");
	static_assert(IsMatrix<const Matrix<CheckElement, CheckDevice>&&>, "Test Error");

	Matrix<CheckElement, CheckDevice> m;
	assert(m.RowNum() == 0);
	assert(m.ColNum() == 0);

	m = Matrix<CheckElement, CheckDevice>(10, 20); //调用编译器提供的赋值运算符
	assert(m.RowNum() == 10);
	assert(m.ColNum() == 20);

	int v = 0;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			m.SetValue(i, j, float(v++));
		}
	}

	const Matrix<CheckElement, CheckDevice> m1 = m;
	v = 0;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			assert(m1(i, j) == float(v++));
		}
	}

	auto m2 = m.SubMatrix(3, 7, 5, 15);
	for (size_t i = 0; i < m2.RowNum(); i++)
	{
		for (size_t j = 0; j < m2.ColNum(); j++)
		{
			assert(m2(i, j) == m(i + 3, j + 5));
		}
	}

	cout << "done" << endl;
}

void TestMatrix2()
{
	cout << "Test general matrix case 2..." << endl;

	auto m1 = Matrix<CheckElement, CheckDevice>(10, 20);
	int v = 0;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			m1.SetValue(i, j, float(v++));
		}
	}

	auto m2 = Matrix<CheckElement, CheckDevice>(3, 7);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			m2.SetValue(i, j, float(v++));
		}
	}

	cout << "done" << endl;
}

int main()
{
	TestMatrix1();
	TestMatrix2();
	return 0;
}