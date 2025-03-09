#include "CTime.h"


//=============================================================================
//		タイムクラス.
//=============================================================================
CTime::CTime()
	: m_DeltaTime	( 0.01667f )	// 初期値に１フレームにかかる秒数を設定.
	, m_LastTime	( std::chrono::steady_clock::now() )
{
}
CTime::~CTime()
{
}


//=============================================================================
//		更新処理.
//=============================================================================
void CTime::Update()
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsedTime = currentTime - m_LastTime;
	m_DeltaTime = elapsedTime.count();
	m_LastTime = currentTime;
}