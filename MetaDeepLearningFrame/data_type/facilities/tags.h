#pragma once

namespace MDL
{   
    /*
    * 基于标签的类型体系
    */
	//data category tags
	struct CategoryTags
	{
		struct Scalar;
		struct Matrix;
		struct ScalarBatch;
		struct MatrixBatch;
	};

	//device tags
	struct DeviceTags
	{
		struct CPU;
	};
}
