#include "render.h"
#include <iostream>
#include <math.h>
#include "ParseJson.h"
#include <windows.h>
#include <tchar.h>
using namespace std;




//=====================================================================
// Win32 窗口及图形绘制：为 device 提供一个 DibSection 的 FB
//=====================================================================
int screen_w, screen_h, screen_exit = 0;
int screen_mx = 0, screen_my = 0, screen_mb = 0;
int screen_keys[512];					// 当前键盘按下状态
static HWND screen_handle = NULL;		// 主窗口 HWND
static HDC screen_dc = NULL;			// 配套的 HDC
static HBITMAP screen_hb = NULL;		// DIB
static HBITMAP screen_ob = NULL;		// 老的 BITMAP
unsigned char *screen_fb = NULL;		// frame buffer
long screen_pitch = 0;

int screen_init(int w, int h, const TCHAR *title);	// 屏幕初始化
int screen_close(void);								// 关闭屏幕
void screen_dispatch(void);							// 处理消息
void screen_update(void);							// 显示 FrameBuffer

// win32 event handler
static LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);	

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#endif

// 初始化窗口并设置标题
int screen_init(int w, int h, const TCHAR *title) {
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0, 
		NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB, 
		w * h * 4, 0, 0, 0, 0 }  };
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;

	screen_close();

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc)) return -1;

	screen_handle = CreateWindow(_T("SCREEN3.1415926"), title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	if (screen_handle == NULL) return -2;

	screen_exit = 0;
	hDC = GetDC(screen_handle);
	screen_dc = CreateCompatibleDC(hDC);
	ReleaseDC(screen_handle, hDC);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if (screen_hb == NULL) return -3;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
	screen_fb = (unsigned char*)ptr;
	screen_w = w;
	screen_h = h;
	screen_pitch = w * 4;
	
	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(screen_handle);

	ShowWindow(screen_handle, SW_NORMAL);
	screen_dispatch();

	memset(screen_keys, 0, sizeof(int) * 512);
	memset(screen_fb, 0, w * h * 4);

	return 0;
}

int screen_close(void) {
	if (screen_dc) {
		if (screen_ob) { 
			SelectObject(screen_dc, screen_ob); 
			screen_ob = NULL; 
		}
		DeleteDC(screen_dc);
		screen_dc = NULL;
	}
	if (screen_hb) { 
		DeleteObject(screen_hb); 
		screen_hb = NULL; 
	}
	if (screen_handle) { 
		CloseWindow(screen_handle); 
		screen_handle = NULL; 
	}
	return 0;
}

static LRESULT screen_events(HWND hWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE: screen_exit = 1; break;
	case WM_KEYDOWN: screen_keys[wParam & 511] = 1; break;
	case WM_KEYUP: screen_keys[wParam & 511] = 0; break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void screen_dispatch(void) {
	MSG msg;
	while (1) {
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, NULL, 0, 0)) break;
		DispatchMessage(&msg);
	}
}

void screen_update(void) {
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
	screen_dispatch();
}



/*测试用*/
Point4D all_verticesJson[] = {
	{ -1, 1,  -1, 1 },
	{ -1, -1,  1, 1 },
	{ -1, -1, -1, 1 },
	{ -1,  1, 1, 1 },
	{ 1, -1, 1, 1 },
	{ 1, 1, 1, 1 },
	{ 1,  -1, -1, 1 },
	{ 1, 1, -1, 1 }
};
Face facesJson[] = {
	{0,1,2},{3,4,1},{5,6,4},{7,2,6},
	{4,2,1},{3,7,5},{0,3,1},{3,5,4},
	{5,7,6},{7,0,2},{4,6,2},{3,0,7}
};
void set_mesh_vertices_faces(Mesh &mesh, Point4D pd[],Face faces1[]) {
	//设置mesh的顶点坐标-
	for (int i = 0; i < mesh.vertex_count; i++) {
		mesh.vertices[i].WorldCoordinates = pd[i];
	}
	for (int i = 0; i < mesh.face_count; i++) {
		mesh.faces[i] = faces1[i];
	}
}
/*测试用*/


int main(int argc,char *argv[])
{

	//描述物体用mesh，这里定义一个立方体的八个顶点,测试用
	/*Mesh mesh(8, 12);
	set_mesh_vertices_faces(mesh, all_verticesJson, facesJson);*/

	Mesh mesh;
	//ReadJsonFromFile("4.babylon", mesh);
	ReadJsonFromFile("monkeytexture.babylon", mesh);
	
	//设置旋转矩阵
	//mesh.Rotation = {0,0, 0,0 };
	mesh.Rotation = { -1,-0.5,1,1 };



	TCHAR *title = _T("YWF Soft Render")
		_T("Left/Right: rotation, Up/Down: forward/backward, Space: switch state");
	if (screen_init(800, 600, title))
		return -1;
	Device device_my(800, 600, screen_fb);

	//初始化摄像机 （观察方式）
	Vector4D eye = { 3.0, 0, 0, 1 }, look_at = { 0, 0, 0, 1 }, up = { 0, 0, 1, 1 };
	device_my.camera_my.Set_Camera(eye, look_at, up);

	//初始化模型的初始变换矩阵
	//设置世界坐标系到相机坐标系的转换
	device_my.transform.view = device_my.camera_my.view;
	
	float aspect = float(800) / ((float)600);   //W/H
	float fovy = 3.1415926f * 0.5f;
	//设置应用于相机坐标系下的透视矩阵
	device_my.transform.Set_Perspective(fovy, aspect, 1.0f, 500.0f);
	//更新综合变换矩阵
	device_my.transform.Update();								

	//加载纹理
	device_my.texture_d.Load("Suzanne.jpg");
	device_my.Clear(0);		//设置背景色

	int op = 0;
	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) {
		screen_dispatch();
		device_my.Clear(0);		//设置背景色
		//渲染一帧
		device_my.Render(mesh,op);					
		//设置旋转
		if (screen_keys[VK_LEFT]) {
			mesh.Rotation.w += 0.01f;
			//at.y += 0.01f;
		}
		if (screen_keys[VK_RIGHT]) {
			mesh.Rotation.w -= 0.01f;
			//at.y -= 0.01f;

		}
		if (screen_keys[VK_SPACE]) {
			op++;
			op %= 5;
		}
		//device_my.camera_my.Set_Camera(eye, at, up);
		//device_my.transform.view = device_my.camera_my.view;
		screen_update();
		Sleep(1);
		//cout << "YES" << endl;
	}
	
	return 0;
}
