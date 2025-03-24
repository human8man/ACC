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
	void SetPos(const D3DXVECTOR3& pos) { m_vPosition = pos; }
	void SetPos( float x, float y, float z ){
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};

	// ��]�ݒ�֐�.
	void SetRot(const D3DXVECTOR3& rot) { m_vRotation = rot; }
	void SetRot( float x, float y, float z ){
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;
	};

	// �g�k�ݒ�֐�.
	void SetScale( float xyz ) { m_vScale = D3DXVECTOR3( xyz, xyz, xyz ); }
	void SetScale( float x, float y, float z ){
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
	}

	// ���ߓx�ݒ�֐�.
	void SetAlpha( float alpha ) { m_Alpha = alpha; }


	const float& GetAlpha() const { return m_Alpha; }				// ���ߓx���擾.
	const D3DXVECTOR3& GetPos()		 const { return m_vPosition; }	// ���W�����擾.
	const D3DXVECTOR3& GetRot()		 const { return m_vRotation; }	// ��]�������擾.
	const D3DXVECTOR3& GetScale()	 const { return m_vScale;	 }	// �T�C�Y���擾.
	// �I�u�W�F�N�g���(���W���,��]���,�T�C�Y)���擾.
	const ObjectInfo& GetObjeInfo()	 const { return ObjectInfo{ m_vPosition, m_vRotation, m_vScale }; }

protected:
	D3DXVECTOR3	m_vPosition;	// ���W���.
	D3DXVECTOR3	m_vRotation;	// ��]���.
	D3DXVECTOR3	m_vScale;		// �T�C�Y.
	float		m_Alpha;		// ���ߓx.
};