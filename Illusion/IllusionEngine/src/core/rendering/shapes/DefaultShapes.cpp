#include "core/rendering/shapes/defaultShapes.h"

namespace illusion::defaultshape {

	void InitializeCube() {
		cube = Mesh("Cube");
		Vec3 positions[] = {
			{ -0.500000, -0.500000, 0.500000 },
			{ -0.500000, 0.500000, 0.500000 },
			{ -0.500000, -0.500000, -0.500000 },
			{ -0.500000, 0.500000, -0.500000 },
			{ 0.500000, -0.500000, 0.500000 },
			{ 0.500000, 0.500000, 0.500000 },
			{ 0.500000, -0.500000, -0.500000 },
			{ 0.500000, 0.500000, -0.500000 }
		};
		Vec2 textures[] = {
			{ 0.375000, 0.000000},
			{ 0.625000, 0.000000},
			{ 0.625000, 0.250000},
			{ 0.375000, 0.250000},
			{ 0.625000, 0.500000},
			{ 0.375000, 0.500000},
			{ 0.625000, 0.750000},
			{ 0.375000, 0.750000},
			{ 0.625000, 1.000000},
			{ 0.375000, 1.000000},
			{ 0.125000, 0.500000},
			{ 0.125000, 0.750000},
			{ 0.875000, 0.500000},
			{ 0.875000, 0.750000}
		};
		Vec3 normals[] = {
			{ -1.0000, 0.0000, 0.0000},
			{ 0.0000, 0.0000, -1.0000},
			{ 1.0000, 0.0000, 0.0000},
			{ 0.0000, 0.0000, 1.0000},
			{ 0.0000, -1.0000, 0.0000},
			{ 0.0000, 1.0000, 0.0000},
		};
		std::vector<Vertex> vertices = {
			{positions[0], normals[0], textures[0]}, {positions[1], normals[0], textures[1]}, {positions[3], normals[0], textures[2]}, {positions[2], normals[0], textures[3]},
			{positions[2], normals[1], textures[3]}, {positions[3], normals[1], textures[2]}, {positions[7], normals[1], textures[4]}, {positions[6], normals[1], textures[5]},
			{positions[6], normals[2], textures[5]}, {positions[7], normals[2], textures[4]}, {positions[5], normals[2], textures[6]}, {positions[4], normals[2], textures[7]},
			{positions[4], normals[3], textures[7]}, {positions[5], normals[3], textures[6]}, {positions[1], normals[3], textures[8]}, {positions[0], normals[3], textures[9]},
			{positions[2], normals[4], textures[10]}, {positions[6], normals[4], textures[5]}, {positions[4], normals[4], textures[7]}, {positions[0], normals[4], textures[11]},
			{positions[7], normals[5], textures[4]}, {positions[3], normals[5], textures[12]}, {positions[1], normals[5], textures[13]}, {positions[5], normals[5], textures[6]}
		};
		std::vector<unsigned int> indices = {
			0, 1, 2, 0, 3, 2,
			4, 5, 6, 4, 7, 6,
			8, 9, 10, 8, 11, 10,
			12, 13, 14, 12, 15, 14,
			16, 17, 18, 16, 19, 18,
			20, 21, 22, 20, 23, 22
		};

		cube.vertices = vertices;
		cube.indices = indices;
	}

	void InitializeIcoSphere() {
		icoSphere = Mesh("Ico Sphere");
		Vec3 positions[] = {
			{ 0,-0.5,0 },{ 0.361804,-0.22361,0.262863 },{ -0.138194,-0.22361,0.425325 },{ -0.447213,-0.223608,0 },{ -0.138194,-0.22361,-0.425325 },{ 0.361804,-0.22361,-0.262863 },{ 0.138194,0.22361,0.425325 },{ -0.361804,0.22361,0.262863 },{ -0.361804,0.22361,-0.262863 },{ 0.138194,0.22361,-0.425325 },{ 0.447213,0.223608,0 },{ 0,0.5,0 },{ -0.081228,-0.425327,0.249998 },{ 0.212661,-0.425327,0.154506 },{ 0.131434,-0.262869,0.404506 },{ 0.425324,-0.262868,0 },{ 0.212661,-0.425327,-0.154506 },{ -0.262865,-0.425326,0 },{ -0.344095,-0.262868,0.249998 },{ -0.081228,-0.425327,-0.249998 },{ -0.344095,-0.262868,-0.249998 },{ 0.131434,-0.262869,-0.404506 },{ 0.475529,0,0.154506 },{ 0.475529,0,-0.154506 },{ 0,0,0.5 },{ 0.293893,0,0.404508 },{ -0.475529,0,0.154506 },{ -0.293893,0,0.404508 },{ -0.293893,0,-0.404508 },{ -0.475529,0,-0.154506 },{ 0.293893,0,-0.404508 },{ 0,0,-0.5 },{ 0.344095,0.262868,0.249998 },{ -0.131434,0.262869,0.404506 },{ -0.425324,0.262868,0 },{ -0.131434,0.262869,-0.404506 },{ 0.344095,0.262868,-0.249998 },{ 0.081228,0.425327,0.249998 },{ 0.262865,0.425326,0 },{ -0.212661,0.425327,0.154506 },{ -0.212661,0.425327,-0.154506 },{ 0.081228,0.425327,-0.249998 }
		};
		Vec2 textures[] = {
			{ 0.818181,0 },{ 0.772726,0.078731 },{ 0.863635,0.078731 },{ 0.727272,0.157461 },{ 0.681818,0.078731 },{ 0.636363,0.157461 },{ 0.090909,0 },{ 0.045454,0.078731 },{ 0.136363,0.078731 },{ 0.272727,0 },{ 0.227273,0.078731 },{ 0.318182,0.078731 },{ 0.454545,0 },{ 0.40909,0.078731 },{ 0.5,0.078731 },{ 0.681818,0.236191 },{ 0.90909,0.157461 },{ 0.818181,0.157461 },{ 0.863635,0.236191 },{ 0.181818,0.157461 },{ 0.090909,0.157461 },{ 0.136363,0.236191 },{ 0.363636,0.157461 },{ 0.272727,0.157461 },{ 0.318182,0.236191 },{ 0.545454,0.157461 },{ 0.454545,0.157461 },{ 0.5,0.236191 },{ 0.772726,0.236191 },{ 0.954545,0.236191 },{ 0.227273,0.236191 },{ 0.40909,0.236191 },{ 0.590909,0.236191 },{ 0.818181,0.314921 },{ 0.727272,0.314921 },{ 0.772726,0.393651 },{ 1,0.314921 },{ 0.909091,0.314921 },{ 0.954545,0.393651 },{ 0.272727,0.314921 },{ 0.181818,0.314921 },{ 0.227273,0.393651 },{ 0.454545,0.314921 },{ 0.363636,0.314921 },{ 0.40909,0.393651 },{ 0.636363,0.314921 },{ 0.545454,0.314921 },{ 0.590909,0.393651 },{ 0.5,0.393651 },{ 0.545454,0.472382 },{ 0.318182,0.393651 },{ 0.363636,0.472382 },{ 0.136363,0.393651 },{ 0.181818,0.472382 },{ 0.090909,0.314921 },{ 0.863635,0.393651 },{ 0.90909,0.472382 },{ 0.681818,0.393651 },{ 0.727272,0.472382 },{ 0.045454,0.236191 },{ 0,0.157461 },{ 0.590909,0.078731 },{ 0.636363,0 }
		};
		Vec3 normals[] = {
			{ 0.1024,-0.9435,0.3151 },{ 0.7002,-0.6617,0.268 },{ -0.268,-0.9435,0.1947 },{ -0.268,-0.9435,-0.1947 },{ 0.1024,-0.9435,-0.3151 },{ 0.905,-0.3304,0.268 },{ 0.0247,-0.3304,0.9435 },{ -0.8897,-0.3304,0.3151 },{ -0.5746,-0.3304,-0.7488 },{ 0.5346,-0.3304,-0.7779 },{ 0.8026,-0.1256,0.5831 },{ -0.3066,-0.1256,0.9435 },{ -0.9921,-0.1256,0 },{ -0.3066,-0.1256,-0.9435 },{ 0.8026,-0.1256,-0.5831 },{ 0.4089,0.6617,0.6284 },{ -0.4713,0.6617,0.5831 },{ -0.7002,0.6617,-0.268 },{ 0.0385,0.6617,-0.7488 },{ 0.724,0.6617,-0.1947 },{ 0.268,0.9435,-0.1947 },{ 0.4911,0.7947,-0.3568 },{ 0.4089,0.6617,-0.6284 },{ -0.1024,0.9435,-0.3151 },{ -0.1876,0.7947,-0.5773 },{ -0.4713,0.6617,-0.5831 },{ -0.3313,0.9435,0 },{ -0.6071,0.7947,0 },{ -0.7002,0.6617,0.268 },{ -0.1024,0.9435,0.3151 },{ -0.1876,0.7947,0.5773 },{ 0.0385,0.6617,0.7488 },{ 0.268,0.9435,0.1947 },{ 0.4911,0.7947,0.3568 },{ 0.724,0.6617,0.1947 },{ 0.8897,0.3304,-0.3151 },{ 0.7947,0.1876,-0.5773 },{ 0.5746,0.3304,-0.7488 },{ -0.0247,0.3304,-0.9435 },{ -0.3035,0.1876,-0.9342 },{ -0.5346,0.3304,-0.7779 },{ -0.905,0.3304,-0.268 },{ -0.9822,0.1876,0 },{ -0.905,0.3304,0.268 },{ -0.5346,0.3304,0.7779 },{ -0.3035,0.1876,0.9342 },{ -0.0247,0.3304,0.9435 },{ 0.5746,0.3304,0.7488 },{ 0.7947,0.1876,0.5773 },{ 0.8897,0.3304,0.3151 },{ 0.3066,0.1256,-0.9435 },{ 0.3035,-0.1876,-0.9342 },{ 0.0247,-0.3304,-0.9435 },{ -0.8026,0.1256,-0.5831 },{ -0.7947,-0.1876,-0.5773 },{ -0.8897,-0.3304,-0.3151 },{ -0.8026,0.1256,0.5831 },{ -0.7947,-0.1876,0.5773 },{ -0.5746,-0.3304,0.7488 },{ 0.3066,0.1256,0.9435 },{ 0.3035,-0.1876,0.9342 },{ 0.5346,-0.3304,0.7779 },{ 0.9921,0.1256,0 },{ 0.9822,-0.1876,0 },{ 0.905,-0.3304,-0.268 },{ 0.4713,-0.6617,-0.5831 },{ 0.1876,-0.7947,-0.5773 },{ -0.0385,-0.6617,-0.7488 },{ -0.4089,-0.6617,-0.6284 },{ -0.4911,-0.7947,-0.3568 },{ -0.724,-0.6617,-0.1947 },{ -0.724,-0.6617,0.1947 },{ -0.4911,-0.7947,0.3568 },{ -0.4089,-0.6617,0.6284 },{ 0.7002,-0.6617,-0.268 },{ 0.6071,-0.7947,0 },{ 0.3313,-0.9435,0 },{ -0.0385,-0.6617,0.7488 },{ 0.1876,-0.7947,0.5773 },{ 0.4713,-0.6617,0.5831 }
		};
		std::vector<Vertex> vertices = {
			{ positions[0], normals[0], textures[0] },{ positions[13], normals[0], textures[1] },{ positions[12], normals[0], textures[2] },{ positions[1], normals[1], textures[3] },{ positions[13], normals[1], textures[4] },{ positions[15], normals[1], textures[5] },{ positions[0], normals[2], textures[6] },{ positions[12], normals[2], textures[7] },{ positions[17], normals[2], textures[8] },{ positions[0], normals[3], textures[9] },{ positions[17], normals[3], textures[10] },{ positions[19], normals[3], textures[11] },{ positions[0], normals[4], textures[12] },{ positions[19], normals[4], textures[13] },{ positions[16], normals[4], textures[14] },{ positions[1], normals[5], textures[3] },{ positions[15], normals[5], textures[5] },{ positions[22], normals[5], textures[15] },{ positions[2], normals[6], textures[16] },{ positions[14], normals[6], textures[17] },{ positions[24], normals[6], textures[18] },{ positions[3], normals[7], textures[19] },{ positions[18], normals[7], textures[20] },{ positions[26], normals[7], textures[21] },{ positions[4], normals[8], textures[22] },{ positions[20], normals[8], textures[23] },{ positions[28], normals[8], textures[24] },{ positions[5], normals[9], textures[25] },{ positions[21], normals[9], textures[26] },{ positions[30], normals[9], textures[27] },{ positions[1], normals[10], textures[3] },{ positions[22], normals[10], textures[15] },{ positions[25], normals[10], textures[28] },{ positions[2], normals[11], textures[16] },{ positions[24], normals[11], textures[18] },{ positions[27], normals[11], textures[29] },{ positions[3], normals[12], textures[19] },{ positions[26], normals[12], textures[21] },{ positions[29], normals[12], textures[30] },{ positions[4], normals[13], textures[22] },{ positions[28], normals[13], textures[24] },{ positions[31], normals[13], textures[31] },{ positions[5], normals[14], textures[25] },{ positions[30], normals[14], textures[27] },{ positions[23], normals[14], textures[32] },{ positions[6], normals[15], textures[33] },{ positions[32], normals[15], textures[34] },{ positions[37], normals[15], textures[35] },{ positions[7], normals[16], textures[36] },{ positions[33], normals[16], textures[37] },{ positions[39], normals[16], textures[38] },{ positions[8], normals[17], textures[39] },{ positions[34], normals[17], textures[40] },{ positions[40], normals[17], textures[41] },{ positions[9], normals[18], textures[42] },{ positions[35], normals[18], textures[43] },{ positions[41], normals[18], textures[44] },{ positions[10], normals[19], textures[45] },{ positions[36], normals[19], textures[46] },{ positions[38], normals[19], textures[47] },{ positions[38], normals[20], textures[47] },{ positions[41], normals[20], textures[48] },{ positions[11], normals[20], textures[49] },{ positions[38], normals[21], textures[47] },{ positions[36], normals[21], textures[46] },{ positions[41], normals[21], textures[48] },{ positions[36], normals[22], textures[46] },{ positions[9], normals[22], textures[42] },{ positions[41], normals[22], textures[48] },{ positions[41], normals[23], textures[44] },{ positions[40], normals[23], textures[50] },{ positions[11], normals[23], textures[51] },{ positions[41], normals[24], textures[44] },{ positions[35], normals[24], textures[43] },{ positions[40], normals[24], textures[50] },{ positions[35], normals[25], textures[43] },{ positions[8], normals[25], textures[39] },{ positions[40], normals[25], textures[50] },{ positions[40], normals[26], textures[41] },{ positions[39], normals[26], textures[52] },{ positions[11], normals[26], textures[53] },{ positions[40], normals[27], textures[41] },{ positions[34], normals[27], textures[40] },{ positions[39], normals[27], textures[52] },{ positions[34], normals[28], textures[40] },{ positions[7], normals[28], textures[54] },{ positions[39], normals[28], textures[52] },{ positions[39], normals[29], textures[38] },{ positions[37], normals[29], textures[55] },{ positions[11], normals[29], textures[56] },{ positions[39], normals[30], textures[38] },{ positions[33], normals[30], textures[37] },{ positions[37], normals[30], textures[55] },{ positions[33], normals[31], textures[37] },{ positions[6], normals[31], textures[33] },{ positions[37], normals[31], textures[55] },{ positions[37], normals[32], textures[35] },{ positions[38], normals[32], textures[57] },{ positions[11], normals[32], textures[58] },{ positions[37], normals[33], textures[35] },{ positions[32], normals[33], textures[34] },{ positions[38], normals[33], textures[57] },{ positions[32], normals[34], textures[34] },{ positions[10], normals[34], textures[45] },{ positions[38], normals[34], textures[57] },{ positions[23], normals[35], textures[32] },{ positions[36], normals[35], textures[46] },{ positions[10], normals[35], textures[45] },{ positions[23], normals[36], textures[32] },{ positions[30], normals[36], textures[27] },{ positions[36], normals[36], textures[46] },{ positions[30], normals[37], textures[27] },{ positions[9], normals[37], textures[42] },{ positions[36], normals[37], textures[46] },{ positions[31], normals[38], textures[31] },{ positions[35], normals[38], textures[43] },{ positions[9], normals[38], textures[42] },{ positions[31], normals[39], textures[31] },{ positions[28], normals[39], textures[24] },{ positions[35], normals[39], textures[43] },{ positions[28], normals[40], textures[24] },{ positions[8], normals[40], textures[39] },{ positions[35], normals[40], textures[43] },{ positions[29], normals[41], textures[30] },{ positions[34], normals[41], textures[40] },{ positions[8], normals[41], textures[39] },{ positions[29], normals[42], textures[30] },{ positions[26], normals[42], textures[21] },{ positions[34], normals[42], textures[40] },{ positions[26], normals[43], textures[21] },{ positions[7], normals[43], textures[54] },{ positions[34], normals[43], textures[40] },{ positions[27], normals[44], textures[29] },{ positions[33], normals[44], textures[37] },{ positions[7], normals[44], textures[36] },{ positions[27], normals[45], textures[29] },{ positions[24], normals[45], textures[18] },{ positions[33], normals[45], textures[37] },{ positions[24], normals[46], textures[18] },{ positions[6], normals[46], textures[33] },{ positions[33], normals[46], textures[37] },{ positions[25], normals[47], textures[28] },{ positions[32], normals[47], textures[34] },{ positions[6], normals[47], textures[33] },{ positions[25], normals[48], textures[28] },{ positions[22], normals[48], textures[15] },{ positions[32], normals[48], textures[34] },{ positions[22], normals[49], textures[15] },{ positions[10], normals[49], textures[45] },{ positions[32], normals[49], textures[34] },{ positions[30], normals[50], textures[27] },{ positions[31], normals[50], textures[31] },{ positions[9], normals[50], textures[42] },{ positions[30], normals[51], textures[27] },{ positions[21], normals[51], textures[26] },{ positions[31], normals[51], textures[31] },{ positions[21], normals[52], textures[26] },{ positions[4], normals[52], textures[22] },{ positions[31], normals[52], textures[31] },{ positions[28], normals[53], textures[24] },{ positions[29], normals[53], textures[30] },{ positions[8], normals[53], textures[39] },{ positions[28], normals[54], textures[24] },{ positions[20], normals[54], textures[23] },{ positions[29], normals[54], textures[30] },{ positions[20], normals[55], textures[23] },{ positions[3], normals[55], textures[19] },{ positions[29], normals[55], textures[30] },{ positions[26], normals[56], textures[21] },{ positions[27], normals[56], textures[59] },{ positions[7], normals[56], textures[54] },{ positions[26], normals[57], textures[21] },{ positions[18], normals[57], textures[20] },{ positions[27], normals[57], textures[59] },{ positions[18], normals[58], textures[20] },{ positions[2], normals[58], textures[60] },{ positions[27], normals[58], textures[59] },{ positions[24], normals[59], textures[18] },{ positions[25], normals[59], textures[28] },{ positions[6], normals[59], textures[33] },{ positions[24], normals[60], textures[18] },{ positions[14], normals[60], textures[17] },{ positions[25], normals[60], textures[28] },{ positions[14], normals[61], textures[17] },{ positions[1], normals[61], textures[3] },{ positions[25], normals[61], textures[28] },{ positions[22], normals[62], textures[15] },{ positions[23], normals[62], textures[32] },{ positions[10], normals[62], textures[45] },{ positions[22], normals[63], textures[15] },{ positions[15], normals[63], textures[5] },{ positions[23], normals[63], textures[32] },{ positions[15], normals[64], textures[5] },{ positions[5], normals[64], textures[25] },{ positions[23], normals[64], textures[32] },{ positions[16], normals[65], textures[14] },{ positions[21], normals[65], textures[26] },{ positions[5], normals[65], textures[25] },{ positions[16], normals[66], textures[14] },{ positions[19], normals[66], textures[13] },{ positions[21], normals[66], textures[26] },{ positions[19], normals[67], textures[13] },{ positions[4], normals[67], textures[22] },{ positions[21], normals[67], textures[26] },{ positions[19], normals[68], textures[11] },{ positions[20], normals[68], textures[23] },{ positions[4], normals[68], textures[22] },{ positions[19], normals[69], textures[11] },{ positions[17], normals[69], textures[10] },{ positions[20], normals[69], textures[23] },{ positions[17], normals[70], textures[10] },{ positions[3], normals[70], textures[19] },{ positions[20], normals[70], textures[23] },{ positions[17], normals[71], textures[8] },{ positions[18], normals[71], textures[20] },{ positions[3], normals[71], textures[19] },{ positions[17], normals[72], textures[8] },{ positions[12], normals[72], textures[7] },{ positions[18], normals[72], textures[20] },{ positions[12], normals[73], textures[7] },{ positions[2], normals[73], textures[60] },{ positions[18], normals[73], textures[20] },{ positions[15], normals[74], textures[5] },{ positions[16], normals[74], textures[61] },{ positions[5], normals[74], textures[25] },{ positions[15], normals[75], textures[5] },{ positions[13], normals[75], textures[4] },{ positions[16], normals[75], textures[61] },{ positions[13], normals[76], textures[4] },{ positions[0], normals[76], textures[62] },{ positions[16], normals[76], textures[61] },{ positions[12], normals[77], textures[2] },{ positions[14], normals[77], textures[17] },{ positions[2], normals[77], textures[16] },{ positions[12], normals[78], textures[2] },{ positions[13], normals[78], textures[1] },{ positions[14], normals[78], textures[17] },{ positions[13], normals[79], textures[1] },{ positions[1], normals[79], textures[3] },{ positions[14], normals[79], textures[17] }
		};
		std::vector<unsigned int> indices = {};
		indices.reserve(vertices.size());

		for (size_t i = 0; i < vertices.size(); i++) indices.push_back(i);

		icoSphere.vertices = vertices;
		icoSphere.indices = indices;
	}

	void Initialize() {
		InitializeCube();
		InitializeIcoSphere();
	}

	Mesh& Cube() { return cube; }
	Mesh& IcoSphere() { return icoSphere; }
}