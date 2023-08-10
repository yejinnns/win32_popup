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

char* szSign = "홍홍길동";

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

	/*
	if (bStretchBlt == FALSE)
	{
		BitBlt(hdc, x, y, bx < 1 ? cx : bx, by < 1 ? cy : by, hMemDC, 0, 0, SRCCOPY);
	}
	else
	{
		StretchBlt(hdc, x, y, cx < 1 ? bx : cx, cy < 1 ? by : cy, hMemDC, 0, 0, bx, by, SRCCOPY);
	}
	*/
	SetStretchBltMode(hdc,  HALFTONE);
	StretchBlt(hdc, x, y, popup_scr_cx, popup_scr_cy, hMemDC, 0, 0, bx, by, SRCCOPY);

	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);

	return TRUE;
}

int DrawBitmapEx2(HDC hdc, int x, int y, int cx, int cy, HBITMAP hBmp, BOOL bStretchBlt)
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

	/*
	if (bStretchBlt == FALSE)
	{
		BitBlt(hdc, x, y, bx < 1 ? cx : bx, by < 1 ? cy : by, hMemDC, 0, 0, SRCCOPY);
	}
	else
	{
		StretchBlt(hdc, x, y, cx < 1 ? bx : cx, cy < 1 ? by : cy, hMemDC, 0, 0, bx, by, SRCCOPY);
	}
	*/
	SetStretchBltMode(hdc,  HALFTONE);
	size_t strLength = strlen(szSign);
	
	if(strLength==8){
		StretchBlt(hdc, scr_cx*0.15, scr_cy*0.2375, scr_cx*0.05, scr_cy*0.035, hMemDC, 0, 0, bx, by, SRCCOPY);
	}
	else{
		StretchBlt(hdc, scr_cx*0.17, scr_cy*0.2375, scr_cx*0.05, scr_cy*0.035, hMemDC, 0, 0, bx, by, SRCCOPY);
	}

	SelectObject(hMemDC, hOldBmp);
	DeleteDC(hMemDC);

	return TRUE;
}

//파일이름(이미지 파일)을 입력받아 비트맵핸들을 넘겨줌

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

	//OS 버전 체크
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
HBITMAP g_ImgWndBg = NULL;		// 배경이미지
HBITMAP g_ImgWndBg1 = NULL;		// 배경이미지
HBITMAP g_ImgWndBg2 = NULL;		// 배경이미지
RECT	g_rcImgWndCloseBtn;		// 배경이미지창 종료버튼 위치
RECT	g_rcImgWndCloseBtn1;	// 배경이미지창 종료버튼 위치
RECT	g_rcImgWndCloseBtn2;	// 배경이미지창 종료버튼 위치


//2023.05.16
HBITMAP g_ImgWndBg3 = NULL;		// 배경이미지
RECT	g_rcImgWndCloseBtn3;	// 배경이미지창 종료버튼 위치
HBITMAP g_popupTextImg = NULL;	// 글씨 이미지
//RECT	g_popup;				//팝업창
int popup_x = 0;	//팝업 x
int popup_y = 0;	//팝업 y
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

	popup_scr_cx = GetSystemMetrics(SM_CXSCREEN); //수정
	popup_scr_cy = GetSystemMetrics(SM_CYSCREEN);

	int nWidth = 0;		//버튼 x
	int nHeight = 0;	//버튼 y
	char szImgFilePath[1024] = {0,};
	
	char szBackImgFilePath[1024] = {0,}; //2023.05.16
	
	// 이미지 로딩
	sprintf(szImgFilePath, "C:\\PowerGate\\Temp\\popup.jpg");


	if(TRUE == PathFileExists(szImgFilePath))
	{
		g_ImgWndBg3 = ImageToBmp(hWnd, szImgFilePath);	// 배경이미지
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
		g_popupTextImg = ImageToBmp(hWnd, szTextImgFilePath);	// 텍스트이미지
	}
	else
	{
		EndDialog(hWnd, IDCANCEL);
	}


	//GetObject(g_hbgBitmap, sizeof bmp, &bmp);

	//이미지 크기 가져오기
	GetObject(g_ImgWndBg3, sizeof bmp, &bmp);
	//nWidth = (int)bmp.bmWidth;
	//nHeight = (int)bmp.bmHeight;

	//모니터 전체 크기
	int iTotalMonitorWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int iTotalMonitorHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	
	int monitorX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int monitorY=GetSystemMetrics(SM_YVIRTUALSCREEN);
	//모니터 개수
	int monitorNum = GetSystemMetrics(SM_CMONITORS);

	// 창 크기 설정
	SetWindowPos(hWnd, HWND_TOPMOST, monitorX,monitorY,iTotalMonitorWidth, iTotalMonitorHeight, 0);
	
	
	// 버튼 영역설정
	//::SetRect(&g_rcImgWndCloseBtn3, 465-34, 0, 465, 34);	// 배경이미지창 종료버튼 위치
	::SetRect(&g_rcImgWndCloseBtn3, popup_scr_cx*0.815, popup_scr_cy*0.84, popup_scr_cx*0.94, popup_scr_cy*0.9);
	
	// 이름 //2023.08.09
	
	HWND hName = GetDlgItem(hWnd, IDC_STATIC_NAME);
	size_t strLength = strlen(szSign);
	if(strLength==8){
		MoveWindow(hName, popup_scr_cx*0.204,popup_scr_cy*0.234,popup_scr_cx*0.2,popup_scr_cy*0.2, TRUE);
	}
	else{
		MoveWindow(hName, popup_scr_cx*0.22,popup_scr_cy*0.234,popup_scr_cx*0.2,popup_scr_cy*0.2, TRUE); 
	}
	HFONT hNameFont = CreateFont(popup_scr_cx*0.021,0,0,0,FW_BOLD,0,0,0,HANGEUL_CHARSET,0,0,0,VARIABLE_PITCH | FF_ROMAN,TEXT("맑은 고딕"));
	SendMessage(hName, WM_SETFONT, (WPARAM)hNameFont, TRUE);



	// 이름2 //2023.08.09
	HWND hName2 = GetDlgItem(hWnd, IDC_STATIC_NAME2);

	if(strLength==8){
		MoveWindow(hName2, popup_scr_cx*0.838,popup_scr_cy*0.793,popup_scr_cx*0.2,popup_scr_cy*0.2, TRUE); 
	}
	else{
		MoveWindow(hName2, popup_scr_cx*0.858,popup_scr_cy*0.793,popup_scr_cx*0.2,popup_scr_cy*0.2, TRUE); 
	}
	HFONT hNameFont2 = CreateFont(popup_scr_cx*0.017,0,0,0,0,0,0,0,HANGEUL_CHARSET,0,0,0,VARIABLE_PITCH | FF_ROMAN,TEXT("새굴림"));
	SendMessage(hName2, WM_SETFONT, (WPARAM)hNameFont2, TRUE);


	// 서명
	HWND hSign = GetDlgItem(hWnd, IDC_STATIC_SIGN);
	MoveWindow(hSign, popup_scr_cx*0.915,popup_scr_cy*0.795,popup_scr_cx*0.2,popup_scr_cy*0.2, TRUE); //수정
	HFONT hSignFont = CreateFont(popup_scr_cx*0.02,0,0,0,0,0,0,0,HANGEUL_CHARSET,0,0,0,VARIABLE_PITCH | FF_ROMAN,TEXT("휴먼편지체"));
	SendMessage(hSign, WM_SETFONT, (WPARAM)hSignFont, TRUE);
	
	// 배경 가릴 비트맵
	//g_BackImg = CreateBitmap(800,800,1,32,NULL);
  
	
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
	
	hdc = BeginPaint(hWnd, &ps);
	GetClientRect(hWnd, &rc);



	// 배경 그리기
	if (g_ImgWndBg3){
		//DrawBitmapEx(hdc, 0, 0, rc.right, rc.bottom, g_ImgWndBg3, FALSE);
		DrawBitmapEx(hdc, 0, 0, 100, 100, g_ImgWndBg3, FALSE);
		SetDlgItemText(hWnd, IDC_STATIC_NAME, szSign);
		SetDlgItemText(hWnd, IDC_STATIC_NAME2, szSign);
		//DrawBitmapEx(hdc, 0,0, GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN), g_ImgWndBg3, FALSE);
	}
	if (g_popupTextImg){

		DrawBitmapEx2(hdc, 0, 0, 100, 100, g_popupTextImg, FALSE);

	}
	
	//Sleep(2000);
	//EndDialog(hWnd, IDCANCEL);
	
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
	wsprintf(buttonupTime,_T("%d시 %d분 %d초"), time.wHour, time.wMinute, time.wSecond);

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