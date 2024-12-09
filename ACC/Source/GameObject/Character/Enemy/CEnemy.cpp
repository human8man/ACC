#include "CEnemy.h"

CEnemy::CEnemy()
{
}

CEnemy::~CEnemy()
{
}

void CEnemy::Update()
{
	//Žè‘O‚ÉˆÚ“®‚µ‚Ä‚­‚é.
	m_vPosition.z -= 0.1f;
	if( m_vPosition.z <= 0.0f ){
		m_vPosition.z = 20.0f;
	}
}
