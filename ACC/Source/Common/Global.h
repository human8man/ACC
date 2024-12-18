#pragma once

// �x���ɂ��Ă̺��ޕ��͂𖳌��ɂ���.4005:�Ē�`.
#pragma warning(disable:4005)
#include <Windows.h>
#include <crtdbg.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <memory>

#pragma warning(push)
#pragma warning(disable: 26495 ) // �x���𖳌��ɂ���.

#include <D3DX9.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3D11.h>
#include <D3D10.h>

#include "MyMacro.h"

// ���C�u�����ǂݍ���.
#pragma comment( lib, "winmm.lib"  )
#pragma comment( lib, "d3dx9.lib"  )
#pragma comment( lib, "d3d9.lib"   )
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib"  )

#pragma warning(pop)

#define DEBUGMODE


//=================================================
//		�萔.
//=================================================
constexpr int WND_W = 1280;	 // �E�B���h�E�̕�.
constexpr int WND_H = 720;	 // �E�B���h�E�̍���.
constexpr int FPS   = 60;	 // �t���[�����[�g.
constexpr int STIC_LOW = 60; // �X�e�B�b�N���͂̃f�b�h�]�[��.

// 0�ɂ���萔(�������ȂǂɎg�p).
const D3DXVECTOR3 ZEROVEC3( 0.f,0.f,0.f );
const D3DXVECTOR2 ZEROVEC2( 0.f,0.f );

//=================================================
//		�\����.
//================================================= 

// �J�������.
struct CAMERA {
	D3DXVECTOR3	Pos;	// ���_.
	D3DXVECTOR3 Look;	// �����_.
	D3DXVECTOR3 UpVec;	// ���x�N�g��.
};

// ���C�g���.
struct LIGHT {
	D3DXVECTOR3	Position;	//�ʒu.
	D3DXVECTOR3	vDirection;	//����.
	D3DXMATRIX	mRotation;	//��]�s��.
	float		Intensity;	//���x(���邳).
};