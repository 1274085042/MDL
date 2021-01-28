#include "scalar.h"
#include "cpu_matrix.h"
#include "duplicate.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;
void TestDuplicate1()
{
	cout << "Test duplicate(matrix) case 1 ..." << endl;
	static_assert(IsMatrixBatch<Duplicate<Matrix<CheckElement, CheckDevice>>>, "Test Error");
	static_assert(IsMatrixBatch< Duplicate<Matrix<CheckElement, CheckDevice>>& >, "Test Error");
	static_assert(IsMatrixBatch< Duplicate<Matrix<CheckElement, CheckDevice>>&& >, "Test Error");
	static_assert(IsMatrixBatch<const Duplicate<Matrix<CheckElement, CheckDevice>> >, "Test Error");
	static_assert(IsMatrixBatch<const Duplicate<Matrix<CheckElement, CheckDevice>>& >, "Test Error");
	static_assert(IsMatrixBatch<const Duplicate<Matrix<CheckElement, CheckDevice>>&& >, "Test Error");

	auto m1 = Matrix<CheckElement, CheckDevice>(10, 20);
	int v = 0;
	for (size_t i = 0; i < 10; i++)
	{
		for (size_t j = 0; j < 20; j++)
		{
			m1.SetValue(i, j, float(v++));
		}
	}

	auto dm1 = MakeDuplicate(13, m1);
	assert(dm1.BatchNum() == 13);
	assert(dm1.RowNum() == 10);
	assert(dm1.ColNum() == 20);

	cout << "done" << endl;
}

void TestDuplicate2()
{
	cout << "Test duplicate(scalar) case 2 ..." << endl;
	static_assert(IsScalarBatch<Duplicate<Scalar<CheckElement, CheckDevice>>>, "Test Error");
	static_assert(IsScalarBatch<Duplicate<Scalar<CheckElement, CheckDevice>> &>, "Test Error");
	static_assert(IsScalarBatch<Duplicate<Scalar<CheckElement, CheckDevice>> &&>, "Test Error");
	static_assert(IsScalarBatch<const Duplicate<Scalar<CheckElement, CheckDevice>>>, "Test Error");
	static_assert(IsScalarBatch<const Duplicate<Scalar<CheckElement, CheckDevice>> &>, "Test Error");
	static_assert(IsScalarBatch<const Duplicate<Scalar<CheckElement, CheckDevice>> &&>, "Test Error");

	auto ds1 = Duplicate<Scalar<CheckElement, CheckDevice>>(3, 13);
	assert(ds1.BatchNum() == 13);
	
	cout << "done" << endl;
}

int main()
{
	TestDuplicate1();
	TestDuplicate2();
	return 0;
}