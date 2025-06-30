#pragma once

// �x���ɂ��ẴR�[�h���͂𖳌��ɂ���
#pragma warning( disable : 4005 )
#pragma warning( disable : 4099 )
#pragma warning( disable : 26495 )
#pragma warning( disable : 6385 )


#include "Effect/Effekseer/include/Effekseer.h"
#include "Effect/Effekseer/include/EffekseerRendererDX11.h"
#include "Singleton/Singleton.h"

#ifdef _DEBUG
#pragma comment( lib, "Effekseerd.lib" )
#pragma comment( lib, "EffekseerRendererDX11d.lib" )
#else// #ifdef _DEBUG
#pragma comment( lib, "Effekseer.lib" )
#pragma comment( lib, "EffekseerRendererDX11.lib" )
#endif// #ifdef _DEBUG

// �G�C���A�X��p��
namespace {
	namespace Es		= ::Effekseer;
	using EsManagerRef	= ::Es::ManagerRef;
	using EsManager		= ::Es::Manager;
	using EsEffectRef	= ::Es::EffectRef;
	using EsEffect		= ::Es::Effect;
	using EsVec3		= ::Es::Vector3D;
	using EsMatrix		= ::Es::Matrix44;
	using EsHandle		= ::Es::Handle;
	using EsRendererRef = ::EffekseerRendererDX11::RendererRef;
	using EsRenderer	= ::EffekseerRendererDX11::Renderer;
};


//=============================================================================
//		�t���[�\�t�g Effekseer �̃f�[�^���g�����߂̃N���X
//=============================================================================
class Effect
	: public Singleton<Effect>
{
public:
	friend class Singleton<Effect>; // �V���O���g���N���X���t�����h�錾

public:
	// �G�t�F�N�g��ޗ񋓌^
	enum enList
	{
		CritHit,	// �N���e�B�J��(�w�b�h�V���b�g)
		Hit,		// ����

		GunFire,	// ���ˎ��̏e���̃G�t�F�N�g
		Dust,		// �_�X�g

		BulletSmoke,// �e�̋O��

		Max,		// �ő吔
	};

public:
	Effect();
	~Effect();

	// �쐬����
	HRESULT Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// �f�[�^�ǂݍ��ݏ���
	HRESULT LoadData();
	// �`�揈��
	void Draw(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const LIGHT& Light);

	//--------------------------------------------------------------------------
	//		�ϊ��n
	//--------------------------------------------------------------------------
	// �x�N�^�[
	::EsVec3 ToEfkVector3( const D3DXVECTOR3* pSrcVec3Dx );
	D3DXVECTOR3 ToDxVector3( const ::EsVec3* pSrcVec3Efk );
	// �s��
	::EsMatrix ToEfkMatrix( const D3DXMATRIX* pSrcMatDx );
	D3DXMATRIX ToDxMatrix( const ::EsMatrix* pSrcMatEfk );

	//--------------------------------------------------------------------------
	//		����n
	//--------------------------------------------------------------------------
	// �Đ�
	static ::EsHandle Play( enList list, const D3DXVECTOR3& pos ){
		Effect* pE = Effect::GetInstance();
		return pE->m_pManager->Play( pE->m_pEffect[list], pos.x, pos.y, pos.z);
	}
	// ��~
	static void Stop( ::EsHandle handle ){
		Effect::GetInstance()->m_pManager->StopEffect( handle );
	}
	// �S�Ē�~
	static void StopAll() {
		Effect::GetInstance()->m_pManager->StopAllEffects();
	}
	// ��]
	static void SetRot(::EsHandle handle, const D3DXVECTOR3& rot) {
		Effect::GetInstance()->m_pManager->SetRotation(handle, rot.x, rot.y, rot.z);
	}
	// �X�P�[��
	static void SetScale(::EsHandle handle, const D3DXVECTOR3& size) {
		Effect::GetInstance()->m_pManager->SetScale(handle, size.x, size.y, size.z);
	}
	// ���x
	static void SetTimeScale(::EsHandle handle, float scale) {
		Effect::GetInstance()->m_pManager->SetTimeScaleByHandle(handle, scale);
	}

private:
	// �f�[�^���
	HRESULT ReleaseData();

	// �r���[�s���ݒ�
	void SetViewMatrix( const D3DXMATRIX& mView );
	// �v���W�F�N�V�����s���ݒ�
	void SetProjectionMatrix( const D3DXMATRIX& mProj );

private:
	// �G�t�F�N�g�𓮍삳���邽�߂ɕK�v
	::EsManagerRef		m_pManager;
	::EsRendererRef		m_pRenderer;

	// �G�t�F�N�g�̎�ނ��ƂɕK�v
	::EsEffectRef		m_pEffect[enList::Max];
};

