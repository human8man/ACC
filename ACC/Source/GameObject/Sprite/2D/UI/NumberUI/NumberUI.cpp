#include "NumberUI.h"
#include "DirectX/DirectX11.h"
#include "DirectSound/SoundManager.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "Easing/Easing.h"


namespace {
	// UI名.
	std::string ImageName = "Number";
}


NumberUI::NumberUI()
	: m_pNumber		()
	, m_pSprite2D	( nullptr )
	, m_UIPos		( ZEROVEC3 )
	, m_NumberPos	()
	, m_Color		( ZEROVEC3 )
	, m_SetValue	( 0 )
	, m_Digit		( 30 )
	, m_Minus		( 0 )
	, m_Rainbow		( false )
	, m_Special		( false )
{
}
NumberUI::~NumberUI()
{
}


//=============================================================================
//		作成処理.
//=============================================================================
void NumberUI::Create()
{
	// インスタンス生成
	m_pSprite2D = CSpriteManager::GetInstance()->Get2DSprite(ImageName);

	// 桁数分UIを用意.
	for (int i = 0; i < m_Digit; ++i) { m_pNumber.push_back(new UIObject()); }

	// 画像データの読み込み
	for (int i = 0; i < m_Digit;++i) { m_pNumber[i]->AttachSprite(m_pSprite2D); }
}


//=============================================================================
//		更新処理.
//=============================================================================
void NumberUI::Update()
{
	float time = Time::GetTotalTime();

	for (int i = 0; i < m_Digit + m_Minus; ++i)
	{
		float speed = 1.f;
		if (m_Special) {
			float floatOffset = sinf(time * 10.0f + i) * 20.0f;
			D3DXVECTOR3 basePos = m_NumberPos[i];
			D3DXVECTOR3 newPos = basePos + D3DXVECTOR3(0.0f, floatOffset, 0.0f);
			m_pNumber[i]->SetPos(newPos);
			speed = 15.f;
		}

		if (!m_Rainbow) continue;
		float t = time * speed + i * 0.3f; // 数字ごとに色をずらす.
		D3DXVECTOR3 color;
		color.x = 0.5f + 0.5f * sinf(t);		// R.
		color.y = 0.5f + 0.5f * sinf(t + 2.0f);	// G.
		color.z = 0.5f + 0.5f * sinf(t + 4.0f);	// B.

		m_pNumber[i]->SetColor(color);
	}
}


//=============================================================================
//		描画処理.
//=============================================================================
void NumberUI::Draw()
{
	for (int i = 0; i < m_Digit + m_Minus; ++i) { m_pNumber[i]->Draw(); }
}


//=============================================================================
//		数字を設定する.
//=============================================================================
void NumberUI::SetNumber(D3DXVECTOR3 pos, int value, float size)
{
	m_NumberPos.clear();
	m_SetValue = value;
	int absValue = std::abs(value);
	m_Digit = (absValue == 0) ? 1 : static_cast<int>(std::log10(absValue)) + 1;

	m_Minus = 0;
	if (value < 0) {
		m_Minus = 1;
		m_pNumber[0]->SetPatternNo(1, 1);
	}

	for (int i = 0; i < m_Digit; ++i)
	{
		int digit = absValue % 10;
		absValue /= 10;
		m_pNumber[m_Digit - i - 1 + m_Minus]->SetPatternNo(digit, 0);
	}

	m_UIPos = pos;
	float stride = m_pNumber[0]->GetSpriteData().Stride.w;

	for (int i = 0; i < m_Digit + m_Minus; ++i) {
		m_NumberPos.push_back(D3DXVECTOR3(m_UIPos.x + stride * i * size, m_UIPos.y, 0.f));
		m_pNumber[i]->SetPos(m_UIPos.x + stride * i * size, m_UIPos.y, 0.f);
		m_pNumber[i]->SetScale(size, size, 0.f);
	}
}


//=============================================================================
//		色を設定.
//=============================================================================
void NumberUI::SetColor(D3DXVECTOR3 color)
{
	for (int i = 0; i < m_Digit + m_Minus; ++i) { m_pNumber[i]->SetColor(color); }
}
//=============================================================================
//		透過設定.
//=============================================================================
void NumberUI::SetAlpha(float value)
{
	for (int i = 0; i < m_Digit + m_Minus; ++i) { m_pNumber[i]->SetAlpha(value); }
}
