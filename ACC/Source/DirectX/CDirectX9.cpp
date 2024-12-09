#include "CDirectX9.h"

CDirectX9::CDirectX9()
	: m_pDevice9	( nullptr )
{

}

CDirectX9::~CDirectX9()
{

}

//Dx9初期化.
HRESULT CDirectX9::Create( HWND hWnd )
{
	//「Direct3D」オブジェクトの作成.
	LPDIRECT3D9 pD3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( pD3d9 == nullptr ) {
		_ASSERT_EXPR( false, _T( "Dx9オブジェクト作成失敗" ) );
		return E_FAIL;
	}

	//Diret3Dデバイスオブジェクトの作成.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	//バックバッファのフォーマット(デフォルト).
	d3dpp.BackBufferCount = 1;					//バックバッファの数.
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//スワップエフェクト(デフォルト).
	d3dpp.Windowed = true;						//ウィンドウモード.
	d3dpp.EnableAutoDepthStencil = true;		//ステンシル有効.
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	//ステンシルのフォーマット(16bit).

	//デバイス作成(HALモード：描画と頂点処理をGPUで行う).
	if( FAILED( pD3d9->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pDevice9 ) ) )
	{
		//デバイス作成(HALモード：描画はGPU、頂点処理をCPUで行う).
		if( FAILED( pD3d9->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &m_pDevice9 ) ) )
		{
			MessageBox( nullptr,
				_T( "HALモードでDIRECT3Dデバイス作成できません\nREFモードで再試行します" ),
				_T( "警告 "), MB_OK );

			//デバイス作成(REFモード：描画はCPU、頂点処理をGPUで行う).
			if( FAILED( pD3d9->CreateDevice(
				D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&d3dpp, &m_pDevice9 ) ) )
			{
				//デバイス作成(REFモード：描画と頂点処理をCPUで行う).
				if( FAILED( pD3d9->CreateDevice(
					D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&d3dpp, &m_pDevice9 ) ) )
				{
					MessageBox( nullptr,
						_T( "DIRECT3Dデバイス作成失敗" ),
						_T( "警告" ), MB_OK );

					SAFE_RELEASE( pD3d9 );
					return E_FAIL;
				}
			}
		}
	}

	SAFE_RELEASE( pD3d9 );
	return S_OK;
}


void CDirectX9::Release()
{

}