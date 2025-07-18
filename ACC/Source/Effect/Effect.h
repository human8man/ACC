#pragma once

// 警告についてのコード分析を無効にする
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

// エイリアスを用意
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
//		フリーソフト Effekseer のデータを使うためのクラス
//=============================================================================
class Effect
	: public Singleton<Effect>
{
public:
	friend class Singleton<Effect>; // シングルトンクラスをフレンド宣言

public:
	// エフェクト種類列挙型
	enum enList
	{
		CritHit,	// クリティカル(ヘッドショット)
		Hit,		// 命中

		GunFire,	// 発射時の銃口のエフェクト
		Dust,		// ダスト

		BulletSmoke,// 弾の軌道

		Max,		// 最大数
	};

public:
	Effect();
	~Effect();

	// 作成処理
	HRESULT Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// データ読み込み処理
	HRESULT LoadData();
	// 描画処理
	void Draw(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const LIGHT& Light);

	//--------------------------------------------------------------------------
	//		変換系
	//--------------------------------------------------------------------------
	// ベクター
	::EsVec3 ToEfkVector3( const D3DXVECTOR3* pSrcVec3Dx );
	D3DXVECTOR3 ToDxVector3( const ::EsVec3* pSrcVec3Efk );
	// 行列
	::EsMatrix ToEfkMatrix( const D3DXMATRIX* pSrcMatDx );
	D3DXMATRIX ToDxMatrix( const ::EsMatrix* pSrcMatEfk );

	//--------------------------------------------------------------------------
	//		制御系
	//--------------------------------------------------------------------------
	// 再生
	static ::EsHandle Play( enList list, const D3DXVECTOR3& pos ){
		Effect* pE = Effect::GetInstance();
		return pE->m_pManager->Play( pE->m_pEffect[list], pos.x, pos.y, pos.z);
	}
	// 停止
	static void Stop( ::EsHandle handle ){
		Effect::GetInstance()->m_pManager->StopEffect( handle );
	}
	// 全て停止
	static void StopAll() {
		Effect::GetInstance()->m_pManager->StopAllEffects();
	}
	// 回転
	static void SetRot(::EsHandle handle, const D3DXVECTOR3& rot) {
		Effect::GetInstance()->m_pManager->SetRotation(handle, rot.x, rot.y, rot.z);
	}
	// スケール
	static void SetScale(::EsHandle handle, const D3DXVECTOR3& size) {
		Effect::GetInstance()->m_pManager->SetScale(handle, size.x, size.y, size.z);
	}
	// 速度
	static void SetTimeScale(::EsHandle handle, float scale) {
		Effect::GetInstance()->m_pManager->SetTimeScaleByHandle(handle, scale);
	}

private:
	// データ解放
	HRESULT ReleaseData();

	// ビュー行列を設定
	void SetViewMatrix( const D3DXMATRIX& mView );
	// プロジェクション行列を設定
	void SetProjectionMatrix( const D3DXMATRIX& mProj );

private:
	// エフェクトを動作させるために必要
	::EsManagerRef		m_pManager;
	::EsRendererRef		m_pRenderer;

	// エフェクトの種類ごとに必要
	::EsEffectRef		m_pEffect[enList::Max];
};

