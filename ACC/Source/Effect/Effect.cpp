#include "Effect.h"
#include "Camera/Camera.h"


//描画スプライト最大数
constexpr int RENDER_SPRITE_MAX = 8000;
//エフェクト管理用インスタンス最大数
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
	// エフェクトデータの解放
	ReleaseData();

	// エフェクトのマネージャーを破棄
	m_pManager.Reset();

	// エフェクトのレンダラーを破棄
	m_pRenderer.Reset();
}


//=============================================================================
//		構築
//=============================================================================
HRESULT Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// エフェクトのマネージャの作成
	m_pManager = ::EsManager::Create( RENDER_SPRITE_MAX );

	// エフェクトのレンダラの作成
	m_pRenderer = ::EsRenderer::Create( pDevice, pContext, EFFECT_INSTANCE_MAX );

	// 描画モジュールの設定
	m_pManager->SetSpriteRenderer( m_pRenderer->CreateSpriteRenderer() );
	m_pManager->SetRibbonRenderer( m_pRenderer->CreateRibbonRenderer() );
	m_pManager->SetRingRenderer( m_pRenderer->CreateRingRenderer() );
	m_pManager->SetTrackRenderer( m_pRenderer->CreateTrackRenderer() );
	m_pManager->SetModelRenderer( m_pRenderer->CreateModelRenderer() );

	// テクスチャ、モデル、マテリアルローダーの設定をする
	//	ユーザーが独自に拡張できる.現在はファイルから読み込んでいる
	m_pManager->SetTextureLoader( m_pRenderer->CreateTextureLoader() );
	m_pManager->SetModelLoader( m_pRenderer->CreateModelLoader() );
	m_pManager->SetMaterialLoader( m_pRenderer->CreateMaterialLoader() );
	m_pManager->SetCurveLoader( ::Es::MakeRefPtr<::Es::CurveLoader>() );

	return S_OK;
}


//=============================================================================
//		データ読み込み
//=============================================================================
HRESULT Effect::LoadData()
{
	struct EffectList
	{
		enList listNo;				// enList列挙型を設定
		const char16_t path[256];	// ファイルの名前(パス付き)
	};
	EffectList EList[] =
	{
		// u""は、UTF-16エンコーディングの文字列リテラルで、const char16_t*に代入可能
		{ enList::CritHit,		u"Data\\Effekseer\\BodyHitCrit.efk"	},
		{ enList::Hit,			u"Data\\Effekseer\\ShieldHit.efk"	},
		
		{ enList::GunFire,		u"Data\\Effekseer\\Fire.efk"		},
		{ enList::Dust,			u"Data\\Effekseer\\Dust.efk"		},

		{ enList::BulletSmoke,	u"Data\\Effekseer\\BulletSmoke.efk"	},
	};

	// 配列の最大要素数を算出（配列全体のサイズ／配列１つ分のサイズ）
	int list_max = sizeof( EList ) / sizeof( EList[0] );
	for (int i = 0; i < list_max; i++)
	{
		int listNo = EList[i].listNo;

		//エフェクトの読み込み
		m_pEffect[listNo] = ::EsEffect::Create(	m_pManager,	EList[i].path );
		
		if ( m_pEffect[listNo] == nullptr ) {
			_ASSERT_EXPR(false, L"Effect::LoadData()");
			return E_FAIL;
		}
	}
	return S_OK;
}


//=============================================================================
//		描画
//=============================================================================
void Effect::Draw(const D3DXMATRIX& mView, const D3DXMATRIX& mProj, const LIGHT& Light)
{
	// ビュー行列を設定
	SetViewMatrix( mView );

	// プロジェクション行列を設定
	SetProjectionMatrix( mProj );

	// レイヤーパラメータ設定
	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = ToEfkVector3( &Camera::GetInstance()->GetPos() );
	m_pManager->SetLayerParameter( 0, layerParameter );

	// エフェクトの更新処理
	m_pManager->Update();

	//----------------------------------------------------------------------------
	//		Effekseerレンダリング
	//----------------------------------------------------------------------------
	// エフェクトの描画開始処理を行う
	m_pRenderer->BeginRendering();

	// エフェクトの描画を行う
	m_pManager->Draw();

	// エフェクトの描画終了処理を行う
	m_pRenderer->EndRendering();
}


//=============================================================================
//		DirectX Vector3 を Effekseer Vector3 に変換する
//=============================================================================
::EsVec3 Effect::ToEfkVector3( const D3DXVECTOR3* pSrcVec3Dx )
{
	return ::EsVec3( pSrcVec3Dx->x, pSrcVec3Dx->y, pSrcVec3Dx->z );
}


//=============================================================================
//		Effekseer Vector3 を DirectX Vector3 に変換する
//=============================================================================
D3DXVECTOR3 Effect::ToDxVector3( const ::EsVec3* pSrcVec3Efk )
{
	return D3DXVECTOR3( pSrcVec3Efk->X, pSrcVec3Efk->Y, pSrcVec3Efk->Z );
}


//=============================================================================
//		DirectX Matrix を Effekseer Matrix に変換する
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
//		Effekseer Matrix を DirectX Matrix に変換する
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
//		データ解放
//-----------------------------------------------------------------------------
HRESULT Effect::ReleaseData()
{
	return S_OK;
}


//-----------------------------------------------------------------------------
//		ビュー行列を設定
//-----------------------------------------------------------------------------
void Effect::SetViewMatrix(const D3DXMATRIX& mView)
{
	::EsMatrix EsCamMat;	// カメラ行列
	EsCamMat = ToEfkMatrix(&mView);

	// カメラ行列を設定
	m_pRenderer->SetCameraMatrix(EsCamMat);
}


//-----------------------------------------------------------------------------
//		プロジェクション行列を設定
//-----------------------------------------------------------------------------
void Effect::SetProjectionMatrix(const D3DXMATRIX& mProj)
{
	::EsMatrix EsProjMat;	// プロジェクション行列
	EsProjMat = ToEfkMatrix(&mProj);

	// プロジェクション行列を設定
	m_pRenderer->SetProjectionMatrix(EsProjMat);
}