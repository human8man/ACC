#include "CStaticMeshObject.h"
#include "Camera/CCamera.h"


//============================================================================
//		StaticMeshObject�N���X.
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
//		�X�V����.
//============================================================================
void CStaticMeshObject::Update()
{
	if( m_pMesh == nullptr ){ return; }
}


//============================================================================
//		�`�揈��.
//============================================================================
void CStaticMeshObject::Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light )
{
	if( m_pMesh == nullptr ){ return; }

	//�`�撼�O�ō��W���]���Ȃǂ��X�V.
	m_pMesh->SetPos( m_vPosition );
	m_pMesh->SetRot( m_vRotation );
	m_pMesh->SetScale( m_vScale );

	//�����_�����O.
	m_pMesh->Render( View, Proj, Light );
}


//============================================================================
//		���C�ƃ��b�V���̓����蔻��.
//============================================================================
RayInfo CStaticMeshObject::IsHitForRay(const RAY& pRay )
{
	FLOAT vDistance;
	D3DXVECTOR3 vAxis, StartPoint, EndPoint, vIntersect;
	D3DXMATRIX mTran, mRot, mScale, mWorld, mInverseWorld, mYaw, mPitch, mRoll;

	// ���C�̕����x�N�g���ƈʒu��ݒ�.
	vAxis		= pRay.Axis;	 // ���C�̕����i���x�N�g���j.
	StartPoint	= pRay.Position; // ���C�̊J�n�ʒu.
	EndPoint = StartPoint + (vAxis * pRay.Length); // ���C�̏I�_���v�Z.

	// �ړ�����.
	D3DXMatrixTranslation( &mTran, m_vPosition.x, m_vPosition.y, m_vPosition.z);

	// ��]����.
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);	// Y����]�s��쐬.
	D3DXMatrixRotationX(&mPitch, m_vRotation.x);// X����]�s��쐬.
	D3DXMatrixRotationZ(&mRoll, m_vRotation.z);	// Z����]�s��쐬.
	// ��]�s��쐬.
	mRot = mYaw * mPitch * mRoll;

	// �g�k����.
	D3DXMatrixScaling(&mScale, m_vScale.x, m_vScale.y, m_vScale.z);

	// ���[���h�s��v�Z.
	mWorld = mScale * mTran;

	// �t�s������߂�.
	D3DXMatrixInverse( &mInverseWorld, nullptr, &mWorld );
	// ���C�̎n�_�A�I�_�ɔ��f.
	D3DXVec3TransformCoord( &StartPoint, &StartPoint, &mInverseWorld );
	D3DXVec3TransformCoord( &EndPoint, &EndPoint, &mInverseWorld );

	// �����ƒ����i�傫���j�����߂�.
	D3DXVECTOR3 vDirection = EndPoint - StartPoint;

	BOOL bHit = FALSE;		// �����t���O.
	DWORD dwIndex = 0;		// �C���f�b�N�X�ԍ�.
	D3DXVECTOR3 Vertex[3];	// ���_���W.
	FLOAT U = 0, V = 0;		// �d�S�q�b�g���W.

	// ���b�V���ƃ��C�̌����𒲂ׂ�.
	D3DXIntersect(
		m_pMesh->GetMeshForRay(),	// �Ώۃ��b�V��.
		&StartPoint,				// ���C�̎n�_.
		&vDirection,				// ���C�̌����ƒ����i�傫���j.
		&bHit,						// (out)���茋��.
		&dwIndex,					// (out)bHit��TRUE���Ƀ��C�̎n�_�ɍł��߂��̖ʂ̃C���f�b�N�X�l�ւ̃|�C���^.	
		&U, &V,						// (out)�d�S�q�b�g���W.
		&vDistance,					// (out)���b�V���Ƃ̋���.
		nullptr, nullptr);

	// �����ɐL�т郌�C�̂ǂ����Ń��b�V�����������Ă�����.
	if (bHit)
	{
		// ����������.
		FindVerticesOnPoly( m_pMesh->GetMeshForRay(), dwIndex, Vertex );

		// �d�S���W�����_���Z�o.
		//	���[�J����_�� v0 + U*(v1-v0) + V*(v2-v0) �ŋ��܂�.
		vIntersect = Vertex[0] + U * (Vertex[1] - Vertex[0]) + V * (Vertex[2] - Vertex[0]);

		// ���f���f�[�^���u�g�k�v�u��]�v�u�ړ��v���Ă���΍s�񂪕K�v.
		D3DXVec3TransformCoord( &vIntersect, &vIntersect, &mWorld );

		// Hit��Ԃƌ����_��Ԃ�.
		return { true, vIntersect ,vDistance }; 
	}

	return { false, ZEROVEC3, 9999.f };
}


//============================================================================
//		�ǂ���̈ʒu���v�Z����.
//============================================================================
void CStaticMeshObject::CalculatePositionFromWall(CROSSRAY* pCrossRay)
{
	FLOAT		Distance[CROSSRAY::max];	// �e���C���Ƃ̋���.
	D3DXVECTOR3 Intersect[CROSSRAY::max];	// �e���C���Ƃ̌����_.

	// ���C�̌����ɂ�蓖����ǂ܂ł̋��������߂�.���x�N�g���i�O�㍶�E�j.
	for (int i = 0; i < CROSSRAY::max; i++) {
		auto [hit, hitPoint, length] = IsHitForRay(pCrossRay->Ray[i]);
		Intersect[i] = hitPoint;
		Distance[i]  = length;
	}

	// ��]���͑S�Ẵ��C�œ����͂��Ȃ̂�XL���g�p����.
	float RotY = fabsf( pCrossRay->Ray[CROSSRAY::XL].RotationY ); // fabsf�֐�:��Βl(float��).
	ClampDirection( &RotY ); // 0�`2�΂̊ԂɎ��߂�.

	//----------------------------
	//	�萔�錾.
	//----------------------------
	static constexpr float WSPACE = 0.8f;	// �ǂƂ̌��E����.
	static constexpr float DEG45  = D3DXToRadian(  45.f );	// 0.785f.
	static constexpr float DEG135 = D3DXToRadian( 135.f );	// 2.355f.
	static constexpr float DEG225 = D3DXToRadian( 225.f );	// 3.925f.
	static constexpr float DEG315 = D3DXToRadian( 315.f );	// 5.496f.

	float Dis = 0.0f;
	float		TrgRotY = pCrossRay->Ray[CROSSRAY::XL].RotationY;
	D3DXVECTOR3 TrgPos  = pCrossRay->Ray[CROSSRAY::XL].Position;

	// �O���ǂɐڋ�.
	Dis = Distance[CROSSRAY::ZF];
	if (0.01f < Dis && Dis < WSPACE) {
		// ���v���.
		if (TrgRotY < 0.f)	{
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.x += WSPACE - Dis; } // �E����.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; } // ������.
			else										{ TrgPos.z -= WSPACE - Dis; } // ������.
		}
		// �����v���.
		else {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.x -= WSPACE - Dis; } // �E����.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z += WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; } // ������.
			else										{ TrgPos.z -= WSPACE - Dis; } // ������.
		}
	}

	// ��낪�ǂɐڋ�.
	Dis = Distance[CROSSRAY::ZB];
	if (0.01f < Dis && Dis < WSPACE) {
		// ���v���.
		if (TrgRotY < 0.0f) {
			if		(DEG45 <= RotY && RotY < DEG135)	{ TrgPos.x -= WSPACE - Dis; } // �E����.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x += WSPACE - Dis; } // ������.
			else										{ TrgPos.z += WSPACE - Dis; } // ������.
		}
		// �����v���.
		else {
			if		(DEG45 <= RotY && RotY < DEG135)	{ TrgPos.x += WSPACE - Dis; } // �E����.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.z -= WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.x -= WSPACE - Dis; } // ������.
			else										{ TrgPos.z += WSPACE - Dis; } // ������.
		}
	}

	// �E���ǂɐڋ�.
	Dis = Distance[CROSSRAY::XR];
	if (0.01f < Dis && Dis < WSPACE) {
		// ���v���.
		if (TrgRotY < 0.0f) {
			if		(DEG45 <= RotY && RotY < DEG135)	{ TrgPos.z -= WSPACE - Dis; } // �E����.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; } // ������.
			else										{ TrgPos.x -= WSPACE - Dis; } // ������.
		}
		// �����v���.
		else {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.z += WSPACE - Dis; } // �E����.
			else if (DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x += WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; } // ������.
			else										{ TrgPos.x -= WSPACE - Dis; } // ������.
		}
	}

	// �����ǂɐڋ�.
	Dis = Distance[CROSSRAY::XL];
	if (0.01f < Dis && Dis < WSPACE) {
		// ���v���.
		if (TrgRotY < 0.0f) {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.z += WSPACE - Dis; } // �E����.
			else if	(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z -= WSPACE - Dis; } // ������.
			else										{ TrgPos.x += WSPACE - Dis; } // ������.
		}
		// �����v���.
		else {
			if		(DEG45  <= RotY && RotY < DEG135)	{ TrgPos.z -= WSPACE - Dis; } // �E����.
			else if	(DEG135 <= RotY && RotY < DEG225)	{ TrgPos.x -= WSPACE - Dis; } // �O����.
			else if (DEG225 <= RotY && RotY < DEG315)	{ TrgPos.z += WSPACE - Dis; } // ������.
			else										{ TrgPos.x += WSPACE - Dis; } // ������.
		}
	}

	// �ŏI�I�ȍ��W�����C�ɕԂ�.
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		pCrossRay->Ray[dir].Position = TrgPos;
	}
}


//----------------------------------------------------------------------------
//	�����ʒu�̃|���S���̒��_��������.
//		�����_���W�̓��[�J�����W.
//----------------------------------------------------------------------------
HRESULT CStaticMeshObject::FindVerticesOnPoly(
	LPD3DXMESH pMesh,
	DWORD dwPolyIndex,
	D3DXVECTOR3* pVertices)
{
	DWORD dwStride	  = pMesh->GetNumBytesPerVertex();// ���_���Ƃ̃o�C�g�����擾.
	DWORD dwVertexAmt = pMesh->GetNumVertices();	  // ���_�����擾.
	DWORD dwPoly	  = pMesh->GetNumFaces();		  // �ʐ����擾.
	WORD* pwPoly	  = nullptr;

	// �C���f�b�N�X�o�b�t�@�����b�N�i�ǂݍ��݃��[�h�j.
	pMesh->LockIndexBuffer(
		D3DLOCK_READONLY,
		reinterpret_cast<VOID**>(&pwPoly));

	BYTE* pbVertices = nullptr;				// ���_(�o�C�g�^).
	FLOAT* pfVertices = nullptr;			// ���_(float�^).
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	// ���_�o�b�t�@.

	// ���_���̎擾.
	pMesh->GetVertexBuffer(&VB);

	// ���_�o�b�t�@�̃��b�N.
	if (SUCCEEDED( VB->Lock(0, 0, reinterpret_cast<VOID**>(&pbVertices), 0 ) ))
	{
		// �|���S���̒��_�P�ڂ��擾.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3]]);
		pVertices[0].x = pfVertices[0];
		pVertices[0].y = pfVertices[1];
		pVertices[0].z = pfVertices[2];

		// �|���S���̒��_�Q�ڂ��擾.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 1]]);
		pVertices[1].x = pfVertices[0];
		pVertices[1].y = pfVertices[1];
		pVertices[1].z = pfVertices[2];

		// �|���S���̒��_�R�ڂ��擾.
		pfVertices =
			reinterpret_cast<FLOAT*>(&pbVertices[dwStride * pwPoly[dwPolyIndex * 3 + 2]]);
		pVertices[2].x = pfVertices[0];
		pVertices[2].y = pfVertices[1];
		pVertices[2].z = pfVertices[2];

		pMesh->UnlockIndexBuffer();	// ���b�N����.
		VB->Unlock();	// ���b�N����.
	}
	VB->Release();	// �s�v�ɂȂ����̂ŉ��.

	return S_OK;
}


//----------------------------------------------------------------------------
//		��]�l�����i�P���ȏサ�Ă��鎞�̒����j.
//----------------------------------------------------------------------------
void CStaticMeshObject::ClampDirection(float* dir)
{
	// �P���ȏサ�Ă���ꍇ�A2��(360��)������.
	if (*dir > D3DX_PI * 2.0f) {
		*dir -= D3DX_PI * 2.0f;
	}

	// �ċA�֐�.
	if (*dir > D3DX_PI * 2.0f) {
		ClampDirection( dir );
	}
}
