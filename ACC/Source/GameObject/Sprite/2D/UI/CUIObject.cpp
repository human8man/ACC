#include "CUIObject.h"
#include "DirectX/CDirectX11.h"


//=============================================================================
//		UI�I�u�W�F�N�g�N���X.
//=============================================================================
CUIObject::CUIObject()
	: m_pSprite			( nullptr )
	, m_PatternNo		()
{
}
CUIObject::~CUIObject()
{
	DetachSprite();
}


//=============================================================================
//		�X�V����.
//=============================================================================
void CUIObject::Update()
{
	if( m_pSprite == nullptr ){ return; }
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CUIObject::Draw()
{
	if( m_pSprite == nullptr ){ return; }

	// �`�撼�O�ō��W���]���Ȃǂ��X�V.
	m_pSprite->SetPosition( m_vPosition );
	m_pSprite->SetRotation( m_vRotation );
	m_pSprite->SetScale( m_vScale );
	m_pSprite->SetAlpha( m_Alpha );

	// �p�^�[���ԍ���ݒ�.
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );


	CDirectX11::GetInstance()->SetDepth(false);
	// �����_�����O.
	m_pSprite->Render();
	CDirectX11::GetInstance()->SetDepth(true);
}


//=============================================================================
//		�X�v���C�g��ڑ�����.
//=============================================================================
void CUIObject::AttachSprite(CSprite2D& pSprite)
{
	m_pSprite = &pSprite;
	m_vRotation = m_pSprite->GetRotation();
	m_vScale = m_pSprite->GetScale();
	m_Alpha = m_pSprite->GetAlpha();
}


//===================================================
//		�E�B���h�E�̕`��J�n�ʒu���擾.
//===================================================
D3DXVECTOR2 CUIObject::WindowRect(HWND hwnd)
{
	// �E�B���h�E�S�̂̈ʒu�ƃT�C�Y���擾�i�E�B���h�E�^�u��g���܂ށj.
	RECT WindowRect;
	GetWindowRect(hwnd, &WindowRect);

	// �N���C�A���g�̈�̈ʒu�ƃT�C�Y���擾�i�E�B���h�E���̕`��͈́j.
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);

	// �N���C�A���g�̈�̍���ƉE���̍��W��������.
	POINT topLeft = { clientRect.left, clientRect.top };
	POINT bottomRight = { clientRect.right, clientRect.bottom };

	// �N���C�A���g�̈�̍��W���X�N���[�����W�n�ɕϊ�.
	ClientToScreen(hwnd, &topLeft);
	ClientToScreen(hwnd, &bottomRight);

	// �E�B���h�E�S�̂̍�����W�ƃN���C�A���g�̈�̍�����W�̍������v�Z.
	int borderLeft	= topLeft.x - WindowRect.left;
	int borderTop	= topLeft.y - WindowRect.top;

	// �t���[�������܂񂾃E�B���h�E�̈ʒu��Ԃ�.
	return D3DXVECTOR2(
		static_cast<float>(borderLeft + WindowRect.left),
		static_cast<float>(borderTop + WindowRect.top));
}


//=============================================================================
//		�_�Ǝl�p�̂����蔻��.
//=============================================================================
bool CUIObject::PointInSquare(POINT ppos, D3DXVECTOR2 windowpos)
{
	D3DXVECTOR3 pos = {
		m_vPosition.x * FULLSCREENSCALE + windowpos.x,
		m_vPosition.y * FULLSCREENSCALE + windowpos.y,
		m_vPosition.z * FULLSCREENSCALE };

 	if (pos.x < ppos.x
	&&  pos.y < ppos.y
	&& ppos.x < pos.x + m_pSprite->GetSpriteData().Disp.w * m_vScale.x * FULLSCREENSCALE 
	&& ppos.y < pos.y + m_pSprite->GetSpriteData().Disp.h * m_vScale.y * FULLSCREENSCALE )
	{
		return true;
	}
	return false;
}


//=============================================================================
//		�`�揈��.
//=============================================================================
void CUIObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	Draw();
}