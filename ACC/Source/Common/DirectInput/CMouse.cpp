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

//マウスデバイスの作成.
bool CMouse::Create(IDirectInput8* pDInput, HWND hWnd)
{
    HRESULT hr;

    if (!pDInput)
    {
        return false;
    }

    m_hWnd = hWnd;
    //マウスデバイスを作成.
    hr = pDInput->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);
    if (FAILED(hr))
    {
        return false;
    }

    //データフォーマットの設定.
    hr = m_pDIMouse->SetDataFormat(&c_dfDIMouse2);
    if (FAILED(hr))
    {
        return false;
    }

    //バッファサイズの設定.
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

    //協調モードの設定.
    hr = m_pDIMouse->SetCooperativeLevel(m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if (FAILED(hr))
    {
        return false;
    }

    //入力を許可する.
    m_pDIMouse->Acquire();

    return true;
}

//マウスの入力状態を更新.
void CMouse::Update()
{
    //リセットする。そのためクリ九はUpdate()を次回呼ぶ前に処理しなければならない.
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
            case DIMOFS_BUTTON0:    //右クリ
                m_LDown = (od.dwData & 0x80) ? true : false;
                if (m_LDown)
                {
                    m_LAction = true;
                }
                break;
            case DIMOFS_BUTTON1:    //左クリ
                m_RDown = (od.dwData & 0x80) ? true : false;
                if (m_RDown)
                {
                    m_RAction = true;
                }
                break;
            case DIMOFS_BUTTON2:    //ホイール
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

//マウスデバイスの解放.
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