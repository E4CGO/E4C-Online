#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/Shaders/Shader.h"

// 256切り上げ
template<class T>
inline T RoundUp256(const T& v)
{
	return (((v)+(const T)255) & ~(const T)255);
}

// シェーダー読み込み
void Shader::LoadShaderFile(const char* filename, std::vector<BYTE>& data)
{
	// ファイルを開く
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// ファイルのサイズを求める
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// メモリ上に頂点シェーダーデータを格納する領域を用意する
	data.resize(size);
	fread(data.data(), size, 1, fp);
	fclose(fp);
}