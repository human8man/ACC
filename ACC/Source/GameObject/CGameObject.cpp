#include "CGameObject.h"

//�R���X�g���N�^.
CGameObject::CGameObject()
	: m_vPosition	()
	, m_vRotation	()
	, m_vScale		( 1.0f, 1.0f, 1.0f )
	, m_Alpha       ( 1.0f )
{
}

//�f�X�g���N�^.
CGameObject::~CGameObject()
{
}