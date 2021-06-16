#pragma once
#include <vector>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>
#include <list>
#include <unordered_map>
#include <stdio.h>

#include "PxPhysicsAPI.h"
#include "geometry/PxTriangleMesh.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "foundation/PxPreprocessor.h"

#include "../Utils/Utils.h"
#include "../Data/Consts.h"
#include "models.h"


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

	static std::unordered_map<PxBase*, BaseModel*> meshToRenderModel;
	/**
	 * @brief			根据OpenGL的渲染模型创建一个objloader，加载obj文件为PhysX的物理模型
	 * @param model     指向渲染模型的指针
	 */
	ObjLoader(BaseModel* renderModel, bool preLoad = false);


	//ObjLoader(BaseModel* renderModel, bool preLoad = false);
	~ObjLoader();

private:

	bool preLoad; //是否加载到cooking文件中
	std::vector<struct vertices> v;//存放顶点(x,y,z)坐标
	std::list<std::vector<struct face>> f;//存放面的三个顶点索引
	std::string name; //模型文件名
	physx::PxVec3  scale;
	physx::PxVec3  initPos;
	BaseModel* renderModel;

	//--------------- TriangleMesh 静态刚体 ---------------------------
	/**
	 * @brief  从顶点集和面集创建PxTriangleMesh。内部使用。
	 */
	physx::PxTriangleMesh* createTriangleMesh(physx::PxVec3 scale);

	/**
	 * @brief  将模型的mesh加载到cooking文件中，在使用时读取。减少即时加载时间。内部使用。
	 */
	int writeTriangleMeshToCookingFile();

	/**
	 * @brief  从cooking文件创建PxTriangleMesh。内部使用。
	 */
	physx::PxTriangleMesh* readTriangleMeshFromCookingFile();
	//-----------------------------------------------------------------

	//--------------- ConvexMesh 动态刚体 ---------------------------
	physx::PxConvexMesh* createConvexMesh(physx::PxVec3 scale);


	/**
	 * @brief  将模型的mesh加载到cooking文件中，在使用时读取。减少即时加载时间。内部使用。
	 */
	int writeConvexMeshToCookingFile();

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
