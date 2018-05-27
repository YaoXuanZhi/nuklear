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
    CImGuiBase():m_pCtx(NULL), m_bIsInited(false){}
    virtual ~CImGuiBase(){}

public:
    virtual void InitUILayout() = 0;

    virtual void CreateImGuiRes() = 0;
    virtual void ReleaseImGuiRes() = 0;
    virtual bool HandleWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult) = 0;

public:
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

protected:
    struct nk_context *m_pCtx;
    bool m_bIsInited;
};

class CImGuiTest:public CImGuiBase
{
protected:
    GdiFont* m_font;
    HDC m_dc;
    HWND m_hImGuiWnd;

public:
    CImGuiTest():m_font(NULL){}
    virtual ~CImGuiTest(){}

    void SetHostWnd(HWND hWnd)
    {
        m_hImGuiWnd = hWnd;
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

int APIENTRY WinMain(HINSTANCE hInstance,
                           HINSTANCE hPrevInstance,
                           LPTSTR    lpCmdLine,
                           int       nCmdShow)
{
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;

    CAppWnd myWnd;
    myWnd.CreateEx(exstyle, _T("NuklearWindowClass"),_T("Nuklear Demo"), style, 
        //0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
        CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top,
        GetActiveWindow(), NULL, hInstance);
    ShowWindow(myWnd, SW_SHOW);
    myWnd.MessageLoop();
    return 0;
}
