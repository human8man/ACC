#include "CMain.h"
#include "DirectX/CDirectX9.h"
#include "DirectX/CDirectX11.h"
#include "Scenes/SceneManager/CSceneManager.h"
#include "DirectSound/CSoundManager.h"

// ImGui�̓f�o�b�O���̂ݎg�p����.
#ifdef _DEBUG
#include "Common/ImGui/CImGui.h"
#endif

//�E�B���h�E����ʒ����ŋN����L���ɂ���.
//#define ENABLE_WINDOWS_CENTERING


const TCHAR WND_TITLE[] = _T( "���߂Ă�3D�V���[�e�B���O" );
const TCHAR APP_NAME[]	= _T( "3DSTG" );


//=================================================
//		���C�����[�v�N���X.
//=================================================
CMain::CMain()
	: m_hWnd	( nullptr )
{
}

CMain::~CMain()
{
	DeleteObject( m_hWnd );
}


//=================================================
//		�X�V����.
//=================================================
void CMain::Update()
{
	// �X�V����.
	CSceneManager::GetInstance()->Update();

	// �o�b�N�o�b�t�@���N���A�ɂ���.
	CDirectX11::GetInstance()->ClearBackBuffer();
	
	// �`�揈��.
	CSceneManager::GetInstance()->Draw();

	// ��ʂɕ\��.
	CDirectX11::GetInstance()->Present();
}


//=================================================
//		�\�z����.
//=================================================
HRESULT CMain::Create()
{
	// DirectX9�\�z.
	if( FAILED( CDirectX9::GetInstance()	->Create( m_hWnd ) ) )	{ return E_FAIL; }
	// DirectX11�\�z.
	if( FAILED( CDirectX11::GetInstance()	->Create( m_hWnd ) ) )	{ return E_FAIL; }
	// �V�[���}�l�[�W���[�\�z.
	if( FAILED( CSceneManager::GetInstance()->Create( m_hWnd ) ) )	{ return E_FAIL; }
	
#ifdef _DEBUG
	// ImGui�̍\�z.
	CImGui::GetInstance()->Create(m_hWnd);
#endif

	return S_OK;
}

//=================================================
//		�������.
//=================================================
void CMain::Release()
{
	CSoundManager::GetInstance()->Release();
	CSceneManager::GetInstance()->Release();
}


//=================================================
//		���b�Z�[�W���[�v.
//=================================================
void CMain::Loop()
{
	//------------------------------------------------
	//		�t���[�����[�g��������.
	//------------------------------------------------
	float Rate = 0.0f;	// ���[�g.
	DWORD sync_old = timeGetTime();			// �ߋ�����.
	DWORD sync_now;							// ���ݎ���.

	// ���ԏ����̂��߁A�ŏ��P�ʂ�1�~���b�ɕύX.
	timeBeginPeriod( 1 );
	Rate = 1000.0f / static_cast<float>(FPS); // ���z���Ԃ��Z�o.

	// ���b�Z�[�W���[�v.
	MSG msg = { 0 };
	ZeroMemory( &msg, sizeof( msg ) );

	while( msg.message != WM_QUIT )
	{
		sync_now = timeGetTime();	// ���݂̎��Ԃ��擾.

		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if( sync_now - sync_old >= Rate )
		{
			sync_old = sync_now;	// ���ݎ��Ԃɒu������.

			// �X�V����.
			Update();
		}
	}
	// �A�v���P�[�V�����̏I��.
	Release();
}


//=================================================
//		�E�B���h�E�������֐�.
//=================================================
HRESULT CMain::InitWindow(
	HINSTANCE hInstance,	// �C���X�^���X.
	INT x, INT y,			// �E�B���h�Ex,y���W.
	INT width, INT height)	// �E�B���h�E��,����.
{
	// �E�B���h�E�̒�`.
	WNDCLASSEX wc;
	ZeroMemory( &wc, sizeof( wc ) ); // ������(0��ݒ�).

	wc.cbSize			= sizeof( wc );
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= MsgProc;//WndProc;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon( nullptr, IDI_APPLICATION );
	wc.hCursor			= LoadCursor( nullptr, IDC_ARROW );
	wc.hbrBackground	= (HBRUSH)GetStockObject( LTGRAY_BRUSH );
	wc.lpszClassName	= APP_NAME;
	wc.hIconSm			= LoadIcon( nullptr, IDI_APPLICATION );

	// �E�B���h�E�N���X��Windows�ɓo�^.
	if( !RegisterClassEx( &wc ) ) {
		_ASSERT_EXPR( false, _T( "�E�B���h�E�N���X�̓o�^�Ɏ��s" ) );
		return E_FAIL;
	}

	//--------------------------------------.
	//		�E�B���h�E�\���ʒu�̒���.
	//--------------------------------------.
	// ���̊֐����ł̂ݎg�p����\���̂������Œ�`.
	struct RECT_WND
	{
		INT x, y, w, h;
		RECT_WND() : x(), y(), w(), h() {}
	} rectWindow; // �����ɕϐ��錾������.

#ifdef ENABLE_WINDOWS_CENTERING
	//�f�B�X�v���C�̕��A�������擾.
	HWND hDeskWnd = nullptr;
	RECT recDisplay;
	hDeskWnd = GetDesktopWindow();
	GetWindowRect( hDeskWnd, &recDisplay );

	//�Z���^�����O.
	rectWindow.x = ( recDisplay.right - width ) / 2;	//�\���ʒux���W.
	rectWindow.y = ( recDisplay.bottom - height ) / 2;	//�\���ʒuy���W.
#endif//ENABLE_WINDOWS_CENTERING

	//--------------------------------------.
	//		�E�B���h�E�̈�̒���.
	//--------------------------------------.
	RECT	rect;			// ��`�\����.
	DWORD	dwStyle;		// �E�B���h�E�X�^�C��.
	rect.top = 0;			// ��.
	rect.left = 0;			// ��.
	rect.right = width;		// �E.
	rect.bottom = height;	// ��.
	dwStyle = WS_OVERLAPPEDWINDOW;	// �E�B���h�E���.

	if( AdjustWindowRect(
		&rect,			// (in)��ʃT�C�Y����������`�\����.(out)�v�Z����.
		dwStyle,		// �E�B���h�E�X�^�C��.
		FALSE ) == 0 )	// ���j���[�������ǂ����̎w��.
	{
		MessageBox(
			nullptr,
			_T( "�E�B���h�E�̈�̒����Ɏ��s" ),
			_T( "�G���[���b�Z�[�W" ),
			MB_OK );
		return 0;
	}

	// �E�B���h�E�̕���������.
	rectWindow.w = rect.right - rect.left;
	rectWindow.h = rect.bottom - rect.top;

	// ����e�����[�N����x,y���W.
	rectWindow.x = 100; //+ 1920;
	rectWindow.y = 100;

	//�E�B���h�E�̍쐬.
	m_hWnd = CreateWindow(
		APP_NAME,					// �A�v����.
		WND_TITLE,					// �E�B���h�E�^�C�g��.
		dwStyle,					// �E�B���h�E���(����).
		rectWindow.x, rectWindow.y,	// �\���ʒux,y���W.
		rectWindow.w, rectWindow.h,	// �E�B���h�E��,����.
		nullptr,					// �e�E�B���h�E�n���h��.
		nullptr,					// ���j���[�ݒ�.
		hInstance,					// �C���X�^���X�ԍ�.
		nullptr );					// �E�B���h�E�쐬���ɔ�������C�x���g�ɓn���f�[�^.
	if( !m_hWnd ) {
		_ASSERT_EXPR( false, _T( "�E�B���h�E�쐬���s" ) );
		return E_FAIL;
	}

	//�E�B���h�E�̕\��.
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	return S_OK;
}


//-----------------------------------------------
//		�E�B���h�E�֐��i���b�Z�[�W���̏����j.
//-----------------------------------------------
LRESULT CALLBACK CMain::MsgProc(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam )
{
#ifdef _DEBUG
	if (CImGui::SetWin(hWnd, uMsg, wParam, lParam)) {
		return true;
	}
#endif

	switch( uMsg ) {
	case WM_DESTROY:// �E�B���h�E���j�����ꂽ�Ƃ�.
		// �A�v���P�[�V�����̏I����Windows�ɒʒm����.
		PostQuitMessage( 0 );
		break;

	case WM_KEYDOWN:// �L�[�{�[�h�������ꂽ�Ƃ�.
		// �L�[�ʂ̏���.
		switch( static_cast<char>( wParam ) ) {
		case VK_ESCAPE:	// ESC��.
			if( MessageBox( nullptr,
				_T( "�Q�[�����I�����܂����H" ),
				_T( "�x��" ), MB_YESNO ) == IDYES )
			{
				// �E�B���h�E��j������.
				DestroyWindow( hWnd );
			}
			break;
		}
		break;
	case WM_DPICHANGED:// �E�B���h�E��DPI(Dot Per Inch)���ύX���ꂽ�Ƃ�.
#ifdef _DEBUG
		// ImGui�̐ݒ�t���O��DPI�X�P�[�����O���L�����m�F.
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			// �V����DPI�̒l���擾
			// const int dpi = HIWORD(wParam);

			// DPI�ύX�����O�ɏo��
			// printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);

			// lParam����V�����E�B���h�E�̈ʒu�ƃT�C�Y������RECT�\���̂��擾
			const RECT* suggested_rect = (RECT*)lParam;

			// SetWindowPos�֐����g���ăE�B���h�E�̈ʒu�ƃT�C�Y���X�V
			::SetWindowPos(
				hWnd,											//�E�B���h�E�n���h��.
				nullptr,										//�E�B���h�E�̃��C���[�͕ύX���Ȃ�����null.
				suggested_rect->left,							//�V�K�̍����W��ݒ�.
				suggested_rect->top,							//�V�K�̏���W��ݒ�.
				suggested_rect->right - suggested_rect->left,	//�V�K�̕���ݒ�.
				suggested_rect->bottom - suggested_rect->top,	//�V�K�̍�����ݒ�.
				SWP_NOZORDER | SWP_NOACTIVATE);					//���C���[��ύX�����A�E�B���h�E���A�N�e�B�u�ɂ��Ȃ�.
		}
#endif
		break;
	}

	//���C���ɕԂ����.
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}