#ifndef __CSPLITEWNDBASE_H
#define __CSPLITEWNDBASE_H
#include "CSimWndFramework.h"
#include <math.h>

class CSpliteWndBase:public CSimWndFramework
{
public:
    CSpliteWndBase()
        :nSplitePos(0)
        ,nSpliterWidth(5)
        ,bClickSpliter(false)
    {}
    virtual HWND GetWnd1st() = 0;
    virtual HWND GetWnd2nd() = 0;

    virtual void CreateSubWindow1st(int x, int y, int nWidth, int nHeight) = 0;
    virtual void CreateSubWindow2nd(int x, int y, int nWidth, int nHeight) = 0;

protected:
    HRESULT OnEraseBKGnd(WPARAM, LPARAM)
    {
        return FALSE;
    }

    HRESULT OnSize(WPARAM, LPARAM)
    {
        //ע�⣬�ڴ�ʱ���ָ�����λ����Ȼ�ǲ���ģ�
        //��Ҫ���ű仯�����Ƿָ�������������ȴ��
        SetSplitePos(*this,this->nSplitePos/*,true*/);
        return TRUE;
    }

    HRESULT OnCreate(WPARAM , LPARAM )
    {
        OutputDebugString(_T("OnCreate\n"));
        RECT rc;
        GetClientRect(*this, &rc);
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;
        SetSplitePos(*this,cx/2, false);

        CreateSubWindow1st(0, 0, rcSpliter.left, cy);
        CreateSubWindow2nd(rcSpliter.right, 0, cx - rcSpliter.right, cy);
        return TRUE;
    }

    HRESULT OnLButtonDown(WPARAM, LPARAM lParam)
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        OutputDebugString(_T("OnLButtonDown\n"));
        if (false == bClickSpliter)
        {
            RECT rc;
            GetClientRect(*this, &rc);
            if (PtInRect(&rcSpliter, pt))
            {
                OutputDebugStringA("���зָ���λ��\n");
                bClickSpliter = true;
                ::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
                ::SetCapture(*this);
            }
            else {
                OutputDebugStringA("û�л��зָ���λ��\n");
            }
        }
        return FALSE;
    }

    HRESULT OnLButtonUp(WPARAM, LPARAM lParam)
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        OutputDebugString(_T("OnLButtonUp\n"));
        if (true == bClickSpliter)
        {
            bClickSpliter = false;
            ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
            ::ReleaseCapture();
        }
        return FALSE;
    }

    HRESULT OnMouseMove(WPARAM, LPARAM lParam)
    {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (true == bClickSpliter)
        {
            SetSplitePos(*this,pt.x);
            OutputDebugString(_T("OnMouseMove\n"));
        }
        return FALSE;
    }

    HRESULT OnPaint(WPARAM , LPARAM )
    {
        OutputDebugString(_T("OnPaint\n"));
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(*this, &ps);
        RECT rc;
        GetClientRect(*this, &rc);
        //FillRect(hDC,&rc,CreateSolidBrush(RGB(0,128,0)));
        rc.left = nSplitePos - nSpliterWidth / 2;
        //rc.left = ceil(nSplitePos - nSpliterWidth / 2.0);
        rc.right = rc.left + nSpliterWidth;
        //FillRect(hDC, &rc, CreateSolidBrush(RGB(255, 255, 0)));
        FillRect(hDC, &rc, CreateSolidBrush(RGB(38, 38, 38)));
        EndPaint(*this, &ps);
        return TRUE;
    }

protected:
    void SetSplitePos(HWND hWnd,int nPosX, bool bIsAutoLayout = true)
    {
        if (bIsAutoLayout)
            AutoLayout(hWnd);
        RECT rc;
        GetClientRect(hWnd, &rc);
        RECT temp = rc;
        nSplitePos = nPosX;
        rc.left = nSplitePos - nSpliterWidth / 2;
        rc.right = rc.left + nSpliterWidth;
        rcSpliter = rc;
    }

    void AutoLayout(HWND hWnd)
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;

        if(!GetWnd1st() || !GetWnd2nd()) return;
        ::MoveWindow(GetWnd1st(), 0, 0, rcSpliter.left, cy, TRUE);
        ::MoveWindow(GetWnd2nd(), rcSpliter.right, 0, cx - rcSpliter.right, cy, TRUE);
    }

protected:
    BEGIN_MSG_MAP_SIMWNDPROC(CSpliteWndBase)
        MSG_MAP_SIMWNDPROC(WM_ERASEBKGND,OnEraseBKGnd)
        MSG_MAP_SIMWNDPROC(WM_CREATE, OnCreate)
        MSG_MAP_SIMWNDPROC(WM_PAINT, OnPaint)
        MSG_MAP_SIMWNDPROC(WM_LBUTTONDOWN, OnLButtonDown)
        MSG_MAP_SIMWNDPROC(WM_LBUTTONUP, OnLButtonUp)
        MSG_MAP_SIMWNDPROC(WM_MOUSEMOVE, OnMouseMove)
        MSG_MAP_SIMWNDPROC(WM_SIZE,OnSize)
    END_MSG_MAP_SIMWNDPROC(CSimWndFramework)

private:
	/*
	�ָ����Ļ���˼·���������ε�����Ϊ�ͻ����ĸ߶ȣ��������ΪnSplitePos-nSpliterWidth/2
	*/
	int nSplitePos;		 //��Կͻ�������������Ϣ
	int nSpliterWidth;	 //�ָ����Ŀ��
	bool bClickSpliter;  //�洢�ָ�����λ��
	RECT rcSpliter;
};

#endif