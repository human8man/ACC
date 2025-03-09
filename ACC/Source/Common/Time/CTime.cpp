#include "CTime.h"


//=============================================================================
//		�^�C���N���X.
//=============================================================================
CTime::CTime()
	: m_DeltaTime	( 0.01667f )	// �����l�ɂP�t���[���ɂ�����b����ݒ�.
	, m_LastTime	( std::chrono::steady_clock::now() )
{
}
CTime::~CTime()
{
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CTime::Update()
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsedTime = currentTime - m_LastTime;
	m_DeltaTime = elapsedTime.count();
	m_LastTime = currentTime;
}