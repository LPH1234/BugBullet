﻿#pragma once
#include <vector>
#include <deque>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <list>
#include <unordered_map>
#include <stdio.h>
#include <thread>
#include <mutex>

#include "PxPhysicsAPI.h"
#include "geometry/PxTriangleMesh.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "foundation/PxPreprocessor.h"

#include "../Utils/Utils.h"
#include "../Data/Consts.h"
#include "models.h"


using namespace physx;

extern std::mutex queue_mutex;

enum MESH_TYPE
{
	TRIANGLE,
	CONVEX
};

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

	static std::unordered_map<PxBase*, BaseModel*> meshToRenderModel;
	/**
	 * @brief				   根据OpenGL的渲染模型创建一个objloader，加载obj文件为PhysX的物理模型
	 * @param renderModel	   指向渲染模型的指针
	 * @param type             要生成的mesh类型  0:triangleMesh    1:convexMesh
	 */
	ObjLoader(BaseModel* renderModel, MESH_TYPE type = TRIANGLE);

	~ObjLoader();

private:

	bool is_triangle_cooked; //是否加载到cooking文件中
	bool is_convex_cooked; //是否加载到cooking文件中

	std::vector<struct vertices> v;//存放顶点(x,y,z)坐标
	std::list<std::vector<struct face>> f;//存放面的三个顶点索引
	std::string name; //模型文件名
	std::string obj_file_path; //模型路径
	std::string triangle_cooking_file_path; //triangle cooking 文件路径
	std::string convex_cooking_file_path; //convex cooking 文件路径
	physx::PxVec3  scale;
	physx::PxVec3  initPos;
	BaseModel* renderModel; //渲染模型
	PxTriangleMesh* triangle_mesh = nullptr;
	PxTriangleMeshDesc* triangle_mesh_desc = nullptr;
	PxConvexMesh* convex_mesh = nullptr;
	PxConvexMeshDesc* convex_mesh_desc = nullptr;


	void parseObjFile();


	/**
	 * @brief  将meshDesc写入到cook_file_path所指向的cooking文件
	 * @return 是否成功
	 */
	static bool cookTriangleMesh(PxTriangleMeshDesc* meshDesc, std::string cook_file_path);
	/**
	 * @brief  将meshDesc写入到cook_file_path所指向的cooking文件
	 * @return 是否成功
	 */
	static bool cookConvexMesh(PxConvexMeshDesc* meshDesc, std::string cook_file_path);

	/**
	 * @brief  将模型的mesh加载到cooking文件中，在使用时读取。减少即时加载时间。内部使用。
	 */
	static int writeConvexMeshToCookingFile(PxConvexMeshDesc* meshDesc, std::string cook_file_path);

	/**
	 * @brief  将模型的mesh加载到cooking文件中，在使用时读取。减少即时加载时间。内部使用。
	 */
	static int writeTriangleMeshToCookingFile(PxTriangleMeshDesc* meshDesc, std::string cook_file_path);



	//================ TriangleMesh 静态刚体 ==========================
	/**
	 * @brief  从顶点集和面集创建PxTriangleMesh。内部使用。
	 */
	physx::PxTriangleMesh* genTriangleMesh(physx::PxVec3 scale);

	/**
	 * @brief  从cooking文件创建PxTriangleMesh。内部使用。
	 */
	physx::PxTriangleMesh* readTriangleMeshFromCookingFile();
	//-----------------------------------------------------------------



	//==================== ConvexMesh 动态刚体========================
	physx::PxConvexMesh* genConvexMesh(physx::PxVec3 scale);

	/**
	 * @brief  从cooking文件创建PxTriangleMesh。内部使用。
	 */
	physx::PxConvexMesh* readConvexMeshFromCookingFile();
	//-----------------------------------------------------------------

public:

	/**
	 * @brief  创建一个模型所表示的static rigid Actor，将其加入到scene中(所以调用此方法会在当前场景中绘制此模型)，并返回它。
	 * @param offset    初始位置偏移量
	 * @return 返回创建好的Actor
	 */
	physx::PxRigidActor* createStaticActorAndAddToScene();


	/**
	 * @brief  创建一个模型所表示的dynamic rigid Actor，将其加入到scene中(所以调用此方法会在当前场景中绘制此模型)，并返回它。
	 * @param offset    初始位置偏移量
	 * @return 返回创建好的Actor
	 */
	physx::PxRigidActor* createDynamicActorAndAddToScene();

};



class CookThread {
	std::deque<PxTriangleMeshDesc*> tds;
	std::deque<PxConvexMeshDesc*> cds;
	bool if_start = false;
public:
	static bool cook_thread_exit_flag;
	void operator ()() {
		while (!cook_thread_exit_flag)
		{
			PxTriangleMeshDesc* temp1 = tds.front();
			if (temp1 != nullptr && temp1->isValid()) {

			}
			tds.pop_front();

		}
	}
	void start() {
		queue_mutex.lock();
		if (if_start) return;
		if_start = true;
		std::thread cooking_obj((CookThread()));
		queue_mutex.unlock();
	}

	void run() {

	}

	void addTriangleMeshDesc(PxTriangleMeshDesc* desc) {
		queue_mutex.lock();
		tds.push_back(desc);
		queue_mutex.unlock();
	}

	void addConvexMeshDesc(PxConvexMeshDesc* desc) {
		queue_mutex.lock();
		cds.push_back(desc);
		queue_mutex.unlock();
	}

};