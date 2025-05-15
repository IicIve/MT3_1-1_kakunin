#include <Novice.h>
#include <cmath>
#include <cassert>

const char kWindowTitle[] = "LE2B_26";

 typedef struct Vector3 {
	float x;
	float y;
	float z;
 }Vector3;

typedef struct Matrix4x4 {
	float m[4][4];
}Matrix4x4;

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];


	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

Vector3 TransformWithoutDivide(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

	return result;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 Cross(const Vector3 & v1, const Vector3 & v2);

	Vector3 v1{ 1.2f, -3.9f, 2.5f };
	Vector3 v2{ 2.0f, 0.4f, -1.3f };

	Vector3 kLocalVertices[3] = {
	{0.0f, 1.0f, 0.0f},   
	{-1.0f, -1.0f, 0.0f}, 
	{1.0f, -1.0f, 0.0f}   
	};


	Vector3 rotate{ 0.0f, 0.0f, 0.0f };
	Vector3 translate{ 0.7f, -0.15f, 5.57f };
	Vector3 cameraPosition{0.0f,0.0f,-10.0f};

	Matrix4x4 MakeAffineMatrix(const Vector3 & scale, const Vector3 & rotate, const Vector3 & translate);
	Matrix4x4 Inverse(const Matrix4x4 & m1);
	Matrix4x4 Multiply(const Matrix4x4 & m1, const Matrix4x4 & m2);
	Matrix4x4 MakePerspectiveFovMatrix(float fowY, float aspectRatio, float nearClip, float farClip);
	Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
	void VectorScreenPrintf(int x, int y, const Vector3 & vector, const char* label);
	//Vector3 Transform(const Vector3 & vector, const Matrix4x4 & matrix);

	Vector3 cross = Cross(v1, v2);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		rotate.y += 0.05f;

		if (keys[DIK_W]) {
			translate.z += 0.5f;
		} else if (keys[DIK_S]) {
			translate.z -= 0.5f;
		}

		if (keys[DIK_A]) {
			translate.x -= 0.1f;
		} else if (keys[DIK_D]) {
			translate.x += 0.1f;
		}

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1.3f, 0.1f, 100.0f);

		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		Vector3 screenVertices[3];

		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = TransformWithoutDivide(ndcVertex, viewportMatrix);
		}



		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawTriangle(static_cast<int>(screenVertices[0].x), static_cast<int>(screenVertices[0].y),
			static_cast<int>(screenVertices[1].x), static_cast<int>(screenVertices[1].y), 
			static_cast<int>(screenVertices[2].x), static_cast<int>(screenVertices[2].y), RED, kFillModeSolid);

		VectorScreenPrintf(10, 10, cross, "cross");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			result.m[row][col] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				result.m[row][col] += m1.m[row][k] * m2.m[k][col];
			}
		}
	}

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

	Matrix4x4 MakeRotateXMatrix;

	MakeRotateXMatrix.m[0][0] = 1.0f;
	MakeRotateXMatrix.m[0][1] = 0.0f;
	MakeRotateXMatrix.m[0][2] = 0.0f;
	MakeRotateXMatrix.m[0][3] = 0.0f;

	MakeRotateXMatrix.m[1][0] = 0.0f;
	MakeRotateXMatrix.m[1][1] = std::cos(rotate.x);
	MakeRotateXMatrix.m[1][2] = std::sin(rotate.x);
	MakeRotateXMatrix.m[1][3] = 0.0f;

	MakeRotateXMatrix.m[2][0] = 0.0f;
	MakeRotateXMatrix.m[2][1] = -std::sin(rotate.x);
	MakeRotateXMatrix.m[2][2] = std::cos(rotate.x);
	MakeRotateXMatrix.m[2][3] = 0.0f;

	MakeRotateXMatrix.m[3][0] = 0.0f;
	MakeRotateXMatrix.m[3][1] = 0.0f;
	MakeRotateXMatrix.m[3][2] = 0.0f;
	MakeRotateXMatrix.m[3][3] = 1.0f;

	Matrix4x4 MakeRotateYMatrix;

	MakeRotateYMatrix.m[0][0] = std::cos(rotate.y);
	MakeRotateYMatrix.m[0][1] = 0.0f;
	MakeRotateYMatrix.m[0][2] = -std::sin(rotate.y);
	MakeRotateYMatrix.m[0][3] = 0.0f;

	MakeRotateYMatrix.m[1][0] = 0.0f;
	MakeRotateYMatrix.m[1][1] = 1.0f;
	MakeRotateYMatrix.m[1][2] = 0.0f;
	MakeRotateYMatrix.m[1][3] = 0.0f;

	MakeRotateYMatrix.m[2][0] = std::sin(rotate.y);
	MakeRotateYMatrix.m[2][1] = 0.0f;
	MakeRotateYMatrix.m[2][2] = std::cos(rotate.y);
	MakeRotateYMatrix.m[2][3] = 0.0f;

	MakeRotateYMatrix.m[3][0] = 0.0f;
	MakeRotateYMatrix.m[3][1] = 0.0f;
	MakeRotateYMatrix.m[3][2] = 0.0f;
	MakeRotateYMatrix.m[3][3] = 1.0f;

	Matrix4x4 MakeRotateZMatrix;

	MakeRotateZMatrix.m[0][0] = std::cos(rotate.z);
	MakeRotateZMatrix.m[0][1] = std::sin(rotate.z);
	MakeRotateZMatrix.m[0][2] = 0.0f;
	MakeRotateZMatrix.m[0][3] = 0.0f;

	MakeRotateZMatrix.m[1][0] = -std::sin(rotate.z);
	MakeRotateZMatrix.m[1][1] = std::cos(rotate.z);
	MakeRotateZMatrix.m[1][2] = 0.0f;
	MakeRotateZMatrix.m[1][3] = 0.0f;

	MakeRotateZMatrix.m[2][0] = 0.0f;
	MakeRotateZMatrix.m[2][1] = 0.0f;
	MakeRotateZMatrix.m[2][2] = 1.0f;
	MakeRotateZMatrix.m[2][3] = 0.0f;

	MakeRotateZMatrix.m[3][0] = 0.0f;
	MakeRotateZMatrix.m[3][1] = 0.0f;
	MakeRotateZMatrix.m[3][2] = 0.0f;
	MakeRotateZMatrix.m[3][3] = 1.0f;

	Matrix4x4 MakeRotateXYZMatrix = Multiply(MakeRotateXMatrix, Multiply(MakeRotateYMatrix, MakeRotateZMatrix));

	Matrix4x4 result;

	result.m[0][0] = MakeRotateXYZMatrix.m[0][0] * scale.x;
	result.m[0][1] = MakeRotateXYZMatrix.m[0][1] * scale.x;
	result.m[0][2] = MakeRotateXYZMatrix.m[0][2] * scale.x;
	result.m[0][3] = 0.0f;

	result.m[1][0] = MakeRotateXYZMatrix.m[1][0] * scale.y;
	result.m[1][1] = MakeRotateXYZMatrix.m[1][1] * scale.y;
	result.m[1][2] = MakeRotateXYZMatrix.m[1][2] * scale.y;
	result.m[1][3] = 0.0f;

	result.m[2][0] = MakeRotateXYZMatrix.m[2][0] * scale.z;
	result.m[2][1] = MakeRotateXYZMatrix.m[2][1] * scale.z;
	result.m[2][2] = MakeRotateXYZMatrix.m[2][2] * scale.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m1) {
	Matrix4x4 result = {};
	float det = 0.0f;

	// 行列式を計算
	det = m1.m[0][0] * (m1.m[1][1] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][2] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) +
		m1.m[1][3] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1])) -
		m1.m[0][1] * (m1.m[1][0] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
			m1.m[1][2] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
			m1.m[1][3] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0])) +
		m1.m[0][2] * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) -
			m1.m[1][1] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
			m1.m[1][3] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0])) -
		m1.m[0][3] * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]) -
			m1.m[1][1] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]) +
			m1.m[1][2] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));

	// 逆行列を計算
	float invDet = 1.0f / det;

	result.m[0][0] = invDet * (m1.m[1][1] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][2] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) +
		m1.m[1][3] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]));
	result.m[0][1] = -invDet * (m1.m[0][1] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) +
		m1.m[0][3] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]));
	result.m[0][2] = invDet * (m1.m[0][1] * (m1.m[1][2] * m1.m[3][3] - m1.m[1][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[1][1] * m1.m[3][3] - m1.m[1][3] * m1.m[3][1]) +
		m1.m[0][3] * (m1.m[1][1] * m1.m[3][2] - m1.m[1][2] * m1.m[3][1]));
	result.m[0][3] = -invDet * (m1.m[0][1] * (m1.m[1][2] * m1.m[2][3] - m1.m[1][3] * m1.m[2][2]) -
		m1.m[0][2] * (m1.m[1][1] * m1.m[2][3] - m1.m[1][3] * m1.m[2][1]) +
		m1.m[0][3] * (m1.m[1][1] * m1.m[2][2] - m1.m[1][2] * m1.m[2][1]));

	result.m[1][0] = -invDet * (m1.m[1][0] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][2] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[1][3] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]));
	result.m[1][1] = invDet * (m1.m[0][0] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]));
	result.m[1][2] = -invDet * (m1.m[0][0] * (m1.m[1][2] * m1.m[3][3] - m1.m[1][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[1][0] * m1.m[3][3] - m1.m[1][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[3][2] - m1.m[1][2] * m1.m[3][0]));
	result.m[1][3] = invDet * (m1.m[0][0] * (m1.m[1][2] * m1.m[2][3] - m1.m[1][3] * m1.m[2][2]) -
		m1.m[0][2] * (m1.m[1][0] * m1.m[2][3] - m1.m[1][3] * m1.m[2][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[2][2] - m1.m[1][2] * m1.m[2][0]));

	result.m[2][0] = invDet * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][1] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[1][3] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[2][1] = -invDet * (m1.m[0][0] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[0][1] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[2][2] = invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[3][3] - m1.m[1][3] * m1.m[3][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[3][3] - m1.m[1][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[3][1] - m1.m[1][1] * m1.m[3][0]));
	result.m[2][3] = -invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[2][3] - m1.m[1][3] * m1.m[2][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[2][3] - m1.m[1][3] * m1.m[2][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[2][1] - m1.m[1][1] * m1.m[2][0]));

	result.m[3][0] = -invDet * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]) -
		m1.m[1][1] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]) +
		m1.m[1][2] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[3][1] = invDet * (m1.m[0][0] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]) -
		m1.m[0][1] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]) +
		m1.m[0][2] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[3][2] = -invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[3][2] - m1.m[1][2] * m1.m[3][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[3][2] - m1.m[1][2] * m1.m[3][0]) +
		m1.m[0][2] * (m1.m[1][0] * m1.m[3][1] - m1.m[1][1] * m1.m[3][0]));
	result.m[3][3] = invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[2][2] - m1.m[1][2] * m1.m[2][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[2][2] - m1.m[1][2] * m1.m[2][0]) +
		m1.m[0][2] * (m1.m[1][0] * m1.m[2][1] - m1.m[1][1] * m1.m[2][0]));

	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	/*Matrix4x4 result;

	result.m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 1.0f / std::tan(fovY / 2.0f);
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	result.m[3][3] = 0.0f;

	return result;*/

	Matrix4x4 result = {};

	float yScale = 1.0f / std::tan(fovY / 2.0f);
	float xScale = yScale / aspectRatio;

	result.m[0][0] = xScale;
	result.m[1][1] = yScale;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = -nearClip * farClip / (farClip - nearClip);

	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;

	result.m[0][0] = width / 2.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = -(height / 2.0f);
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0.0f;

	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);

}
