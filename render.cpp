#include "render.h"
#include <math.h>
#include <iostream>
#include <assert.h>
#include <opencv2\opencv.hpp>


//=====================================================================
// 数学库：此部分应该不用详解，熟悉线性代数矩阵变换即可
// 对应游戏引擎中的数学引擎，这里主要为Vector4D 与 Mat的定义与相关操作
//=====================================================================

///////Vector4D/////////////////////////////////////////////
Vector4D::Vector4D(float x, float y, float z, float w) {
	this->x = x; this->y = y;
	this->z = z; this->w = w;
}
Vector4D::Vector4D() {
	this->x = this->y = this->z = 0.0f;
	this->w = 1.0f;
}
float Vector4D::Length() {
	float sq = x*x + y*y + z*z;
	return (float)sqrt(sq);
}
void Vector4D::Add(Vector4D &op, Vector4D &re) {
	re.x = x + op.x;
	re.y = y + op.y;
	re.z = z + op.z;
	re.w = 1.0;
}

void Vector4D::Sub(Vector4D &op, Vector4D &re) {
	re.x = x - op.x;
	re.y = y - op.y;
	re.z = z - op.z;
	re.w = 1.0;
}
float Vector4D::Mul_Dot(Vector4D &op) {
	return x*op.x + y*op.y + z*op.z;
}
void Vector4D::Mul_Cross(Vector4D &op, Vector4D &re) {
	re.x = y*op.z - z*op.y;
	re.y = z*op.x - x*op.z;
	re.z = x*op.y - y*op.x;
	re.w = 1.0f;
}
void Vector4D::Mul_float(float s, Vector4D &re) {
	re.x = x*s;
	re.y = y*s;
	re.z = z*s;
	re.w = 1.0f;
}
void Vector4D::Normalize() {
	float length = Length();
	if (length != 0.0f) {
		float inv = 1.0f / length;
		x *= inv;
		y *= inv;
		z *= inv;
	}
	w = 1.0;
}
void Vector4D::show() {
	cout << M[0] << '\t' << M[1] << '\t' << M[2] << '\t' << M[3] << endl;;
}


///////Mat/////////////////////////////////////////////
void Mat::Add(Mat &op, Mat &re) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			re.m[i][j] = m[i][j] + op.m[i][j];
	}
}
void Mat::Sub(Mat &op, Mat &re) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			re.m[i][j] = m[i][j] - op.m[i][j];
	}
}
void Mat::Mul(Mat &op, Mat &re) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			re.m[i][j] = (m[i][0] * op.m[0][j]) +
				(m[i][1] * op.m[1][j]) +
				(m[i][2] * op.m[2][j]) +
				(m[i][3] * op.m[3][j]);
		}
	}
}
void Mat::Mul_Vec(Vector4D &op, Vector4D &re) {
	re.x = op.x * m[0][0] + op.y * m[0][1] + op.z * m[0][2] + op.w * m[0][3];
	re.y = op.x * m[1][0] + op.y * m[1][1] + op.z * m[1][2] + op.w * m[1][3];
	re.z = op.x * m[2][0] + op.y * m[2][1] + op.z * m[2][2] + op.w * m[2][3];
	re.w = op.x * m[3][0] + op.y * m[3][1] + op.z * m[3][2] + op.w * m[3][3];
}
void Mat::Scale(float f) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			m[i][j] = m[i][j] * f;
	}
}
void Mat::Set_Identity() {
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
	m[0][1] = m[0][2] = m[0][3] = 0.0f;
	m[1][0] = m[1][2] = m[1][3] = 0.0f;
	m[2][0] = m[2][1] = m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = 0.0f;
}
void Mat::Set_Zero() {
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 0.0f;
	m[0][1] = m[0][2] = m[0][3] = 0.0f;
	m[1][0] = m[1][2] = m[1][3] = 0.0f;
	m[2][0] = m[2][1] = m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = 0.0f;
}
void Mat::Set_As_Translate(float x, float y, float z) {
	//设置为平移矩阵
	Set_Identity();
	/*m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;*/
	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;
}
void Mat::Set_As_Scale(float x, float y, float z) {
	//设置为缩放矩阵
	Set_Identity();
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;

}
void Mat::Set_As_Rotate(float x, float y, float z, float theta) {
	//设置为旋转矩阵
	//theta是弧度值
	float qsin = (float)sin(theta);
	float qcos = (float)cos(theta);
	float one_qcos = 1 - qcos;
	Vector4D vi(x, y, z, 1);
	vi.Normalize();
	float X = vi.x, Y = vi.y, Z = vi.z;
	/*m[0][0] = qcos + X*X*one_qcos;
	m[1][0] = X*Y*one_qcos - Z*qsin;
	m[2][0] = X*Z*one_qcos + Y*qsin;
	m[3][0] = 0.0f;
	m[0][1] = Y*X*one_qcos + Z*qsin;
	m[1][1] = qcos + Y*Y*one_qcos;
	m[2][1] = Y*Z*one_qcos - X*qsin;
	m[3][1] = 0.0f;
	m[0][2] = Z*X*one_qcos - Y*qsin;
	m[1][2] = Z*Y*one_qcos + X*qsin;
	m[2][2] = qcos + Z*Z*one_qcos;
	m[3][2] = 0.0f;
	m[0][3] = 0;
	m[1][3] = 0;
	m[2][3] = 0;
	m[3][3] = 1.0f;*/
	m[0][0] = qcos + X*X*one_qcos;
	m[0][1] = X*Y*one_qcos - Z*qsin;
	m[0][2] = X*Z*one_qcos + Y*qsin;
	m[0][3] = 0.0f;
	m[1][0] = Y*X*one_qcos + Z*qsin;
	m[1][1] = qcos + Y*Y*one_qcos;
	m[1][2] = Y*Z*one_qcos - X*qsin;
	m[1][3] = 0.0f;
	m[2][0] = Z*X*one_qcos - Y*qsin;
	m[2][1] = Z*Y*one_qcos + X*qsin;
	m[2][2] = qcos + Z*Z*one_qcos;
	m[2][3] = 0.0f;
	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1.0f;
}
void Mat::show() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			cout << m[i][j] << '\t';
		}
		cout << endl;
	}

}




//=====================================================================
// 相关数据结构定义
//=====================================================================
///////Camera/////////////////////////////////////////////
Camera::Camera() {
	view.Set_Identity();
}
void Camera::Set_Camera(Vector4D &eye_pos, Vector4D &eye_look_at, Vector4D &eye_up) {
	//由eye_pos 相机在世界坐标系位置，look_at 观察场景的中心在世界坐标系位置
	//eye_up,相机的正上方方向在世界坐标系中的表示 
	//设置由世界坐标系到观察坐标系的转换矩阵
	pos = eye_pos;
	look_at = eye_look_at;
	up = eye_up;
	Vector4D x_axis, y_axis, z_axis;	//U V N
	pos.Sub(look_at, z_axis);//Z = pos - look_at 
							 //pos.Sub(look_at, z_axis);
	z_axis.Normalize();
	up.Mul_Cross(z_axis, x_axis);
	x_axis.Normalize();
	z_axis.Mul_Cross(x_axis, y_axis);
	y_axis.Normalize();
	//x_axis.show();
	view.m[0][0] = x_axis.x;
	view.m[0][1] = x_axis.y;
	view.m[0][2] = x_axis.z;
	view.m[0][3] = -x_axis.Mul_Dot(pos);

	view.m[1][0] = y_axis.x;
	view.m[1][1] = y_axis.y;
	view.m[1][2] = y_axis.z;
	view.m[1][3] = -y_axis.Mul_Dot(pos);

	view.m[2][0] = z_axis.x;
	view.m[2][1] = z_axis.y;
	view.m[2][2] = z_axis.z;
	view.m[2][3] = -z_axis.Mul_Dot(pos);

	view.m[3][0] = view.m[3][1] = view.m[3][2] = 0;
	view.m[3][3] = 1.0;
	/*
	pos = eye_pos;
	look_at = eye_look_at;
	up = eye_up;
	Vector4D x_axis, y_axis, z_axis;	//U V N
	pos.Sub(look_at, z_axis);//Z = look_at - pos 
							 //pos.Sub(look_at, z_axis);
	z_axis.Normalize();
	up.Mul_Cross(z_axis, x_axis);
	x_axis.Normalize();
	z_axis.Mul_Cross(x_axis, y_axis);
	y_axis.Normalize();
	//x_axis.show();
	view.m[0][0] = x_axis.x;	//U
	view.m[1][0] = x_axis.y;
	view.m[2][0] = x_axis.z;
	view.m[3][0] = -x_axis.Mul_Dot(pos);

	view.m[0][1] = y_axis.x;	//V
	view.m[1][1] = y_axis.y;
	view.m[2][1] = y_axis.z;
	view.m[3][1] = -y_axis.Mul_Dot(pos);

	view.m[0][2] = z_axis.x;	//N
	view.m[1][2] = z_axis.y;
	view.m[2][2] = z_axis.z;
	view.m[3][2] = -z_axis.Mul_Dot(pos);

	view.m[0][3] = view.m[1][3] = view.m[2][3] = 0;
	view.m[3][3] = 1.0;*/
}
float Camera::plane_camera_cos(Vector4D center, Vector4D normal)
{
	//依据法线计算与视线的夹角
	Vector4D eye_dir;
	center.Sub(pos, eye_dir);			//获取光线向量
	eye_dir.Normalize();
	normal.Normalize();
	return  normal.Mul_Dot(eye_dir);
}


void Color::Set(UINT32 x, float s) {
	//依据S来调整X所对应的颜色值
	uint32 = x;
	argb[0] *= s;
	argb[1] *= s;
	argb[2] *= s;
}
void Color::Mul(Color a, Color &re) {
	re.argb[0] = argb[0] * a.argb[0];
	re.argb[1] = argb[1] * a.argb[1];
	re.argb[2] = argb[2] * a.argb[2];
}


void Texture::Load(const char *filename) {
	buf = cv::imread(filename);
	width = buf.size().width;
	height = buf.size().height;
	if (buf.empty()) cout << "Load Texture Error" << endl;
	//cv::imshow("buf", buf);
}
Color Texture::Map(float tu, float tv) {
	Color re;
	re.Set(0x00000000, 1.0f);			//默认为黑
	if (buf.empty()) return re;
	int u = (int)(tu*width) % width;	//%为防止复用
	int v = (int)(tv*height) % height;
	if (u<0 || v<0)
		cout << tu << ' ' << tv << endl;
	u = u >= 0 ? u : -u;
	v = v >= 0 ? v : -v;

	cv::Vec3b tex_w = buf.at<cv::Vec3b>(v, u);	//U是X,V是Y
	re.argb[3] = 0x00;
	re.argb[0] = tex_w[0];	//B
	re.argb[1] = tex_w[1];	//G
	re.argb[2] = tex_w[2];	//R
	return re;
}



Mesh::Mesh(int count , int face_count) {
	if (!count) return;
	this->vertex_count = count;
	this->face_count = face_count;
	faces = NULL;
	vertices = NULL;
	if (count) vertices = new Vertex[count];
	if (face_count) faces = new Face[face_count];
}
Mesh::~Mesh() {
	/*if(vertices)
	delete [] vertices;
	if (faces)
	delete[] faces;*/
	vertices = NULL;
	faces = NULL;

}
void Mesh::get_Face_normal(int i, Vector4D &normal) {
	Vector4D edge1, edge2;
	vertices[faces[i].v3].WorldCoordinates.Sub(vertices[faces[i].v1].WorldCoordinates, edge1);
	vertices[faces[i].v2].WorldCoordinates.Sub(vertices[faces[i].v1].WorldCoordinates, edge1);
	edge2.Mul_Cross(edge1, normal);
}


//=====================================================================
// 坐标变换,物体的模型坐标系变换到世界坐标再经过相机变换，然后再透视变换
//=====================================================================

void Transform::Update() {
	// 矩阵更新，计算 transform =   projection * view * world
	static Mat m;
	m.Set_Identity();
	view.Mul(world, m);
	projection.Mul(m, transform);
}
void Transform::Init(int width, int height) {
	// 初始化，设置屏幕长宽

	world.Set_Identity();
	view.Set_Identity();
	w = (float)width;
	h = (float)height;
}
void Transform::Apply(Vector4D &op, Vector4D &re) {
	//old  将矢量 op 进行 project 
	//此操作后，re里的x,y即为在屏幕中显示的位置，z留作深度测试
	transform.Mul_Vec(op, re);

}
void Transform::Apply(Vertex &op, Vertex &re) {
	// new 将顶点 op 进行 project，坐标结果保存在re
	//此操作后，re里的x,y即为在屏幕中显示的位置，z留作深度测试
	//顶点法线normal同样
	transform.Mul_Vec(op.WorldCoordinates, re.Coordinates);		//完整投影变换，用于显示
	world.Mul_Vec(op.WorldCoordinates, re.WorldCoordinates);	//对于顶点仅世界坐标变换，用于光照
	world.Mul_Vec(op.Normal, re.Normal);							//对于法线仅世界坐标变换，用于光照
	re.u = op.u;												//纹理坐标不变
	re.v = op.v;
}


void Transform::Homogenize(Vector4D &op, Vector4D &re) {
	// 归一化，得到屏幕坐标
	float rhw = 1.0f / op.w;
	re.x = (op.x * rhw + 1.0f) * w * 0.5f;
	re.y = (1.0f - op.y * rhw) * h * 0.5f;
	re.z = op.z * rhw;
	re.w = 1.0f;
}

void Transform::Set_Perspective(float fovy, float aspect, float zn, float zf) {
	//设置透视矩阵,相当于D3DXMatrixPerspectiveFovLH
	//fovy = view frustum 与Z轴夹角 弧度制
	//aspect 投影面宽长比（显示区宽长比）
	//zn 相机到近裁剪平面距离，zf 相机到远裁剪平面距离

	float fax = 1.0f / (float)tan(fovy * 0.5f);
	projection.Set_Zero();
	projection.m[0][0] = (float)(fax / aspect);
	projection.m[1][1] = (float)(fax);
	projection.m[2][2] = zf / (zf - zn);
	projection.m[2][3] = zn * zf / (zn - zf);
	projection.m[3][2] = 1;
	/*projection.m[3][2] = zn * zf / (zn - zf);
	projection.m[2][3] = 1;*/
}

float Illumination::plane_light_cos(Vector4D &vertex, Vector4D &normal) {
	//依据法线计算与光线向量的夹角，结果归一化至 0-1，依次来设置颜色亮度
	Vector4D light_dir;
	light_source.Sub(vertex, light_dir);			//获取光线向量
	light_dir.Normalize();
	normal.Normalize();
	float angle_cos = normal.Mul_Dot(light_dir);
	//if (angle_cos < 0) cout << angle_cos << endl;
	angle_cos = angle_cos < 0 ? 0 : angle_cos;		//夹角大于90，处于背光面
	angle_cos = angle_cos > 1 ? 1 : angle_cos;		//防止由于浮点运算而越界
	return angle_cos;

}

//=====================================================================
// 渲染设备抽象
//=====================================================================
///////main function definition/////////////////////////////////////////
Device::Device(int w, int h, void *fb) {
	width = w;
	height = h;
	transform.Init(w, h);
	//以下无需查看
	// 设备初始化，fb为外部帧缓存，非 NULL 将引用外部帧缓存（每行 4字节对齐）
	int need = sizeof(void*) * (height * 2 + 1024) + width * height * 8;
	char *ptr = (char*)malloc(need + 64);
	char *framebuf, *zbuf;
	int j;
	assert(ptr != NULL);
	framebuffer = (UINT32**)ptr;
	zbuffer = (float**)(ptr + sizeof(void*) * height);
	ptr += sizeof(void*) * height * 2;
	//device->texture = (IUINT32**)ptr;
	ptr += sizeof(void*) * 1024;
	framebuf = (char*)ptr;
	zbuf = (char*)ptr + width * height * 4;
	ptr += width * height * 8;
	if (fb != NULL) framebuf = (char*)fb;
	for (j = 0; j < height; j++) {
		framebuffer[j] = (UINT32*)(framebuf + width * 4 * j);
		zbuffer[j] = (float*)(zbuf + width * 4 * j);
	}
	//ZBuffer = new float[800 * 600];
	background = 0;	//表明背景色为0黑色
}
Device::~Device() {
	if (framebuffer)
		free(framebuffer);
	framebuffer = NULL;
	zbuffer = NULL;
	//delete[] ZBuffer;
}

void Device::Clear(int mode) {
	// 清空 framebuffer 和 zbuffer
	//无需查看
	int y, x, height = this->height;
	for (y = 0; y < height; y++) {
		UINT32 *dst = framebuffer[y];
		UINT32 cc = (height - 1 - y) * 230 / (height - 1);
		cc = (cc << 16) | (cc << 8) | cc;
		if (mode == 0) cc = background;	//mode == 0,则，设为默认背景色
		for (x = width; x > 0; dst++, x--) dst[0] = cc;
	}
	//将深度内存全置为一个足够大的默认值
	for (y = 0; y < height; y++) {
		float *dst = zbuffer[y];
		//memset((void*)(dst), 0, width * sizeof(float));
		for (x = width; x > 0; dst++, x--) dst[0] = 65535.0f;
	}
	//memset((void*)ZBuffer, 0, 800*600*sizeof(float));
}
bool Device::BackfaceCulling(Vertex pa_v, Vertex pb_v, Vertex pc_v)
{
	//判断pa_v,pb_v,pc_v所组成的平面是否可见
	//可见返回true;

	//计算三角形面的法向量 == 三个顶点法向量的平均值
	Vector4D normal;
	pa_v.Normal.Add(pb_v.Normal, normal);
	normal.Add(pc_v.Normal, normal);
	normal.Mul_float(0.3333f, normal);
	//计算三顶点的中心位置（所代表面的中心位置）
	Vector4D center_point;
	pa_v.WorldCoordinates.Add(pb_v.WorldCoordinates, center_point);
	center_point.Add(pc_v.WorldCoordinates, center_point);
	center_point.Mul_float(0.3333f, center_point);
	if (camera_my.plane_camera_cos(center_point, normal) < 0) return true;
	else return false;
}

void Device::PutPixel(int x, int y, UINT32 &color) {
	if (((UINT32)x) < (UINT32)width && ((UINT32)y) < (UINT32)height) {
		framebuffer[y][x] = color;
	}
}
void Device::PutPixel(int x, int y, float z, UINT32 &color) {
	//含有深度测试的像素填充函数，z用作深度测试
	if (((UINT32)x) < (UINT32)width && ((UINT32)y) < (UINT32)height) {
		if (this->camera_my.pos.z - z >= zbuffer[y][x]) return;
		zbuffer[y][x] = this->camera_my.pos.z - z;
		framebuffer[y][x] = color;
	}
}

void Device::DrawLine(int xs, int ys, int xe, int ye, UINT32 color) {
	// Bresenham line algorithm
	int dx = abs(xe - xs);
	int dy = abs(ye - ys);
	int sx = (xs < xe) ? 1 : -1;
	int sy = (ys < ye) ? 1 : -1;
	int err = dx - dy;

	while (true) {
		PutPixel(xs, ys, color);

		if ((xs == xe) && (ys == ye)) break;
		int e2 = 2 * err;
		if (e2 > -dy) { err -= dy; xs += sx; }
		if (e2 < dx) { err += dx; ys += sy; }
	}
}
void Device::DrawLine(int xs, int ys, int xe, int ye, int zs, int ze, UINT32 color) {
	// Bresenham line algorithm
	// 含有简单Z插值处理
	int dx = abs(xe - xs);
	int dy = abs(ye - ys);
	int sx = (xs < xe) ? 1 : -1;
	int sy = (ys < ye) ? 1 : -1;
	int err = dx - dy;
	int ys_0 = ys, ye_0 = ye, xs_0 = xs, xe_0 = xe;

	while (true) {
		float gradient = 0;
		if ((ye_0 - ys_0)) gradient = (ys - ys_0) / (float)(ye_0 - ys_0);	//先以y
		else if (xe_0 - xs_0) gradient = (xs - xs_0) / (float)(xe_0 - xs_0);//否则x
		float z = zs + (ze - zs) * gradient;
		PutPixel(xs, ys, z, color);
		if ((xs == xe) && (ys == ye)) break;
		int e2 = 2 * err;
		if (e2 > -dy) { err -= dy; xs += sx; }
		if (e2 < dx) { err += dx; ys += sy; }
	}
}

void Device::ProcessScanLine(int curY, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color) {
	//old
	//pa,pb,pc,pd四点组成三角形，
	//借助三角形相似由简单线性插值来确定扫描线
	//默认Vector4D.w = 1.0


	//下面的梯度因子完成了归一化，也就是可以适用于平顶或者是平底三角形。
	float gradient_s = pa.y != pb.y ? (curY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (curY - pc.y) / (pd.y - pc.y) : 1;

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//求解深度平面方程
	float A = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
	float B = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
	float C = (pb.x - pa.x)*(pc.y - pa.y) - (pc.x - pa.x)*(pb.y - pa.y);
	float D = -(A * pa.x + B * pa.y + C * pa.z);
	float C_inv = 1 / C;

	//画出扫描线
	for (int x = sx; x <= ex; x++) {
		float z = (-D - A*x - B*curY)*C_inv;	//计算该点深度
		PutPixel(x, curY, z, color);
	}
}
void Device::ProcessScanLine(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color) {
	//pa,pb,pc,pd四点组成三角形，
	//借助三角形相似由简单线性插值来确定扫描线
	//默认Vector4D.w = 1.0
	//已经添加平面着色


	//下面的梯度因子完成了归一化，也就是可以适用于平顶或者是平底三角形。
	float gradient_s = pa.y != pb.y ? (scld.currentY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (scld.currentY - pc.y) / (pd.y - pc.y) : 1;

	//扫描线横坐标线性插值

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//每点的深度值求解两种方法
	//1.深度线性插值
	float z1 = INTERP(pa.z, pb.z, gradient_s);
	float z2 = INTERP(pc.z, pd.z, gradient_e);

	//or 2求解深度平面方程
	//float A = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
	//float B = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
	//float C = (pb.x - pa.x)*(pc.y - pa.y) - (pc.x - pa.x)*(pb.y - pa.y);
	//float D = - (A * pa.x + B * pa.y + C * pa.z);
	//float C_inv = 1 / C;

	Color color_s;
	//画出扫描线
	for (int x = sx; x <= ex; x++) {

		float gradient = (x - sx) / (float)(ex - sx);	//1.插值该点深度
		float z = INTERP(z1, z2, gradient);

		//float z = (-D - A*x - B*scld.currentY)*C_inv;	//2.计算该点深度

		// 基于光向量和法线向量之间角度的余弦改变颜色值  
		color_s.Set(color, scld.ndotla);
		PutPixel(x, scld.currentY, z, color_s.uint32);
	}
}
void Device::ProcessScanLineGouraud(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color) {
	//pa,pb,pc,pd四点组成三角形，
	//借助三角形相似由简单线性插值来确定扫描线
	//默认Vector4D.w = 1.0
	//已经添加高氏着色


	//下面的梯度因子完成了归一化，也就是可以适用于平顶或者是平底三角形。
	float gradient_s = pa.y != pb.y ? (scld.currentY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (scld.currentY - pc.y) / (pd.y - pc.y) : 1;

	//扫描线横坐标线性插值

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//每点的深度值求解两种方法
	//1.深度线性插值
	float z1 = INTERP(pa.z, pb.z, gradient_s);
	float z2 = INTERP(pc.z, pd.z, gradient_e);

	//or 2求解深度平面方程
	//float A = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
	//float B = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
	//float C = (pb.x - pa.x)*(pc.y - pa.y) - (pc.x - pa.x)*(pb.y - pa.y);
	//float D = - (A * pa.x + B * pa.y + C * pa.z);
	//float C_inv = 1 / C;

	//高氏着色-渐变,插值
	float snl = INTERP(scld.ndotla, scld.ndotlb, gradient_s);
	float enl = INTERP(scld.ndotlc, scld.ndotld, gradient_e);;

	Color color_s;
	//画出扫描线
	for (int x = sx; x <= ex; x++) {

		float gradient = (x - sx) / (float)(ex - sx);
		float z = INTERP(z1, z2, gradient);				//1.插值该点深度
		float ndot = INTERP(snl, enl, gradient);		//插值该点的"光照强度"

														//float z = (-D - A*x - B*scld.currentY)*C_inv;	//2.计算该点深度

														// 基于光向量和法线向量之间角度的余弦改变颜色值  
		color_s.Set(color, ndot);
		PutPixel(x, scld.currentY, z, color_s.uint32);
	}
}
void Device::ProcessScanLineGouraudTexture(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc,
	Vector4D &pd, UINT32& color, Texture &tex) {
	//pa,pb,pc,pd四点组成三角形，
	//借助三角形相似由简单线性插值来确定扫描线
	//默认Vector4D.w = 1.0
	//已经添加高氏着色与纹理映射


	//下面的梯度因子完成了归一化，也就是可以适用于平顶或者是平底三角形。
	float gradient_s = pa.y != pb.y ? (scld.currentY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (scld.currentY - pc.y) / (pd.y - pc.y) : 1;

	//扫描线横坐标线性插值

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//每点的深度值求解两种方法
	//1.深度线性插值
	float z1 = INTERP(pa.z, pb.z, gradient_s);
	float z2 = INTERP(pc.z, pd.z, gradient_e);

	//高氏着色-渐变,插值
	float snl = INTERP(scld.ndotla, scld.ndotlb, gradient_s);
	float enl = INTERP(scld.ndotlc, scld.ndotld, gradient_e);;

	// 将纹理坐标插值到Y中  
	float su = INTERP(scld.ua, scld.ub, gradient_s);
	float eu = INTERP(scld.uc, scld.ud, gradient_e);
	float sv = INTERP(scld.va, scld.vb, gradient_s);
	float ev = INTERP(scld.vc, scld.vd, gradient_e);

	Color color_s, color_tex;
	//画出扫描线
	for (int x = sx; x <= ex; x++) {

		float gradient = (x - sx) / (float)(ex - sx);
		float z = INTERP(z1, z2, gradient);				//插值该点深度
		float ndot = INTERP(snl, enl, gradient);		//插值该点的"光照强度"
		float u = INTERP(su, eu, gradient);				//纹理映射插值
		float v = INTERP(sv, ev, gradient);
		if (!tex.buf.empty()) color_tex = tex.Map(u, v);		//取纹理
		else color_tex.Set(color, 1.0f);
		// 基于光向量和法线向量之间角度的余弦以及纹理颜色
		color_s.Set(color_tex.uint32, ndot);
		//color_s.Mul(color_tex, color_s);
		PutPixel(x, scld.currentY, z, color_s.uint32);
	}
}

void Device::DrawTriangleFrame(Vector4D re2, Vector4D re3, Vector4D re4, UINT32 color_point) {
	DrawLine(re2.x, re2.y, re3.x, re3.y, re2.z, re3.z, color_point);
	DrawLine(re2.x, re2.y, re4.x, re4.y, re2.z, re4.z, color_point);
	DrawLine(re3.x, re3.y, re4.x, re4.y, re3.z, re4.z, color_point);
}
void Device::DrawTriangle(Vector4D &pa, Vector4D &pb, Vector4D &pc, UINT32 color) {
	//old 光栅化画三角形  未使用光照版
	//首先将a,b,c按照行从小到大排列,且当pb与某一点位于同一水平线上时，使b在左侧
	Vector4D tmp;
	if (pa.y > pb.y) {
		tmp = pa;
		pa = pb;
		pb = tmp;
	}

	if (pa.y > pc.y) {
		tmp = pa;
		pa = pc;
		pc = tmp;
	}
	if (pb.y > pc.y) {
		tmp = pb;
		pb = pc;
		pc = tmp;
	}
	//2种特殊
	//平顶
	if (pa.y == pb.y) {
		if (pa.x < pb.x) {
			tmp = pa;
			pa = pb;
			pb = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
			ProcessScanLine(row, pb, pc, pa, pc, color);
		return;
	}
	//平底
	if (pc.y == pb.y) {
		if (pc.x < pb.x) {
			tmp = pb;
			pb = pc;
			pc = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
			ProcessScanLine(row, pa, pb, pc, pa, color);
		return;
	}



	float dPaPb, dPaPc;
	if (pb.y - pa.y > 0)
		dPaPb = (pb.x - pa.x) / (pb.y - pa.y);
	else
		dPaPb = 0;

	if (pc.y - pa.y > 0)
		dPaPc = (pc.x - pa.x) / (pc.y - pa.y);
	else
		dPaPc = 0;
	// Pa
	// -  Pb
	// Pc
	if (dPaPb > dPaPc)
	{
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			if (row < pb.y)
			{
				ProcessScanLine(row, pa, pc, pb, pa, color);
			}
			else
			{
				ProcessScanLine(row, pa, pc, pb, pc, color);
			}
		}
	}
	//   Pa
	//Pb - 
	//	 Pc
	else
	{	//dPaPb <= dPaPc
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			if (row < pb.y)
			{
				ProcessScanLine(row, pa, pb, pc, pa, color);
			}
			else
			{
				ProcessScanLine(row, pb, pc, pa, pc, color);
			}
		}
	}
}
void Device::DrawTriangleFlat(Vertex pa_v, Vertex pb_v, Vertex pc_v, UINT32 color) {
	//光栅化画三角形 使用光照平面着色
	//首先将a,b,c按照行从小到大排列,且当pb与某一点位于同一水平线上时，使b在左侧
	Vector4D tmp, pa, pb, pc;
	Vertex tmp_v;
	if (pa_v.Coordinates.y > pb_v.Coordinates.y) {
		tmp_v = pa_v;
		pa_v = pb_v;
		pb_v = tmp_v;
	}

	if (pa_v.Coordinates.y > pc_v.Coordinates.y) {
		tmp_v = pa_v;
		pa_v = pc_v;
		pc_v = tmp_v;
	}
	if (pb_v.Coordinates.y > pc_v.Coordinates.y) {
		tmp_v = pb_v;
		pb_v = pc_v;
		pc_v = tmp_v;
	}
	pa = pa_v.Coordinates;
	pb = pb_v.Coordinates;
	pc = pc_v.Coordinates;

	//计算三角形面的法向量 == 三个顶点法向量的平均值
	Vector4D normal;
	pa_v.Normal.Add(pb_v.Normal, normal);
	normal.Add(pc_v.Normal, normal);
	normal.Mul_float(0.3333f, normal);
	//cout << normal.x << ' ' << normal.y << ' ' << normal.z << endl;
	//计算三顶点的中心位置（所代表面的中心位置）
	Vector4D center_point;
	pa_v.WorldCoordinates.Add(pb_v.WorldCoordinates, center_point);
	center_point.Add(pc_v.WorldCoordinates, center_point);
	center_point.Mul_float(0.3333f, center_point);
	//光照位置
	light_s.light_source.x = 0;
	light_s.light_source.y = 10;
	light_s.light_source.z = 0;
	float ndot = light_s.plane_light_cos(center_point, normal);
	//if(ndot!=0) cout << ndot << endl;
	ScanLineData scld;
	scld.ndotla = ndot;
	//scld.ndotla = 1.0;

	//2种特殊
	//平顶
	if (pa.y == pb.y) {
		if (pa.x < pb.x) {
			tmp = pa;
			pa = pb;
			pb = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++) {
			scld.currentY = row;
			ProcessScanLine(scld, pb, pc, pa, pc, color);	//这里的顶点排列是顺序相关的
		}
		return;
	}
	//平底
	if (pc.y == pb.y) {
		if (pc.x < pb.x) {
			tmp = pb;
			pb = pc;
			pc = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++) {
			scld.currentY = row;
			ProcessScanLine(scld, pa, pb, pc, pa, color);
		}
		return;
	}

	float dPaPb, dPaPc;
	if (pb.y - pa.y > 0)
		dPaPb = (pb.x - pa.x) / (pb.y - pa.y);
	else
		dPaPb = 0;

	if (pc.y - pa.y > 0)
		dPaPc = (pc.x - pa.x) / (pc.y - pa.y);
	else
		dPaPc = 0;
	// Pa
	// -  Pb
	// Pc
	if (dPaPb > dPaPc)
	{
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			if (row < pb.y)
			{
				scld.currentY = row;
				ProcessScanLine(scld, pa, pc, pb, pa, color);
			}
			else
			{
				scld.currentY = row;
				ProcessScanLine(scld, pa, pc, pb, pc, color);
			}
		}
	}
	//   Pa
	//Pb - 
	//	 Pc
	else
	{	//dPaPb <= dPaPc
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			if (row < pb.y)
			{
				scld.currentY = row;
				ProcessScanLine(scld, pa, pb, pc, pa, color);
			}
			else
			{
				scld.currentY = row;
				ProcessScanLine(scld, pb, pc, pa, pc, color);
			}
		}
	}
}
void Device::DrawTriangleGouraud(Vertex pa_v, Vertex pb_v, Vertex pc_v, UINT32 color) {
	//光栅化画三角形 使用光照高氏着色
	//首先将a,b,c按照行从小到大排列,且当pb与某一点位于同一水平线上时，使b在左侧
	Vector4D tmp, pa, pb, pc;
	Vertex tmp_v;

	if (pa_v.Coordinates.y > pb_v.Coordinates.y) {
		tmp_v = pa_v;
		pa_v = pb_v;
		pb_v = tmp_v;
	}

	if (pa_v.Coordinates.y > pc_v.Coordinates.y) {
		tmp_v = pa_v;
		pa_v = pc_v;
		pc_v = tmp_v;
	}
	if (pb_v.Coordinates.y > pc_v.Coordinates.y) {
		tmp_v = pb_v;
		pb_v = pc_v;
		pc_v = tmp_v;
	}
	pa = pa_v.Coordinates;
	pb = pb_v.Coordinates;
	pc = pc_v.Coordinates;

	//光照位置
	light_s.light_source.x = 0;
	light_s.light_source.y = 10;
	light_s.light_source.z = 0;
	//每顶点的法向量与光线向量的cos
	float ndot1 = light_s.plane_light_cos(pa_v.WorldCoordinates, pa_v.Normal);
	float ndot2 = light_s.plane_light_cos(pb_v.WorldCoordinates, pb_v.Normal);
	float ndot3 = light_s.plane_light_cos(pc_v.WorldCoordinates, pc_v.Normal);

	ScanLineData scld;

	//2种特殊
	//平顶
	if (pa.y == pb.y) {
		if (pa.x < pb.x) {
			tmp = pa;
			pa = pb;
			pb = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++) {
			scld.currentY = row;
			scld.ndotla = ndot2;
			scld.ndotlb = ndot3;
			scld.ndotlc = ndot1;
			scld.ndotld = ndot3;
			ProcessScanLineGouraud(scld, pb, pc, pa, pc, color);	//这里的顶点排列是顺序相关的
		}
		return;
	}
	//平底
	if (pc.y == pb.y) {
		if (pc.x < pb.x) {
			tmp = pb;
			pb = pc;
			pc = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++) {
			scld.currentY = row;
			scld.ndotla = ndot1;
			scld.ndotlb = ndot2;
			scld.ndotlc = ndot3;
			scld.ndotld = ndot1;
			ProcessScanLineGouraud(scld, pa, pb, pc, pa, color);
		}
		return;
	}



	float dPaPb, dPaPc;
	if (pb.y - pa.y > 0)
		dPaPb = (pb.x - pa.x) / (pb.y - pa.y);
	else
		dPaPb = 0;

	if (pc.y - pa.y > 0)
		dPaPc = (pc.x - pa.x) / (pc.y - pa.y);
	else
		dPaPc = 0;
	// Pa
	// -  Pb
	// Pc
	if (dPaPb > dPaPc)
	{
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			scld.currentY = row;
			if (row < pb.y)
			{
				scld.ndotla = ndot1;
				scld.ndotlb = ndot3;
				scld.ndotlc = ndot2;
				scld.ndotld = ndot1;
				ProcessScanLineGouraud(scld, pa, pc, pb, pa, color);
			}
			else
			{
				scld.ndotla = ndot1;
				scld.ndotlb = ndot3;
				scld.ndotlc = ndot2;
				scld.ndotld = ndot3;
				ProcessScanLineGouraud(scld, pa, pc, pb, pc, color);
			}
		}
	}
	//   Pa
	//Pb - 
	//	 Pc
	else
	{	//dPaPb <= dPaPc
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			scld.currentY = row;
			if (row < pb.y)
			{
				scld.ndotla = ndot1;
				scld.ndotlb = ndot2;
				scld.ndotlc = ndot3;
				scld.ndotld = ndot1;
				ProcessScanLineGouraud(scld, pa, pb, pc, pa, color);
			}
			else
			{
				scld.ndotla = ndot2;
				scld.ndotlb = ndot3;
				scld.ndotlc = ndot1;
				scld.ndotld = ndot3;
				ProcessScanLineGouraud(scld, pb, pc, pa, pc, color);
			}
		}
	}
}
void Device::DrawTriangleGouraudTexture(Vertex pa_v, Vertex pb_v, Vertex pc_v, UINT32 color) {
	//光栅化画三角形 使用光照高氏着色并纹理映射
	//首先将a,b,c按照行从小到大排列,且当pb与某一点位于同一水平线上时，使b在左侧

	//首先判断当前面是否可视
	if (!BackfaceCulling(pa_v, pb_v, pc_v)) return;

	Vector4D tmp, pa, pb, pc;
	Vertex tmp_v;

	if (pa_v.Coordinates.y > pb_v.Coordinates.y) {
		tmp_v = pa_v;
		pa_v = pb_v;
		pb_v = tmp_v;
	}
	if (pa_v.Coordinates.y > pc_v.Coordinates.y) {
		tmp_v = pa_v;
		pa_v = pc_v;
		pc_v = tmp_v;
	}
	if (pb_v.Coordinates.y > pc_v.Coordinates.y) {
		tmp_v = pb_v;
		pb_v = pc_v;
		pc_v = tmp_v;
	}
	pa = pa_v.Coordinates;
	pb = pb_v.Coordinates;
	pc = pc_v.Coordinates;

	//光照位置
	light_s.light_source.x = 0;
	light_s.light_source.y = 10;
	light_s.light_source.z = 0;
	//每顶点的法向量与光线向量的cos
	float ndot1 = light_s.plane_light_cos(pa_v.WorldCoordinates, pa_v.Normal);
	float ndot2 = light_s.plane_light_cos(pb_v.WorldCoordinates, pb_v.Normal);
	float ndot3 = light_s.plane_light_cos(pc_v.WorldCoordinates, pc_v.Normal);

	ScanLineData scld;

	//2种特殊
	//平顶
	if (pa.y == pb.y) {
		if (pa.x < pb.x) {
			tmp = pa;
			pa = pb;
			pb = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++) {
			scld.currentY = row;
			scld.ndotla = ndot2;
			scld.ndotlb = ndot3;
			scld.ndotlc = ndot1;
			scld.ndotld = ndot3;
			scld.ua = pb_v.u;
			scld.ub = pc_v.u;
			scld.uc = pa_v.u;
			scld.ud = pc_v.u;
			scld.va = pb_v.v;
			scld.vb = pc_v.v;
			scld.vc = pa_v.v;
			scld.vd = pc_v.v;
			//这里的顶点排列是顺序相关的
			ProcessScanLineGouraudTexture(scld, pb, pc, pa, pc, color, texture_d);
		}
		return;
	}
	//平底
	if (pc.y == pb.y) {
		if (pc.x < pb.x) {
			tmp = pb;
			pb = pc;
			pc = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++) {
			scld.currentY = row;
			scld.ndotla = ndot1;
			scld.ndotlb = ndot2;
			scld.ndotlc = ndot3;
			scld.ndotld = ndot1;
			scld.ua = pa_v.u;
			scld.ub = pb_v.u;
			scld.uc = pc_v.u;
			scld.ud = pa_v.u;
			scld.va = pa_v.v;
			scld.vb = pb_v.v;
			scld.vc = pc_v.v;
			scld.vd = pa_v.v;
			ProcessScanLineGouraudTexture(scld, pa, pb, pc, pa, color, texture_d);
		}
		return;
	}

	float dPaPb, dPaPc;
	if (pb.y - pa.y > 0)
		dPaPb = (pb.x - pa.x) / (pb.y - pa.y);
	else
		dPaPb = 0;

	if (pc.y - pa.y > 0)
		dPaPc = (pc.x - pa.x) / (pc.y - pa.y);
	else
		dPaPc = 0;
	// Pa
	// -  Pb
	// Pc
	if (dPaPb > dPaPc)
	{
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			scld.currentY = row;
			if (row < pb.y)
			{
				scld.ndotla = ndot1;
				scld.ndotlb = ndot3;
				scld.ndotlc = ndot2;
				scld.ndotld = ndot1;
				scld.ua = pa_v.u;
				scld.ub = pc_v.u;
				scld.uc = pb_v.u;
				scld.ud = pa_v.u;
				scld.va = pa_v.v;
				scld.vb = pc_v.v;
				scld.vc = pb_v.v;
				scld.vd = pa_v.v;
				ProcessScanLineGouraudTexture(scld, pa, pc, pb, pa, color, texture_d);
			}
			else
			{
				scld.ndotla = ndot1;
				scld.ndotlb = ndot3;
				scld.ndotlc = ndot2;
				scld.ndotld = ndot3;
				scld.ua = pa_v.u;
				scld.ub = pc_v.u;
				scld.uc = pb_v.u;
				scld.ud = pc_v.u;
				scld.va = pa_v.v;
				scld.vb = pc_v.v;
				scld.vc = pb_v.v;
				scld.vd = pc_v.v;
				ProcessScanLineGouraudTexture(scld, pa, pc, pb, pc, color, texture_d);
			}
		}
	}
	//   Pa
	//Pb - 
	//	 Pc
	else
	{	//dPaPb <= dPaPc
		for (int row = (int)pa.y; row <= (int)pc.y; row++)
		{
			scld.currentY = row;
			if (row < pb.y)
			{
				scld.ndotla = ndot1;
				scld.ndotlb = ndot2;
				scld.ndotlc = ndot3;
				scld.ndotld = ndot1;
				scld.ua = pa_v.u;
				scld.ub = pb_v.u;
				scld.uc = pc_v.u;
				scld.ud = pa_v.u;
				scld.va = pa_v.v;
				scld.vb = pb_v.v;
				scld.vc = pc_v.v;
				scld.vd = pa_v.v;
				ProcessScanLineGouraudTexture(scld, pa, pb, pc, pa, color, texture_d);
			}
			else
			{
				scld.ndotla = ndot2;
				scld.ndotlb = ndot3;
				scld.ndotlc = ndot1;
				scld.ndotld = ndot3;
				scld.ua = pb_v.u;
				scld.ub = pc_v.u;
				scld.uc = pa_v.u;
				scld.ud = pc_v.u;
				scld.va = pb_v.v;
				scld.vb = pc_v.v;
				scld.vc = pa_v.v;
				scld.vd = pc_v.v;
				ProcessScanLineGouraudTexture(scld, pb, pc, pa, pc, color, texture_d);
			}
		}
	}
}

void Device::Render(Mesh &mesh,int op) {
	

	//设置相关颜色,测试用
	UINT32 color_point = 0xc0c0c0;		//点的颜色
	UINT32 colorarry[] = {
		0x00ff0000 ,0x0000ff00,0x000000ff,0x00ffff00,
		0x00efefef,0x00eeffcc,0x00cc00ff,0x0015ffff,
		0x00121212,0x00001233,0x5615cc,0x353578,
		0x00ffffff
	};
	//设置相关颜色,测试用

	Vertex re, re2, re3, re4;

	//设置投影变换矩阵
	//transform.world.Set_As_Translate(mesh.Position.x, mesh.Position.y, mesh.Position.z, mesh.Position.w);	//设置平移矩阵
	transform.world.Set_As_Rotate(mesh.Rotation.x, mesh.Rotation.y, mesh.Rotation.z, mesh.Rotation.w);	//设置旋转矩阵
	transform.Update();

	//清空每一帧颜色与Z_bufffer
	Clear(0);

	//开始渲染每个面
	for (int i = 0; i < mesh.face_count; i++) {
		transform.Apply(mesh.vertices[mesh.faces[i].v1], re2);	//应用变换
		transform.Homogenize(re2.Coordinates, re2.Coordinates);	//归一化到屏幕
		transform.Apply(mesh.vertices[mesh.faces[i].v2], re3);
		transform.Homogenize(re3.Coordinates, re3.Coordinates);
		transform.Apply(mesh.vertices[mesh.faces[i].v3], re4);
		transform.Homogenize(re4.Coordinates, re4.Coordinates);

		//画每个三角形
		switch (op) {
		case 0://线框模型
			DrawTriangleFrame(re2.Coordinates, re3.Coordinates, re4.Coordinates, colorarry[12]);
			break;
		case 1://统一着色
			DrawTriangle(re2.Coordinates, re3.Coordinates, re4.Coordinates, colorarry[i % 12]);
			break;
		case 2://平面着色附光照
			DrawTriangleFlat(re2, re3, re4, colorarry[12]);
			break;
		case 3://高氏着色
			DrawTriangleGouraud(re2, re3, re4, colorarry[12]);
			break;
		case 4://高氏着色加纹理
			DrawTriangleGouraudTexture(re2, re3, re4, colorarry[12]);
			break;
		}	
	}


}


