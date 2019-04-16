#include "LibraryExport.h"
// Create Cube Model
ENGINE_API float* CUBE_VERTS = new float[180] {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0 L Bottom Back
	0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // 1 R Bottom Back
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f, // 2 R Top Back
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f, // 2 R Top Back
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 3 L Top Back
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0 L Bottom Back

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 1.0f, //
	0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, //
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,   0.0f, 1.0f, //
	0.5f, -0.5f, -0.5f,   0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,   1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,   1.0f, 0.0f, //
	0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 0.0f, //
	0.5f,  0.5f,  0.5f,   1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
ENGINE_API unsigned int* CUBE_INDICES = new unsigned int[36]{
	0, 1, 2,	0, 2, 3,      // front
	4, 5, 6,	4, 6, 7,      // back
	8, 9, 10,	8, 10, 11,    // top
	12, 13, 14, 12, 14, 15,   // bottom
	16, 17, 18, 16, 18, 19,   // right
	20, 21, 22, 20, 22, 23,   // left
};


ENGINE_API float* DiffusedMappedCube = new float[288]{
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,	// lbf // front ---
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,	// rbf			+--
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,	// rtf			++-
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,	// rtf			++-
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,	// ltf			-+-
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,	// lbf			---

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,	// lbb // back	--+
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,	// rbb			+-+
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,	// rtb			+++
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,	// rtb			+++
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,	// ltb			-++
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,	// lbb			--+

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// ltb // left	-++
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,	// ltf			-+-
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// lbf			---
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// lbf			---
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,	// lbb			--+
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// ltb			-++

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// rtb // right	+++
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,	// rtf			++-
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// rbf			+--
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,	// rbf			+--
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,	// rbb			+-+
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,	// rtb			+++

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,	// lbf // bottom---
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,	// rbf			+--
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,	// rbb			+-+
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,	// rbb			+-+
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,	// lbb			--+
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,	// lbf			---

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,	// ltf // top	-+-
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,	// rtf			++-
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,	// rtb			+++
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,	// rtb			+++
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,	// ltb			-++
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f	// ltf			-+-
};

ENGINE_API unsigned int* DiffusedMappedCubeIndices = new unsigned int[36] {
	0, 1, 2,	3, 4, 5,      // front
	6, 7, 8,	9, 10, 11,      // back
	12, 13, 14,	15, 16, 17,    // top
	18, 19, 20, 21, 22, 23,   // bottom
	24, 25, 26, 27, 28, 29,   // right
	30, 31, 32, 33, 34, 35   // left
};