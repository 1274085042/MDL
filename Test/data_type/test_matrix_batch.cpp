
#include "matrix_batch.h"
#include <cassert>
#include <iostream>

using namespace std;
using namespace MDL;

using CheckElement = float;
using CheckDevice = DeviceTags::CPU;

void TestMatrixBatch1()
{
	cout << "Test matrix batch case 1..." << endl;
	static_assert(IsMatrixBatch<Batch<int, CheckDevice, CategoryTags::Matrix>>, "Test Error");
	static_assert(IsMatrixBatch<Batch<int, CheckDevice, CategoryTags::Matrix> &>, "Test Error");
	static_assert(IsMatrixBatch<Batch<int, CheckDevice, CategoryTags::Matrix> &&>, "Test Error");
	static_assert(IsMatrixBatch<const Batch<int, CheckDevice, CategoryTags::Matrix>>, "Test Error");
	static_assert(IsMatrixBatch<const Batch<int, CheckDevice, CategoryTags::Matrix> &>, "Test Error");
	static_assert(IsMatrixBatch<const Batch<int, CheckDevice, CategoryTags::Matrix> &&>, "Test Error");

	Batch<int, CheckDevice, CategoryTags::Matrix> b(10, 13, 35);
	assert(b.BatchNum() == 10);
	assert(b.RowNum() == 13);
	assert(b.ColNum() == 35);

	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < 13; i++)
		{
			for (int j = 0; j < 35; j++)
			{
				b.SetValue(k, i, j, (int)(k * 1000 + i * 100 + j));
			}
		}
	}
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < 13; i++)
		{
			for (int j = 0; j < 35; j++)
			{
				assert(b[k](i,j)== (int)(k * 1000 + i * 100 + j));
			}
		}
	}

	auto b1 = b.SubMatrixBatch(3, 7, 11, 22);
	assert(b.AvailableForWrite() == 2);
	assert(b1.AvailableForWrite() == 2);
	assert(b1.BatchNum() == 10);
	assert(b1.RowNum() == 5);
	assert(b1.ColNum() == 12);

	for (int k = 0; k < 10; k++)
	{
		for (int i = 3; i <= 7; i++)
		{
			for (int j = 11; j <= 22; j++)
			{
				//std::cout << b1[k](i-3, j-11) << " " << k * 1000 + i * 100 + j << std::endl;
				assert(b1[k](i-3, j-11) == (int)(k*1000+i*100+j));
			}
		}
	}
	cout << "done" << endl;
}

void TestMatrixBatch2()
{
	cout << "Test matrix batch case 2..." << endl;
	static_assert(IsMatrixBatch<Batch<CheckElement, CheckDevice, CategoryTags::Matrix>>, "Test Error");
	static_assert(IsMatrixBatch<Batch<CheckElement, CheckDevice, CategoryTags::Matrix>&>, "Test Error");
	static_assert(IsMatrixBatch<Batch<CheckElement, CheckDevice, CategoryTags::Matrix>&&>, "Test Error");
	static_assert(IsMatrixBatch<const Batch<CheckElement, CheckDevice, CategoryTags::Matrix>>, "Test Error");
	static_assert(IsMatrixBatch<const Batch<CheckElement, CheckDevice, CategoryTags::Matrix>&>, "Test Error");
	static_assert(IsMatrixBatch<const Batch<CheckElement, CheckDevice, CategoryTags::Matrix>&&>, "Test Error");

	auto b1 = Batch<CheckElement, CheckDevice, CategoryTags::Matrix>(3, 10, 20);
	assert(b1.BatchNum() == 3);

	int c = 0;
	auto m1 = Matrix<CheckElement, CheckDevice>(10, 20);
	auto m2 = Matrix<CheckElement, CheckDevice>(10, 20);
	auto m3 = Matrix<CheckElement, CheckDevice>(10, 20);
	for (size_t i = 0; i < 10; i++)
	{
		for (size_t j = 0; j < 20; j++)
		{
			m1.SetValue(i, j, (float)(c++));
			m2.SetValue(i, j, (float)(c++));
			m3.SetValue(i, j, (float)(c++));
			b1.SetValue(0, i, j, m1(i, j));
			b1.SetValue(1, i, j, m2(i, j));
			b1.SetValue(2, i, j, m3(i, j));
		}
	}
	for (size_t i = 0; i < 10; i++)
	{
		for (size_t j = 0; j < 20; j++)
		{
			assert(b1[0](i, j) == m1(i, j));
			assert(b1[1](i, j) == m2(i, j));
			assert(b1[2](i, j) == m3(i, j));
		}
	}

	b1 = b1.SubMatrixBatch(3, 7, 11, 16);
	assert(b1.RowNum() == 5);
	assert(b1.ColNum() == 6);
	assert(b1.BatchNum() == 3);
	m1 = m1.SubMatrix(3, 7, 11, 16);
	m2 = m2.SubMatrix(3, 7, 11, 16);
	m3 = m3.SubMatrix(3, 7, 11, 16);
	for (size_t i = 0; i < 5; i++)
	{
		for (size_t j = 0; j < 6; j++)
		{
			assert(b1[0](i, j) == m1(i, j));
			assert(b1[1](i, j) == m2(i, j));
			assert(b1[2](i, j) == m3(i, j));
		}
	}

	cout << "done" << endl;
}

int main()
{
	TestMatrixBatch1();
	TestMatrixBatch2();
	return 0;
}