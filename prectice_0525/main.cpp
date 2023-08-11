#include <windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <shlwapi.h>
#include "olectl.h"
#include <winuser.h>
#include<Shlwapi.h>
#include "main.h"
#include "Resource.h"

#include <iostream>
using namespace std;

#pragma comment(lib, "Shlwapi.lib")

int popup_scr_cx;
int popup_scr_cy;

char* szSign = "ȫ�浿";

int DrawBitmapEx(HDC hdc, int x, int y, int cx, int cy, HBITMAP hBmp, BOOL bStretchBlt)
{
	HBITMAP hOldBmp = NULL;
	HDC hMemDC = NULL;
	BITMAP bit;
	int	bx=1, by=1;

	hMemDC	= ::CreateCompatibleDC(hdc);
	hOldBmp	= (HBITMAP)::SelectObject(hMemDC, (HGDIOBJ)hBmp);
	GetObject(hBmp, sizeof(BITMAP),&bit);
	bx = bit.bmWidth;
	by = bit.bmHeight;

	int scr_cx = GetSystemMetrics(SM_CXSCREEN);
	int scr_cy = GetSystemMetrics(SM_CYSCREEN);


	SetStretchBltMode(hdc,  HALFTONE);
	StretchBlt(hdc, x, y, popup_scr_cx, popup_scr_cy, hMemDC, 0, 0, bx, by, SRCCOPY);

	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);

	return TRUE;
}

//�����̸�(�̹��� ����)�� �Է¹޾� ��Ʈ���ڵ��� �Ѱ���

#define	STRARRAYSIZE(x)		(sizeof(x)/sizeof(x[0]))

HBITMAP ImageToBmp(HWND hWnd, char * FileName)
{
	IPicture *Ipic = NULL;
	SIZE sizeInHiMetric,sizeInPix;
	const int HIMETRIC_PER_INCH = 2540;
	HDC hDCScreen = GetDC(hWnd);
	HRESULT hr;

	int nPixelsPerInchX = GetDeviceCaps(hDCScreen, LOGPIXELSX);
	int nPixelsPerInchY = GetDeviceCaps(hDCScreen, LOGPIXELSY);
	//unsigned short OlePathName[512];
	WCHAR OlePathName[512];

	
//	OSVERSIONINFO	osVersion;
//	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//	GetVersionEx(&osVersion);

	//OS ���� üũ
//	if (osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
	MultiByteToWideChar(CP_ACP, 0, FileName, strlen(FileName)+1, OlePathName, STRARRAYSIZE(OlePathName));

//	else 
 //	mbstowcs(OlePathName,FileName,lstrlen(FileName));
	
	hr = OleLoadPicturePath(OlePathName, NULL, 0,0,IID_IPicture,(void **)(&Ipic));
	if (!SUCCEEDED(hr))
	{
		ReleaseDC(hWnd,hDCScreen);
		return NULL;
	}
	if (Ipic) 
	{
		// get width and height of picture
		hr = Ipic->get_Width(&sizeInHiMetric.cx);
		if (!SUCCEEDED(hr))
		{
			ReleaseDC(hWnd,hDCScreen);
			return NULL;
		}
		hr = Ipic->get_Height(&sizeInHiMetric.cy);
		if (!SUCCEEDED(hr))
		{
			ReleaseDC(hWnd,hDCScreen);
			return NULL;
		}
		// convert himetric to pixels
		sizeInPix.cx = (nPixelsPerInchX * sizeInHiMetric.cx + HIMETRIC_PER_INCH / 2) / HIMETRIC_PER_INCH;
		sizeInPix.cy = (nPixelsPerInchY * sizeInHiMetric.cy + HIMETRIC_PER_INCH / 2) / HIMETRIC_PER_INCH;
	}
	else 
	{
		return NULL;
		ReleaseDC(hWnd,hDCScreen);
	}

	DWORD dwAttr = 0;
	HBITMAP Bmp,BmpOld;
	RECT rc;
	HPALETTE pPalMemOld;

	if (Ipic)
	{
		// get palette
		OLE_HANDLE hPal = 0;
		HPALETTE hPalOld=NULL,hPalMemOld=NULL;
		hr = Ipic->get_hPal(&hPal);

		if (!SUCCEEDED(hr))
			return NULL;
		if (hPal != 0)
		{
			hPalOld = SelectPalette(hDCScreen,(HPALETTE)hPal,FALSE);
			RealizePalette(hDCScreen);
		}
		// Fit the image to the size of the client area. Change this
		// For more sophisticated scaling
		GetClientRect(hWnd,&rc);
		// transparent?
		if (SUCCEEDED(Ipic->get_Attributes(&dwAttr)) ||
			(dwAttr & PICTURE_TRANSPARENT))
		{
			// use an off-screen DC to prevent flickering
			HDC MemDC = CreateCompatibleDC(hDCScreen);
			Bmp = CreateCompatibleBitmap(hDCScreen,sizeInPix.cx,sizeInPix.cy);

			BmpOld = (HBITMAP)SelectObject(MemDC,Bmp);
			pPalMemOld = NULL;
			if (hPal != 0)
			{
				hPalMemOld = SelectPalette(MemDC,(HPALETTE)hPal, FALSE);
				RealizePalette(MemDC);
			}
/* Use this to show the left corner
			rc.left = rc.top = 0;
			rc.right = ImageInfo.sizeInPix.cx;
		    rc.bottom = ImageInfo.sizeInPix.cy;
*/
			// display picture using IPicture::Render
			hr = Ipic->Render(MemDC, 0, 0, sizeInPix.cx, sizeInPix.cy, 0, sizeInHiMetric.cy, sizeInHiMetric.cx, -sizeInHiMetric.cy,	&rc);

			SelectObject(MemDC,BmpOld);

			if (pPalMemOld)
				SelectPalette(MemDC,pPalMemOld, FALSE);

			DeleteDC(MemDC);
		}
		else
		{
			HDC MemDC = CreateCompatibleDC(hDCScreen);
			Bmp = CreateCompatibleBitmap(hDCScreen,sizeInPix.cx,sizeInPix.cy);

			BmpOld = (HBITMAP)SelectObject(MemDC,Bmp);
			// display picture using IPicture::Render
			Ipic->Render(MemDC, 0, 0, sizeInPix.cx, sizeInPix.cy, 0, sizeInHiMetric.cy, sizeInHiMetric.cx, -sizeInHiMetric.cy, &rc);
			SelectObject(MemDC,BmpOld);
			DeleteDC(MemDC);
		}

		if (hPalOld != NULL) SelectPalette(hDCScreen,hPalOld, FALSE);
		if (hPal)	DeleteObject((HPALETTE)hPal);
	}
	Ipic->Release();
	ReleaseDC(hWnd,hDCScreen);
	return Bmp;
}
///////////////////////////////////////////////////////////////////////////////	


//2023.05.16
HBITMAP g_ImgWndBg3 = NULL;		// ����̹���
RECT	g_rcImgWndCloseBtn3;	// ����̹���â �����ư ��ġ
HBITMAP g_popupTextImg = NULL;	// �۾� �̹���
//RECT	g_popup;				//�˾�â
int popup_x = 0;	//�˾� x
int popup_y = 0;	//�˾� y


//2023.08.10
RECT g_rcName;
RECT g_rcName2;

BOOL/*LRESULT*/ CALLBACK IMGDlgPopupWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;

	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	switch(iMsg)
	{
	case WM_INITDIALOG:
		return OnIMGDlgPopupWndDialogInit(hWnd, iMsg, wParam, lParam);
	case WM_COMMAND:
		return OnIMGDlgPopupWndCommand(hWnd, iMsg, wParam, lParam);
	case WM_PAINT:
		return OnIMGDlgPopupWndPaint(hWnd, iMsg, wParam, lParam);
	case WM_MOUSEMOVE:
		if(g_ImgWndBg3)
		{
			HCURSOR hCursor;

			if (PtInRect(&g_rcImgWndCloseBtn3, pt))
			{
				hCursor = LoadCursor(NULL, IDC_HAND);
				SetCursor(hCursor);
			}
			else
			{
				hCursor = LoadCursor(NULL, IDC_ARROW);
				SetCursor(hCursor);
			}
		}
		return FALSE;
	case WM_LBUTTONUP:
		return OnIMGDlgPopupWndLButtonUp(hWnd, iMsg, wParam, lParam);
	case WM_CTLCOLORSTATIC:	//2023.05.11
    {
		//2023.08.09
		HDC hdcStatic = (HDC)wParam;
		int controlId = GetDlgCtrlID((HWND)lParam);
		if (controlId == IDC_STATIC_NAME) // Check the control's ID
		{
			
			SetTextColor(hdcStatic, RGB(19, 84, 138)); // Set text color to red
		}
		if (controlId == IDC_STATIC_NAME2) // Check the control's ID
		{
			//SetTextAlign(hdcStatic, TA_CENTER);
		}


		SetBkMode(hdcStatic,TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}
	case WM_DESTROY:
		return OnIMGDlgPopupWndDestroy(hWnd, iMsg, wParam, lParam);
	}
	
	return FALSE;
//	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

BOOL OnIMGDlgPopupWndDialogInit(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	
	int scr_cx = GetSystemMetrics(SM_CXSCREEN);
	int scr_cy = GetSystemMetrics(SM_CYSCREEN);

	popup_scr_cx = GetSystemMetrics(SM_CXSCREEN); //����
	popup_scr_cy = GetSystemMetrics(SM_CYSCREEN);

	int nWidth = 0;		//��ư x
	int nHeight = 0;	//��ư y
	char szImgFilePath[1024] = {0,};
	
	char szBackImgFilePath[1024] = {0,}; //2023.05.16
	
	// �̹��� �ε�
	sprintf(szImgFilePath, "C:\\PowerGate\\Temp\\popup.jpg");


	if(TRUE == PathFileExists(szImgFilePath))
	{
		g_ImgWndBg3 = ImageToBmp(hWnd, szImgFilePath);	// ����̹���
	}
	else
	{
		EndDialog(hWnd, IDCANCEL);
	}
	BITMAP bmp;

	//2023.08.09
	char szTextImgFilePath[1024] = {0,}; //2023.05.16
	sprintf(szTextImgFilePath, "C:\\PowerGate\\Temp\\popupText.jpg");
	if(TRUE == PathFileExists(szTextImgFilePath))
	{
		g_popupTextImg = ImageToBmp(hWnd, szTextImgFilePath);	// �ؽ�Ʈ�̹���
	}
	else
	{
		EndDialog(hWnd, IDCANCEL);
	}


	//�̹��� ũ�� ��������
	GetObject(g_ImgWndBg3, sizeof bmp, &bmp);
	//nWidth = (int)bmp.bmWidth;
	//nHeight = (int)bmp.bmHeight;

	//����� ��ü ũ��
	int iTotalMonitorWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int iTotalMonitorHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	
	int monitorX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int monitorY=GetSystemMetrics(SM_YVIRTUALSCREEN);
	//����� ����
	int monitorNum = GetSystemMetrics(SM_CMONITORS);

	// â ũ�� ����
	SetWindowPos(hWnd, HWND_TOPMOST, monitorX,monitorY,iTotalMonitorWidth, iTotalMonitorHeight, 0);
	
	
	// ��ư ��������
	::SetRect(&g_rcImgWndCloseBtn3, popup_scr_cx*0.815, popup_scr_cy*0.84, popup_scr_cx*0.94, popup_scr_cy*0.9);
	
	// �̸� ���� ���� //2023.08.09
	//�̸�1
	::SetRect(&g_rcName, popup_scr_cx*0.202,popup_scr_cy*0.234,popup_scr_cx*0.202*1.3,popup_scr_cy*0.234*1.2);
	//�̸�2
	::SetRect(&g_rcName2, popup_scr_cx*0.838,popup_scr_cy*0.793,popup_scr_cx*0.838*1.08,popup_scr_cy*0.793*1.2);
	

	// ����
	
	HWND hSign = GetDlgItem(hWnd, IDC_STATIC_SIGN);
	MoveWindow(hSign, popup_scr_cx*0.915,popup_scr_cy*0.795,popup_scr_cx*0.2,popup_scr_cy*0.2, TRUE); //����
	HFONT hSignFont = CreateFont(popup_scr_cx*0.02,0,0,0,0,0,0,0,HANGEUL_CHARSET,0,0,0,VARIABLE_PITCH | FF_ROMAN,TEXT("�޸�����ü"));
	SendMessage(hSign, WM_SETFONT, (WPARAM)hSignFont, TRUE);

	//::SetRect(&g_rcSign, popup_scr_cx*0.915,popup_scr_cy*0.795,popup_scr_cx*0.915*1.08,popup_scr_cy*0.795*1.2);
	

	return FALSE;
}

BOOL OnIMGDlgPopupWndNotify(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL OnIMGDlgPopupWndCommand(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

BOOL OnIMGDlgPopupWndPaint(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HBITMAP hOldbmp;
	RECT rc;

	//2023.08.11
	HFONT hNameFont, hNameFont2, hSignFont, hOldFont;

	hOldFont = (HFONT)SelectObject(hdc, hOldFont);
	hNameFont = CreateFont(popup_scr_cx*0.021,0,0,0,FW_BOLD,0,0,0,HANGEUL_CHARSET,0,0,0,VARIABLE_PITCH | FF_ROMAN,TEXT("���� ���"));
	hNameFont2 = CreateFont(popup_scr_cx*0.017,0,0,0,0,0,0,0,HANGEUL_CHARSET,0,0,0,VARIABLE_PITCH | FF_ROMAN,TEXT("������"));
	hSignFont = CreateFont(popup_scr_cx*0.02,0,0,0,0,0,0,0,HANGEUL_CHARSET,0,0,0,VARIABLE_PITCH | FF_ROMAN,TEXT("�޸�����ü"));

	SetBkMode(hdc, TRANSPARENT);
	
	hdc = BeginPaint(hWnd, &ps);
	GetClientRect(hWnd, &rc);


	// ��� �׸���
	if (g_ImgWndBg3){
		DrawBitmapEx(hdc, 0, 0, 100, 100, g_ImgWndBg3, FALSE);
	}
	
	//�̸�1
	SelectObject(hdc, hNameFont);
	SetTextColor(hdc, RGB(19, 84, 138));
	DrawText(hdc,szSign,-1,&g_rcName, DT_RIGHT | DT_WORDBREAK); 
	SelectObject(hdc, hOldFont);

	//�̸�2
	SelectObject(hdc, hNameFont2);
	SetTextColor(hdc, RGB(0, 0, 0));
	DrawText(hdc,szSign,-1,&g_rcName2, DT_RIGHT | DT_WORDBREAK); 
	SelectObject(hdc, hOldFont);

	//����
	
	//SelectObject(hdc, hSignFont);
	//DrawText(hdc,szSign,-1,&g_rcSign, DT_LEFT | DT_WORDBREAK); 
	//SelectObject(hdc, hOldFont);


	return FALSE;
}

BOOL OnIMGDlgPopupWndLButtonUp(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;

	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);
	

	//extern char m_szID;
	static SYSTEMTIME time;
	static TCHAR buttonupTime[40];

	GetLocalTime(&time);
	wsprintf(buttonupTime,_T("%d�� %d�� %d��"), time.wHour, time.wMinute, time.wSecond);

	if(g_ImgWndBg3)
	{
		if (PtInRect(&g_rcImgWndCloseBtn3, pt))
		{
			
			SetDlgItemText(hWnd, IDC_STATIC_SIGN, szSign);
			Sleep(2000);
			EndDialog(hWnd, IDCANCEL);

		}
	}

	return FALSE;
}

BOOL OnIMGDlgPopupWndDestroy(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	if(g_ImgWndBg3)
	{
		DeleteObject(g_ImgWndBg3);
		g_ImgWndBg3 = NULL;
	}
	return FALSE;
}



int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)

{
	HWND m_hWnd;
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), m_hWnd, IMGDlgPopupWndProc, NULL);
}


