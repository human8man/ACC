#include "CEffect.h"
#include "Camera/CCamera.h"

//描画スプライト最大数.
constexpr int RENDER_SPRITE_MAX = 8000;
//エフェクト管理用インスタンス最大数.
constexpr int EFFECT_INSTANCE_MAX = 1000;


CEffect::CEffect()
	: m_pManager	( nullptr )
	, m_pRenderer	( nullptr )

	, m_pEffect		()
{
}
//データ解放.
HRESULT CEffect::ReleaseData()
{
	return S_OK;
}

//ビュー行列を設定.
void CEffect::SetViewMatrix( const D3DXMATRIX& mView )
{
	::EsMatrix EsCamMat;	//カメラ行列.
	EsCamMat = ToEfkMatrix( &mView );

	//カメラ行列を設定.
	m_pRenderer->SetCameraMatrix( EsCamMat );
}

//プロジェクション行列を設定.
void CEffect::SetProjectionMatrix( const D3DXMATRIX& mProj )
{
	::EsMatrix EsProjMat;	//プロジェクション行列.
	EsProjMat = ToEfkMatrix( &mProj );

	//プロジェクション行列を設定.
	m_pRenderer->SetProjectionMatrix( EsProjMat );
}

CEffect::~CEffect()
{
	//エフェクトデータの解放.
	ReleaseData();

	//エフェクトのマネージャーを破棄.
	m_pManager.Reset();

	//エフェクトのレンダラーを破棄.
	m_pRenderer.Reset();
}

//構築.
HRESULT CEffect::Create(
	ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//エフェクトのマネージャの作成.
	m_pManager = ::EsManager::Create( RENDER_SPRITE_MAX );

	//エフェクトのレンダラの作成.
	m_pRenderer = ::EsRenderer::Create( pDevice, pContext, EFFECT_INSTANCE_MAX );

	//描画モジュールの設定.
	m_pManager->SetSpriteRenderer( m_pRenderer->CreateSpriteRenderer() );
	m_pManager->SetRibbonRenderer( m_pRenderer->CreateRibbonRenderer() );
	m_pManager->SetRingRenderer( m_pRenderer->CreateRingRenderer() );
	m_pManager->SetTrackRenderer( m_pRenderer->CreateTrackRenderer() );
	m_pManager->SetModelRenderer( m_pRenderer->CreateModelRenderer() );

	//テクスチャ、モデル、マテリアルローダーの設定をする.
	//ユーザーが独自に拡張できる.現在はファイルから読み込んでいる.
	m_pManager->SetTextureLoader( m_pRenderer->CreateTextureLoader() );
	m_pManager->SetModelLoader( m_pRenderer->CreateModelLoader() );
	m_pManager->SetMaterialLoader( m_pRenderer->CreateMaterialLoader() );
	m_pManager->SetCurveLoader( ::Es::MakeRefPtr<::Es::CurveLoader>() );

	return S_OK;
}

//データ読み込み.
HRESULT CEffect::LoadData()
{
	struct EffectList
	{
		enList listNo;				//enList列挙型を設定.
		const char16_t path[256];	//ファイルの名前(パス付き).
	};
	EffectList EList[] =
	{
		//u""は、UTF-16エンコーディングの文字列リテラルで、const char16_t*に代入可能.
		{ enList::Test0, u"Data\\Effekseer\\Laser01.efk"					},
		{ enList::Test1, u"Data\\Effekseer\\Simple_Sprite_BillBoard.efk"	},
		{ enList::Test2, u"Data\\Effekseer\\Simple_Sprite_Fixed.efk"		},
	};
	//配列の最大要素数を算出（配列全体のサイズ／配列１つ分のサイズ）.
	int list_max = sizeof( EList ) / sizeof( EList[0] );
	for (int i = 0; i < list_max; i++)
	{
		int listNo = EList[i].listNo;

		//エフェクトの読み込み.
		m_pEffect[listNo] = ::EsEffect::Create(	m_pManager,	EList[i].path );
		
		if ( m_pEffect[listNo] == nullptr ) {
			_ASSERT_EXPR(false, L"CEffect::LoadData()");
			return E_FAIL;
		}
	}
	return S_OK;
}

//描画.
void CEffect::Draw(
	const D3DXMATRIX& mView, const D3DXMATRIX& mProj,
	const LIGHT& Light)
{
	//ビュー行列を設定.
	SetViewMatrix( mView );

	//プロジェクション行列を設定.
	SetProjectionMatrix( mProj );

	//レイヤーパラメータ設定.
	Effekseer::Manager::LayerParameter layerParameter;
	layerParameter.ViewerPosition = ToEfkVector3( &CCamera::GetInstance()->GetPos() );
	m_pManager->SetLayerParameter( 0, layerParameter );

	//エフェクトの更新処理.
	m_pManager->Update();

	//----------------------------.
	//	Effekseerレンダリング.
	//----------------------------.
	//エフェクトの描画開始処理を行う.
	m_pRenderer->BeginRendering();

	//エフェクトの描画を行う.
	m_pManager->Draw();

	//エフェクトの描画終了処理を行う.
	m_pRenderer->EndRendering();
}


//DirectX Vector3 を Effekseer Vector3 に変換する.
::EsVec3 CEffect::ToEfkVector3( const D3DXVECTOR3* pSrcVec3Dx )
{
	return ::EsVec3( pSrcVec3Dx->x, pSrcVec3Dx->y, pSrcVec3Dx->z );
}

//Effekseer Vector3 を DirectX Vector3 に変換する.
D3DXVECTOR3 CEffect::ToDxVector3( const ::EsVec3* pSrcVec3Efk )
{
	return D3DXVECTOR3( pSrcVec3Efk->X, pSrcVec3Efk->Y, pSrcVec3Efk->Z );
}

//DirectX Matrix を Effekseer Matrix に変換する.
::EsMatrix CEffect::ToEfkMatrix( const D3DXMATRIX* pSrcMatDx )
{
	::EsMatrix OutMatEfk;
#if 0
	OutMatEfk.Values[0][0] = pSrcMatDx->m[0][0];//_11.
	OutMatEfk.Values[0][1] = pSrcMatDx->m[0][1];//_12.
	OutMatEfk.Values[0][2] = pSrcMatDx->m[0][2];//_13.
	OutMatEfk.Values[0][3] = pSrcMatDx->m[0][3];//_14.

	OutMatEfk.Values[1][0] = pSrcMatDx->m[1][0];//_21.
	OutMatEfk.Values[1][1] = pSrcMatDx->m[1][1];//_22.
	OutMatEfk.Values[1][2] = pSrcMatDx->m[1][2];//_23.
	OutMatEfk.Values[1][3] = pSrcMatDx->m[1][3];//_24.

	OutMatEfk.Values[2][0] = pSrcMatDx->m[2][0];//_31.
	OutMatEfk.Values[2][1] = pSrcMatDx->m[2][1];//_32.
	OutMatEfk.Values[2][2] = pSrcMatDx->m[2][2];//_33.
	OutMatEfk.Values[2][3] = pSrcMatDx->m[2][3];//_34.

	OutMatEfk.Values[3][0] = pSrcMatDx->m[3][0];//_41.
	OutMatEfk.Values[3][1] = pSrcMatDx->m[3][1];//_42.
	OutMatEfk.Values[3][2] = pSrcMatDx->m[3][2];//_43.
	OutMatEfk.Values[3][3] = pSrcMatDx->m[3][3];//_44.
#else
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			OutMatEfk.Values[i][j] = pSrcMatDx->m[i][j];
		}
	}
#endif
	return OutMatEfk;
}

//Effekseer Matrix を DirectX Matrix に変換する.
D3DXMATRIX CEffect::ToDxMatrix( const::EsMatrix* pSrcMatEfk )
{
	D3DXMATRIX OutMatDx;
#if 0
	OutMatDx.m[0][0] = pSrcMatEfk->Values[0][0];//_11.
	OutMatDx.m[0][1] = pSrcMatEfk->Values[0][1];//_12.
	OutMatDx.m[0][2] = pSrcMatEfk->Values[0][2];//_13.
	OutMatDx.m[0][3] = pSrcMatEfk->Values[0][3];//_14.

	OutMatDx.m[1][0] = pSrcMatEfk->Values[1][0];//_21.
	OutMatDx.m[1][1] = pSrcMatEfk->Values[1][1];//_22.
	OutMatDx.m[1][2] = pSrcMatEfk->Values[1][2];//_23.
	OutMatDx.m[1][3] = pSrcMatEfk->Values[1][3];//_24.

	OutMatDx.m[2][0] = pSrcMatEfk->Values[2][0];//_31.
	OutMatDx.m[2][1] = pSrcMatEfk->Values[2][1];//_32.
	OutMatDx.m[2][2] = pSrcMatEfk->Values[2][2];//_33.
	OutMatDx.m[2][3] = pSrcMatEfk->Values[2][3];//_34.

	OutMatDx.m[3][0] = pSrcMatEfk->Values[3][0];//_41.
	OutMatDx.m[3][1] = pSrcMatEfk->Values[3][1];//_42.
	OutMatDx.m[3][2] = pSrcMatEfk->Values[3][2];//_43.
	OutMatDx.m[3][3] = pSrcMatEfk->Values[3][3];//_44.
#else
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			OutMatDx.m[i][j] = pSrcMatEfk->Values[i][j];
		}
	}
#endif
	return OutMatDx;
}
