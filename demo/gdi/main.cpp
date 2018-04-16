/* nuklear - 1.32.0 - public domain */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "../../contrib/CSimWndFramework.h"
#include "../../contrib/CharsetConvert.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#include "../../nuklear.h"
#include "nuklear_gdi.h"

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

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    //将win32 的消息路由转发到imgui中
    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;

    return DefWindowProcW(wnd, msg, wparam, lparam);
}

int main(void)
{
    GdiFont* font;
    struct nk_context *ctx;

    WNDCLASSW wc;
    ATOM atom;
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;
    HWND wnd;
    HDC dc;
    int running = 1;
    int needs_refresh = 1;

    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"NuklearWindowClass";
    atom = RegisterClassW(&wc);

    //AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Nuklear Demo",
        style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);
    dc = GetDC(wnd);

    /* GUI */
    font = nk_gdifont_create("Arial", 14);
    ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* style.c */
    #ifdef INCLUDE_STYLE
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    /*set_style(ctx, THEME_DARK);*/
    #endif

    while (running)
    {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        if (needs_refresh == 0) {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                running = 0;
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            needs_refresh = 1;
        } else needs_refresh = 0;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            needs_refresh = 1;
        }
        nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 200),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
        #ifdef INCLUDE_CALCULATOR
          calculator(ctx);
        #endif
        #ifdef INCLUDE_OVERVIEW
          overview(ctx);
        #endif
        #ifdef INCLUDE_NODE_EDITOR
          node_editor(ctx);
        #endif
        /* ----------------------------------------- */

        /* Draw */
        nk_gdi_render(nk_rgb(30,30,30));
    }

    nk_gdifont_del(font);
    ReleaseDC(wnd, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

//class CAppWnd:public CSimModalDialog{
class CAppWnd:public CSimWndFramework{
private:
    GdiFont* font;
    struct nk_context *ctx;
    HDC dc;
    int isloop;
public:
    CAppWnd():font(NULL)
        ,ctx(NULL)
        ,isloop(1)
    {
    }
protected:
    void InitImGui()
    {
        dc = GetDC(*this);

        /* GUI */
        //font = nk_gdifont_create("Arial", 14);
        font = nk_gdifont_create("微软雅黑", 20);
        //font = nk_gdifont_create("宋体", 20);
        ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    void CreateImGui()
    {
        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 200),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, CharsetConvert::MBCSToUTF8("按钮").c_str()))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            //nk_property_int(ctx, "压缩:", 0, &property, 100, 10, 1);
            nk_property_int(ctx, CharsetConvert::MBCSToUTF8("压缩:").c_str(), 0, &property, 100, 10, 1);
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
#ifdef INCLUDE_CALCULATOR
        calculator(ctx);
#endif
#ifdef INCLUDE_OVERVIEW
        overview(ctx);
#endif
#ifdef INCLUDE_NODE_EDITOR
        node_editor(ctx);
#endif
        /* ----------------------------------------- */

        /* Draw */
        nk_gdi_render(nk_rgb(30,30,30));
    }

public:
    void ReleaseImGui()
    {
        nk_gdifont_del(font);
        ReleaseDC(*this, dc);
    }

    virtual bool BeforeProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
    {
        if (nk_gdi_handle_event(*this, uMsg, wParam, lParam))
            return true;
        return false;
    }

    LRESULT OnCreate(WPARAM, LPARAM)
    {
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
        InitImGui();
        MSG msg;
        while (isloop)
        {
            nk_input_begin(ctx); 
            GetMessage(&msg, NULL, 0, 0);
            //拦截感兴趣的窗口消息
            if (PreTranslateMessage(&msg))
                continue;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            nk_input_end(ctx);
            CreateImGui();
        }
        ReleaseImGui();
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
    //main();
    return 0;
}
