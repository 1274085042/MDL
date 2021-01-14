#include "scalar.h"
#include <iostream>

using namespace std;
using namespace MDL;

int main()
{
	cout << "Test scalar ..." << endl;
	static_assert(IsScalar<Scalar<int, DeviceTags::CPU>>, "Test Error");
	static_assert(IsScalar<Scalar<int, DeviceTags::CPU> &>, "Test Error");
	static_assert(IsScalar<Scalar<int, DeviceTags::CPU> &&>, "Test Error");
	static_assert(IsScalar<const Scalar<int, DeviceTags::CPU> &>, "Test Error");
	static_assert(IsScalar<const Scalar<int, DeviceTags::CPU> &&>, "Test Error");

	Scalar<float, DeviceTags::CPU> pi(3.1415926f);
	cout << "done" << endl;
}