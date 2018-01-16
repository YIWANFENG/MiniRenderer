#include "render.h"
#include "lib_json\json.h"
#include <fstream>

int ReadJsonFromFile(const char* filename, Mesh &mesh)
{

	Json::Reader reader;// 解析json用Json::Reader   
	Json::Value root; // Json::Value是一种很重要的类型，可以代表任意类型。如int, string, object, array         

	std::ifstream is;
	is.open(filename, std::ios::binary);
	if (reader.parse(is, root, 0))
	{
		std::string mesh_name;
		//Mesh mesh;
		int mesh_count = 0;
		if (!root["meshes"].isNull())  // 访问节点，Access an object value by name, create a null member if it does not exist.  
			mesh_count = root["meshes"].size();
		for (int i = 0; i < mesh_count; i++) {

			int vertices_count = root["meshes"][i]["vertices"].size();
			int faces_count = root["meshes"][i]["indices"].size();
			int uvCount = root["meshes"][i]["uvCount"].asInt();

			int ver_span = 1;	//每个顶底跨距
			int face_span = 3;	//每个面跨距
			switch (uvCount)
			{
			case 0:
				ver_span = 6;
				break;
			case 1:
				ver_span = 8;
				break;
			case 2:
				ver_span = 10;
				break;
			}

			mesh.face_count = faces_count / face_span;
			mesh.vertex_count = vertices_count / ver_span;
			mesh.vertices = new Vertex[mesh.vertex_count];
			mesh.faces = new Face[mesh.face_count];
			//解析顶点
			Json::Value vertices = root["meshes"][i]["vertices"];
			for (int j = 0; j < mesh.vertex_count; j++) {
				// 加载Blender导出的顶点
				mesh.vertices[j].WorldCoordinates.x = vertices[j*ver_span].asDouble();
				mesh.vertices[j].WorldCoordinates.y = vertices[j*ver_span + 1].asDouble();
				mesh.vertices[j].WorldCoordinates.z = vertices[j*ver_span + 2].asDouble();
				mesh.vertices[j].WorldCoordinates.w = 1.0f;
				// 加载Blender导出的顶点法线
				mesh.vertices[j].Normal.x = vertices[j*ver_span + 3].asDouble();
				mesh.vertices[j].Normal.y = vertices[j*ver_span + 4].asDouble();
				mesh.vertices[j].Normal.z = vertices[j*ver_span + 5].asDouble();
				mesh.vertices[j].Normal.w = 1.0f;
				if (uvCount == 0) continue;
				//加载纹理坐标
				mesh.vertices[j].u = vertices[j*ver_span + 6].asDouble();
				mesh.vertices[j].v = vertices[j*ver_span + 7].asDouble();
				//cout << mesh.vertices[j].u << " " << mesh.vertices[j].v << endl;

			}
			//解析面
			Json::Value faces = root["meshes"][i]["indices"];
			for (int j = 0; j < mesh.face_count; j++) {
				mesh.faces[j].v1 = faces[j*face_span].asInt();
				mesh.faces[j].v2 = faces[j*face_span + 1].asInt();
				mesh.faces[j].v3 = faces[j*face_span + 2].asInt();
			}
			//解析设置的mesh位置
			Json::Value js_position = root["meshes"][i]["position"];
			int k = 0;
			mesh.Position.x = js_position[k].asDouble();
			mesh.Position.y = js_position[1].asDouble();
			mesh.Position.z = js_position[2].asDouble();

		}
	}
	is.close();

	return 0;
}


