#include "UIObject.h"
#include "DirectX/DirectX11.h"
#include "Sprite/2D/SpriteManager/SpriteManager.h"
#include "FileManager/FileManager.h"


UIObject::UIObject()
	: m_pSprite			( nullptr )
	, m_PatternNo		()
{
}
UIObject::~UIObject()
{
	DetachSprite();
}


//=============================================================================
//		�X�V����
//=============================================================================
void UIObject::Update()
{
	if( m_pSprite == nullptr ){ return; }
}


//=============================================================================
//		�`�揈��
//=============================================================================
void UIObject::Draw()
{
	if( m_pSprite == nullptr ){ return; }

	// �`�撼�O�ō��W���]���Ȃǂ��X�V
	m_pSprite->SetPos( m_vPosition );
	m_pSprite->SetPivot( m_vPivot );
	m_pSprite->SetRot( m_vRotation );
	m_pSprite->SetScale( m_vScale );
	m_pSprite->SetColor( m_vColor );
	m_pSprite->SetAlpha( m_Alpha );

	// �p�^�[���ԍ���ݒ�
	m_pSprite->SetPatternNo( m_PatternNo.x, m_PatternNo.y );


	DirectX11::GetInstance()->SetDepth(false);
	// �����_�����O
	m_pSprite->Render();
	DirectX11::GetInstance()->SetDepth(true);
}


//=============================================================================
//		�X�v���C�g��ڑ�����
//=============================================================================
void UIObject::AttachSprite(Sprite2D* pSprite)
{
	m_pSprite	= new Sprite2D(*pSprite);
	m_vPosition	= m_pSprite->GetPos();
	m_vPivot	= m_pSprite->GetPivot();
	m_vRotation	= m_pSprite->GetRot();
	m_vScale	= m_pSprite->GetScale();
	m_vColor	= m_pSprite->GetColor();
	m_Alpha		= m_pSprite->GetAlpha();
}


//=============================================================================
//		�X�v���C�g��؂藣��
//=============================================================================
void UIObject::DetachSprite()
{
	m_pSprite = nullptr;
}


//=============================================================================
//		�_�Ǝl�p�̂����蔻��
//=============================================================================
bool UIObject::PointInSquare( POINT ppos, D3DXVECTOR2 windowpos )
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
//		�`�揈��
//=============================================================================
void UIObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	Draw();
}


//=============================================================================
//		�摜�����X�g�̃f�[�^�Ǎ�
//=============================================================================
void UIObject::LoadFromJson(
	const std::string& scenepath,
	std::vector<UIObject*>& uis)
{
	// JSON�ǂݍ���
	Json jsonData = FileManager::JsonLoad(scenepath);

	// �ۑ����ꂽUI�f�[�^��ǂݍ��݁A�W�J
	for (auto& [imageName, spriteArray] : jsonData.items()) {
		// �g���q�� .json �Ȃ�X�L�b�v
		std::string::size_type dotPos = imageName.find_last_of('.');
		if (dotPos != std::string::npos) {
			std::string ext = imageName.substr(dotPos);
			if (ext == ".json" || ext == ".JSON") continue;
		}

		// �X�v���C�g�擾
		Sprite2D* pSprite = CSpriteManager::GetInstance()->Clone2DSprite(GetBaseName(imageName));
		if (!pSprite) {
			MessageBoxA(NULL, ("�X�v���C�g��������܂���: " + imageName).c_str(), "Error", MB_OK);
			continue;
		}

		// �eUI�C���X�^���X��W�J
		for (auto& value : spriteArray) {
			auto ui = new UIObject();

			// �e�����ݒ�
			pSprite->SetPos(D3DXVECTOR3(value["Pos"]["x"], value["Pos"]["y"], value["Pos"]["z"]));
			pSprite->SetColor(D3DXVECTOR3(value["Color"]["x"], value["Color"]["y"], value["Color"]["z"]));
			pSprite->SetAlpha(value["Alpha"]);
			pSprite->SetScale(D3DXVECTOR3(value["Scale"]["x"], value["Scale"]["y"], value["Scale"]["z"]));
			pSprite->SetPivot(D3DXVECTOR3(value["Pivot"]["x"], value["Pivot"]["y"], value["Pivot"]["z"]));
			pSprite->SetRot(D3DXVECTOR3(value["Rotate"]["x"], value["Rotate"]["y"], value["Rotate"]["z"]));

			// SpriteData�̈ꕔ���㏑��
			pSprite->SetBase(D3DXVECTOR2(value["Base"]["w"], value["Base"]["h"]));
			pSprite->SetDisp(D3DXVECTOR2(value["Disp"]["w"], value["Disp"]["h"]));
			pSprite->SetStride(D3DXVECTOR2(value["Stride"]["w"], value["Stride"]["h"]));

			// ���X�g�ɒǉ�
			ui->AttachSprite(pSprite);
			uis.push_back(ui);
		}
	}
	// Z���W����Ƀ\�[�g����
	std::sort(m_pUIs.begin(), m_pUIs.end(), [](const UIObject* a, const UIObject* b) {
		return a->GetPos().z < b->GetPos().z;
		});
}


//=============================================================================
//		�p�^�[���ԍ���ݒ�
//=============================================================================
void UIObject::SetPatternNo( SHORT x, SHORT y )
{
	m_PatternNo = POINTS(x,y);
}


//=============================================================================
//		�摜�̌��T�C�Y��ݒ�
//=============================================================================
void UIObject::SetBase( D3DXVECTOR2& base )
{
	m_pSprite->SetBase(base);
}


//=============================================================================
//		�摜�̕\���͈͂�ݒ�
//=============================================================================
void UIObject::SetDisp( D3DXVECTOR2& disp )
{
	m_pSprite->SetDisp(disp);
}


//=============================================================================
//		�摜�̕����T�C�Y��ݒ�
//=============================================================================
void UIObject::SetStride( D3DXVECTOR2& stride )
{
	m_pSprite->SetStride(stride);
}


//=============================================================================
//		�摜����ݒ�
//=============================================================================
void UIObject::SetSpriteName( const std::string& name )
{
	m_pSprite->SetSpriteName(name);
}


//=============================================================================
//		�p�^�[���ԍ����擾
//=============================================================================
POINTS UIObject::GetPatternNo() const
{
	return m_PatternNo;
}


//=============================================================================
//		�X�v���C�g���̎擾
//=============================================================================
Sprite2D::SPRITE_STATE UIObject::GetSpriteData() const
{  
	return m_pSprite->GetSpriteData();
}
