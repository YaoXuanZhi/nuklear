/*****************************************************************************
*  @file     CSimWndFramework.h
*  @brief    һ���򵥵�win32 ���ڿ��
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
	* @brief: �����Ѵ��ڵĿؼ�������������໯
	* @param[in] HWND  hDstWnd  �����໯�Ŀؼ��Ĵ��ھ��
	* @param[in] HINSTANCE  hInstancce  �����ʵ�����
	* @return ��
	* @note
	* ע�⣬�˺�����ר������Ѿ����ڵĿؼ��������໯�ģ���
	* ���ᴴ�����ھ��
	*/
	bool AttachAndSubclass(HWND hDstWnd, HINSTANCE hInstance);

	WNDPROC GetOldWndProc();

protected:
	void Attach(HWND newWindow);
	void SetSubclassProc();
	static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK _SubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND m_hWnd;					/**< ����Ĵ��ھ�� */
	HINSTANCE m_hInstance;			/**< �����������ĳ���ʵ����� */
	WNDPROC _origWindowProc;		/**< ��Ŵ����໯�Ŀؼ���ԭ���Ĵ��ڻص������ĺ���ָ�� */

protected:
	/**
	* @brief: ������Ϣѭ��
	* @return �������Ϊtrue�������Ϣ�������´���
	* @note
	* ע�⣬������������ֵΪtrue��ʱ�����Ч����ʱlResult����ֵ����
	* Ӱ�촰����Ϊ���ȷ�˵����Ҫ���ޱ߿��ޱ������Ĵ�����ʵ�ִ������죬��ô
	* ����Ҫ���ش�����ϢWM_NCHITTEST����������Ҫ��lResult��ΪHTBOTTOM��HTTOP��
	*/
	virtual bool HandleMessage(UINT, WPARAM, LPARAM, LRESULT &lResult);

	/** 
	* @brief: ���봰�ڻص�����֮ǰ����Ϣ·��
	* @note   
	* �ο�MFC���������
	*/
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/**
	* @brief: �ڵײ㴰�ڴ���֮�����ص���Ϣ·��
	* @note
	* �˺���Ӧ�ú����õ���ͨ����Ȼ��һ����ģ��ؼ�����֮���ٴ��ݵ�
	* ģ��ײ㴰���ϵģ���Ϊ���տؼ���Z-Order�ķ���,��Ӧ����ء�����
	* Ϊ�˱���ĳЩ����£��ȷ�˵���Ӵ��ڿؼ���ģ��ؼ�ͬʱˢ�µ�����
	* ��˱����������е��Ҫ��
	*/
	virtual void AfterProcessMessage(UINT uMsg, WPARAM, LPARAM);

	/**
	* @brief: �ڵײ㴰�ڴ���֮ǰ���ص���Ϣ·��
	* @note
	*
	*/
	virtual bool BeforeProcessMessage(UINT uMsg, WPARAM, LPARAM, LRESULT &lResult);

	/**
	* @brief: ����������������Ӧ����Ϣ�¼�
	* @note
	*
	*/
	virtual bool ProcessMessage(UINT uMsg, WPARAM, LPARAM, LRESULT &lResult);

};


/**
* @class CSimNormalDialog
* @brief ��ģ̬�Ի�����
* @note
* �ڲ��Ѿ������˴��ڹر���
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
* @brief ģ̬�Ի�����
* @note
* �ڲ��Ѿ��ṩ�ӿ�������ģ̬���ڵĴ����͹ر�
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
	int m_nRetCode;/**< ����ģ̬�Ի���ı�ʶ */
	virtual int MessageLoop();

protected:
	LRESULT OnClose(WPARAM, LPARAM);

	BEGIN_MSG_MAP_SIMWNDPROC(CSimModalDialog)
		MSG_MAP_SIMWNDPROC(WM_CLOSE, OnClose)
	END_MSG_MAP_SIMWNDPROC(CSimNormalDialog)

};

#endif