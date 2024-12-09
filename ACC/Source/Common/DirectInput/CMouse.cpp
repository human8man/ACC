#include "CDirectInput.h"
#include "Common/Global.h"

CMouse::CMouse()
{
    m_hWnd = NULL;
    m_pDIMouse = NULL;
    m_LDown = false;
    m_RDown = false;
    m_MDown = false;
    m_LAction = false;
    m_RAction = false;
    m_MAction = false;
}

//�}�E�X�f�o�C�X�̍쐬.
bool CMouse::Create(IDirectInput8* pDInput, HWND hWnd)
{
    HRESULT hr;

    if (!pDInput)
    {
        return false;
    }

    m_hWnd = hWnd;
    //�}�E�X�f�o�C�X���쐬.
    hr = pDInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);
    if (FAILED(hr))
    {
        return false;
    }

    //�f�[�^�t�H�[�}�b�g�̐ݒ�.
    hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse2);
    if (FAILED(hr))
    {
        return false;
    }

    //�o�b�t�@�T�C�Y�̐ݒ�.
    DIPROPDWORD diprop;
    diprop.diph.dwSize = sizeof(diprop);
    diprop.diph.dwHeaderSize = sizeof(diprop.diph);
    diprop.diph.dwObj = 0;
    diprop.diph.dwHow = DIPH_DEVICE;
    diprop.dwData = 1000;

    hr = m_pDIMouse->SetProperty(DIPROP_BUFFERSIZE, &diprop.diph);
    if (FAILED(hr))
    {
        return false;
    }

    //�������[�h�̐ݒ�.
    hr = m_pDIMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr))
    {
        return false;
    }

    //���͂�������.
    m_pDIMouse->Acquire();

    return true;
}

//�}�E�X�̓��͏�Ԃ��X�V.
void CMouse::Update()
{
    //���Z�b�g����B���̂��߃N�����Update()������ĂԑO�ɏ������Ȃ���΂Ȃ�Ȃ�.
    m_LAction = false;
    m_RAction = false;
    m_MAction = false;

    DIDEVICEOBJECTDATA od;
    DWORD dwItems = 1;
    HRESULT hr;
    while (true)
    {
        hr = m_pDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwItems, 0);

        if (hr == DIERR_INPUTLOST)
        {
            m_pDIMouse->Acquire();
        }
        else if (dwItems == 0 || FAILED(hr))
        {
            if (hr == DIERR_NOTACQUIRED)
            {
                m_pDIMouse->Acquire();
            }
            break;
        }
        else
        {
            switch (od.dwOfs)
            {
            case DIMOFS_BUTTON0:    //�E�N��
                m_LDown = (od.dwData & 0x80) ? true : false;
                if (m_LDown)
                {
                    m_LAction = true;
                }
                break;
            case DIMOFS_BUTTON1:    //���N��
                m_RDown = (od.dwData & 0x80) ? true : false;
                if (m_RDown)
                {
                    m_RAction = true;
                }
                break;
            case DIMOFS_BUTTON2:    //�z�C�[��
                m_MDown = (od.dwData & 0x80) ? true : false;
                if (m_MDown)
                {
                    m_MAction = true;
                }
                break;
            }
            od.dwData = static_cast<DWORD>(0.0f);
        }
    }
}

//�}�E�X�f�o�C�X�̉��.
void CMouse::Release()
{
    if (m_pDIMouse != NULL)
    {
        m_pDIMouse->Unacquire();
        m_pDIMouse->Release();
        m_pDIMouse = NULL;
    }

    m_hWnd = NULL;
    m_LDown = false;
    m_RDown = false;
    m_LAction = false;
    m_RAction = false;
}