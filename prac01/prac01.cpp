﻿// prac01.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
// 2024. 5. 9.
// Created by Soo Kyun Kim

#include "framework.h"
#include "prac01.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "glut.h"

#define PI 3.14159265358979323846
#define MAX_LOADSTRING 100
#define	IDT_TIMER	1


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

/////////////////////// sk
HDC hDeviceContext;								// current device context
HGLRC hRenderingContext;						// current rendering context

//-----------------------------------------------------
     // virtual trackball
#include "trackball.h"
TrackBall trball;
bool bLButtonDown = false; 
//-----------------------------------------------------

bool bSetupPixelFormat(HDC hdc);
void Resize(int width, int height);
void DrawScene(HDC MyDC);

GLUquadric* pObj;
void BaseArm(void);
void LowerArm(void);
void UpperArm(void);

GLfloat viewer[3] = { 2.0f, 2.0f, 2.0f };

//-----------------------------------------------------
GLfloat vertices[8][3] = {
    { -1.0f, -1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f },
    { -1.0f, -1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f } };
GLfloat colors[8][3] = {
    { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
    { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } };

void Quad(int a, int b, int c, int d);
//-----------------------------------------------------


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
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

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PRAC01, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRAC01));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRAC01));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_PRAC01);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
// 
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT clientRect;

    switch (message)
    {
    case WM_CREATE:
        // Initialize for the OpenGL rendering
        hDeviceContext = GetDC(hWnd);
        if (!bSetupPixelFormat(hDeviceContext)) {
            MessageBox(hWnd, "Error in setting up pixel format for OpenGL", "Error", MB_OK | MB_ICONERROR);
            DestroyWindow(hWnd);
        }
        hRenderingContext = wglCreateContext(hDeviceContext);
        wglMakeCurrent(hDeviceContext, hRenderingContext);

        // virtual trackball
        trball.initialize();

        // create a quadric object
        pObj = gluNewQuadric();
        gluQuadricDrawStyle(pObj, GLU_LINE);

        break;

    case WM_SIZE:
        GetClientRect(hWnd, &clientRect);
        Resize(clientRect.right, clientRect.bottom);

        // virtual trackball
        trball.resize(clientRect.right, clientRect.bottom);

        InvalidateRect(hWnd, NULL, false);

        break;
        /*
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        */
    case WM_PAINT:
        {
        DrawScene(hDeviceContext);
        ValidateRect(hWnd, NULL);
      
        /*
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
            */
        }
        break;

    

    case WM_LBUTTONDOWN:
        if (!bLButtonDown)
        {
            bLButtonDown = true;
            trball.start((int)LOWORD(lParam), (int)HIWORD(lParam));
        }
        break;

    case WM_LBUTTONUP:
        bLButtonDown = false;
        break;

    case WM_MOUSEMOVE:
        if (bLButtonDown)
        {
            trball.end((int)LOWORD(lParam), (int)HIWORD(lParam));
            InvalidateRect(hWnd, NULL, true);
        }
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:
            viewer[0] -= 0.5f;
            break;
        case VK_RIGHT:
            viewer[0] += 0.5f;
            break;
        case VK_DOWN:
            viewer[1] -= 0.5f;
            break;
        case VK_UP:
            viewer[1] += 0.5f;
            break;
        case VK_PRIOR:
            viewer[2] -= 0.5f;
            break;
        case VK_NEXT:
            viewer[2] += 0.5f;
            break;
        case VK_ESCAPE:
            DestroyWindow(hWnd);
            break;
        case 0x57: // 어퍼 암을 시계 방향으로 회전시키기

        }
        InvalidateRect(hWnd, NULL, true);
        break;

    case WM_DESTROY:
        // Destroy all about OpenGL
        if (hRenderingContext)
            wglDeleteContext(hRenderingContext);
        if (hDeviceContext)
            ReleaseDC(hWnd, hDeviceContext);

        // Destroy a timer
        KillTimer(hWnd, IDT_TIMER);

        // delete a quadric object
        gluDeleteQuadric(pObj);

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

bool bSetupPixelFormat(HDC hdc)
{
    PIXELFORMATDESCRIPTOR pfd;
    int pixelformat;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.dwLayerMask = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.cAccumBits = 0;
    pfd.cStencilBits = 0;

    if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0) {
        MessageBox(NULL, "ChoosePixelFormat() failed!!!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    if (SetPixelFormat(hdc, pixelformat, &pfd) == false) {
        MessageBox(NULL, "SetPixelFormat() failed!!!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}
/*
 * Resize : to resize the window
*/

void Resize(const int cx, const int cy)
{
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();

    glViewport(0, 0, cx, cy);

    // 3D orthographic viewing
    if (cx <= cy)
        glOrtho( -2.0, 2.0, -2.0 * (GLfloat)cy / (GLfloat)cx, 2.0 * (GLfloat)cy / (GLfloat)cx, 1.0, 10.0);
    else
        glOrtho(-2.0 * (GLfloat)cx / (GLfloat)cy, 2.0 * (GLfloat)cx / (GLfloat)cy, -2.0, 2.0, 1.0, 10.0);

    return;

}
    

/*
* DrawScene : to draw a scene
*/
void DrawScene(HDC MyDC)
{
    glEnable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // Identity matrix - CTM (Current Transformation Matrix)
    
    gluLookAt(viewer[0], viewer[1], viewer[2],
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glMultMatrixd(trball.rMat);

    // 층밀림 행렬 직접 입력하기
    /*
    GLfloat m[16];
    for (int i = 0; i < 16; i++) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
    m[4] = 1.0f / tanf(60 / 180.f * PI);
    glMultMatrixf(m);
    */


    glPushMatrix(); // push the current matrix to the stack
    glTranslatef(0.0f, -1.0f, 0.0f); // post-multiply the current matrix by a translation matrix

    /*
    glColor3f(1.0f, 0.0f, 0.0f);

    //glutWireTetrahedron(); // tetrahedron

    glPopMatrix(); // pop the current matrix from the stack

    glTranslatef(0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glutWireCube(0.5); // cube
    */

    /*
    Quad(0, 3, 2, 1);
    Quad(1, 2, 6, 5);
    Quad(2, 3, 7, 6);
    Quad(3, 0, 4, 7);
    Quad(4, 5, 6, 7);
    Quad(5, 4, 0, 1);
    */

    BaseArm();
    LowerArm();
    UpperArm();
       
    SwapBuffers(MyDC);

    return;
}

void Quad(int a, int b, int c, int d)
{
    glBegin(GL_QUADS);
    glColor3fv(colors[a]);
    glVertex3fv(vertices[a]);
    glColor3fv(colors[b]);
    glVertex3fv(vertices[b]);
    glColor3fv(colors[c]);
    glVertex3fv(vertices[c]);
    glColor3fv(colors[d]);
    glVertex3fv(vertices[d]);
    glEnd();

    return;
}
/*
 * BaseArm : the base arm (radius : 0.5, height : 0.3)
 */
void BaseArm(void)
{
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(pObj, 0.5, 0.5, 0.3, 20, 1);
    glPopMatrix();

    return;
}
/*
 * LowerArm : the lower arm (width : 0.2, height : 1.0, depth : 0.2)
 */
void LowerArm(void)
{
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.7f, 0.0f);
	glScalef(0.2f, 1.0f, 0.2f);
    glutWireCube(1.0);
	glPopMatrix();

	return;
}
/*
 * UpperArm : the upper arm (width : 0.2, height : 0.8, depth : 0.2)
 */
void UpperArm(void)
{
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 1.6f, 0.0f);
	glScalef(0.2f, 0.8f, 0.2f);
	glutWireCube(1.0);
	glPopMatrix();

	return;
}