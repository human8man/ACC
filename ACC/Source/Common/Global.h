#pragma once

// �x���ɂ��Ă̺��ޕ��͂𖳌��ɂ���.4005:�Ē�`
#pragma warning(disable:4005)
#include <Windows.h>
#include <crtdbg.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>

#pragma warning(push)
#pragma warning(disable: 26495 ) // �x���𖳌��ɂ���

#include <D3DX9.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <D3D11.h>
#include <D3D10.h>

#include "MyMacro.h"

// ���C�u�����ǂݍ���
#pragma comment( lib, "winmm.lib"  )
#pragma comment( lib, "d3dx9.lib"  )
#pragma comment( lib, "d3d9.lib"   )
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib"  )

#pragma warning(pop)

#define DEBUGMODE


//=============================================================================
//		�萔
//=============================================================================
constexpr int WND_W = 1280;		 // �f�t�H���g�E�B���h�E�̕�(int)
constexpr int WND_H = 720;		 // �f�t�H���g�E�B���h�E�̍���(int)
constexpr float FWND_W = 1280.f; // �f�t�H���g�E�B���h�E�̕�(float)
constexpr float FWND_H = 720.f;	 // �f�t�H���g�E�B���h�E�̍���(float)

// ��ʔ{����}�邽��float�̂݁A���A���I�ɕύX����邽��extern�Ő錾
extern float NOWFWND_W;	// �t���E�B���h�E�̕�(float)
extern float NOWFWND_H;	// �t���E�B���h�E�̍���(float)

// �t���X�N���[��(���t���[��������Ƃ�A�ʂ̃N���X���m�ŎQ�Ƃ����邽��extern)
extern bool FULLSCREEN;
extern bool RAINBOW_WINDOW;
// �t���X�N���[������UI�����p�̕ϐ�
extern float FULLSCREENSCALEX;
extern float FULLSCREENSCALEY;
// �E�B���h�E�ʒu
extern D3DXVECTOR2 WINDOWRECT;
// �E�B���h�E�̕`��J�n�ʒu
extern D3DXVECTOR2 CLIENTRECT;

constexpr int FPS   = 60;	 // �t���[�����[�g
constexpr int STIC_LOW = 60; // �X�e�B�b�N���͂̃f�b�h�]�[��

// 0�ɂ���萔(�������ȂǂɎg�p)
constexpr float ZEROFLOAT3[3] = { 0.f,0.f,0.f };
constexpr float ZEROFLOAT2[2] = { 0.f,0.f };

const D3DXVECTOR3 ZEROVEC3( 0.f,0.f,0.f );
const D3DXVECTOR2 ZEROVEC2( 0.f,0.f );

// �f�o�b�O�؂�ւ�
extern bool ISDEBUG;
// UIeditorScene�ŕҏW���������ۂɎg��
extern bool UIEDIOR_MOVEANY;

// �Q�[���ƃ^�C�g���ɋ��ʓG���̈��p�Ɏg�p
extern int ENEMY_COUNT;
extern int ENEMY_COUNTMAX;

//=============================================================================
//		�\����
//=============================================================================
// �J�������
struct CAMERA {
	D3DXVECTOR3	Pos;	// ���_
	D3DXVECTOR3 Look;	// �����_
	D3DXVECTOR3 UpVec;	// ���x�N�g��
};

// ���C�g���
struct LIGHT {
	D3DXVECTOR3	Position;			// �ʒu
	D3DXVECTOR3	vDirection;			// ����
	D3DXMATRIX	mRotation;			// ��]�s��
	float		Intensity = 0.5;	// ���x(���邳)
};

// �I�u�W�F�N�g�̍��W�A��]�A�T�C�Y���܂Ƃ߂��\����
struct ObjectInfo {
	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
	D3DXVECTOR3 scale;
};