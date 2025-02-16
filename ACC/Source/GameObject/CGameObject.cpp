#include "CGameObject.h"


//===================================================================
//		ゲームオブジェクトクラス.
//===================================================================
CGameObject::CGameObject()
	: m_vPosition	()
	, m_vRotation	()
	, m_vScale		( 1.f, 1.f, 1.f )
	, m_Alpha		( 1.f )
{
}

CGameObject::~CGameObject()
{
}