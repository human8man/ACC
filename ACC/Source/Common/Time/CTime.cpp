#include "CTime.h"


CTime::CTime()
	: m_DeltaTime			( 0.01667f )
	, m_NormalDeltaTime		( 0.01667f )
	, m_UnscaledDeltaTime	( 0.01667f )
	, m_TotalTime			( 0.f )
	, m_LastTime			( std::chrono::steady_clock::now() )
	, m_TimeScale			( 1.f )
{
}
CTime::~CTime()
{
}


//=============================================================================
//		çXêVèàóù.
//=============================================================================
void CTime::Update()
{
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> elapsedTime = currentTime - m_LastTime;

	m_UnscaledDeltaTime = elapsedTime.count();
	m_DeltaTime = elapsedTime.count() * m_TimeScale;
	m_TotalTime += m_DeltaTime;
	m_LastTime = currentTime;
}