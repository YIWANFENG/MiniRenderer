#include "render.h"
#include <math.h>
#include <iostream>
#include <assert.h>
#include <opencv2\opencv.hpp>


//=====================================================================
// ��ѧ�⣺�˲���Ӧ�ò�����⣬��Ϥ���Դ�������任����
// ��Ӧ��Ϸ�����е���ѧ���棬������ҪΪVector4D �� Mat�Ķ�������ز���
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
	//����Ϊƽ�ƾ���
	Set_Identity();
	/*m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;*/
	m[0][3] = x;
	m[1][3] = y;
	m[2][3] = z;
}
void Mat::Set_As_Scale(float x, float y, float z) {
	//����Ϊ���ž���
	Set_Identity();
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;

}
void Mat::Set_As_Rotate(float x, float y, float z, float theta) {
	//����Ϊ��ת����
	//theta�ǻ���ֵ
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
// ������ݽṹ����
//=====================================================================
///////Camera/////////////////////////////////////////////
Camera::Camera() {
	view.Set_Identity();
}
void Camera::Set_Camera(Vector4D &eye_pos, Vector4D &eye_look_at, Vector4D &eye_up) {
	//��eye_pos �������������ϵλ�ã�look_at �۲쳡������������������ϵλ��
	//eye_up,��������Ϸ���������������ϵ�еı�ʾ 
	//��������������ϵ���۲�����ϵ��ת������
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
	//���ݷ��߼��������ߵļн�
	Vector4D eye_dir;
	center.Sub(pos, eye_dir);			//��ȡ��������
	eye_dir.Normalize();
	normal.Normalize();
	return  normal.Mul_Dot(eye_dir);
}


void Color::Set(UINT32 x, float s) {
	//����S������X����Ӧ����ɫֵ
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
	re.Set(0x00000000, 1.0f);			//Ĭ��Ϊ��
	if (buf.empty()) return re;
	int u = (int)(tu*width) % width;	//%Ϊ��ֹ����
	int v = (int)(tv*height) % height;
	if (u<0 || v<0)
		cout << tu << ' ' << tv << endl;
	u = u >= 0 ? u : -u;
	v = v >= 0 ? v : -v;

	cv::Vec3b tex_w = buf.at<cv::Vec3b>(v, u);	//U��X,V��Y
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
// ����任,�����ģ������ϵ�任�����������پ�������任��Ȼ����͸�ӱ任
//=====================================================================

void Transform::Update() {
	// ������£����� transform =   projection * view * world
	static Mat m;
	m.Set_Identity();
	view.Mul(world, m);
	projection.Mul(m, transform);
}
void Transform::Init(int width, int height) {
	// ��ʼ����������Ļ����

	world.Set_Identity();
	view.Set_Identity();
	w = (float)width;
	h = (float)height;
}
void Transform::Apply(Vector4D &op, Vector4D &re) {
	//old  ��ʸ�� op ���� project 
	//�˲�����re���x,y��Ϊ����Ļ����ʾ��λ�ã�z������Ȳ���
	transform.Mul_Vec(op, re);

}
void Transform::Apply(Vertex &op, Vertex &re) {
	// new ������ op ���� project��������������re
	//�˲�����re���x,y��Ϊ����Ļ����ʾ��λ�ã�z������Ȳ���
	//���㷨��normalͬ��
	transform.Mul_Vec(op.WorldCoordinates, re.Coordinates);		//����ͶӰ�任��������ʾ
	world.Mul_Vec(op.WorldCoordinates, re.WorldCoordinates);	//���ڶ������������任�����ڹ���
	world.Mul_Vec(op.Normal, re.Normal);							//���ڷ��߽���������任�����ڹ���
	re.u = op.u;												//�������겻��
	re.v = op.v;
}


void Transform::Homogenize(Vector4D &op, Vector4D &re) {
	// ��һ�����õ���Ļ����
	float rhw = 1.0f / op.w;
	re.x = (op.x * rhw + 1.0f) * w * 0.5f;
	re.y = (1.0f - op.y * rhw) * h * 0.5f;
	re.z = op.z * rhw;
	re.w = 1.0f;
}

void Transform::Set_Perspective(float fovy, float aspect, float zn, float zf) {
	//����͸�Ӿ���,�൱��D3DXMatrixPerspectiveFovLH
	//fovy = view frustum ��Z��н� ������
	//aspect ͶӰ����ȣ���ʾ�����ȣ�
	//zn ��������ü�ƽ����룬zf �����Զ�ü�ƽ�����

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
	//���ݷ��߼�������������ļнǣ������һ���� 0-1��������������ɫ����
	Vector4D light_dir;
	light_source.Sub(vertex, light_dir);			//��ȡ��������
	light_dir.Normalize();
	normal.Normalize();
	float angle_cos = normal.Mul_Dot(light_dir);
	//if (angle_cos < 0) cout << angle_cos << endl;
	angle_cos = angle_cos < 0 ? 0 : angle_cos;		//�нǴ���90�����ڱ�����
	angle_cos = angle_cos > 1 ? 1 : angle_cos;		//��ֹ���ڸ��������Խ��
	return angle_cos;

}

//=====================================================================
// ��Ⱦ�豸����
//=====================================================================
///////main function definition/////////////////////////////////////////
Device::Device(int w, int h, void *fb) {
	width = w;
	height = h;
	transform.Init(w, h);
	//��������鿴
	// �豸��ʼ����fbΪ�ⲿ֡���棬�� NULL �������ⲿ֡���棨ÿ�� 4�ֽڶ��룩
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
	background = 0;	//��������ɫΪ0��ɫ
}
Device::~Device() {
	if (framebuffer)
		free(framebuffer);
	framebuffer = NULL;
	zbuffer = NULL;
	//delete[] ZBuffer;
}

void Device::Clear(int mode) {
	// ��� framebuffer �� zbuffer
	//����鿴
	int y, x, height = this->height;
	for (y = 0; y < height; y++) {
		UINT32 *dst = framebuffer[y];
		UINT32 cc = (height - 1 - y) * 230 / (height - 1);
		cc = (cc << 16) | (cc << 8) | cc;
		if (mode == 0) cc = background;	//mode == 0,����ΪĬ�ϱ���ɫ
		for (x = width; x > 0; dst++, x--) dst[0] = cc;
	}
	//������ڴ�ȫ��Ϊһ���㹻���Ĭ��ֵ
	for (y = 0; y < height; y++) {
		float *dst = zbuffer[y];
		//memset((void*)(dst), 0, width * sizeof(float));
		for (x = width; x > 0; dst++, x--) dst[0] = 65535.0f;
	}
	//memset((void*)ZBuffer, 0, 800*600*sizeof(float));
}
bool Device::BackfaceCulling(Vertex pa_v, Vertex pb_v, Vertex pc_v)
{
	//�ж�pa_v,pb_v,pc_v����ɵ�ƽ���Ƿ�ɼ�
	//�ɼ�����true;

	//������������ķ����� == �������㷨������ƽ��ֵ
	Vector4D normal;
	pa_v.Normal.Add(pb_v.Normal, normal);
	normal.Add(pc_v.Normal, normal);
	normal.Mul_float(0.3333f, normal);
	//���������������λ�ã��������������λ�ã�
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
	//������Ȳ��Ե�������亯����z������Ȳ���
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
	// ���м�Z��ֵ����
	int dx = abs(xe - xs);
	int dy = abs(ye - ys);
	int sx = (xs < xe) ? 1 : -1;
	int sy = (ys < ye) ? 1 : -1;
	int err = dx - dy;
	int ys_0 = ys, ye_0 = ye, xs_0 = xs, xe_0 = xe;

	while (true) {
		float gradient = 0;
		if ((ye_0 - ys_0)) gradient = (ys - ys_0) / (float)(ye_0 - ys_0);	//����y
		else if (xe_0 - xs_0) gradient = (xs - xs_0) / (float)(xe_0 - xs_0);//����x
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
	//pa,pb,pc,pd�ĵ���������Σ�
	//���������������ɼ����Բ�ֵ��ȷ��ɨ����
	//Ĭ��Vector4D.w = 1.0


	//������ݶ���������˹�һ����Ҳ���ǿ���������ƽ��������ƽ�������Ρ�
	float gradient_s = pa.y != pb.y ? (curY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (curY - pc.y) / (pd.y - pc.y) : 1;

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//������ƽ�淽��
	float A = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
	float B = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
	float C = (pb.x - pa.x)*(pc.y - pa.y) - (pc.x - pa.x)*(pb.y - pa.y);
	float D = -(A * pa.x + B * pa.y + C * pa.z);
	float C_inv = 1 / C;

	//����ɨ����
	for (int x = sx; x <= ex; x++) {
		float z = (-D - A*x - B*curY)*C_inv;	//����õ����
		PutPixel(x, curY, z, color);
	}
}
void Device::ProcessScanLine(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color) {
	//pa,pb,pc,pd�ĵ���������Σ�
	//���������������ɼ����Բ�ֵ��ȷ��ɨ����
	//Ĭ��Vector4D.w = 1.0
	//�Ѿ����ƽ����ɫ


	//������ݶ���������˹�һ����Ҳ���ǿ���������ƽ��������ƽ�������Ρ�
	float gradient_s = pa.y != pb.y ? (scld.currentY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (scld.currentY - pc.y) / (pd.y - pc.y) : 1;

	//ɨ���ߺ��������Բ�ֵ

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//ÿ������ֵ������ַ���
	//1.������Բ�ֵ
	float z1 = INTERP(pa.z, pb.z, gradient_s);
	float z2 = INTERP(pc.z, pd.z, gradient_e);

	//or 2������ƽ�淽��
	//float A = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
	//float B = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
	//float C = (pb.x - pa.x)*(pc.y - pa.y) - (pc.x - pa.x)*(pb.y - pa.y);
	//float D = - (A * pa.x + B * pa.y + C * pa.z);
	//float C_inv = 1 / C;

	Color color_s;
	//����ɨ����
	for (int x = sx; x <= ex; x++) {

		float gradient = (x - sx) / (float)(ex - sx);	//1.��ֵ�õ����
		float z = INTERP(z1, z2, gradient);

		//float z = (-D - A*x - B*scld.currentY)*C_inv;	//2.����õ����

		// ���ڹ������ͷ�������֮��Ƕȵ����Ҹı���ɫֵ  
		color_s.Set(color, scld.ndotla);
		PutPixel(x, scld.currentY, z, color_s.uint32);
	}
}
void Device::ProcessScanLineGouraud(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc, Vector4D &pd, UINT32& color) {
	//pa,pb,pc,pd�ĵ���������Σ�
	//���������������ɼ����Բ�ֵ��ȷ��ɨ����
	//Ĭ��Vector4D.w = 1.0
	//�Ѿ���Ӹ�����ɫ


	//������ݶ���������˹�һ����Ҳ���ǿ���������ƽ��������ƽ�������Ρ�
	float gradient_s = pa.y != pb.y ? (scld.currentY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (scld.currentY - pc.y) / (pd.y - pc.y) : 1;

	//ɨ���ߺ��������Բ�ֵ

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//ÿ������ֵ������ַ���
	//1.������Բ�ֵ
	float z1 = INTERP(pa.z, pb.z, gradient_s);
	float z2 = INTERP(pc.z, pd.z, gradient_e);

	//or 2������ƽ�淽��
	//float A = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
	//float B = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
	//float C = (pb.x - pa.x)*(pc.y - pa.y) - (pc.x - pa.x)*(pb.y - pa.y);
	//float D = - (A * pa.x + B * pa.y + C * pa.z);
	//float C_inv = 1 / C;

	//������ɫ-����,��ֵ
	float snl = INTERP(scld.ndotla, scld.ndotlb, gradient_s);
	float enl = INTERP(scld.ndotlc, scld.ndotld, gradient_e);;

	Color color_s;
	//����ɨ����
	for (int x = sx; x <= ex; x++) {

		float gradient = (x - sx) / (float)(ex - sx);
		float z = INTERP(z1, z2, gradient);				//1.��ֵ�õ����
		float ndot = INTERP(snl, enl, gradient);		//��ֵ�õ��"����ǿ��"

														//float z = (-D - A*x - B*scld.currentY)*C_inv;	//2.����õ����

														// ���ڹ������ͷ�������֮��Ƕȵ����Ҹı���ɫֵ  
		color_s.Set(color, ndot);
		PutPixel(x, scld.currentY, z, color_s.uint32);
	}
}
void Device::ProcessScanLineGouraudTexture(ScanLineData &scld, Vector4D &pa, Vector4D &pb, Vector4D &pc,
	Vector4D &pd, UINT32& color, Texture &tex) {
	//pa,pb,pc,pd�ĵ���������Σ�
	//���������������ɼ����Բ�ֵ��ȷ��ɨ����
	//Ĭ��Vector4D.w = 1.0
	//�Ѿ���Ӹ�����ɫ������ӳ��


	//������ݶ���������˹�һ����Ҳ���ǿ���������ƽ��������ƽ�������Ρ�
	float gradient_s = pa.y != pb.y ? (scld.currentY - pa.y) / (pb.y - pa.y) : 1;
	float gradient_e = pc.y != pd.y ? (scld.currentY - pc.y) / (pd.y - pc.y) : 1;

	//ɨ���ߺ��������Բ�ֵ

	int sx = INTERP(pa.x, pb.x, gradient_s);
	int ex = INTERP(pc.x, pd.x, gradient_e);
	//ÿ������ֵ������ַ���
	//1.������Բ�ֵ
	float z1 = INTERP(pa.z, pb.z, gradient_s);
	float z2 = INTERP(pc.z, pd.z, gradient_e);

	//������ɫ-����,��ֵ
	float snl = INTERP(scld.ndotla, scld.ndotlb, gradient_s);
	float enl = INTERP(scld.ndotlc, scld.ndotld, gradient_e);;

	// �����������ֵ��Y��  
	float su = INTERP(scld.ua, scld.ub, gradient_s);
	float eu = INTERP(scld.uc, scld.ud, gradient_e);
	float sv = INTERP(scld.va, scld.vb, gradient_s);
	float ev = INTERP(scld.vc, scld.vd, gradient_e);

	Color color_s, color_tex;
	//����ɨ����
	for (int x = sx; x <= ex; x++) {

		float gradient = (x - sx) / (float)(ex - sx);
		float z = INTERP(z1, z2, gradient);				//��ֵ�õ����
		float ndot = INTERP(snl, enl, gradient);		//��ֵ�õ��"����ǿ��"
		float u = INTERP(su, eu, gradient);				//����ӳ���ֵ
		float v = INTERP(sv, ev, gradient);
		if (!tex.buf.empty()) color_tex = tex.Map(u, v);		//ȡ����
		else color_tex.Set(color, 1.0f);
		// ���ڹ������ͷ�������֮��Ƕȵ������Լ�������ɫ
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
	//old ��դ����������  δʹ�ù��հ�
	//���Ƚ�a,b,c�����д�С��������,�ҵ�pb��ĳһ��λ��ͬһˮƽ����ʱ��ʹb�����
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
	//2������
	//ƽ��
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
	//ƽ��
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
	//��դ���������� ʹ�ù���ƽ����ɫ
	//���Ƚ�a,b,c�����д�С��������,�ҵ�pb��ĳһ��λ��ͬһˮƽ����ʱ��ʹb�����
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

	//������������ķ����� == �������㷨������ƽ��ֵ
	Vector4D normal;
	pa_v.Normal.Add(pb_v.Normal, normal);
	normal.Add(pc_v.Normal, normal);
	normal.Mul_float(0.3333f, normal);
	//cout << normal.x << ' ' << normal.y << ' ' << normal.z << endl;
	//���������������λ�ã��������������λ�ã�
	Vector4D center_point;
	pa_v.WorldCoordinates.Add(pb_v.WorldCoordinates, center_point);
	center_point.Add(pc_v.WorldCoordinates, center_point);
	center_point.Mul_float(0.3333f, center_point);
	//����λ��
	light_s.light_source.x = 0;
	light_s.light_source.y = 10;
	light_s.light_source.z = 0;
	float ndot = light_s.plane_light_cos(center_point, normal);
	//if(ndot!=0) cout << ndot << endl;
	ScanLineData scld;
	scld.ndotla = ndot;
	//scld.ndotla = 1.0;

	//2������
	//ƽ��
	if (pa.y == pb.y) {
		if (pa.x < pb.x) {
			tmp = pa;
			pa = pb;
			pb = tmp;
		}
		for (int row = (int)pa.y; row <= (int)pc.y; row++) {
			scld.currentY = row;
			ProcessScanLine(scld, pb, pc, pa, pc, color);	//����Ķ���������˳����ص�
		}
		return;
	}
	//ƽ��
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
	//��դ���������� ʹ�ù��ո�����ɫ
	//���Ƚ�a,b,c�����д�С��������,�ҵ�pb��ĳһ��λ��ͬһˮƽ����ʱ��ʹb�����
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

	//����λ��
	light_s.light_source.x = 0;
	light_s.light_source.y = 10;
	light_s.light_source.z = 0;
	//ÿ����ķ����������������cos
	float ndot1 = light_s.plane_light_cos(pa_v.WorldCoordinates, pa_v.Normal);
	float ndot2 = light_s.plane_light_cos(pb_v.WorldCoordinates, pb_v.Normal);
	float ndot3 = light_s.plane_light_cos(pc_v.WorldCoordinates, pc_v.Normal);

	ScanLineData scld;

	//2������
	//ƽ��
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
			ProcessScanLineGouraud(scld, pb, pc, pa, pc, color);	//����Ķ���������˳����ص�
		}
		return;
	}
	//ƽ��
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
	//��դ���������� ʹ�ù��ո�����ɫ������ӳ��
	//���Ƚ�a,b,c�����д�С��������,�ҵ�pb��ĳһ��λ��ͬһˮƽ����ʱ��ʹb�����

	//�����жϵ�ǰ���Ƿ����
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

	//����λ��
	light_s.light_source.x = 0;
	light_s.light_source.y = 10;
	light_s.light_source.z = 0;
	//ÿ����ķ����������������cos
	float ndot1 = light_s.plane_light_cos(pa_v.WorldCoordinates, pa_v.Normal);
	float ndot2 = light_s.plane_light_cos(pb_v.WorldCoordinates, pb_v.Normal);
	float ndot3 = light_s.plane_light_cos(pc_v.WorldCoordinates, pc_v.Normal);

	ScanLineData scld;

	//2������
	//ƽ��
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
			//����Ķ���������˳����ص�
			ProcessScanLineGouraudTexture(scld, pb, pc, pa, pc, color, texture_d);
		}
		return;
	}
	//ƽ��
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
	

	//���������ɫ,������
	UINT32 color_point = 0xc0c0c0;		//�����ɫ
	UINT32 colorarry[] = {
		0x00ff0000 ,0x0000ff00,0x000000ff,0x00ffff00,
		0x00efefef,0x00eeffcc,0x00cc00ff,0x0015ffff,
		0x00121212,0x00001233,0x5615cc,0x353578,
		0x00ffffff
	};
	//���������ɫ,������

	Vertex re, re2, re3, re4;

	//����ͶӰ�任����
	//transform.world.Set_As_Translate(mesh.Position.x, mesh.Position.y, mesh.Position.z, mesh.Position.w);	//����ƽ�ƾ���
	transform.world.Set_As_Rotate(mesh.Rotation.x, mesh.Rotation.y, mesh.Rotation.z, mesh.Rotation.w);	//������ת����
	transform.Update();

	//���ÿһ֡��ɫ��Z_bufffer
	Clear(0);

	//��ʼ��Ⱦÿ����
	for (int i = 0; i < mesh.face_count; i++) {
		transform.Apply(mesh.vertices[mesh.faces[i].v1], re2);	//Ӧ�ñ任
		transform.Homogenize(re2.Coordinates, re2.Coordinates);	//��һ������Ļ
		transform.Apply(mesh.vertices[mesh.faces[i].v2], re3);
		transform.Homogenize(re3.Coordinates, re3.Coordinates);
		transform.Apply(mesh.vertices[mesh.faces[i].v3], re4);
		transform.Homogenize(re4.Coordinates, re4.Coordinates);

		//��ÿ��������
		switch (op) {
		case 0://�߿�ģ��
			DrawTriangleFrame(re2.Coordinates, re3.Coordinates, re4.Coordinates, colorarry[12]);
			break;
		case 1://ͳһ��ɫ
			DrawTriangle(re2.Coordinates, re3.Coordinates, re4.Coordinates, colorarry[i % 12]);
			break;
		case 2://ƽ����ɫ������
			DrawTriangleFlat(re2, re3, re4, colorarry[12]);
			break;
		case 3://������ɫ
			DrawTriangleGouraud(re2, re3, re4, colorarry[12]);
			break;
		case 4://������ɫ������
			DrawTriangleGouraudTexture(re2, re3, re4, colorarry[12]);
			break;
		}	
	}


}


