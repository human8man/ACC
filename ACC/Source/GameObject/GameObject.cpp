#include "GameObject.h"


GameObject::GameObject()
	: m_vPosition	(ZEROVEC3)
	, m_vPivot		(ZEROVEC3)
	, m_vRotation	(ZEROVEC3)
	, m_vScale		( 1.f, 1.f, 1.f )
	, m_vColor		( 1.f, 1.f, 1.f )
	, m_Alpha		( 1.f )
{
}
GameObject::~GameObject()
{
}