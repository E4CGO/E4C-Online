#include "TAKOEngine/Runtime/tentacle_lib.h"

#include "Effect.h"
#include "EffectManager.h"

// コンストラクタ
Effect::Effect(const char* filename)
{
	std::lock_guard<std::mutex> lock(T_GRAPHICS.GetMutex());
	// Effekseerのリソースを読み込む
	// EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);
	// Effekseerエフェクトを読み込み
	effekseerEffect = Effekseer::Effect::Create(Manager(), (EFK_CHAR*)utf16Filename);
}

// コンストラクタ
Effect::Effect(const char* filename, std::string version)
{
	std::lock_guard<std::mutex> lock(T_GRAPHICS.GetMutex());
	// Effekseerのリソースを読み込む
	// EffekseerはUTF-16のファイルパス以外は対応していないため文字コード変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);
	// Effekseerエフェクトを読み込み
	effekseerEffect = Effekseer::Effect::Create(ManagerDX12(), (EFK_CHAR*)utf16Filename);
}

// 再生
Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	Effekseer::Handle handle = Manager()->Play(effekseerEffect, position.x, position.y, position.z);
	Manager()->SetScale(handle, scale, scale, scale);
	return handle;
}
// 再生
Effekseer::Handle Effect::PlayDX12(const DirectX::XMFLOAT3& position, float scale)
{
	Effekseer::Handle handle = ManagerDX12()->Play(effekseerEffect, position.x, position.y, position.z);
	ManagerDX12()->SetScale(handle, scale, scale, scale);
	return handle;
}

// 停止
void Effect::Stop(Effekseer::Handle handle)
{
	Manager()->StopEffect(handle);
}
// 座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Manager()->SetLocation(handle, position.x, position.y, position.z);
	Manager()->SetTargetLocation(handle, position.x, position.y, position.z);
}
// スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Manager()->SetScale(handle, scale.x, scale.y, scale.z);
}

void Effect::SetRotation(Effekseer::Handle handle, const DirectX::XMFLOAT3& rotation)
{
	Manager()->SetRotation(handle, rotation.x, rotation.y, rotation.z);
}

Effekseer::ManagerRef Effect::Manager() {
	return EFFECTS.GetEffekseerManager();
}

Effekseer::ManagerRef Effect::ManagerDX12() {
	return EFFECTS.GetEffekseerDX12Manager();
}