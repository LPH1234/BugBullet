﻿#pragma once
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
#include "../Utils/Consts.h"

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
	struct vt
	{
		float x;
		float y;
	};

	static std::unordered_map<PxTriangleMesh*, std::vector<vt>> meshToVts;
	static std::unordered_map<PxTriangleMesh*, std::vector<int>> meshToVtIdxes;

	ObjLoader(std::string &obj_file_path, PxPhysics* gPhysics, PxCooking* gCooking, PxScene* gScene, PxMaterial* gMaterial, int scale, bool preLoad = false);
	~ObjLoader();

private:


	PxPhysics* gPhysics;
	PxCooking*	gCooking;
	PxScene* gScene;
	PxMaterial* gMaterial;
	bool preLoad; //是否加载到cooking文件中
	std::vector<struct vertices> v;//存放顶点(x,y,z)坐标
	std::list<std::vector<struct face>> f;//存放面的三个顶点索引
	std::vector<ObjLoader::vt> vts; //贴图坐标
	std::vector<int> vt_idx; //贴图坐标索引
	std::string name; //模型文件名
	int scale;

	//--------------- TriangleMesh 静态刚体 ---------------------------
	/**
	 * @brief  从顶点集和面集创建PxTriangleMesh。内部使用。
	 */
	physx::PxTriangleMesh* createTriangleMesh(int scale);

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
	physx::PxConvexMesh* createConvexMesh(int scale);

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
	physx::PxRigidActor* createStaticActorAndAddToScene(physx::PxVec3 &offset);

	/**
	 * @brief  创建一个模型所表示的dynamic rigid Actor，将其加入到scene中(所以调用此方法会在当前场景中绘制此模型)，并返回它。
	 * @param offset    初始位置偏移量
	 * @return 返回创建好的Actor
	 */
	physx::PxRigidActor* createDynamicActorAndAddToScene(physx::PxVec3 &offset);

};
