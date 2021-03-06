// 1512034.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "1512034.h"
#include <windowsx.h>
#include <commctrl.h>
#include <codecvt>
#include <fstream>
#include <locale>
#include <string>
#include <vector>

#include <gdiplus.h>
#include <ObjIdl.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ComCtl32.lib")
using namespace Gdiplus;
using namespace std;

#define MAX_LOADSTRING 100
#define FILE_PATH		L"input.txt"

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MY1512034, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY1512034));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY1512034));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY1512034);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, L"Quản lý chi tiêu", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 700, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

typedef struct MH
{
	WCHAR Type[40];
	unsigned long long Tien;
	wstring NoiDung;
};

vector<MH> danhsachMH;
int SoMatHang = 0;
long TongTien = 0;
WCHAR Loai[6][35] = { L"Ăn uống", L"Di chuyển", L"Nhà cửa", L"Xe cộ", L"Nhu yếu phẩm", L"Dịch vụ" };
unsigned long TiLe[6];

GdiplusStartupInput GdiPlusStartup;
ULONG_PTR           GdiPlusToken;
HWND hCbb;
HWND hTien;
HWND hNoiDung;
HWND hTongTien; 
HWND hThem;
HWND my_hWnd;
HWND my_hListview;
HWND hTemp;

void wFile(wstring);
void loadFile(wstring);
void loadListView(HWND);
void setWindow(HWND, long long, wstring, wstring);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		GdiplusStartup(&GdiPlusToken, &GdiPlusStartup, NULL);
		INITCOMMONCONTROLSEX icc;
		icc.dwSize = sizeof(icc);
		icc.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx(&icc);

		//lay font he thong
		HFONT hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial"); SetWindowFont(hWnd, hFont, NULL);

		//combobox
		hCbb = CreateWindowEx(0, WC_COMBOBOX, TEXT(""),
			CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
			40, 60, 110, 100, hWnd, NULL, hInst,
			NULL);
		SendMessage(hCbb, WM_SETFONT, WPARAM(hFont), TRUE);

		//them chi tieu
		HWND hGroupb1 = CreateWindowEx(0, L"BUTTON", L"Thêm một loại chi tiêu", WS_CHILD | WS_VISIBLE | BS_GROUPBOX ,
			20, 10, 540, 100, hWnd, (HMENU)IDC_GROUPBOX, hInst, NULL);
		SendMessage(hGroupb1, WM_SETFONT, WPARAM(hFont), TRUE);

		hNoiDung = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
			170, 60, 110, 22, hWnd, NULL, hInst, NULL);
		SendMessage(hNoiDung, WM_SETFONT, WPARAM(hFont), TRUE);

		hTien = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			300, 60, 110, 22, hWnd, NULL, hInst, NULL);
		SendMessage(hTien, WM_SETFONT, WPARAM(hFont), TRUE);

		hThem = CreateWindowEx(0, L"BUTTON", L"Thêm", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			440, 57, 70, 25, hWnd, (HMENU)IDC_ADD, hInst, NULL);
		SendMessage(hThem, WM_SETFONT, WPARAM(hFont), TRUE);

		hTemp = CreateWindowEx(0, L"STATIC", L"Số tiền:", WS_CHILD | WS_VISIBLE,
			300, 40, 40, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);

		hTemp = CreateWindowEx(0, L"STATIC", L"Loại chi tiêu:", WS_CHILD | WS_VISIBLE,
			40, 40, 65, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);

		hTemp = CreateWindowEx(0, L"STATIC", L"Nội dung:", WS_CHILD | WS_VISIBLE,
			170, 40, 50, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);

		TCHAR T[16];
		memset(&T, 0, sizeof(T));
		for (int i = 0; i < 6; i++)
		{
			wcscpy_s(T, sizeof(T) / sizeof(TCHAR), (TCHAR*)Loai[i]);
			TiLe[i] = 0;
			SendMessage(hCbb, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)T);
		}

		SendMessage(hCbb, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

		//danh sach chi tiet cac chi tieu
		HWND hGroupb2 = CreateWindowEx(0, L"BUTTON", L"Toàn bộ danh sách các chi tiêu", WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
			20, 115, 540, 200, hWnd, (HMENU)IDC_GROUPBOX, hInst, NULL);
		SendMessage(hGroupb2, WM_SETFONT, WPARAM(hFont), TRUE);
		long extStyle = WS_EX_CLIENTEDGE;
		long style = LVS_REPORT | LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS;
		my_hListview = CreateWindowEx(extStyle, WC_LISTVIEW, _T("List View"),
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | style,
			40, 145, 500, 150, hWnd, (HMENU)IDL_LISTVIEW, hInst, NULL);
		LVCOLUMN lc;
		lc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lc.fmt = LVCFMT_LEFT;
		lc.cx = 165;
		lc.pszText = _T("Loại chi tiêu");
		ListView_InsertColumn(my_hListview, 0, &lc);

		lc.fmt = LVCFMT_LEFT;
		lc.cx = 165;
		lc.pszText = _T("Nội dung");
		ListView_InsertColumn(my_hListview, 1, &lc);
		lc.fmt = LVCFMT_LEFT;
		lc.pszText = _T("Số tiền");
		lc.cx = 120;
		ListView_InsertColumn(my_hListview, 2, &lc);

		//thong ke
		HWND hGroupb3 = CreateWindowEx(0, L"BUTTON", L"Thông tin thống kê", WS_CHILD | WS_VISIBLE | BS_GROUPBOX | WS_GROUP,
			20, 320, 540, 205, hWnd, (HMENU)IDC_GROUPBOX, hInst, NULL);
		SendMessage(hGroupb3, WM_SETFONT, WPARAM(hFont), TRUE);
		hTongTien = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
			250, 350, 110, 22, hWnd, NULL, hInst, NULL);
		SendMessage(hTongTien, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindowEx(0, L"STATIC", L"Tổng cộng:", WS_CHILD | WS_VISIBLE, 175, 350, 55, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindowEx(0, L"STATIC", L"Ăn uống", WS_CHILD | WS_VISIBLE, 60, 487, 55, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindowEx(0, L"STATIC", L"Nhu yếu phẩm", WS_CHILD | WS_VISIBLE, 370, 487, 90, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindowEx(0, L"STATIC", L"Dịch vụ", WS_CHILD | WS_VISIBLE, 460, 487, 55, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindowEx(0, L"STATIC", L"Di chuyển", WS_CHILD | WS_VISIBLE, 140, 487, 55, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindowEx(0, L"STATIC", L"Nhà cửa", WS_CHILD | WS_VISIBLE, 220, 487, 55, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hTemp = CreateWindowEx(0, L"STATIC", L"Xe cộ", WS_CHILD | WS_VISIBLE, 300, 487, 55, 15, hWnd, NULL, hInst, NULL);
		SendMessage(hTemp, WM_SETFONT, WPARAM(hFont), TRUE);
		hThem = CreateWindowEx(0, L"BUTTON", L"Thoát", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			240, 540, 90, 35, hWnd, (HMENU)IDC_CLOSE, hInst, NULL);
		SendMessage(hThem, WM_SETFONT, WPARAM(hFont), TRUE);
		loadFile(FILE_PATH);
		loadListView(my_hListview);
		SoMatHang = danhsachMH.size();
		setWindow(hTongTien, TongTien, L"", L"");
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_ADD:
		{
			MH mh;
			WCHAR* buffer;

			//Get NoiDung
			int l = GetWindowTextLength(hNoiDung);
			if (l > 0)
			{
				buffer = new WCHAR[l + 1];
				GetWindowText(hNoiDung, buffer, l + 1);
				mh.NoiDung = wstring(buffer);
			}
			else
			{
				MessageBox(my_hWnd, L"Chưa điền nội dung!", L"Error", MB_ICONWARNING | MB_OK);
				break;
			}

			l = GetWindowTextLength(hTien);
			if (l > 0)
			{
				buffer = new WCHAR[l + 1];
				GetWindowText(hTien, buffer, l + 1);
				mh.Tien = _wtoi64(buffer);
			}
			else
			{
				MessageBox(my_hWnd, L"Chưa điền nội dung!", L"Error", MB_ICONWARNING | MB_OK);
				break;
			}
			buffer = new WCHAR[20];
			GetWindowText(hCbb, buffer, 20);
			wcscpy_s(mh.Type, buffer);
			danhsachMH.push_back(mh);

			if (wcscmp(mh.Type, L"Ăn uống") == 0)
				TiLe[0] += mh.Tien;
			if (wcscmp(mh.Type, L"Di chuyển") == 0)
				TiLe[1] += mh.Tien;
			if (wcscmp(mh.Type, L"Nhà cửa") == 0)
				TiLe[2] += mh.Tien;
			if (wcscmp(mh.Type, L"Xe cộ") == 0)
				TiLe[3] += mh.Tien;
			if (wcscmp(mh.Type, L"Nhu yếu phẩm") == 0)
				TiLe[4] += mh.Tien;
			if (wcscmp(mh.Type, L"Dịch vụ") == 0)
				TiLe[5] += mh.Tien;

			TongTien += mh.Tien;
			LV_ITEM lv;
			lv.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			lv.iItem = SoMatHang;
			lv.iSubItem = 0;
			lv.pszText = mh.Type;

			ListView_InsertItem(my_hListview, &lv);
			lv.mask = LVIF_TEXT;
			lv.iSubItem = 1;
			lv.pszText = (WCHAR*)mh.NoiDung.c_str();
			ListView_SetItem(my_hListview, &lv);
			lv.iSubItem = 2;
			buffer = new WCHAR[20];
			wsprintf(buffer, L"%I64d", mh.Tien);
			lv.pszText = buffer;
			ListView_SetItem(my_hListview, &lv);
			setWindow(hTongTien, TongTien, L"", L"");
			RedrawWindow(my_hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ERASENOW | RDW_INVALIDATE);
			RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_UPDATENOW | RDW_ERASENOW | RDW_INVALIDATE);
			break;
		}
		case IDC_CLOSE:
			wFile(FILE_PATH);
			DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			wFile(FILE_PATH);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		HDC hdc = BeginPaint(hWnd, &paint);
		// TODO: Add any drawing code that uses hdc here...
		Pen* p = new Pen(Color(255, 0, 0, 0), 1);
		Graphics* graphics = new Graphics(hdc);
		HatchBrush* Red = new HatchBrush(HatchStyleCross, Color::Red, Color::Red);
		HatchBrush* Violet = new HatchBrush(HatchStyleCross, Color::Violet, Color::Violet);
		HatchBrush* Yellow = new HatchBrush(HatchStyleCross, Color::Yellow, Color::Yellow);
		HatchBrush* Green = new HatchBrush(HatchStyleCross, Color::Green, Color::Green);
		HatchBrush* Blue = new HatchBrush(HatchStyleCross, Color::Blue, Color::Blue);
		HatchBrush* Pink = new HatchBrush(HatchStyleCross, Color::Pink, Color::Pink);

		int po = 40;
		graphics->DrawRectangle(p, po, 393, 497, 35);
		if (TongTien != 0)
			for (int i = 0; i < 6; i++)
			{
				graphics->DrawRectangle(p, po, 393, 500 * TiLe[i] / TongTien, 35);
				if (i == 0)
					graphics->FillRectangle(Red, po, 393, 500 * TiLe[i] / TongTien, 35);
				if (i == 1)
					graphics->FillRectangle(Green, po, 393, 500 * TiLe[i] / TongTien, 35);
				if (i == 2)
					graphics->FillRectangle(Blue, po, 393, 500 * TiLe[i] / TongTien, 35);
				if (i == 3)
					graphics->FillRectangle(Yellow, po, 393, 500 * TiLe[i] / TongTien, 35);
				if (i == 4)
					graphics->FillRectangle(Violet, po, 393, 500 * TiLe[i] / TongTien, 35);
				if (i == 5)
					graphics->FillRectangle(Pink, po, 393, 500 * TiLe[i] / TongTien, 35);
				po += 500 * TiLe[i] / TongTien;
			}

		graphics->DrawRectangle(p, 40, 460, 100, 19);
		graphics->FillRectangle(Red, 40, 460, 100, 19);
		graphics->DrawRectangle(p, 120, 460, 100, 19);
		graphics->FillRectangle(Green, 120, 460, 100, 19);
		graphics->DrawRectangle(p, 200, 460, 100, 19);
		graphics->FillRectangle(Blue, 200, 460, 100, 19);
		graphics->DrawRectangle(p, 280, 460, 100, 19);
		graphics->FillRectangle(Yellow, 280, 460, 100, 19);
		graphics->DrawRectangle(p, 360, 460, 100, 19);
		graphics->FillRectangle(Violet, 360, 460, 100, 19);
		graphics->DrawRectangle(p, 440, 460, 100, 19);
		graphics->FillRectangle(Pink, 440, 460, 100, 19);

		delete p;
		delete graphics;
		EndPaint(hWnd, &paint);
	}
	break;
	case WM_DESTROY:
		wFile(FILE_PATH);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void loadFile(wstring path)
{
	const locale utf8 = locale(locale(), new codecvt_utf8<wchar_t>());
	wfstream file;
	file.imbue(utf8);
	file.open(path, ios::in);

	wstring buffer;
	if (file.is_open())
	{
		if (getline(file, buffer))
			TongTien = _wtoi64(buffer.c_str());
		while (getline(file, buffer))
		{
			MH mh;

			wcscpy_s(mh.Type, buffer.c_str());
			getline(file, buffer);
			mh.NoiDung = buffer;
			getline(file, buffer);
			mh.Tien = _wtoi64(buffer.c_str());

			danhsachMH.push_back(mh);
		}
	}
	file.close();
}

void loadListView(HWND m_hListview)
{
	LV_ITEM l;
	WCHAR* buffer = new WCHAR[20];

	for (int i = 0; i < danhsachMH.size(); i++)
	{
		l.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
		l.iSubItem = 0;
		l.iItem = i;
		l.pszText = danhsachMH[i].Type;

		ListView_InsertItem(m_hListview, &l);

		l.mask = LVIF_TEXT;
		l.iSubItem = 1;
		l.pszText = (WCHAR*)danhsachMH[i].NoiDung.c_str();
		ListView_SetItem(m_hListview, &l);
		l.iSubItem = 2;
		buffer = new WCHAR[20];
		wsprintf(buffer, L"%I64d", danhsachMH[i].Tien);
		l.pszText = buffer;
		ListView_SetItem(m_hListview, &l);
		if (wcscmp(danhsachMH[i].Type, L"Ăn uống") == 0)
			TiLe[0] += danhsachMH[i].Tien;
		if (wcscmp(danhsachMH[i].Type, L"Di chuyển") == 0)
			TiLe[1] += danhsachMH[i].Tien;
		if (wcscmp(danhsachMH[i].Type, L"Nhà cửa") == 0)
			TiLe[2] += danhsachMH[i].Tien;
		if (wcscmp(danhsachMH[i].Type, L"Xe cộ") == 0)
			TiLe[3] += danhsachMH[i].Tien;
		if (wcscmp(danhsachMH[i].Type, L"Nhu yếu phẩm") == 0)
			TiLe[4] += danhsachMH[i].Tien;
		if (wcscmp(danhsachMH[i].Type, L"Dịch vụ") == 0)
			TiLe[5] += danhsachMH[i].Tien;
	}
}

void setWindow(HWND hWnd, long long value, wstring tBefore, wstring tAfter)
{
	WCHAR buffer[255];
	if (value < 10) wsprintf(buffer, L"0%I64d", value);
	else wsprintf(buffer, L"%I64d", value);
	SetWindowText(hWnd, (tBefore + wstring(buffer) + tAfter).c_str());
}

void wFile(wstring p)
{
	const locale utf8 = locale(locale(), new codecvt_utf8<wchar_t>());
	wofstream file(p);
	file.imbue(utf8);

	file << TongTien << endl;
	for (int i = 0; i < danhsachMH.size(); i++)
	{
		file << wstring(danhsachMH[i].Type) << endl;
		file << wstring(danhsachMH[i].NoiDung) << endl;
		file << danhsachMH[i].Tien << endl;
	}
	file.close();
}

