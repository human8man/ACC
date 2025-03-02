#include "CStaticMeshObject.h"
#include "Camera/CCamera.h"


//============================================================================
//		StaticMeshObjectクラス.
//============================================================================
CStaticMeshObject::CStaticMeshObject()
	: m_pMesh	( nullptr )
{
}
CStaticMeshObject::~CStaticMeshObject()
{
	DetachMesh();
}


//============================================================================
//		更新処理.
//============================================================================
void CStaticMeshObject::Update()
{
	if( m_pMesh == nullptr ){ return; }
}


//============================================================================
//		描画処理.
//============================================================================
void CStaticMeshObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	if( m_pMesh == nullptr ){ return; }

	//描画直前で座標や回転情報などを更新.
	m_pMesh->SetPos( m_vPosition );
	m_pMesh->SetRot( m_vRotation );
	m_pMesh->SetScale( m_vScale );

	//レンダリング.
	m_pMesh->Render( View, Proj, Light );
}


//============================================================================
//		レイとメッシュの当たり判定.
//============================================================================
RayInfo CStaticMeshObject::IsHitForRay(const RAY& pRay )
{
	FLOAT vDistance;
	D3DXVECTOR3 vAxis, StartPoint, EndPoint, vIntersect;
	D3DXMATRIX mTran, mRot, mScale, mWorld, mInverseWorld, mYaw, mPitch, mRoll;

	// レイの方向ベクトルと位置を設定.
	vAxis		= pRay.Axis;	 // レイの方向（軸ベクトル）.
	StartPoint	= pRay.Position; // レイの開始位置.
	EndPoint = StartPoint + (vAxis * pRay.Length); // レイの終点を計算.

	// 移動処理.
	D3DXMatrixTranslation( &mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// 回転処理.
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);	// Y軸回転行列作成.
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);// X軸回転行列作成.
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);	// Z軸回転行列作成.
	// 回転行列作成.
	mRot = mYaw * mPitch * mRoll;

	// 拡縮処理.
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// ワールド行列計算.
	mWorld = mScale * mTran;

	// 逆行列を求める.
	D3DXMatrixInverse( &mInverseWorld, nullptr, &mWorld );
	// レイの始点、終点に反映.
	D3DXVec3TransformCoord( &StartPoint, &StartPoint, &mInverseWorld );
	D3DXVec3TransformCoord( &EndPoint, &EndPoint, &mInverseWorld );

	// 向きと長さ（大きさ）を求める.
	D3DXVECTOR3 vDirection = EndPoint - StartPoint;

	BOOL bHit = FALSE;		// 命中フラグ.
	DWORD dwIndex = 0;		// インデックス番号.
	D3DXVECTOR3 Vertex[3];	// 頂点座標.
	FLOAT U = 0, V = 0;		// 重心ヒット座標.

	// メッシュとレイの交差を調べる.
	D3DXIntersect(
		m_pMesh->GetMeshForRay(),	// 対象メッシュ.
		&StartPoint,				// レイの始点.
		&vDirection,				// レイの向きと長さ（大きさ）.
		&bHit,						// (out)判定結果.
		&dwIndex,					// (out)bHitがTRUE時にレイの始点に最も近くの面のインデックス値へのポインタ.	
		&U, &V,						// (out)重心ヒット座標.
		&vDistance,					// (out)メッシュとの距離.
		nullptr, nullptr);

	// 無限に伸びるレイのどこかでメッシュが当たっていたら.
	if (bHit)
	{
		// 命中した時.
		FindVerticesOnPoly( m_pMesh->GetMeshForRay(), dwIndex, Vertex );

		// 重心座標から交点を算出.
		//	ローカル交点は v0 + U*(v1-v0) + V*(v2-v0) で求まる.
		vIntersect = Vertex[0] + U * (Vertex[1] - Vertex[0]) + V * (Vertex[2] - Vertex[0]);

		// モデルデータが「拡縮」「回転」「移動」していれば行列が必要.
		D3DXVec3TransformCoord( &vIntersect, &vIntersect, &mWorld );

		// Hit状態と交差点を返す.
		return { true, vIntersect ,vDistance }; 
	}

	return { false, ZEROVEC3, 9999.f };
}


//============================================================================
//		壁からの位置を計算する.
//============================================================================
void CStaticMeshObject::CalculatePositionFromWall(CROSSRAY* pCrossRay)
{
	FLOAT		Distance[CROSSRAY::max];	// 各レイごとの距離.
	D3DXVECTOR3 Intersect[CROSSRAY::max];	// 各レイごとの交差点.

	// レイの向きにより当たる壁までの距離を求める.軸ベクトル（前後左右）.
	for (int i = 0; i < CROSSRAY::max; i++) {
		auto [hit, hitPoint, length] = IsHitForRay(pCrossRay->Ray[i]);
		Intersect[i] = hitPoint;
		Distance[i]  = length;
	}

	// 回転情報は全てのレイで同じはずなのでXLを使用する.
	float RotY = fabsf( pCrossRay->Ray[CROSSRAY::XL].RotationY ); // fabsf関数:絶対値(float版).
	ClampDirection( &RotY ); // 0～2πの間に収める.

	//----------------------------
	//	定数宣言.
	//----------------------------
	static constexpr float WSPACE = 0.8f;	// 壁との限界距離.
	static constexpr float DEG45  = D3DXToRadian(  45.f );	// 0.785f.
	static constexpr float DEG135 = D3DXToRadian( 135.f );	// 2.355f.
	static constexpr float DEG225 = D3DXToRadian( 225.f );	// 3.925f.
	static constexpr float DEG315 = D3DXToRadian( 315.f );	// 5.496f.

	float Dis = 0.0f;
	float		TrgRotY = pCrossRay->Ray[CROSSRAY::XL].RotationY;
	D3DXVECTOR3 TrgPos  = pCrossRay->Ray[CROSSRAY::XL].Position;

	// 前が壁に接近.
	Dis = Distance[CROSSRAY::ZF];
	if (0.01f < Dis && Dis < WSPACE) {
		// 時計回り.
		if (TrgRotY < 0.f)	{
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.x += WSPACE - Dis; } // 右から.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; } // 左から.
			else										{ TrgPos.z -= WSPACE - Dis; } // 奥から.
		}
		// 反時計回り.
		else {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.x -= WSPACE - Dis; } // 右から.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; } // 左から.
			else										{ TrgPos.z -= WSPACE - Dis; } // 奥から.
		}
	}

	// 後ろが壁に接近.
	Dis = Distance[CROSSRAY::ZB];
	if (0.01f < Dis && Dis < WSPACE) {
		// 時計回り.
		if (TrgRotY < 0.0f) {
			if		(DEG45 <= RotY && RotY < DEG135)	{ TrgPos.x -= WSPACE - Dis; } // 右から.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; } // 左から.
			else										{ TrgPos.z += WSPACE - Dis; } // 奥から.
		}
		// 反時計回り.
		else {
			if		(DEG45 <= RotY && RotY < DEG135)	{ TrgPos.x += WSPACE - Dis; } // 右から.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; } // 左から.
			else										{ TrgPos.z += WSPACE - Dis; } // 奥から.
		}
	}

	// 右が壁に接近.
	Dis = Distance[CROSSRAY::XR];
	if (0.01f < Dis && Dis < WSPACE) {
		// 時計回り.
		if (TrgRotY < 0.0f) {
			if		(DEG45 <= RotY && RotY < DEG135)	{ TrgPos.z -= WSPACE - Dis; } // 右から.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; } // 左から.
			else										{ TrgPos.x -= WSPACE - Dis; } // 奥から.
		}
		// 反時計回り.
		else {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.z += WSPACE - Dis; } // 右から.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; } // 左から.
			else										{ TrgPos.x -= WSPACE - Dis; } // 奥から.
		}
	}

	// 左が壁に接近.
	Dis = Distance[CROSSRAY::XL];
	if (0.01f < Dis && Dis < WSPACE) {
		// 時計回り.
		if (TrgRotY < 0.0f) {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.z += WSPACE - Dis; } // 右から.
			else if	(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; } // 左から.
			else										{ TrgPos.x += WSPACE - Dis; } // 奥から.
		}
		// 反時計回り.
		else {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.z -= WSPACE - Dis; } // 右から.
			else if	(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; } // 前から.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; } // 左から.
			else										{ TrgPos.x += WSPACE - Dis; } // 奥から.
		}
	}

	// 最終的な座標をレイに返す.
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		pCrossRay->Ray[dir].Position = TrgPos;
	}
}


//----------------------------------------------------------------------------
//	交差位置のポリゴンの頂点を見つける.
//		※頂点座標はローカル座標.
//----------------------------------------------------------------------------
HRESULT CStaticMeshObject::FindVerticesOnPoly(
	LPD3DXMESH pMesh,
	DWORD dwPolyIndex,
	D3DXVECTOR3* pVertices)
{
	DWORD dwStride	  = pMesh->GetNumBytesPerVertex();// 頂点ごとのバイト数を取得.
	DWORD dwVertexAmt = pMesh->GetNumVertices();	  // 頂点数を取得.
	DWORD dwPoly	  = pMesh->GetNumFaces();		  // 面数を取得.
	WORD* pwPoly	  = nullptr;

	// インデックスバッファをロック（読み込みモード）.
	pMesh->LockIndexBuffer(
		D3DLOCK_READONLY,
		reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr;				// 頂点(バイト型).
	FLOAT* pfVertices = nullptr;			// 頂点(float型).
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	// 頂点バッファ.

	// 頂点情報の取得.
	pMesh->GetVertexBuffer(&VB);

	// 頂点バッファのロック.
	if (SUCCEEDED( VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0 ) ))
	{
		// ポリゴンの頂点１つ目を取得.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];

		// ポリゴンの頂点２つ目を取得.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];

		// ポリゴンの頂点３つ目を取得.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer();	// ロック解除.
		VB->Unlock();	// ロック解除.
	}
	VB->Release();	// 不要になったので解放.

	return S_OK;
}


//----------------------------------------------------------------------------
//		回転値調整（１周以上している時の調整）.
//----------------------------------------------------------------------------
void CStaticMeshObject::ClampDirection(float* dir)
{
	// １周以上している場合、2π(360°)分引く.
	if (*dir > D3DX_PI * 2.0f) {
		*dir -= D3DX_PI * 2.0f;
	}

	// 再帰関数.
	if (*dir > D3DX_PI * 2.0f) {
		ClampDirection( dir );
	}
}
