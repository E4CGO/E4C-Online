#pragma once

#include "TAKOEngine/Editor/Camera/Camera.h"
#include "TAKOEngine/Rendering/RenderState.h"
#include "TAKOEngine/Rendering/MyRender.h"

//	UV�X�N���[�����
struct UVScrollData
{
	DirectX::XMFLOAT2	uvScrollValue;	// UV�X�N���[���l
};

// �}�X�N�f�[�^
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float dissolveThreshold;
	float edgeThreshold; 			// ����臒l
	DirectX::XMFLOAT4 edgeColor;	// ���̐F
};

struct DirectionalLightData
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 color;
};

//Deferred Rendering�pSRV
struct DeferredData
{
	ID3D11ShaderResourceView* normal;
	ID3D11ShaderResourceView* position;
};

//TODO : ShaderData Set
#pragma region �V�F�[�_�[���
struct Default
{
	DirectX::XMFLOAT4 ka = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 kd = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 ks = DirectX::XMFLOAT4(0, 0, 0, 1);
	float shiness = 128;
	DirectX::XMFLOAT3 dummy;
};

struct Phong
{
	DirectX::XMFLOAT4 ka = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 kd = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 ks = DirectX::XMFLOAT4(1, 1, 1, 1);
	float shiness = 128;
	DirectX::XMFLOAT3 dummy;
};

struct Toon
{
	DirectX::XMFLOAT4 ka = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 kd = DirectX::XMFLOAT4(1, 1, 1, 1);
	DirectX::XMFLOAT4 ks = DirectX::XMFLOAT4(1, 1, 1, 1);
	float shiness = 128;
	DirectX::XMFLOAT3 dummy;
};

#pragma endregion

struct ShaderData
{
	Default defaultShader;
	Phong   phongShader;
	Toon    toonShader;
};

// �_�������
struct PointLightData
{
	DirectX::XMFLOAT4 position;		// ���W
	DirectX::XMFLOAT4 color;		// �F
	float range;					// �͈�
	DirectX::XMFLOAT3 dummy;
};

// �_�����̍ő吔
static constexpr int PointLightMax = 8;
// �X�|�b�g���C�g���
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
	float				range;		// �͈�
	float				innerCorn; 	// �C���i�[�p�x�͈�
	float				outerCorn; 	// �A�E�^�[�p�x�͈�
	float				dummy;
};

// �X�|�b�g���C�g�̍ő吔
static	constexpr	int	SpotLightMax = 8;

// �F���␳���
struct ColorGradingData
{
	float	hueShift = 0;	// �F������
	float	saturation = 1;	// �ʓx����
	float	brightness = 1;	// ���x����
	float	dummy;
};

// �K�E�X�t�B���^�[�v�Z���
struct GaussianFilterData
{
	int					kernelSize = 8;		// �J�[�l���T�C�Y
	float				deviation = 10.0f;	// �W���΍�
	DirectX::XMFLOAT2	textureSize;			// �򂷃e�N�X�`���̃T�C�Y
};

// �K�E�X�t�B���^�[�̍ő�J�[�l���T�C�Y
static const int MaxKernelSize = 16;

// ���P�x���o�p���
struct LuminanceExtractionData
{
	float				threshold = 0.5f;	// 臒l
	float				intensity = 1.0f;	// �u���[���̋��x
	DirectX::XMFLOAT2	dummy2;
};

// �|�X�g�G�t�F�N�g�̍ŏI�p�X�p���
struct FinalpassnData
{
	//	�u���[���e�N�X�`��
	ID3D11ShaderResourceView* bloomTexture;
};

struct ShadowMapData
{
	//�J�������
	DirectX::XMFLOAT4X4 view = {};
	DirectX::XMFLOAT4X4 projection = {};

	//�V���h�E�}�b�v�p�[�x�X�e���V���o�b�t�@
	ID3D11ShaderResourceView* shadowMap[myRenderer::NUM_SHADOW_MAP] = {};

	//���C�g�r���[�v���W�F�N�V�����s��
	DirectX::XMFLOAT4X4 lightViewProjection[myRenderer::NUM_SHADOW_MAP] = {};

	//�[�x��r�p�̃I�t�Z�b�g�l
	float shadowBias[myRenderer::NUM_SHADOW_MAP] = { 0.001f, 0.002f, 0.004f, 0.01f }; //�[�x��r�p�̃I�t�Z�b�g�l
	
	//�e�̐F
	DirectX::XMFLOAT3 shadowColor = { 0.5f,0.5f,0.5f };
};

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	const Camera* camera;

	//	�X�N���[���f�[�^
	UVScrollData			uvScrollData;

	// �}�X�N�f�[�^
	MaskData				maskData;

	//	���C�g���
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData			pointLightData[PointLightMax];	// �_�������
	SpotLightData			spotLightData[SpotLightMax];	// �X�|�b�g���C�g���
	int						pointLightCount = 0;			// �_������
	int						spotLightCount = 0;				// �X�|�b�g���C�g��

	ColorGradingData		colorGradingData;				//	�F���␳���
	GaussianFilterData		gaussianFilterData;				//	�K�E�X�t�B���^�[���
	LuminanceExtractionData	luminanceExtractionData;		//	���P�x���o�p���
	FinalpassnData			finalpassnData;					//	�ŏI�p�X���
	ShadowMapData           shadowMapData;                  //  �V���h�E�}�b�v���

	//���f���V�F�[�_�[���
	ShaderData shaderData;

	//Deferred Rendering�pSRV
	DeferredData deferredData;
};