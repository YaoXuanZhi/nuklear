#include "CSimWndFramework.h"
#include <assert.h>

//#define OUTPUTWNDMSG

#ifdef OUTPUTWNDMSG
void OutputStr(const TCHAR* lpszFormat, ...)
{
	if (lpszFormat != NULL)
	{
		va_list args;
		TCHAR szText[2048] = { 0 };
		va_start(args, lpszFormat);
		_vstprintf(szText, lpszFormat, args);
		OutputDebugString(szText);
		va_end(args);
	}
}
#endif

static WNDPROC SetWindowLongEx(
	HWND m_hWnd,
	int nIndex,
#ifndef _WIN64
	LONG dwNewLong
#else
	LONG_PTR dwNewLong
#endif
	)
{
#ifdef _WIN64
	return (WNDPROC)::SetWindowLongPtr(m_hWnd, nIndex, dwNewLong);
#else
	return (WNDPROC)::SetWindowLong(m_hWnd, nIndex, dwNewLong);
#endif
}

#ifdef _WIN64
LONG_PTR
#else
LONG
#endif
GetWindowLongEx(
	HWND hWnd,
	int nIndex
	)
{
#ifdef _WIN64
	return ::GetWindowLongPtr(hWnd, nIndex);
#else
	return ::GetWindowLong(hWnd, nIndex);
#endif
}

#ifndef GWL_USERDATAEX
#ifdef _WIN64
#define GWL_USERDATAEX GWLP_USERDATA
#else
#define GWL_USERDATAEX GWL_USERDATA
#endif
#else
#error "GWL_USERDATAEX已被定义"
#endif

#ifndef GWL_WNDPROCEX
#ifdef _WIN64
#define GWL_WNDPROCEX GWLP_WNDPROC
#else
#define GWL_WNDPROCEX GWL_WNDPROC
#endif
#else
#error "GWL_WNDPROCEX已被定义"
#endif

CSimWndFramework::CSimWndFramework(HWND hWnd) :_origWindowProc(NULL), m_hWnd(NULL), m_hInstance(NULL) { m_hWnd = hWnd; }

CSimWndFramework::~CSimWndFramework()
{

}

bool CSimWndFramework::CreateEx(DWORD dwExStyle, LPCTSTR szClassName,
	LPCTSTR szWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight,
	HWND hParentWnd, HMENU hMenu /*= NULL*/, HINSTANCE hInstance /*= NULL*/)
{
	WNDCLASS wc = { 0 };
	if (NULL == hInstance)
		hInstance = GetModuleHandle(NULL);
	::GetClassInfo(hInstance, szClassName, &wc);
	bool bIsRegistered = true;
	if (NULL == wc.lpszClassName)
	{
		bIsRegistered = false;
		wc.style = CS_DBLCLKS;//让其响应鼠标双击事件
		wc.lpfnWndProc = _WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = ::CreateSolidBrush(RGB(255, 255, 255));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = szClassName;
		RegisterClass(&wc);
	}

	m_hWnd = ::CreateWindowEx(dwExStyle, szClassName, szWindowName,
		dwStyle, x, y, nWidth, nHeight, hParentWnd,
		hMenu, hInstance, this);
	m_hInstance = hInstance;
	if (bIsRegistered)
	{
		SetSubclassProc();
		::SendMessage(m_hWnd, WM_CREATE, NULL, NULL);
	}
	return (NULL != m_hWnd);
}

bool CSimWndFramework::Create(LPCTSTR szClassName, LPCTSTR szWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight,
	HWND hParentWnd, HMENU hMenu/*= NULL*/, HINSTANCE hInstance/*= NULL*/)
{
	return this->CreateEx(0L, szClassName, szWindowName, dwStyle,
		x, y, nWidth, nHeight, hParentWnd, hMenu, hInstance);
}

bool CSimWndFramework::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	assert(::IsWindow(m_hWnd));
	DWORD dwStyle = (DWORD)GetWindowLongEx(m_hWnd, GWL_STYLE);
	DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
		return false;

	SetWindowLongEx(m_hWnd, GWL_STYLE, dwNewStyle);
	if (nFlags != 0)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}

	return true;
}

int CSimWndFramework::MessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//拦截感兴趣的窗口消息
		if (PreTranslateMessage(&msg))
			continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

bool CSimWndFramework::AttachAndSubclass(HWND hDstWnd, HINSTANCE hInstance)
{
	if (NULL != hDstWnd)
	{
		if (NULL == hInstance)
			hInstance = GetModuleHandle(NULL);
		m_hInstance = hInstance;
		Attach(hDstWnd);
		SetSubclassProc();
		return true;
	}
	else {
		return false;
	}
}

WNDPROC CSimWndFramework::GetOldWndProc()
{
	return _origWindowProc;
}

LRESULT CALLBACK CSimWndFramework::_WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		SetWindowLongEx(hWnd, GWL_USERDATAEX, reinterpret_cast<LPARAM>(((LPCREATESTRUCT)lParam)->lpCreateParams));
	}
	CSimWndFramework *pThis = NULL;
	pThis = reinterpret_cast<CSimWndFramework*>(GetWindowLongEx(hWnd, GWL_USERDATAEX));

	if (pThis != NULL && message == WM_NCCREATE)
		pThis->Attach(hWnd);
	if (NULL == pThis)
		return DefWindowProc(hWnd, message, wParam, lParam);
	LRESULT lResult = FALSE;
	if (pThis->HandleMessage(message, wParam, lParam, lResult))
		return lResult;
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK CSimWndFramework::_SubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CSimWndFramework *pThis = NULL;
	pThis = reinterpret_cast<CSimWndFramework*>(GetWindowLongEx(hWnd, GWL_USERDATAEX));

	if (pThis != NULL)
	{
		LRESULT lResult = FALSE;
		if (pThis->HandleMessage(message, wParam, lParam, lResult))
			return lResult;
	}

	if (NULL != pThis->_origWindowProc)
		return CallWindowProc(pThis->_origWindowProc, *pThis, message, wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CSimWndFramework::Attach(HWND newWindow)
{
	m_hWnd = newWindow;
}

void CSimWndFramework::SetSubclassProc()
{
	SetWindowLongEx(m_hWnd, GWL_USERDATAEX, reinterpret_cast<LPARAM>(this));
	_origWindowProc = (WNDPROC)SetWindowLongEx(m_hWnd, GWL_WNDPROCEX, reinterpret_cast<LPARAM>(_SubclassProc));
}

CSimWndFramework::operator HWND() const
{
	assert(::IsWindow(m_hWnd));
	return m_hWnd;
}

HINSTANCE CSimWndFramework::GetInstance()
{
	if (NULL == m_hInstance)
		m_hInstance = ::GetModuleHandle(NULL);
	return m_hInstance;
}

bool CSimWndFramework::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult)
{
#ifdef OUTPUTWNDMSG
	OutputStr(_T("HandleMessage：%u\n"), uMsg);
#endif
	if (BeforeProcessMessage(uMsg, wParam, lParam, lResult))
		return true;
	if (ProcessMessage(uMsg, wParam, lParam, lResult))
		return true;
	AfterProcessMessage(uMsg, wParam, lParam);
	return false;
}

BOOL CSimWndFramework::PreTranslateMessage(MSG* pMsg)
{
	if (NULL != pMsg)
	{
#ifdef OUTPUTWNDMSG
		OutputStr(_T("PreTranslateMessage：%u\n"), pMsg->message);
#endif
	}
	return FALSE;
}

void CSimWndFramework::AfterProcessMessage(UINT uMsg, WPARAM, LPARAM)
{
#ifdef OUTPUTWNDMSG
	OutputStr(_T("AfterHandleMessage：%u\n"), uMsg);
#endif
}

bool CSimWndFramework::BeforeProcessMessage(UINT uMsg, WPARAM, LPARAM, LRESULT &lResult)
{
#ifdef OUTPUTWNDMSG
	OutputStr(_T("BeforeHandleMessage：%u\n"), uMsg);
#endif
	return false;
}

bool CSimWndFramework::ProcessMessage(UINT uMsg, WPARAM, LPARAM, LRESULT &lResult)
{
#ifdef OUTPUTWNDMSG
	OutputStr(_T("ProcessingMessage：%u\n"), uMsg);
#endif
	return false;
}

LRESULT CSimNormalDialog::OnDestory(WPARAM, LPARAM)
{
	::PostQuitMessage(0);
	return FALSE;
}

LRESULT CSimNormalDialog::OnNcActive(WPARAM wParam, LPARAM)
{
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CSimNormalDialog::OnNcCalcsize(WPARAM, LPARAM)
{
	return 0;
}

LRESULT CSimNormalDialog::OnNcPaint(WPARAM, LPARAM)
{
	return 0;
}

LRESULT CSimNormalDialog::OnNcHitTest(WPARAM /*wParam*/, LPARAM lParam)
{
	const LONG offset = 10;
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	//设置窗口拉伸的区域位置信息，仅当窗口非最大化的时候支持
	if (!::IsZoomed(*this) && pt.x<rcClient.left + offset && pt.y < rcClient.top + offset)
	{
		//左上角
		return HTTOPLEFT;
	}
	else if (!::IsZoomed(*this) && pt.x > rcClient.right - offset && pt.y < rcClient.top + offset)
	{
		//右上角
		return HTTOPRIGHT;
	}
	else if (!::IsZoomed(*this) && pt.x<rcClient.left + offset && pt.y>rcClient.bottom - offset)
	{
		//左下角
		return HTBOTTOMLEFT;
	}
	else if (!::IsZoomed(*this) && pt.x > rcClient.right - offset && pt.y>rcClient.bottom - offset)
	{
		//右下角
		return HTBOTTOMRIGHT;
	}
	else if (!::IsZoomed(*this) && pt.x < rcClient.left + offset)
	{
		//左端
		return HTLEFT;
	}
	else if (!::IsZoomed(*this) && pt.x > rcClient.right - offset)
	{
		//右端
		return HTRIGHT;
	}
	else if (!::IsZoomed(*this) && pt.y < rcClient.top + offset)
	{
		//顶端
		return HTTOP;
	}
	else if (!::IsZoomed(*this) && pt.y > rcClient.bottom - offset)
	{
		//底端
		return HTBOTTOM;
	}
	else
	{
		return HTCLIENT;
	}
}


int CSimModalDialog::DoModal(LPCTSTR szClassName, LPCTSTR szWndName,
	int x, int y, int nWidth, int nHeight,
	HWND hParentWnd /*= NULL*/, HINSTANCE hInstance /*= NULL*/)
{
	if (NULL == hParentWnd)
		hParentWnd = ::GetActiveWindow();
	if (NULL == hInstance)
		hInstance = ::GetModuleHandle(NULL);

	BOOL bEnableParent = FALSE;
	if (hParentWnd &&hParentWnd != ::GetDesktopWindow() && ::IsWindowEnabled(hParentWnd))
	{
		::EnableWindow(hParentWnd, FALSE);
		bEnableParent = TRUE;
	}

	if (!CSimWndFramework::CreateEx(WS_EX_ACCEPTFILES | WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
		szClassName, szWndName,
		WS_DLGFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN | WS_THICKFRAME,
		x, y, nWidth, nHeight, hParentWnd, NULL, hInstance
		))
	{
		return 0;
	}

	//去掉WS_CAPTION
	//去掉标题栏风格，因为如果还保留标题栏的话，那么窗口
	//的左上角和右上角会出现圆角效果
	//CSimWndFramework::ModifyStyle(WS_CAPTION, NULL);

	if (-1 == m_nRetCode)
	{
		HWND hWndLastActive = ::SetActiveWindow(*this);
		DWORD dwExStyle = (DWORD)GetWindowLongEx(*this, GWL_EXSTYLE);
		if (dwExStyle&WS_EX_TOOLWINDOW)
			::ShowWindow(*this, SW_SHOWNOACTIVATE);
		else
			::ShowWindow(*this, SW_SHOWNORMAL);

		//接管窗口消息循环
		MessageLoop();

		if (IsWindow(*this))
		{
			::SetWindowPos(*this, NULL, 0, 0, 0, 0,
				SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		}

		if (bEnableParent)
		{
			::EnableWindow(hParentWnd, TRUE);
		}

		::SetActiveWindow(hWndLastActive);
	}

	if (::IsWindow(*this))
		::DestroyWindow(*this);

	return m_nRetCode;
}

int CSimModalDialog::MessageLoop()
{
	MSG msg;
	while (-1 == m_nRetCode)
	{
#ifdef OUTPUTWNDMSG
		OutputStr(_T("m_nRetCode：%d\n"),m_nRetCode);
#endif
		GetMessage(&msg, NULL, 0, 0);
		//拦截感兴趣的窗口消息
		if (PreTranslateMessage(&msg))
			continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

void CSimModalDialog::EndDialog(int nResult)
{
	m_nRetCode = nResult;
}

LRESULT CSimModalDialog::OnClose(WPARAM wParam, LPARAM lParam)
{
	EndDialog(IDCLOSE);
	return FALSE;
}
