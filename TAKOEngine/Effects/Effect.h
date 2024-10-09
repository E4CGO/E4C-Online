#pragma once
#include <string>
#include <DirectXMath.h>
#include <Effekseer/Effekseer.h>

class Effect
{
public:
	Effect(const char* filename);
	Effect(const char* filename, std::string version);
	~Effect() {};

	// �Đ�
	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);
	Effekseer::Handle PlayDX12(const DirectX::XMFLOAT3& position, float scale = 1.0f);

	// ��~
	void Stop(Effekseer::Handle handle);
	// ���W�ݒ�
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);
	// �X�P�[���ݒ�
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	void SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation);

private:
	Effekseer::EffectRef effekseerEffect;

	Effekseer::ManagerRef Manager();
	Effekseer::ManagerRef ManagerDX12();
};
