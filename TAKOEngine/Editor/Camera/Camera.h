
#pragma once

#include <DirectXMath.h>
#include <vector>
// �J����

class Camera
{
public:
	Camera() {};
	~Camera() {};
public:
	// �w�����������
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);
	
	// �p�[�X�y�N�e�B�u�ݒ�
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);
	
	// �r���[�s��擾
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// �v���W�F�N�V�����s��擾
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }
	
	// ���_�擾
	const DirectX::XMFLOAT3& GetEye() const { return eye; }
	
	// �����_�擾
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }
	
	// ������擾
	const DirectX::XMFLOAT3& GetUp() const { return up; }
	
	// �O�����擾
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	
	// �E�����擾
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	//����p�擾
	const float& GetFovY() { return fovY; }

	//�A�X�y�N�g��擾
	const float& GetAspect() { return aspect; }

	//�����ʂ܂ł̋������擾
	const float& GetFarZ() { return farZ; }

	//�ߕ��ʂ܂ł̋������擾
	const float& GetNearZ() { return nearZ; }

	//�C���f�b�N�X�擾
	int& GetSegment() { return currentSegment; }

	//�����J����
	void MoveToCamera(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, float transitiontime, float transitionDuration, float elapsedTime);
	//��]����J����
	void RotateToCamera(const DirectX::XMFLOAT3& target, float& angle, float radius, float speed, float elapsedTime);
	//�n�_�A���_�A�I�_�̃|�C���g���o�R���Ĉړ�����J����
	void MovePointToCamera(const std::vector<DirectX::XMFLOAT3>& positions, const std::vector<DirectX::XMFLOAT3>& focusPoints,float& transitionTime, float transitionDuration, float elapsedTime);
	//�n�_�A�I�_�̃|�C���g���o�R���Ĉړ�����J����
	void Move2PointToCamera(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& startFocus, const DirectX::XMFLOAT3& endFocus, float& transitionTime, float transitionDuration, float elapsedTime);
private:
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	DirectX::XMFLOAT3 eye;
	DirectX::XMFLOAT3 focus;

	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 front;
	DirectX::XMFLOAT3 right;

	float fovY;
	float aspect;
	float nearZ;
	float farZ;

	std::vector<DirectX::XMFLOAT3> cameraPositions;  // �J�����̊e�|�C���g�̈ʒu
	std::vector<DirectX::XMFLOAT3> cameraFocusPoints;  // �J�����̊e�|�C���g�̒����_
	
	int currentSegment = 0;
};
