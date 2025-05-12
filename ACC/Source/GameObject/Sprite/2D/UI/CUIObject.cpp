#include "CUIObject.h"
#include "DirectX/CDirectX11.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"

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
void CUIObject::AttachSprite(CSprite2D* pSprite)
{
	m_pSprite = new CSprite2D(*pSprite);
	m_vRotation = m_pSprite->GetRotation();
	m_vScale = m_pSprite->GetScale();
	m_Alpha = m_pSprite->GetAlpha();
}


//=============================================================================
//		�X�v���C�g��؂藣��.
//=============================================================================
void CUIObject::DetachSprite()
{
	m_pSprite = nullptr;
}


//=============================================================================
//		�_�Ǝl�p�̂����蔻��.
//=============================================================================
bool CUIObject::PointInSquare( POINT ppos, D3DXVECTOR2 windowpos )
{
	D3DXVECTOR3 pos = {
		m_vPosition.x * FULLSCREENSCALEX + windowpos.x,
		m_vPosition.y * FULLSCREENSCALEY + windowpos.y,
		0.f};

 	if (pos.x < ppos.x
	&&  pos.y < ppos.y
	&& ppos.x < pos.x + m_pSprite->GetSpriteData().Disp.w * m_vScale.x * FULLSCREENSCALEX 
	&& ppos.y < pos.y + m_pSprite->GetSpriteData().Disp.h * m_vScale.y * FULLSCREENSCALEY )
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


//=============================================================================
//		�摜�����X�g�̃f�[�^�Ǎ�.
//=============================================================================
void CUIObject::LoadSpriteList(
	const std::vector<std::string>& name, 
	std::vector<CUIObject*> &uis, 
	std::vector<CSprite2D*> &sprites)
{
	std::unordered_map<std::string, int> nameCounts; // ���O���Ƃ̏o���񐔂��L�^.

	for (size_t i = 0; i < name.size(); ++i)
	{
		// ���O��肪����ꍇ�̏���.
		std::string baseName = name[i];
		std::string numberedName;

		if (nameCounts.count(baseName) == 0) {
			numberedName = baseName;	// 1�ڂ͂��̂܂�.
			nameCounts[baseName] = 1;	// �������1�X�^�[�g.
		}
		else {
			numberedName = baseName + "_" + std::to_string(nameCounts[baseName]);
			nameCounts[baseName]++;
		}

		// �C���X�^���X����.
		sprites.push_back(CSpriteManager::GetInstance()->GetSprite(baseName));
		uis.push_back(new CUIObject());
		CSprite2D* pSprite = CSpriteManager::GetInstance()->GetSprite(name[i]);

		// �摜�f�[�^�̓ǂݍ���.
		uis.back()->AttachSprite(pSprite);
		uis.back()->SetPos(sprites.back()->GetSpriteData().Pos);

		// �ύX��̖��O�ɂ��Ȃ���.
		uis.back()->SetSpriteName(numberedName);
	}

	std::sort(uis.begin(), uis.end(), [](const CUIObject* a, const CUIObject* b) {
		if (a && a->GetPos() && b && b->GetPos()) {
			return a->GetPos().z < b->GetPos().z;
		}
		return false;
		});
}


//=============================================================================
//		�p�^�[���ԍ���ݒ�.
//=============================================================================
void CUIObject::SetPatternNo( SHORT x, SHORT y )
{
	m_PatternNo = POINTS(x,y);
}


//=============================================================================
//		�摜�̌��T�C�Y��ݒ�.
//=============================================================================
void CUIObject::SetBase( D3DXVECTOR2& base )
{
	m_pSprite->SetBase(base);
}


//=============================================================================
//		�摜�̕\���͈͂�ݒ�.
//=============================================================================
void CUIObject::SetDisp( D3DXVECTOR2& disp )
{
	m_pSprite->SetDisp(disp);
}


//=============================================================================
//		�摜�̕����T�C�Y��ݒ�.
//=============================================================================
void CUIObject::SetStride( D3DXVECTOR2& stride )
{
	m_pSprite->SetStride(stride);
}


//=============================================================================
//		�摜����ݒ�.
//=============================================================================
void CUIObject::SetSpriteName( const std::string& name )
{
	m_pSprite->SetSpriteName(name);
}


//=============================================================================
//		�p�^�[���ԍ����擾.
//=============================================================================
POINTS CUIObject::GetPatternNo() const
{
	return m_PatternNo;
}


//=============================================================================
//		�X�v���C�g���̎擾.
//=============================================================================
CSprite2D::SPRITE_STATE CUIObject::GetSpriteData() const
{  
	return m_pSprite->GetSpriteData();
}
