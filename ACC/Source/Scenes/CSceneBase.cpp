#include "CSceneBase.h"
#include "Camera/CCamera.h"


//===================================================
//		�V�[���x�[�X�N���X
//===================================================
CSceneBase::CSceneBase()
	:m_hWnd		()
	,m_Light	()
	,m_mView	()
	,m_mProj	()
{
}

CSceneBase::~CSceneBase()
{
}


//===================================================
//		�v���W�F�N�V�����֐�
//===================================================
void CSceneBase::Projection( D3DXMATRIX& Proj )
{
	// y�����̎���p�B���l��傫�������王�삪�����Ȃ�
	float fov_y = static_cast<FLOAT>(D3DXToRadian(CCamera::GetViewAngle()));
	
	// �A�X�y�N�g�i���������j
	float aspect = static_cast<FLOAT>(WND_W) / static_cast<FLOAT>(WND_H);
	float near_z = 0.1f;	// ��O
	float far_z = 1000.0f;	// ��(�l�𑝂₷�ƌ����鋗�����L�т�)

	// �v���W�F�N�V�����i�ˉe�j�ϊ�
	D3DXMatrixPerspectiveFovLH(
		&Proj,	// (out)�v���W�F�N�V�����v�Z����
		fov_y,	// ����p�iFOV�FField of View�j
		aspect,	// �A�X�y�N�g
		near_z,	// �߂��r���[���ʂ�z�l
		far_z);	// �����r���[���ʂ�z�l
}