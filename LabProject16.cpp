// LabProject16.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//



#include "stdafx.h"
#include "LabProject16.h"
#include "GameFramework.h"

CGameFramework gGameFramework;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

												// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/* 윈도우 응용 프로그램의 시작진입점 */
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LABPROJECT16, szWindowClass, MAX_LOADSTRING);

	/* 윈도우 클래스를 등록해줘 */
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	/* ( 주 윈도우 생성 ) */
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT16));

	MSG msg;

	// 기본 메시지 루프입니다.
	while (1)	// 기존 : 응용 프로그램에서 메시지 Queue에서 메시지를 받아온다. = Queue가 비어 있으면 CPU를 운영체제로 반납한다.
	{			// 현재 : 프로그램이 처리할 메시지가 없더라고 화면 렌더링 , 사용자 입력 처리 , 길찾기 등의 작업이 계속 진행 되어야 한다.									
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// PeekMessage : 메시지 큐를 살펴보고 메시지가 있으면 TRUE를 반환 없으면 FALSE를 반환.
		{
			if (msg.message == WM_QUIT) break;
			// False를 나타낼 때는 WM_QUIT 메시지를 처리할 때이다.
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);			// 해당 윈도우로 전달한다. 
			}
		}
		/* 만약 윈도우 메시지가 비어있으면 ? */
		else
		{
			gGameFramework.FrameAdvance();	// FrameAdvance를 호출해 CPU를 사용할 수 있게 한다.
		}
	}

	gGameFramework.OnDestroy();	// 객체 소멸
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.

/*  윈도우 클래스 등록하면 그 클래스의 윈도우를 생성할 수 있다.  */
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc; // 윈도우 프로시저를 설정
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT16));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;	// 주 윈도우의 메뉴가 나타나지 않게 설정
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_LABPROJECT16);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.

/*	 주 윈도우를 생성하고 화면에 보이도록 하는 함수   */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	RECT rc = { 0 , 0 , 640 , 480 };

	/* AdjustWindowRect( LPRECT , DWORD , BOOL ) : 윈도우가 원하는 클라이언트 영역의 크기를 가지도록 윈도우의 크기를 계산해준다. */
	/* LPRECT : 원하는 클라이언트의 크기를 지정한다. , 윈도우의 새로운 크기를 반환 받기도 한다. */
	/* DWORD  : 윈도우 스타일을 지정한다. */
	/* BOOL   : 윈도우가 메뉴를 가지는 가의 여부를 지정한다. 참일경우 윈도우가 메뉴를 갖고 있다. */
	AdjustWindowRect(&rc, dwStyle, FALSE);

	HWND hMainWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	// 윈도우를 생성하는 윈도우 API 함수

	if (!hMainWnd)
		return (FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	ShowWindow(hMainWnd, nCmdShow);
	UpdateWindow(hMainWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//

/*  윈도우 프로시져 함수이다 - 메시지 루프에서 DispatchMessage() 윈도우 API함수가 호출되면 이함수가 실행된다.  */
/*  hWnd - 윈도우 핸들 , message - 메시지 ID , wParam , lParam - 2개의 파라미터로 이루어져 있다.  */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
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
