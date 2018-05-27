/* nuklear - 1.32.0 - public domain */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <list>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#include "nuklear_gdi.h"
#include "../../contrib/CharsetConvert.h"
#include "../../contrib/CSpliteWndBase.h"
/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the defines */
//#define INCLUDE_ALL 
/*#define INCLUDE_STYLE */
/*#define INCLUDE_CALCULATOR */
/*#define INCLUDE_OVERVIEW */
#define INCLUDE_NODE_EDITOR 

#ifdef INCLUDE_ALL
  #define INCLUDE_STYLE
  #define INCLUDE_CALCULATOR
  #define INCLUDE_OVERVIEW
  #define INCLUDE_NODE_EDITOR
#endif

#ifdef INCLUDE_STYLE
  #include "../style.c"
#endif
#ifdef INCLUDE_CALCULATOR
  #include "../calculator.c"
#endif
#ifdef INCLUDE_OVERVIEW
  #include "../overview.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
  #include "../node_editor.c"
#endif

class CImGuiBase:public CImGuiProvider
{
public:
    CImGuiBase():m_pCtx(NULL){}
    virtual ~CImGuiBase(){}

public:
    virtual void InitUILayout() = 0;
    virtual void EnterInputStatus() = 0;
    virtual void LeaveInputStatus() = 0;
    virtual void CreateImGuiRes() = 0;
    virtual void ReleaseImGuiRes() = 0;
    virtual bool HandleWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult) = 0;

protected:
    struct nk_context *m_pCtx;
};

class CImGuiTest:public CImGuiBase
{
protected:
    GdiFont* m_font;
    HDC m_dc;
    HWND m_hImGuiWnd;
    bool m_bIsInited;

public:
    CImGuiTest():m_font(NULL), m_bIsInited(false){}
    virtual ~CImGuiTest(){}

    void SetHostWnd(HWND hWnd)
    {
        m_hImGuiWnd = hWnd;
    }

    void EnterInputStatus()
    {
        if (m_bIsInited)
            nk_input_begin(m_pCtx);
    }

    void LeaveInputStatus()
    {
        if (m_bIsInited)
            nk_input_end(m_pCtx);
    }

    virtual void CreateImGuiRes()
    {
        m_bIsInited = true;
        m_dc = GetDC(m_hImGuiWnd);

        /* GUI */
        //font = nk_gdifont_create("Arial", 14);
        m_font = nk_gdifont_create("Î¢ÈíÑÅºÚ", 20);
        //font = nk_gdifont_create("ËÎÌå", 20);
        m_pCtx = nk_gdi_init(m_font, m_dc, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    virtual void InitUILayout()
    {
        /* GUI */
        if (nk_begin(m_pCtx, "Demo", nk_rect(50, 50, 200, 200),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(m_pCtx, 30, 80, 1);
            if (nk_button_label(m_pCtx, CharsetConvert::MBCSToUTF8("°´Å¥").c_str()))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(m_pCtx, 30, 2);
            if (nk_option_label(m_pCtx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(m_pCtx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(m_pCtx, 22, 1);
            //nk_property_int(ctx, "Ñ¹Ëõ:", 0, &property, 100, 10, 1);
            nk_property_int(m_pCtx, CharsetConvert::MBCSToUTF8("Ñ¹Ëõ:").c_str(), 0, &property, 100, 10, 1);
        }
        nk_end(m_pCtx);

        /* -------------- EXAMPLES ---------------- */
#ifdef INCLUDE_CALCULATOR
        calculator(m_pCtx);
#endif
#ifdef INCLUDE_OVERVIEW
        overview(m_pCtx);
#endif
#ifdef INCLUDE_NODE_EDITOR
        node_editor(m_pCtx);
#endif
        /* ----------------------------------------- */

        /* Draw */
        nk_gdi_render(nk_rgb(30,30,30));
    }

    virtual void ReleaseImGuiRes()
    {
        nk_gdifont_del(m_font);
        ReleaseDC(m_hImGuiWnd, m_dc);
        nk_gdi_shutdown();
    }

    virtual bool HandleWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
    {
        if(nk_gdi_handle_event(m_hImGuiWnd, uMsg, wParam, lParam))
            return true;
        return false;
    }
};

class CAppWnd: public CSimWndFramework{
private:
    int isloop;
    CImGuiTest m_imGui;
public:
    CAppWnd():isloop(1){}
    virtual ~CAppWnd()
    {
        m_imGui.ReleaseImGuiRes();
    }
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        m_imGui.InitUILayout();
        return FALSE;
    }

	virtual void AfterProcessMessage(UINT uMsg, WPARAM, LPARAM)
    {
        m_imGui.LeaveInputStatus();
    }

	virtual bool BeforeProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
    {
        m_imGui.EnterInputStatus();
        if (m_imGui.HandleWindowMessage(uMsg, wParam, lParam, lResult))
            return true;
        return false;
    }

    LRESULT OnCreate(WPARAM, LPARAM)
    {
        m_imGui.SetHostWnd(*this);
        m_imGui.CreateImGuiRes();
        return FALSE;
    }

    LRESULT OnClose(WPARAM, LPARAM)
    {
        ::PostQuitMessage(0);
        return FALSE;
    }

public:
    BEGIN_MSG_MAP_SIMWNDPROC(CAppWnd)
        MSG_MAP_SIMWNDPROC(WM_CREATE, OnCreate)
        MSG_MAP_SIMWNDPROC(WM_CLOSE, OnClose)
    END_MSG_MAP_SIMWNDPROC(CSimWndFramework)
};

class CImGuiMgr{
protected:
    std::list<CImGuiBase *> m_listImGuiObjs;
public:
    void RegisterImGui(CImGuiBase *pObj)
    {
        if (!pObj) return;
        m_listImGuiObjs.push_back(pObj);
    }

    void DispatchLayout()
    {
        for(std::list<CImGuiBase *>::iterator it = m_listImGuiObjs.begin(); 
            it != m_listImGuiObjs.end(); it++)
        {
            CImGuiBase * pObj = *it;
            if (pObj)
                pObj->InitUILayout();
        }
    }
}; 

class CImGuiWnd: public CSimWndFramework{
private:
    CImGuiTest m_imGui;
public:
    CImGuiWnd(){}
    virtual ~CImGuiWnd()
    {
        m_imGui.ReleaseImGuiRes();
    }

    CImGuiBase *GetImGui()
    {
        return &m_imGui;
    }

protected:
    virtual void AfterProcessMessage(UINT uMsg, WPARAM, LPARAM)
    {
        m_imGui.LeaveInputStatus();
    }

    virtual bool BeforeProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
    {
        m_imGui.EnterInputStatus();
        if (m_imGui.HandleWindowMessage(uMsg, wParam, lParam, lResult))
            return true;
        return false;
    }

public:
    LRESULT OnCreate(WPARAM, LPARAM)
    {
        m_imGui.SetHostWnd(*this);
        m_imGui.CreateImGuiRes();
        return FALSE;
    }

public:
    BEGIN_MSG_MAP_SIMWNDPROC(CImGuiWnd)
        MSG_MAP_SIMWNDPROC(WM_CREATE, OnCreate)
    END_MSG_MAP_SIMWNDPROC(CSimWndFramework)
};

class CMainWnd:public CSpliteWndBase
{
public:
    virtual ~CMainWnd(){}
protected:
    HWND GetWnd1st(){ return m_hView1st;}
    HWND GetWnd2nd(){ return m_hView2nd;}

    void CreateSubWindow1st(int x, int y, int nWidth, int nHeight)
    {
        DWORD style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE /*| LVS_REPORT*/;
        m_hView1st.CreateEx(WS_EX_CLIENTEDGE,_T("Window1"), _T("splite1st"), style, x, y, nWidth, nHeight,
            *this, (HMENU)(UINT_PTR)(1001 + 1), this->GetInstance());
        ::ShowWindow(m_hView1st, SW_SHOW);
    }

    void CreateSubWindow2nd(int x, int y, int nWidth, int nHeight)
    {
        DWORD style = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE /*| LVS_REPORT*/;
        m_hView2nd.CreateEx(WS_EX_CLIENTEDGE,_T("Window2"), _T("splite2nd"), style, x, y, nWidth, nHeight,
            *this, (HMENU)(UINT_PTR)(1001 + 2), this->GetInstance());
        ::ShowWindow(m_hView2nd, SW_SHOW);
    }

protected:
    HRESULT OnCreate(WPARAM wParam, LPARAM lParam)
    {
        CSpliteWndBase::OnCreate(wParam, lParam);
        m_hImGuiMgr.RegisterImGui(m_hView1st.GetImGui());
        m_hImGuiMgr.RegisterImGui(m_hView2nd.GetImGui());
        return TRUE;
    }

    HRESULT OnDestory(WPARAM, LPARAM)
    {
        PostQuitMessage(0);
        return TRUE;
    }

    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        m_hImGuiMgr.DispatchLayout();
        return FALSE;
    }

protected:
    BEGIN_MSG_MAP_SIMWNDPROC(CMainWnd)
        MSG_MAP_SIMWNDPROC(WM_DESTROY, OnDestory)
        MSG_MAP_SIMWNDPROC(WM_CREATE, OnCreate)
    END_MSG_MAP_SIMWNDPROC(CSpliteWndBase)

private:
    CImGuiWnd m_hView1st;
    CImGuiWnd m_hView2nd;
    CImGuiMgr m_hImGuiMgr;
};

int APIENTRY WinMain(HINSTANCE hInstance,
                           HINSTANCE hPrevInstance,
                           LPTSTR    lpCmdLine,
                           int       nCmdShow)
{
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;

    CMainWnd myWnd;
    //CAppWnd myWnd;
    myWnd.CreateEx(exstyle, _T("NuklearWindowClass"),_T("Nuklear Demo"), style, 
        //0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        GetActiveWindow(), NULL, hInstance);
    ShowWindow(myWnd, SW_SHOW);
    myWnd.MessageLoop();
    return 0;
}
