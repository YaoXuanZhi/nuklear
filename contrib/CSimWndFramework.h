/*****************************************************************************
*  @file     CSimWndFramework.h
*  @brief    一个简单的win32 窗口框架
*  @note
*****************************************************************************/
#ifndef __CSIMWNDFRAMEWORK_H
#define __CSIMWNDFRAMEWORK_H

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#pragma warning(disable:4996)

#ifndef BEGIN_MSG_MAP_SIMWNDPROC
#define BEGIN_MSG_MAP_SIMWNDPROC(theClass)\
protected:\
	virtual bool ProcessMessage(UINT message, WPARAM wParam , LPARAM lParam,LRESULT &lResult)\
{\
	switch (message)\
	{
#endif

#ifndef MSG_MAP_SIMWNDPROC
#define MSG_MAP_SIMWNDPROC(message,func)\
	case message:\
		{\
		lResult = this->func(wParam, lParam);\
		return true;\
		};break;
#endif

#ifndef END_MSG_MAP_SIMWNDPROC
#define END_MSG_MAP_SIMWNDPROC(BaseClass)\
	}\
	return BaseClass::ProcessMessage(message,wParam,lParam,lResult);\
}
#endif

class CSimWndFramework
{
public:
	CSimWndFramework(HWND hWnd = NULL);
	virtual ~CSimWndFramework();
	operator HWND() const;
	HINSTANCE GetInstance();

	bool CreateEx(DWORD dwExStyle, LPCTSTR szClassName,
		LPCTSTR szWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hParentWnd, HMENU hMenu = NULL,
		HINSTANCE hInstance = NULL);

	bool Create(LPCTSTR szClassName,
		LPCTSTR szWindowName, DWORD dwStyle,
		int x, int y, int nWidth, int nHeight,
		HWND hParentWnd, HMENU hMenu = NULL,
		HINSTANCE hInstance = NULL);

	bool ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0L);

	virtual int MessageLoop();

	/**
	* @brief: 吸附已存在的控件句柄并对其子类化
	* @param[in] HWND  hDstWnd  待子类化的控件的窗口句柄
	* @param[in] HINSTANCE  hInstancce  程序的实例句柄
	* @return 无
	* @note
	* 注意，此函数是专门针对已经存在的控件进行子类化的，并
	* 不会创建窗口句柄
	*/
	bool AttachAndSubclass(HWND hDstWnd, HINSTANCE hInstance);

	WNDPROC GetOldWndProc();

protected:
	void Attach(HWND newWindow);
	void SetSubclassProc();
	static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _SubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hWnd;					/**< 本类的窗口句柄 */
	HINSTANCE m_hInstance;			/**< 本类所依附的程序实例句柄 */
	WNDPROC _origWindowProc;		/**< 存放待子类化的控件的原本的窗口回调函数的函数指针 */

protected:
	/**
	* @brief: 窗口消息循环
	* @return 如果返回为true，则此消息不再往下传递
	* @note
	* 注意，仅当函数返回值为true的时候才有效，此时lResult的数值将会
	* 影响窗口行为，比方说，想要在无边框无标题栏的窗口上实现窗口拉伸，那么
	* 就需要拦截窗口消息WM_NCHITTEST，并根据需要将lResult设为HTBOTTOM、HTTOP等
	*/
	virtual bool HandleMessage(UINT, WPARAM, LPARAM, LRESULT &lResult);

	/** 
	* @brief: 进入窗口回调函数之前的消息路由
	* @note   
	* 参考MFC里面的做法
	*/
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/**
	* @brief: 在底层窗口处理之后拦截的消息路由
	* @note
	* 此函数应该很少用到，通常而然，一般是模拟控件处理之后，再传递到
	* 模拟底层窗口上的，因为按照控件的Z-Order的分析,理应如此呢。不过
	* 为了避免某些情况下，比方说，子窗口控件和模拟控件同时刷新的问题
	* 因此保留它还是有点必要的
	*/
	virtual void AfterProcessMessage(UINT uMsg, WPARAM, LPARAM);

	/**
	* @brief: 在底层窗口处理之前拦截的消息路由
	* @note
	*
	*/
	virtual bool BeforeProcessMessage(UINT uMsg, WPARAM, LPARAM, LRESULT &lResult);

	/**
	* @brief: 本窗口真正可以响应的消息事件
	* @note
	*
	*/
	virtual bool ProcessMessage(UINT uMsg, WPARAM, LPARAM, LRESULT &lResult);

};


/**
* @class CSimNormalDialog
* @brief 非模态对话框类
* @note
* 内部已经处理了窗口关闭了
*/
class CSimNormalDialog :public CSimWndFramework
{
public:
	virtual ~CSimNormalDialog() {}
protected:
	virtual LRESULT OnDestory(WPARAM, LPARAM);
	LRESULT OnNcActive(WPARAM wParam, LPARAM);
	LRESULT OnNcCalcsize(WPARAM, LPARAM);
	LRESULT OnNcPaint(WPARAM, LPARAM);
	LRESULT OnNcHitTest(WPARAM, LPARAM);

	BEGIN_MSG_MAP_SIMWNDPROC(CSimNormalDialog)
		MSG_MAP_SIMWNDPROC(WM_NCACTIVATE, OnNcActive)
		MSG_MAP_SIMWNDPROC(WM_NCCALCSIZE, OnNcCalcsize)
		MSG_MAP_SIMWNDPROC(WM_NCPAINT, OnNcPaint)
		MSG_MAP_SIMWNDPROC(WM_DESTROY, OnDestory)
		MSG_MAP_SIMWNDPROC(WM_NCHITTEST, OnNcHitTest)
	END_MSG_MAP_SIMWNDPROC(CSimWndFramework)
};

/**
* @class CSimModalDialog
* @brief 模态对话框类
* @note
* 内部已经提供接口来处理模态窗口的创建和关闭
*/

class CSimModalDialog :public CSimNormalDialog
{
public:
	CSimModalDialog() :m_nRetCode(-1) {}
	virtual ~CSimModalDialog() {}
	int DoModal(LPCTSTR szClassName, LPCTSTR szWndName,
		int x, int y, int nWidth, int nHeight,
		HWND hParentWnd /*= NULL*/, HINSTANCE hInstance /*= NULL*/);

	virtual void EndDialog(int nResult);

protected:
	int m_nRetCode;/**< 管理模态对话框的标识 */
	virtual int MessageLoop();

protected:
	LRESULT OnClose(WPARAM, LPARAM);

	BEGIN_MSG_MAP_SIMWNDPROC(CSimModalDialog)
		MSG_MAP_SIMWNDPROC(WM_CLOSE, OnClose)
	END_MSG_MAP_SIMWNDPROC(CSimNormalDialog)

};

#endif