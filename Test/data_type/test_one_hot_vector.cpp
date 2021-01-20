#include "one_hot_vector.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;

void TestOneHotVector1()
{
	cout << "Test one-hot vector case 1..." << endl;
	static_assert(IsMatrix<OneHotVector<int, CheckDevice>>, "Test Error");
	static_assert(IsMatrix<OneHotVector<int, CheckDevice> &> , "Test Error");
	static_assert(IsMatrix<OneHotVector<int, CheckDevice> &&>, "Test Error");
	static_assert(IsMatrix<const OneHotVector<int, CheckDevice>>, "Test Error");
	static_assert(IsMatrix<const OneHotVector<int, CheckDevice> &>, "Test Error");
	static_assert(IsMatrix<const OneHotVector<int, CheckDevice> &&>, "Test Error");

	auto onehot = OneHotVector<int, CheckDevice>(100, 37);
	assert(onehot.RowNum() == 1);
	assert(onehot.ColNum() == 100);
	assert(onehot.HosPos() == 37);

	cout << "done" << endl;
}

void TestOneHotVector2()
{
	cout << "Test one-hot vector case 2..." << endl;

	auto onehot1 = OneHotVector<int, CheckDevice>(100, 37);
	auto onehot2 = OneHotVector<float, CheckDevice>(50, 16);

	cout << "done" << endl;
}

int main()
{
	TestOneHotVector1();
	TestOneHotVector2();
	return 0;
}