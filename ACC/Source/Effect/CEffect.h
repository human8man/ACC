#pragma once

//警告についてのコード分析を無効にする.4005:再定義.
#pragma warning( disable : 4005 )

//---------------------------------------.
//	Effekseer関係ヘッダ、ライブラリ.
//---------------------------------------.
#include "Effect/Effekseer/include/Effekseer.h"
#include "Effect/Effekseer/include/EffekseerRendererDX11.h"

#ifdef _DEBUG
	#pragma comment( lib, "Effekseerd.lib" )
	#pragma comment( lib, "EffekseerRendererDX11d.lib" )
#else//#ifdef _DEBUG
	#pragma comment( lib, "Effekseer.lib" )
	#pragma comment( lib, "EffekseerRendererDX11.lib" )
#endif//#ifdef _DEBUG

//エイリアスを用意.
//※コードが横に長くなって読み辛いため.
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

/**************************************************
*	フリーソフト Effekseerのデータを使うためのクラス.
*	singleton(シングルトン：デザインパターンの１つ）で作成.
**/
class CEffect
{
public:
	//エフェクト種類列挙型.
	enum enList
	{
		Test0	= 0,	//仮で設定.
		Test1,			//仮で設定.
		Test2,			//仮で設定.
		Max,			//最大数.
	};

	//インスタンス取得(唯一のアクセス経路).
	static CEffect* GetInstance()
	{
		//唯一のインスタンスを作成する.
		//※staticで作成されたので2回目以降は下の1行が無視される.
		static CEffect s_Instance;	//s_:staticの意味.
		return &s_Instance;
	}
	~CEffect();

	//構築.
	HRESULT Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//データ読み込み.
	HRESULT LoadData();
	//描画.
	void Draw(const D3DXMATRIX& mView, const D3DXMATRIX& mProj,
		const LIGHT& Light, const CAMERA& Camera);

	//--------------------------------------.
	//	変換系.
	//--------------------------------------.
	//ベクター.
	::EsVec3 ToEfkVector3( const D3DXVECTOR3* pSrcVec3Dx );
	D3DXVECTOR3 ToDxVector3( const ::EsVec3* pSrcVec3Efk );
	//行列.
	::EsMatrix ToEfkMatrix( const D3DXMATRIX* pSrcMatDx );
	D3DXMATRIX ToDxMatrix( const ::EsMatrix* pSrcMatEfk );

	//--------------------------------------.
	//	制御系.
	//--------------------------------------.
	//再生.
	static ::EsHandle Play( enList list, const D3DXVECTOR3& pos ){
		CEffect* pE = CEffect::GetInstance();
		return pE->m_pManager->Play( pE->m_pEffect[list], pos.x, pos.y, pos.z);
	}
	//停止.
	static void Stop( ::EsHandle handle ){
		CEffect::GetInstance()->m_pManager->StopEffect( handle );
	}
	//全て停止.
	static void StopAll() {
		CEffect::GetInstance()->m_pManager->StopAllEffects();
	}

private:
	//生成やコピーを禁止する.
	CEffect();
	CEffect( const CEffect& rhs ) = delete;
	CEffect& operator = (const CEffect& rhs) = delete;

	//データ解放.
	HRESULT ReleaseData();

	//ビュー行列を設定.
	void SetViewMatrix( const D3DXMATRIX& mView );
	//プロジェクション行列を設定.
	void SetProjectionMatrix( const D3DXMATRIX& mProj );

private:
	//エフェクトを動作させるために必要.
	::EsManagerRef		m_pManager;
	::EsRendererRef		m_pRenderer;

	//エフェクトの種類ごとに必要.
	::EsEffectRef		m_pEffect[enList::Max];
};

