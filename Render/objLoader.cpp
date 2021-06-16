#include <vector>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>

#include <math.h>

#include "ObjLoader.h"


using namespace std;

std::unordered_map<PxBase*, BaseModel*> ObjLoader::meshToRenderModel;

extern PxPhysics* gPhysics;
extern PxCooking* gCooking;
extern PxScene* gScene;
extern PxMaterial* gMaterial;


ObjLoader::ObjLoader(BaseModel* renderModel, bool preLoad) {
	//初始化
	this->renderModel = renderModel;
	std::string obj_file_path = renderModel->getModelPath();
	this->name = obj_file_path.substr(obj_file_path.find_last_of("/") + 1);
	this->scale = glmVec3ToPxVec3(renderModel->getScaleValue());
	this->initPos = glmVec3ToPxVec3(renderModel->getPosition());
	this->preLoad = preLoad;
	//开始解析obj
	Logger::debug("打开模型文件：" + name);
	ifstream obj_file(obj_file_path);
	string line;
	Logger::debug("开始加载物理模型：" + name);
	while (getline(obj_file, line))
	{

		if (line.substr(0, 2) == "v ") //顶点
		{
			struct vertices vtmp = { 0 };
			istringstream s(line.substr(2));
			// v -1.038081 -0.8783139 -2.667491
			s >> vtmp.x >> vtmp.y >> vtmp.z;

			v.push_back(vtmp);
		}
		else if (line.substr(0, 1) == "f") //面
		{
			istringstream s(line.substr(2)); //25/25/25  28/28/28  26/26/26 (27/27/27) 可能是四个顶点
			string f_points[4];
			s >> f_points[0] >> f_points[1] >> f_points[2] >> f_points[3];

			{
				vector<struct face> vface;
				for (int i = 0; i < 3; i++)
				{
					struct face faceTmp;
					std::vector<std::string> splits;
					StringUtils::split(f_points[i], "/", splits);// f_points[i]里面存储的是形如： 25/25/25 的数据
					faceTmp.u = atoi(splits[0].c_str()) - 1; //需要 -1 是因为索引从0开始，而文件中得到的索引从1开始
					vface.push_back(faceTmp);
				}
				f.push_back(vface);
			}
			if (f_points[3] != "") { //4个点的面
				vector<struct face> vface;
				for (int i = 0; i < 3; i++)
				{
					int j = (i + 2) % 4;
					struct face faceTmp;
					std::vector<std::string> splits;
					StringUtils::split(f_points[j], "/", splits);// f_points[i]里面存储的是形如： 25/25/25 的数据
					faceTmp.u = atoi(splits[0].c_str()) - 1; //需要 -1 是因为索引从0开始，而文件中得到的索引从1开始
					vface.push_back(faceTmp);
				}
				f.push_back(vface);
			}

		}

	}
	obj_file.close(); //关闭obj文件

	if (preLoad) {
		Logger::info("开始写入cooking文件中...");
		writeTriangleMeshToCookingFile();
		Logger::info("完成");
	}
	Logger::info("v:" + std::to_string(v.size()) + "   f:" + std::to_string(f.size()));
}

ObjLoader::~ObjLoader() {
	v.clear();
	f.clear();
}

physx::PxTriangleMesh* ObjLoader::createTriangleMesh(physx::PxVec3 scale)
{

	const PxU32 numVertices = this->v.size();
	const PxU32 numTriangles = this->f.size();

	PxVec3* vertices = new PxVec3[numVertices];
	PxU32* indices = new PxU32[numTriangles * 3];


	// 加载顶点
	for (int i = 0; i < numVertices; ++i) {
		PxVec3 vectmp(this->v[i].x * scale.x, this->v[i].y * scale.y, this->v[i].z * scale.z);
		vertices[i] = vectmp;
	}
	//memcpy(vertices + 1, &objtmp->v[0], sizeof(PxVec3)* (numVertices));

	// 加载面和顶点贴图索引
	auto faceIt = this->f.begin();
	for (int i = 0; i < numTriangles && faceIt != this->f.end(); faceIt++, ++i) {
		for (int j = 0; j < 3; j++)
		{
			if ((*faceIt).size() >= j + 1) {
				indices[i * 3 + j] = (*faceIt)[j].u;
			}
		}
	}

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = numVertices;
	meshDesc.points.data = vertices;
	meshDesc.points.stride = sizeof(PxVec3);

	meshDesc.triangles.count = numTriangles;
	meshDesc.triangles.data = indices;
	meshDesc.triangles.stride = sizeof(PxU32) * 3;


	PxTriangleMesh* triMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());
	if (!triMesh) {
		Logger::error("triMesh create fail.");
	}

	meshToRenderModel[triMesh] = this->renderModel;
	return triMesh;
}


PxRigidActor* ObjLoader::createStaticActorAndAddToScene()
{
	PxTriangleMesh* mesh;
	if (preLoad) //从cooking中读取
		mesh = readTriangleMeshFromCookingFile();
	else  //即时创建
		mesh = createTriangleMesh(scale);

	// 创建出它的几何体
	PxTriangleMeshGeometry geom(mesh);
	// 创建网格面
	PxRigidStatic* TriangleMesh = gPhysics->createRigidStatic(PxTransform(initPos));

	// 创建三角网格形状 *gMaterial
	PxShape* shape = gPhysics->createShape(geom, *gMaterial);

	{
		// 设置厚度， 相当于多了一层 0.03厚的皮肤，也就是为了提前预判
		shape->setContactOffset(0.03f);
		// A negative rest offset helps to avoid jittering when the deformed mesh moves away from objects resting on it.
		// 允许穿透的厚度，当穿透指定的厚度后，就是发生弹开等动作 -0.02f 负数代表穿透后，正数代表穿透前
		shape->setRestOffset(-0.02f);
	}

	TriangleMesh->attachShape(*shape);
	shape->release();

	//TriangleMesh->userData = new int;
	//TriangleMesh->userData = TriangleMesh;
	TriangleMesh->setName("map");
	
	int testid = 88888888;
	//memcpy(TriangleMesh->userData, &testid, sizeof(int));



	gScene->addActor(*TriangleMesh);
	return TriangleMesh;
}

physx::PxConvexMesh* ObjLoader::createConvexMesh(physx::PxVec3  scale) {
	const PxU32 numVertices = this->v.size();
	const PxU32 numTriangles = this->f.size();

	PxVec3* vertices = new PxVec3[numVertices];
	PxU32* indices = new PxU32[numTriangles * 3];


	// 加载顶点
	for (int i = 0; i < numVertices; ++i) {
		PxVec3 vectmp(this->v[i].x * scale.x, this->v[i].y * scale.y, this->v[i].z * scale.z);
		vertices[i] = vectmp;
	}
	//memcpy(vertices + 1, &objtmp->v[0], sizeof(PxVec3)* (numVertices));

	// 加载面
	auto faceIt = this->f.begin();
	for (int i = 0; i < numTriangles && faceIt != this->f.end(); faceIt++, ++i) {
		for (int j = 0; j < 3; j++)
		{
			if ((*faceIt).size() >= j + 1) {
				indices[i * 3 + j] = (*faceIt)[j].u;
			}
		}
	}

	PxConvexMeshDesc meshDesc;
	meshDesc.points.count = numVertices;
	meshDesc.points.data = vertices;
	meshDesc.points.stride = sizeof(PxVec3);

	meshDesc.indices.count = numTriangles;
	meshDesc.indices.data = indices;
	meshDesc.indices.stride = 3 * sizeof(PxU32);

	meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	bool res = gCooking->validateConvexMesh(meshDesc);
	if (!res) {
		Logger::error("invalid Convex Mesh Desc!");
	}
	PX_ASSERT(res);

	PxDefaultMemoryOutputStream buf;
	PxConvexMeshCookingResult::Enum result;

	if (!gCooking->cookConvexMesh(meshDesc, buf, &result)) {
		Logger::error("cook convex mesh fail!");
	}
	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);
	meshToRenderModel[convexMesh] = this->renderModel;

	return convexMesh;

}

PxRigidActor* ObjLoader::createDynamicActorAndAddToScene()
{
	PxConvexMesh* mesh;
	if (preLoad) //从cooking中读取
		mesh = readConvexMeshFromCookingFile();
	else  //即时创建
		mesh = createConvexMesh(scale);


	// 创建出它的几何体
	PxConvexMeshGeometry geom(mesh);
	// 创建网格面
	PxRigidDynamic* convexMesh = gPhysics->createRigidDynamic(PxTransform(initPos));

	// 创建三角网格形状 *gMaterial
	//PxShape* shape = gPhysics->createShape(geom, *gMaterial);
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*convexMesh,
		PxConvexMeshGeometry(mesh), *gMaterial);

	{
		// 设置厚度， 相当于多了一层 0.03厚的皮肤，也就是为了提前预判
		shape->setContactOffset(0.03f);
		// A negative rest offset helps to avoid jittering when the deformed mesh moves away from objects resting on it.
		// 允许穿透的厚度，当穿透指定的厚度后，就是发生弹开等动作 -0.02f 负数代表穿透后，正数代表穿透前
		shape->setRestOffset(-0.02f);
	}

	convexMesh->attachShape(*shape);
	shape->release();

	//convexMesh->userData = new int;
	//convexMesh->userData = 
	int testid = 88888888;
	//memcpy(convexMesh->userData, &testid, sizeof(int));

	gScene->addActor(*convexMesh);

	return convexMesh;
}




int ObjLoader::writeTriangleMeshToCookingFile()
{
	if (!this)
		return -1;
	const PxU32 numVertices = this->v.size();
	const PxU32 numTriangles = this->f.size();

	PxVec3* vertices = new PxVec3[numVertices];
	PxU32* indices = new PxU32[numTriangles * 3];

	// 加载顶点
	for (int i = 0; i < numVertices; ++i) {
		PxVec3 vectmp(this->v[i].x * scale.x, this->v[i].y * scale.y, this->v[i].z * scale.z);
		vertices[i] = vectmp;
	}
	//memcpy(vertices + 1, &objtmp->v[0], sizeof(PxVec3)* (numVertices));

	// 加载面
	auto faceIt = this->f.begin();
	for (int i = 0; i < numTriangles && faceIt != this->f.end(); faceIt++, ++i) {
		indices[i * 3 + 0] = (*faceIt)[0].u;
		indices[i * 3 + 1] = (*faceIt)[1].u;
		if ((*faceIt).size() >= 3)
			indices[i * 3 + 2] = (*faceIt)[2].u;
	}


	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = numVertices;
	meshDesc.points.data = vertices;
	meshDesc.points.stride = sizeof(PxVec3);

	meshDesc.triangles.count = numTriangles;
	meshDesc.triangles.data = indices;
	meshDesc.triangles.stride = sizeof(PxU32) * 3;



	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = gCooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
		return -1;

	delete[] vertices;
	delete[] indices;

	// 将流写入到文件
	FILE * filefd = fopen((this->name + COOKING_FILE_SUFFIX).c_str(), "wb+");
	if (!filefd) {
		Logger::error("open cooking read fail");
		return -1;
	}

	int size = writeBuffer.getSize();
	fwrite(&size, sizeof(unsigned int), 1, filefd);

	fwrite(writeBuffer.getData(), sizeof(PxU8), size, filefd);
	fclose(filefd);
	return 0;
}

physx::PxTriangleMesh*  ObjLoader::readTriangleMeshFromCookingFile() {
	ObjLoader *objtmp1 = this;
	// 输入流
	FILE * filefd = fopen((this->name + COOKING_FILE_SUFFIX).c_str(), "rb+");
	if (!filefd) {
		Logger::error("file open fail :" + (this->name + COOKING_FILE_SUFFIX));
		return NULL;
	}
	int rsize = 0;
	fread(&rsize, sizeof(unsigned int), 1, filefd);
	std::cout << "rsize:" << rsize << std::endl;
	Logger::info("cooking file read size:" + to_string(rsize));
	PxU8 *filebuff = new PxU8[rsize + 1];
	fread(filebuff, sizeof(PxU8), rsize, filefd);

	PxDefaultMemoryInputData readBuffer(filebuff, rsize);
	PxTriangleMesh * triangle = gPhysics->createTriangleMesh(readBuffer);
	delete[] filebuff;
	return triangle;
}



int ObjLoader::writeConvexMeshToCookingFile() {
	return 0;
}


physx::PxConvexMesh* ObjLoader::readConvexMeshFromCookingFile() {
	return nullptr;
}