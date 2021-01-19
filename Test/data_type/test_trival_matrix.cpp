#include "scalar.h"
#include "trival_matrix.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;

void TestTrivalMatrix1()
{
	cout << "Test trival matrix case 1..." << endl;
	static_assert(IsMatrix<TrivalMatrix<int, CheckDevice, Scalar<int, DeviceTags::CPU>>>, "Test Error");
	static_assert(IsMatrix<TrivalMatrix<int, CheckDevice, Scalar<int, DeviceTags::CPU>> &>, "Test Error");
	static_assert(IsMatrix<TrivalMatrix<int, CheckDevice, Scalar<int, DeviceTags::CPU>> &&>, "Test Error");
	static_assert(IsMatrix<const TrivalMatrix<int, CheckDevice, Scalar<int, DeviceTags::CPU>> &>, "Test Error");
	static_assert(IsMatrix<const TrivalMatrix<int, CheckDevice, Scalar<int, DeviceTags::CPU>> &&>, "Test Error");

	auto tm = MakeTrivalMatrix<int, CheckDevice>(10, 20, 100);
	assert(tm.RowNum() == 10);
	assert(tm.ColNum() == 20);

	cout << "done" << endl;
}

void TestTrivalMatrix2()
{
	cout << "Test trival matrix case 2..." << endl;
	auto tm1 = MakeTrivalMatrix<int, CheckDevice>(100, 10, 14);
	auto tm2 = MakeTrivalMatrix<int, CheckDevice>(10, 20, 35);
	cout << "done" << endl;
}

int main()
{
	TestTrivalMatrix1();
	TestTrivalMatrix2();
	return 0;
}