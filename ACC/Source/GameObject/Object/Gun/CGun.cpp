#include "CGun.h"


//============================================================================
//		�e�N���X.
//============================================================================
CGun::CGun()
	:m_ShootPoint	(0.1f, 3.1f, 0.1f )
{
}

CGun::~CGun()
{
}


//============================================================================
//		�X�V����.
//============================================================================
void CGun::Update()
{
}


//============================================================================
//		�`�揈��.
//============================================================================
void CGun::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light)
{
	// �I�u�W�F�N�g�̕`��.
	CStaticMeshObject::Draw( View, Proj, Light );
}


//============================================================================
//		�e�̈ʒu���v���C���[�ɍ��킹�ĕύX����.
//============================================================================
void CGun::UpdateGunPos(const D3DXVECTOR3& center, float radius, float playerYaw)
{
	// �����ʒu�i�v���C���[�̉E������radius���������ꂽ�ʒu�j.
	D3DXVECTOR3 initialPosition(radius, 0.f, 0.f);
	D3DXVECTOR3 Upvec(0.f, -1.f, 0.f);	// �t��]�ɂ���.

	// �v���C���[��yaw��]��\���N�H�[�^�j�I��.
	D3DXQUATERNION playerYawQuat;
	D3DXQuaternionRotationAxis(&playerYawQuat, &Upvec, playerYaw);

	// �N�H�[�^�j�I������]�s��ɕϊ�.
	D3DXMATRIX rotationMatrix;
	D3DXMatrixRotationQuaternion(&rotationMatrix, &playerYawQuat);

	// �����ʒu����]���čŏI�I�Ȉʒu���v�Z.
	D3DXVECTOR3 rotatedPosition;
	D3DXVec3TransformCoord(&rotatedPosition, &initialPosition, &rotationMatrix);
	rotatedPosition.y += 2.f;

	// �v���C���[�̈ʒu����ɍŏI�ʒu���v�Z.
	m_vPosition = center + rotatedPosition;

	// �e���̑��Έʒu���w��.
	// X: �e�̉E�����i�e�̕����K�v�Ȃ�l���j�AY: �e�̍����AZ: �e�̑O�����i�e�g�̒����j.
	D3DXVECTOR3 barrelOffset(3.f, 0.7f, 1.f);

	// �I�t�Z�b�g���e�̉�]�Ɋ�Â��ĉ�]������.
	D3DXVECTOR3 rotatedBarrelOffset;
	D3DXVec3TransformCoord(&rotatedBarrelOffset, &barrelOffset, &rotationMatrix);

	// �e���̍ŏI�ʒu���v�Z.
	m_ShootPoint = m_vPosition + rotatedBarrelOffset;
}
