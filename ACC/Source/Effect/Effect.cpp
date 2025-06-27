#include "Effect.h"
#include "Camera/Camera.h"


//�`��X�v���C�g�ő吔
constexpr int RENDER_SPRITE_MAX = 8000;
//�G�t�F�N�g�Ǘ��p�C���X�^���X�ő吔
constexpr int EFFECT_INSTANCE_MAX = 1000;


//=============================================================================
//		Effekseer
//=============================================================================
Effect::Effect()
	: m_pManager	( nullptr )
	, m_pRenderer	( nullptr )

	, m_pEffect		()
{
}
Effect::~Effect()
{
	// �G�t�F�N�g�f�[�^�̉��
	ReleaseData();

	// �G�t�F�N�g�̃}�l�[�W���[��j��
	m_pManager.Reset();

	// �G�t�F�N�g�̃����_���[��j��
	m_pRenderer.Reset();
}


//=============================================================================
//		�\�z
//=============================================================================
HRESULT Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// �G�t�F�N�g�̃}�l�[�W���̍쐬
	m_pManager = ::EsManager::Create( RENDER_SPRITE_MAX );

	// �G�t�F�N�g�̃����_���̍쐬
	m_pRenderer = ::EsRenderer::Create( pDevice, pContext, EFFECT_INSTANCE_MAX );

	// �`�惂�W���[���̐ݒ�
	m_pManager->SetSpriteRenderer( m_pRenderer->CreateSpriteRenderer() );
	m_pManager->SetRibbonRenderer( m_pRenderer->CreateRibbonRenderer() );
	m_pManager->SetRingRenderer( m_pRenderer->CreateRingRenderer() );
	m_pManager->SetTrackRenderer( m_pRenderer->CreateTrackRenderer() );
	m_pManager->SetModelRenderer( m_pRenderer->CreateModelRenderer() );

	// �e�N�X�`���A���f���A�}�e���A�����[�_�[�̐ݒ������
	//	���[�U�[���Ǝ��Ɋg���ł���.���݂̓t�@�C������ǂݍ���ł���
	m_pManager->SetTextureLoader( m_pRenderer->CreateTextureLoader() );
	m_pManager->SetModelLoader( m_pRenderer->CreateModelLoader() );
	m_pManager->SetMaterialLoader( m_pRenderer->CreateMaterialLoader() );
	m_pManager->SetCurveLoader( ::Es::MakeRefPtr<::Es::CurveLoader>() );

	return S_OK;
}


//=============================================================================
//		�f�[�^�ǂݍ���
//=============================================================================
HRESULT Effect::LoadData()
{
	struct EffectList
	{
		enList listNo;				// enList�񋓌^��ݒ�
		const char16_t path[256];	// �t�@�C���̖��O(�p�X�t��)
	};
	EffectList EList[] =
	{
		// u""�́AUTF-16�G���R�[�f�B���O�̕����񃊃e�����ŁAconst char16_t*�ɑ���\
		{ enList::CritHit,		u"Data\\Effekseer\\BodyHitCrit.efk"	},
		{ enList::Hit,			u"Data\\Effekseer\\ShieldHit.efk"	},
		
		{ enList::GunFire,		u"Data\\Effekseer\\Fire.efk"		},
		{ enList::Dust,			u"Data\\Effekseer\\Dust.efk"		},

		{ enList::BulletSmoke,	u"Data\\Effekseer\\BulletSmoke.efk"	},
	};

	// �z��̍ő�v�f�����Z�o�i�z��S�̂̃T�C�Y�^�z��P���̃T�C�Y�j
	int list_max = sizeof( EList ) / sizeof( EList[0] );
	for (int i = 0; i < list_max; i++)
	{
		int listNo = EList[i].listNo;

		//�G�t�F�N�g�̓ǂݍ���
		m_pEffect[listNo] = ::EsEffect::Create(	m_pManager,	EList[i].path );
		
		if ( m_pEffect[listNo] == nullptr ) {
			_ASSERT_EXPR(false, L"Effect::LoadData()");
			return E_FAIL;
		}
	}
	return S_OK;
}


//=============================================================================
//		�`��
//=============================================================================
void Effect::Draw(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const LIGHT& Light)
{
	// �r���[�s���ݒ�
	SetViewMatrix( mView );

	// �v���W�F�N�V�����s���ݒ�
	SetProjectionMatrix( mProj );

	// ���C���[�p�����[�^�ݒ�
	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = ToEfkVector3( &Camera::GetInstance()->GetPos() );
	m_pManager->SetLayerParameter( 0, layerParameter );

	// �G�t�F�N�g�̍X�V����
	m_pManager->Update();

	//----------------------------------------------------------------------------
	//		Effekseer�����_�����O
	//----------------------------------------------------------------------------
	// �G�t�F�N�g�̕`��J�n�������s��
	m_pRenderer->BeginRendering();

	// �G�t�F�N�g�̕`����s��
	m_pManager->Draw();

	// �G�t�F�N�g�̕`��I���������s��
	m_pRenderer->EndRendering();
}


//=============================================================================
//		DirectX Vector3 �� Effekseer Vector3 �ɕϊ�����
//=============================================================================
::EsVec3 Effect::ToEfkVector3( const D3DXVECTOR3* pSrcVec3Dx )
{
	return ::EsVec3( pSrcVec3Dx->x, pSrcVec3Dx->y, pSrcVec3Dx->z );
}


//=============================================================================
//		Effekseer Vector3 �� DirectX Vector3 �ɕϊ�����
//=============================================================================
D3DXVECTOR3 Effect::ToDxVector3( const ::EsVec3* pSrcVec3Efk )
{
	return D3DXVECTOR3( pSrcVec3Efk->X, pSrcVec3Efk->Y, pSrcVec3Efk->Z );
}


//=============================================================================
//		DirectX Matrix �� Effekseer Matrix �ɕϊ�����
//=============================================================================
::EsMatrix Effect::ToEfkMatrix( const D3DXMATRIX* pSrcMatDx )
{
	::EsMatrix OutMatEfk;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			OutMatEfk.Values[i][j] = pSrcMatDx->m[i][j];
		}
	}

	return OutMatEfk;
}


//=============================================================================
//		Effekseer Matrix �� DirectX Matrix �ɕϊ�����
//=============================================================================
D3DXMATRIX Effect::ToDxMatrix( const::EsMatrix* pSrcMatEfk )
{
	D3DXMATRIX OutMatDx;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			OutMatDx.m[i][j] = pSrcMatEfk->Values[i][j];
		}
	}

	return OutMatDx;
}


//-----------------------------------------------------------------------------
//		�f�[�^���
//-----------------------------------------------------------------------------
HRESULT Effect::ReleaseData()
{
	return S_OK;
}


//-----------------------------------------------------------------------------
//		�r���[�s���ݒ�
//-----------------------------------------------------------------------------
void Effect::SetViewMatrix(const D3DXMATRIX& mView)
{
	::EsMatrix EsCamMat;	// �J�����s��
	EsCamMat = ToEfkMatrix(&mView);

	// �J�����s���ݒ�
	m_pRenderer->SetCameraMatrix(EsCamMat);
}


//-----------------------------------------------------------------------------
//		�v���W�F�N�V�����s���ݒ�
//-----------------------------------------------------------------------------
void Effect::SetProjectionMatrix(const D3DXMATRIX& mProj)
{
	::EsMatrix EsProjMat;	// �v���W�F�N�V�����s��
	EsProjMat = ToEfkMatrix(&mProj);

	// �v���W�F�N�V�����s���ݒ�
	m_pRenderer->SetProjectionMatrix(EsProjMat);
}