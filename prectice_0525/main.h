

HBITMAP ImageToBmp(HWND hWnd,char * FileName);

BOOL/*LRESULT*/ CALLBACK IMGDlgPopupWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

BOOL OnIMGDlgPopupWndDialogInit(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL OnIMGDlgPopupWndNotify(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL OnIMGDlgPopupWndCommand(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL OnIMGDlgPopupWndPaint(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL OnIMGDlgPopupWndLButtonUp(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL OnIMGDlgPopupWndDestroy(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL DlgPopupSign(HWND hWnd);
