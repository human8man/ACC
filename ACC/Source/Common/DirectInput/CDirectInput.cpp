#include "CDirectInput.h"

CDInput::CDInput()
{
    m_pDCInput = NULL;
    m_hWnd = NULL;
    m_UseDevice = 0;
}

CDInput::~CDInput()
{
    Release();
}

bool CDInput::Create(HWND hWnd, int useDevice)
{
    if (m_pDCInput)
    {
        Release();
    }
    m_hWnd = hWnd;

    //DirectInput�I�u�W�F�N�g�̐���.
    HRESULT hr = DirectInput8Create(
        ::GetModuleHandle(NULL),
        DIRECTINPUT_VERSION,
        IID_IDirectInput8,
        (void**)&m_pDCInput,
        NULL);

    if (FAILED(hr))
    {
        return false;
    }

    m_UseDevice = useDevice;

    if (useDevice & UseInputDevice_KEYBOARD)
    {
        m_Key.Create(m_pDCInput, hWnd);
    }
    if (useDevice & UseInputDevice_GAMEPAD)
    {
        m_Mouse.Create(m_pDCInput, hWnd);
    }

    m_GamePad.Create();

    return true;
}

bool CDInput::GamePadConnect()
{
    if (!m_GamePad.IsValid())
    {
        //�Q�[���p�b�h���쐬����Ă��Ȃ��Ƃ�.
        return false;
    }

    HRESULT hr = m_GamePad.GetDeviceState();

    //���s�����ꍇ�A
    if (FAILED(hr) || hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
    {
        return false;
    }

    return true;
}

void CDInput::Release()
{
    //DirectInput�I�u�W�F�N�g�̉��.
    if (m_pDCInput != NULL)
    {
        m_pDCInput->Release();
        m_pDCInput = NULL;
    }

    m_Key.Release();
    m_Mouse.Release();
    m_GamePad.Release();

    m_hWnd = NULL;
    m_UseDevice = 0;
}
