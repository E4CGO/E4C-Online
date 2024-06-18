#include "TAKOEngine/Rendering/Misc.h"
#include "TAKOEngine/Rendering/Graphics.h"
#include "TAKOEngine/Rendering/Shaders/Shader.h"

// 256�؂�グ
template<class T>
inline T RoundUp256(const T& v)
{
	return (((v)+(const T)255) & ~(const T)255);
}

// �V�F�[�_�[�ǂݍ���
void Shader::LoadShaderFile(const char* filename, std::vector<BYTE>& data)
{
	// �t�@�C�����J��
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	// �t�@�C���̃T�C�Y�����߂�
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
	data.resize(size);
	fread(data.data(), size, 1, fp);
	fclose(fp);
}