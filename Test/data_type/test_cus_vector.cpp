#include "scalar.h"
#include "cpu_matrix.h"
#include "cus_vector.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;

void TestCusVector1()
{
	cout << "Test CusVector(matrix) case 1 ..." << endl;
	static_assert(IsMatrixBatch<CusVector<Matrix<CheckElement, CheckDevice>>>, "Test Error");
	static_assert(IsMatrixBatch<CusVector<Matrix<CheckElement, CheckDevice>> &>, "Test Error");
	static_assert(IsMatrixBatch<CusVector<Matrix<CheckElement, CheckDevice>> &&>, "Test Error");
	static_assert(IsMatrixBatch<const CusVector<Matrix<CheckElement, CheckDevice>>>, "Test Error");
	static_assert(IsMatrixBatch<const CusVector<Matrix<CheckElement, CheckDevice>> &>, "Test Error");
	static_assert(IsMatrixBatch<const CusVector<Matrix<CheckElement, CheckDevice>> &&>, "Test Error");

	auto MatrixBatch1 = CusVector<Matrix<CheckElement, CheckDevice>>(10, 20);
	assert(MatrixBatch1.BatchNum() == 0);
	assert(MatrixBatch1.Empty());

	int v = 0;
	auto m1 = Matrix<CheckElement, CheckDevice>(10, 20);
	auto m2 = Matrix<CheckElement, CheckDevice>(10, 20);
	auto m3 = Matrix<CheckElement, CheckDevice>(10, 20);
	
	for (size_t i = 0; i < 10; i++)
	{
		for (size_t j = 0; j < 20; j++)
		{
			m1.SetValue(i, j, (float)(v++));
			m2.SetValue(i, j, (float)(v++));
			m3.SetValue(i, j, (float)(v++));
		}
	}

	MatrixBatch1.Push_Back(m1);
	MatrixBatch1.Push_Back(m2);
	MatrixBatch1.Push_Back(m3);

	assert(MatrixBatch1.BatchNum() == 3);
	assert(!MatrixBatch1.Empty());

	for (size_t i = 0; i < 10; i++)
	{
		for (size_t j = 0; j < 20; j++)
		{
			assert(MatrixBatch1[0](i, j) == m1(i, j));
			assert(MatrixBatch1[1](i, j) == m2(i, j));
			assert(MatrixBatch1[2](i, j) == m3(i, j));
		}
	}
	cout << "done" << endl;
}

void TestCusVector2()
{
	cout << "Test CusVector(scalar) case 2 ..." << endl;
	static_assert(IsScalarBatch<CusVector<Scalar<CheckElement, CheckDevice>>>, "Test Error");
	static_assert(IsScalarBatch<CusVector<Scalar<CheckElement, CheckDevice>> &>, "Test Error");
	static_assert(IsScalarBatch<CusVector<Scalar<CheckElement, CheckDevice>> &&>, "Test Error");
	static_assert(IsScalarBatch<const CusVector<Scalar<CheckElement, CheckDevice>>>, "Test Error");
	static_assert(IsScalarBatch<const CusVector<Scalar<CheckElement, CheckDevice>>&>, "Test Error");
	static_assert(IsScalarBatch<const CusVector<Scalar<CheckElement, CheckDevice>>&&>, "Test Error");

	auto ScalarBatch1 = CusVector<Scalar<CheckElement, CheckDevice>>();
	assert(ScalarBatch1.BatchNum() == 0);
	assert(ScalarBatch1.Empty());

	ScalarBatch1.Push_Back(3);
	ScalarBatch1.Push_Back(8);
	ScalarBatch1.Push_Back(2);

	assert(ScalarBatch1.BatchNum() == 3);
	assert(!ScalarBatch1.Empty());

	assert(ScalarBatch1[0].Value() == 3);
	assert(ScalarBatch1[1].Value() == 8);
	assert(ScalarBatch1[2].Value() == 2);

	cout << "done" << endl;
}

void TestCusVector3()
{
	cout << "Test CusVector case 3 ..." << endl;

	vector<Matrix<CheckElement, CheckDevice>> check1;
	check1.emplace_back(10, 16);
	check1.emplace_back(10, 16);
	check1.emplace_back(10, 16);

	auto tmp1 = MakeCusVector(check1.begin(), check1.end());
	assert(tmp1.RowNum() == 10);
	assert(tmp1.ColNum() == 16);
	assert(tmp1.BatchNum() == 3);

	vector<Scalar<CheckElement, CheckDevice>> check2{3,5,8,6};
	auto tmp2 = MakeCusVector(check2.begin(), check2.end());
	assert(tmp2.BatchNum() == 4);
	assert(tmp2[0].Value() == 3);
	assert(tmp2[1].Value() == 5);
	assert(tmp2[2].Value() == 8);
	assert(tmp2[3].Value() == 6);
	cout << "done" << endl;
}

int main()
{
	TestCusVector1();
	TestCusVector2();
	TestCusVector3();
	return 0;
}