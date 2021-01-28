#include "scalar_batch.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;

void TestScalarBatch()
{
	cout << "Test scalar batch case 1 ..." << endl;
	static_assert(IsScalarBatch<Batch<CheckElement, CheckDevice, CategoryTags::Scalar>>, "Test Error");
	static_assert(IsScalarBatch<Batch<CheckElement, CheckDevice, CategoryTags::Scalar> &>, "Test Error");
	static_assert(IsScalarBatch<Batch<CheckElement, CheckDevice, CategoryTags::Scalar> &&>, "Test Error");
	static_assert(IsScalarBatch<const Batch<CheckElement, CheckDevice, CategoryTags::Scalar>>, "Test Error");
	static_assert(IsScalarBatch<const Batch<CheckElement, CheckDevice, CategoryTags::Scalar> &>, "Test Error");
	static_assert(IsScalarBatch<const Batch<CheckElement, CheckDevice, CategoryTags::Scalar> &&>, "Test Error");

	Batch<CheckElement, CheckDevice, CategoryTags::Scalar> bSca1;
	assert(bSca1.BatchNum() == 0);

	bSca1 = Batch<CheckElement, CheckDevice, CategoryTags::Scalar>(13);

	int v = 0;
	for (size_t i = 0; i < 13; i++)
	{
		bSca1.SetValue(i, float(v++));
	}

	const Batch<CheckElement, CheckDevice, CategoryTags::Scalar> bSca2 = bSca1;
	v = 0;
	for (size_t i = 0; i < 13; i++)
	{
		assert(bSca2[i] == float(v++));
	}


	cout << "done" << endl;
}
int main()
{
	TestScalarBatch();
	return 0;
}