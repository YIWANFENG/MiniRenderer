#ifndef _RENDER_ENGINE_H_
#define _RENDER_ENGINE_H_
#include <iostream>
#include <opencv2\opencv.hpp>
#include <string>

using namespace std;

typedef unsigned char BYTE;
typedef unsigned int UINT32;


class Mat;	//前置声明
class Vector4D;
	
// 计算插值：t 为 [0, 1] 之间的数值
#define INTERP(x1,x2,t) ((x1) + ((x2) - (x1))*(t) )
inline int CMID(int x, int min, int max) { return (x < min)? min : ((x > max)? max : x); }


class Vector4D {
public:
	union {
		float M[4];
		struct{
			float x,y,z,w;
		};
	};
	Vector4D(float x, float y, float z, float w);
	Vector4D();
	float Length();
	void Add(Vector4D &op, Vector4D &re);

	void Sub(Vector4D &op, Vector4D &re);
	float Mul_Dot(Vector4D &op);
	void Mul_Cross(Vector4D &op, Vector4D &re);
	void Mul_float(float s, Vector4D &re);
	void Normalize();
	void show();
	
};

typedef Vector4D Point4D;

class Mat {
public:
	float m[4][4];
	void Add(Mat &op, Mat &re);
	void Sub(Mat &op, Mat &re);
	void Mul(Mat &op, Mat &re);
	void Mul_Vec(Vector4D &op, Vector4D &re);
	void Scale(float f);
	
	void Set_Identity();
	void Set_Zero();
	void Set_As_Translate(float x, float y, float z);
	void Set_As_Scale(float x, float y, float z);
	void Set_As_Rotate(float x, float y, float z, float theta);
	void show();
};

class Camera{
public:
	Vector4D pos;
	Vector4D look_at;
	Vector4D up;
	Mat view;
	
	Camera();
	void Set_Camera(Vector4D &eye_pos, Vector4D &eye_look_at, Vector4D &eye_up);
	float plane_camera_cos(Vector4D center, Vector4D normal);
};

class Color { 
public:
	//用于方便调整颜色值
	union {
		BYTE argb[4]; //bgra 小端存储
		UINT32 uint32;
	};
	void Set(UINT32 x, float s);
	void Mul(Color a, Color &re);
};

class Vertex{
public:
	Vector4D Normal;			//法向量
	Vector4D Coordinates;		//投影后的坐标
	Vector4D WorldCoordinates;	//原世界坐标
	float u, v;					//纹理坐标

};

typedef struct { int v1, v2, v3; } Face;

class Texture {
public:
	cv::Mat buf;
	int width;
	int height;
	string name;
	void Load(const char *filename);
	Color Map(float tu, float tv);

};

class Mesh{
public:
	string name;
	Vertex *vertices;
	Face  *faces;
	Vector4D Position;
	Vector4D Rotation;	//x,y,z确定旋转轴，w为旋转角度
	int vertex_count, face_count;

	//获得平面的法向量
	void get_Face_normal(int i, Vector4D &normal);

	Mesh(int count = 0, int face_count = 0);
	~Mesh();
	
};

class Transform{
public:
	Mat world;		// 世界坐标变换
	Mat view;		// 摄影机坐标变换
	Mat projection;	// 投影变换
	Mat transform;	// transform = projection * view * world
	float w,h;		// 屏幕大小

	void Update();
	void Init(int width, int height);
	void Apply(Vector4D &op, Vector4D &re);
	void Apply(Vertex &op, Vertex &re);
	//int Check_CVV(Vector4D &v)
	void Homogenize(Vector4D &op, Vector4D &re);
	void Set_Perspective(float fovy, float aspect, float zn, float zf);
	
};

class Illumination {
public:
	Vector4D light_source;
	float plane_light_cos(Vector4D &vertex, Vector4D &normal);
};

class ScanLineData {
public:		//扫描线相关属性，主要为了实现插值计算
	 int currentY;
	 float ndotla;
	 float ndotlb;
	 float ndotlc;
	 float ndotld;
	 float ua, ub, uc, ud;
	 float va, vb, vc, vd;
};

class Device {
public:
	Transform transform;		// 变换矩阵
	int width;                  // 窗口宽度
	int height;                 // 窗口高度
	UINT32 **framebuffer;       // 像素缓存：framebuffer[y] 代表第 y行
	float **zbuffer;            // 深度缓存：zbuffer[y] 为第 y行指针;
	UINT32 background;         // 背景颜色
	Illumination light_s;		//光源
	Texture texture_d;			//纹理对象
	Camera camera_my;			//观察
	Device(int w, int h, void *fb);
	~Device();

	void Clear(int mode);
	bool BackfaceCulling(Vertex pa_v, Vertex pb_v, Vertex pc_v);

	void PutPixel(int x, int y, UINT32 &color);
	void PutPixel(int x, int y, float z, UINT32 &color);

	void DrawLine(int xs, int ys, int xe, int ye, UINT32 color);
	void DrawLine(int xs, int ys, int xe, int ye, int zs, int ze, UINT32 color);

	void ProcessScanLine(int curY, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color);
	void ProcessScanLine(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color);
	void ProcessScanLineGouraud(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color);
	void ProcessScanLineGouraudTexture(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc,
		Vector4D &pd, UINT32& color, Texture &tex);

	void DrawTriangleFrame(Vector4D re2, Vector4D re3, Vector4D re4, UINT32 color_point);
	void DrawTriangle(Vector4D &pa, Vector4D &pb, Vector4D &pc, UINT32 color);
	void DrawTriangleFlat(Vertex pa_v, Vertex pb_v, Vertex pc_v, UINT32 color);
	void DrawTriangleGouraud(Vertex pa_v, Vertex pb_v, Vertex pc_v, UINT32 color);
	void DrawTriangleGouraudTexture(Vertex pa_v, Vertex pb_v, Vertex pc_v, UINT32 color);

	void Render(Mesh &mesh,int op = 0);
};
#endif 
//_RENDER_ENGINE_H_ 
