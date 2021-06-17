#include <vector>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>

#include <math.h>

#include "ObjLoader.h"


using namespace std;

std::unordered_map<PxBase*, BaseModel*> ObjLoader::meshToRenderModel;
std::mutex queue_mutex;

volatile bool CookThread::cook_thread_exit_flag = false;
std::deque<CookThread::CookTask>* CookThread::tasks = new std::deque<CookThread::CookTask>();
std::vector<CookThread*>* CookThread::threads = new std::vector<CookThread*>;

extern PxPhysics* gPhysics;
extern PxCooking* gCooking;
extern PxScene* gScene;
extern PxMaterial* gMaterial;


ObjLoader::ObjLoader(BaseModel* renderModel, MESH_TYPE type) {
	//初始化
	this->renderModel = renderModel;
	this->obj_file_path = renderModel->getModelPath();
	this->name = obj_file_path.substr(obj_file_path.find_last_of("/") + 1);
	this->triangle_cooking_file_path = COOKING_DIR + COOKING_TRIANGLE_PREFIX + this->name + COOKING_FILE_SUFFIX;
	this->convex_cooking_file_path = COOKING_DIR + COOKING_CONVEX_PREFIX + this->name + COOKING_FILE_SUFFIX;
	this->scale = glmVec3ToPxVec3(renderModel->getScaleValue());
	this->initPos = glmVec3ToPxVec3(renderModel->getPosition());
	this->is_triangle_cooked = FileUtils::isFileExist(this->triangle_cooking_file_path);
	this->is_convex_cooked = FileUtils::isFileExist(this->convex_cooking_file_path);

	bool is_need_load_obj_file = !(type == MESH_TYPE::TRIANGLE ? this->is_triangle_cooked : this->is_convex_cooked);

	if (is_need_load_obj_file) {// 对应mesh的cooking文件不存在,需要重新加载obj文件
		parseObjFile();
	}
	Logger::info("开始创建Mesh");
	if (type == MESH_TYPE::TRIANGLE) {
		genTriangleMesh(this->scale);
		//cookTriangleMesh(this->triangle_mesh_desc, this->triangle_cooking_file_path);
	}
	if (type == MESH_TYPE::CONVEX) {
		genConvexMesh(this->scale);
		//cookConvexMesh(this->convex_mesh_desc, this->convex_cooking_file_path);
	}
	Logger::info("Mesh加载完成");

	if (is_need_load_obj_file) {// 需要将mesh数据写出到cooking文件
		Logger::debug("准备开启线程cook");
		CookThread::CookTask cookTask;
		cookTask.tm = this->triangle_mesh_desc;
		cookTask.cm = this->convex_mesh_desc;
		cookTask.cook_file_path = (type == MESH_TYPE::TRIANGLE ? this->triangle_cooking_file_path : this->convex_cooking_file_path);
		CookThread::newInstance()->start();
		CookThread::newInstance()->addTask(cookTask);
	}

}

ObjLoader::~ObjLoader() {
	v.clear();
	f.clear();
}

void ObjLoader::free_memory() {
	delete renderModel;
}

physx::PxTriangleMesh* ObjLoader::genTriangleMesh(physx::PxVec3 scale) {
	if (this->is_triangle_cooked) {//从cooking中读取
		this->triangle_mesh = readTriangleMeshFromCookingFile();
		Logger::debug("存在cooking文件");
	}
	else {
		Logger::debug("不存在cooking文件");
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

		PxTriangleMeshDesc* meshDesc = new PxTriangleMeshDesc();
		meshDesc->points.count = numVertices;
		meshDesc->points.data = vertices;
		meshDesc->points.stride = sizeof(PxVec3);

		meshDesc->triangles.count = numTriangles;
		meshDesc->triangles.data = indices;
		meshDesc->triangles.stride = sizeof(PxU32) * 3;

		this->triangle_mesh_desc = meshDesc;
		this->triangle_mesh = gCooking->createTriangleMesh(*meshDesc, gPhysics->getPhysicsInsertionCallback());
		if (!this->triangle_mesh) {
			Logger::error("triMesh create fail.");
		}
	}
	meshToRenderModel[this->triangle_mesh] = this->renderModel;
	return this->triangle_mesh;
}


PxRigidActor* ObjLoader::createStaticActorAndAddToScene() {

	// 创建出它的几何体
	PxTriangleMeshGeometry geom(this->triangle_mesh);
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


bool ObjLoader::cookTriangleMesh(PxTriangleMeshDesc* meshDesc, std::string cook_file_path) {
	int rt = writeTriangleMeshToCookingFile(meshDesc, cook_file_path);
	if (rt == -1) {
		Logger::error("cook triangle mesh fail!");
		return false;
	}
	if (rt == 1) {
		Logger::notice("cooking file already existd.");
		return false;
	}
	if (rt == 2) {
		Logger::notice("TriangleMeshDesc is null!");
		return false;
	}
	if (rt == 3) {
		Logger::notice("TriangleMeshDesc is invalid!");
		return false;
	}
	return rt == 0;
}

int ObjLoader::writeTriangleMeshToCookingFile(PxTriangleMeshDesc* meshDesc, std::string cook_file_path) {
	if (FileUtils::isFileExist(cook_file_path))
		return 1;
	if (meshDesc == nullptr)
		return 2;
	if (!meshDesc->isValid())
		return 3;
	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = gCooking->cookTriangleMesh(*meshDesc, writeBuffer, &result);
	if (!status) return -1;

	// 将流写入到文件
	FILE * filefd = fopen(cook_file_path.c_str(), "wb+");
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

physx::PxTriangleMesh* ObjLoader::readTriangleMeshFromCookingFile() {
	// 输入流
	FILE * filefd = fopen(this->triangle_cooking_file_path.c_str(), "rb+");
	if (!filefd) {
		Logger::error("cooking file open fail :" + this->triangle_cooking_file_path);
		return nullptr;
	}
	int rsize = 0;
	fread(&rsize, sizeof(unsigned int), 1, filefd);
	Logger::info(this->triangle_cooking_file_path + ":cooking file read size:" + to_string(rsize));
	PxU8 *filebuff = new PxU8[rsize + 1];
	fread(filebuff, sizeof(PxU8), rsize, filefd);

	PxDefaultMemoryInputData readBuffer(filebuff, rsize);
	PxTriangleMesh * triangle = gPhysics->createTriangleMesh(readBuffer);
	delete[] filebuff;
	return triangle;
}









physx::PxConvexMesh* ObjLoader::genConvexMesh(physx::PxVec3  scale) {
	if (this->is_convex_cooked) {//从cooking中读取
		this->convex_mesh = readConvexMeshFromCookingFile();
		Logger::debug("存在cooking");
	}
	else {
		Logger::debug("不存在cooking");
		const PxU32 numVertices = this->v.size();
		const PxU32 numTriangles = this->f.size();
		PxVec3* vertices = new PxVec3[numVertices];
		//PxU32* indices = new PxU32[numTriangles * 3];

		// 加载顶点
		for (int i = 0; i < numVertices; ++i) {
			PxVec3 vectmp(this->v[i].x * scale.x, this->v[i].y * scale.y, this->v[i].z * scale.z);
			vertices[i] = vectmp;
		}

		// 加载面
		/*auto faceIt = this->f.begin();
		for (int i = 0; i < numTriangles && faceIt != this->f.end(); faceIt++, ++i) {
			for (int j = 0; j < 3; j++)
				if ((*faceIt).size() >= j + 1)
					indices[i * 3 + j] = (*faceIt)[j].u;
		}*/

		PxConvexMeshDesc* meshDesc = new PxConvexMeshDesc();
		meshDesc->points.count = numVertices;
		meshDesc->points.data = vertices;
		meshDesc->points.stride = sizeof(PxVec3);

		/*meshDesc->indices.count = numTriangles;
		meshDesc->indices.data = indices;
		meshDesc->indices.stride = 3 * sizeof(PxU32);*/

		meshDesc->flags = PxConvexFlag::eCOMPUTE_CONVEX;

		bool res = gCooking->validateConvexMesh(*meshDesc);
		if (!res) {
			Logger::error("invalid Convex Mesh Desc!");
		}
		//PX_ASSERT(res);

		PxDefaultMemoryOutputStream buf;
		PxConvexMeshCookingResult::Enum result;

		if (!gCooking->cookConvexMesh(*meshDesc, buf, &result)) {
			Logger::error("cook convex mesh fail!");
		}
		this->convex_mesh_desc = meshDesc;
		PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		this->convex_mesh = gPhysics->createConvexMesh(input);
	}
	meshToRenderModel[convex_mesh] = this->renderModel;
	return convex_mesh;
}

PxRigidActor* ObjLoader::createDynamicActorAndAddToScene() {
	// 创建出它的几何体
	PxConvexMeshGeometry geom(this->convex_mesh);
	// 创建网格面
	PxRigidDynamic* convexMesh = gPhysics->createRigidDynamic(PxTransform(initPos));

	// 创建三角网格形状
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*convexMesh, geom, *gMaterial);

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



bool  ObjLoader::cookConvexMesh(PxConvexMeshDesc* meshDesc, std::string cook_file_path) {
	int rt = writeConvexMeshToCookingFile(meshDesc, cook_file_path);
	if (rt == -1) {
		Logger::error("cook convex mesh fail!");
		return false;
	}
	if (rt == 1) {
		Logger::notice("cooking file already existd.");
		return false;
	}
	if (rt == 2) {
		Logger::notice("ConvexMeshDesc is null!");
		return false;
	}
	if (rt == 3) {
		Logger::notice("ConvexMeshDesc is invalid!");
		return false;
	}
	return rt == 0;
}

int ObjLoader::writeConvexMeshToCookingFile(PxConvexMeshDesc* meshDesc, std::string cook_file_path) {
	if (FileUtils::isFileExist(cook_file_path))
		return 1;
	if (meshDesc == nullptr)
		return 2;
	if (!meshDesc->isValid())
		return 3;
	PxDefaultMemoryOutputStream writeBuffer;
	PxConvexMeshCookingResult::Enum result;
	bool status = gCooking->cookConvexMesh(*meshDesc, writeBuffer, &result);
	if (!status) return -1;

	// 将流写入到文件
	FILE * filefd = fopen(cook_file_path.c_str(), "wb+");
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

physx::PxConvexMesh* ObjLoader::readConvexMeshFromCookingFile() {
	// 输入流
	FILE * filefd = fopen(this->convex_cooking_file_path.c_str(), "rb+");
	if (!filefd) {
		Logger::error("cooking file open fail :" + this->convex_cooking_file_path);
		return nullptr;
	}
	int rsize = 0;
	fread(&rsize, sizeof(unsigned int), 1, filefd);
	Logger::info(this->convex_cooking_file_path + ":cooking file read size:" + to_string(rsize));
	PxU8 *filebuff = new PxU8[rsize + 1];
	fread(filebuff, sizeof(PxU8), rsize, filefd);

	PxDefaultMemoryInputData readBuffer(filebuff, rsize);
	PxConvexMesh * convex = gPhysics->createConvexMesh(readBuffer);
	delete[] filebuff;
	return convex;
}









DWORD WINAPI run(LPVOID lpParamter) {
	std::deque<CookThread::CookTask>* tasks = (std::deque<CookThread::CookTask>*)lpParamter;
	int continuous_sleep_times = 0;
	while (!CookThread::getExitFlag())
	{
		if (tasks == nullptr || tasks->empty())
		{
			Sleep(60);
			if (++continuous_sleep_times == 500) { //连续休眠达到500次，即30秒时，退出
				CookThread::remove_thread(GetCurrentThread());
				break;
			}
			continue;
		}
		continuous_sleep_times = 0;
		Logger::debug("consume task");
		queue_mutex.lock();
		CookThread::CookTask task = tasks->front();
		tasks->pop_front();
		queue_mutex.unlock();
		if (task.tm != nullptr && task.tm->isValid()) {
			ObjLoader::cookTriangleMesh(task.tm, task.cook_file_path);
			delete task.tm;
		}
		if (task.cm != nullptr && task.cm->isValid()) {
			ObjLoader::cookConvexMesh(task.cm, task.cook_file_path);
			delete task.cm;
		}
	}
	return 0;
}






void ObjLoader::parseObjFile() {
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

	Logger::info("顶点数:" + std::to_string(v.size()) + "   三角面片数:" + std::to_string(f.size()));
}