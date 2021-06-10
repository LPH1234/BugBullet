#pragma once
#include <vector>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <list>

#include "PxPhysicsAPI.h"
#include "geometry/PxTriangleMesh.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "foundation/PxPreprocessor.h"


using namespace physx;


class ObjLoader
{
public:
	// 几何体顶点v
	struct vertices {
		float x;
		float y;
		float z;
	};
	// 面f
	struct face {
		unsigned int u; //顶点标号
		unsigned int v; // 贴图顶点标号
		unsigned int w;	// 法向量标号
	};
	ObjLoader(std::string &filename, PxPhysics* gPhysics, PxCooking*	gCooking, PxScene* gScene, PxMaterial* gMaterial);//读取函数
	~ObjLoader();
	//void obj_parse_face(const string& szface, struct face facesrc);
private:
	physx::PxTriangleMesh* createOjbMesh(int scale);
	PxPhysics* gPhysics;
	PxCooking*	gCooking;
	PxScene* gScene;
	PxMaterial* gMaterial;

public:
	std::vector<struct vertices> v;//存放顶点(x,y,z)坐标
	std::list<std::vector<struct face>> f;//存放面的三个顶点索引
	std::string name;

	physx::PxRigidActor* createObjMeshs(physx::PxVec3 &offset, int scale);

	int writeMeshCookingFile(int scale);
	PxTriangleMesh * readCookingFile();
};
