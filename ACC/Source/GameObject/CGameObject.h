#pragma once

//===================================================================
//		�Q�[���I�u�W�F�N�g�N���X.
//===================================================================
class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	virtual void Update() = 0;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) = 0;

	// ���W�ݒ�֐�.
	void SetPosition( float x, float y, float z ){
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};

	// ��]�ݒ�֐�.
	void SetRotation( float x, float y, float z ){
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;
	};

	//�g�k�ݒ�֐�.
	void SetScale( float x, float y, float z ){
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
	}

	void SetAlpha	( float alpha )			  { m_Alpha = alpha; }
	void SetPosition( const D3DXVECTOR3& pos ){ m_vPosition = pos;}
	void SetRotation( const D3DXVECTOR3& rot ){ m_vRotation = rot; }
	void SetScale	( float xyz )			  { m_vScale = D3DXVECTOR3( xyz, xyz, xyz ); }

	const D3DXVECTOR3& GetScale()	 const { return m_vScale;	 }
	const D3DXVECTOR3& GetPosition() const { return m_vPosition; }
	const D3DXVECTOR3& GetRotation() const { return m_vRotation; }

protected:
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR3	m_vRotation;
	D3DXVECTOR3	m_vScale;
	float		m_Alpha;
};