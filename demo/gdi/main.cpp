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
    CImGuiBase():m_pCtx(NULL){}
    virtual ~CImGuiBase(){}

public:
    virtual void InitUILayout() = 0;

    virtual void CreateImGuiRes() = 0;
    virtual void ReleaseImGuiRes() = 0;

public:
    void EnterInputStatus()
    {
        nk_input_begin(m_pCtx);
    }

    void LeaveInputStatus()
    {
        nk_input_end(m_pCtx);
    }

protected:
    struct nk_context *m_pCtx;
};

class CImGuiMgr{
public:
    CImGuiMgr():m_hHostWnd(NULL){}
private:
    std::list<CImGuiBase*> m_listImGui;
    HWND m_hHostWnd;

public:
    bool HandleWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
    {
        bool bRet = false;
        if (m_listImGui.empty()) return false;
        for(std::list<CImGuiBase*>::iterator it = m_listImGui.begin(); 
            it != m_listImGui.end(); it++)
        {
            if((*it)->nk_gdi_handle_event(m_hHostWnd, uMsg, wParam, lParam))
                bRet = true;
        }
        return bRet;
    }

    void EnterInputStatus()
    {
        for(std::list<CImGuiBase*>::iterator it = m_listImGui.begin(); 
            it != m_listImGui.end(); it++)
            (*it)->EnterInputStatus();
    }

    void LeaveInputStatus()
    {
        for(std::list<CImGuiBase*>::iterator it = m_listImGui.begin(); 
            it != m_listImGui.end(); it++)
            (*it)->LeaveInputStatus();
    }

    void CreateImGuiRes()
    {
        for(std::list<CImGuiBase*>::iterator it = m_listImGui.begin(); 
            it != m_listImGui.end(); it++)
            (*it)->CreateImGuiRes();
    }

    void InitUILayout()
    {
        for(std::list<CImGuiBase*>::iterator it = m_listImGui.begin(); 
            it != m_listImGui.end(); it++)
            (*it)->InitUILayout();
    }

public:
    //NOTE:建议在WM_CREATE中执行
    void AttachHostWnd(HWND hWnd)
    {
        m_hHostWnd = hWnd;
    }

    //NOTE:建议在WM_CREATE中执行
    void AddImgGuiObject(CImGuiBase * pWnd)
    {
        if (!pWnd) return;
        m_listImGui.push_back(pWnd);
    }

    //NOTE:建议在退出消息循环后执行
    void ReleaseImGuiObjs()
    {
        for(std::list<CImGuiBase*>::iterator it = m_listImGui.begin(); 
            it != m_listImGui.end(); it++)
        {
            CImGuiBase* pWnd = *it;
            pWnd->ReleaseImGuiRes();
            delete pWnd;
            pWnd = NULL;
        }
    }
};

class CImGuiTest:public CImGuiBase
{
protected:
    GdiFont* m_font;
    HDC m_dc;
    HWND m_hImGuiWnd;

public:
    CImGuiTest(HWND hWnd):m_font(NULL) ,m_hImGuiWnd(hWnd){}
    virtual ~CImGuiTest(){}

    virtual void CreateImGuiRes()
    {
        m_dc = GetDC(m_hImGuiWnd);

        /* GUI */
        //font = nk_gdifont_create("Arial", 14);
        m_font = nk_gdifont_create("微软雅黑", 20);
        //font = nk_gdifont_create("宋体", 20);
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
            if (nk_button_label(m_pCtx, CharsetConvert::MBCSToUTF8("按钮").c_str()))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(m_pCtx, 30, 2);
            if (nk_option_label(m_pCtx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(m_pCtx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(m_pCtx, 22, 1);
            //nk_property_int(ctx, "压缩:", 0, &property, 100, 10, 1);
            nk_property_int(m_pCtx, CharsetConvert::MBCSToUTF8("压缩:").c_str(), 0, &property, 100, 10, 1);
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

};

class CAppWnd:public CSimWndFramework{
private:
    int isloop;
    CImGuiMgr m_imGuiMgr;
public:
    CAppWnd():isloop(1)
    {
    }
protected:
    bool BeforeProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
    {
        if (m_imGuiMgr.HandleWindowMessage(uMsg, wParam, lParam, lResult))
            return true;
        return false;
    }

    LRESULT OnCreate(WPARAM, LPARAM)
    {
        m_imGuiMgr.AttachHostWnd(*this);
        m_imGuiMgr.AddImgGuiObject(new CImGuiTest(*this));
        return FALSE;
    }

    LRESULT OnClose(WPARAM, LPARAM)
    {
        isloop = 0;
        return FALSE;
    }

public:
    int MessageLoop()
    {
        m_imGuiMgr.CreateImGuiRes();
        MSG msg;
        while (isloop)
        {
            m_imGuiMgr.EnterInputStatus();
            GetMessage(&msg, NULL, 0, 0);
            //拦截感兴趣的窗口消息
            if (PreTranslateMessage(&msg))
                continue;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            m_imGuiMgr.LeaveInputStatus();
            m_imGuiMgr.InitUILayout();
        }
        m_imGuiMgr.ReleaseImGuiObjs();
        return (int)msg.wParam;
    }

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
