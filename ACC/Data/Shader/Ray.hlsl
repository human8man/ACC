//=======================================================
//		���C�i�����j�\���N���X�p�V�F�[�_�t�@�C��
//=======================================================
// �O���[�o���ϐ�
// �R���X�^���g�o�b�t�@
// �A�v�����Ɠ����o�b�t�@�T�C�Y�ɂȂ��Ă���K�v������
cbuffer per_mesh : register( b0 )	//���W�X�^�ԍ�
{
	matrix	g_mWVP;		// ���[���h�A�r���[�A�v���W�F�N�V�����̕ϊ������s��
	float4	g_Color;	// �F�iRGBA:xyzw�j
}

// ���_�V�F�[�_
// ��Ƀ��f���̒��_�̕\���ʒu�����肷��
float4 VS_Main( float4 Pos : POSITION ) : SV_Position
{
	Pos = mul( Pos, g_mWVP );
	
	return Pos;
}

// �s�N�Z���V�F�[�_
// ��ʏ�ɕ]�����s�N�Z���i�h�b�g�P���j�̐F�����肷��
float4 PS_Main( float4 Pos : SV_Position ) : SV_Target
{
	return g_Color;
}