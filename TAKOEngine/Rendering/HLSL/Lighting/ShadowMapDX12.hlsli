#include "Light.hlsli"
#include "../Model/Constants.hlsli"

cbuffer CbScene : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 Projection;
    float4             cameraPosition;
    row_major float4x4 lightViewProjection;
    
    //���C�g���
    float4               ambientLightColor;
    DirectionalLightData directionalLightData;
    PointLightData       pointLightData[PointLightMax]; 	// �_�������
    SpotLightData        spotLightData[SpotLightMax];   	// �X�|�b�g���C�g���
    int                  pointLightCount;              		// �_������
    int                  spotLightCount;                	// �X�|�b�g���C�g��
};

cbuffer CbMesh : register(b1)
{
    row_major float4x4 world_transform;
    row_major float4x4 boneTransforms[MAX_BONES];
    row_major float4x4 instancingTransform[MAX_INSTANCES];
};

#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), \
                  DescriptorTable(CBV(b0), visibility=SHADER_VISIBILITY_VERTEX), \
                  DescriptorTable(CBV(b1), visibility=SHADER_VISIBILITY_VERTEX)"
