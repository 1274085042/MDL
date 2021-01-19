#include "zero_matrix.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;

void TestZeroMatrix()
{
	cout << "Test zero matrix case 1..." << endl;
	static_assert(IsMatrix<ZeroMatrix<int, CheckDevice>>, "Test Error");
	static_assert(IsMatrix<ZeroMatrix<int, CheckDevice> &>, "Test Error");
	static_assert(IsMatrix<ZeroMatrix<int, CheckDevice> &&>, "Test Error");
	static_assert(IsMatrix<const ZeroMatrix<int, CheckDevice>>, "Test Error");
	static_assert(IsMatrix<const ZeroMatrix<int, CheckDevice> &>, "Test Error");
	static_assert(IsMatrix<const ZeroMatrix<int, CheckDevice> &&>, "Test Error");

	auto zm = ZeroMatrix<int, CheckDevice>(10, 20);

	assert(zm.RowNum() == 10);
	assert(zm.ColNum() == 20);

	cout << "done" << endl;
}

int main()
{
	TestZeroMatrix();
	return 0;
}