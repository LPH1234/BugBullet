#include <vector>
#include <iostream> 
#include <string>
#include <fstream>
#include <sstream>

#include <math.h>

#include "ObjLoader.h"

using namespace std;




ObjLoader::ObjLoader(std::string &filename, PxPhysics* gPhysics, PxCooking*	gCooking, PxScene* gScene, PxMaterial* gMaterial, int scale, bool preLoad) {
	this->gPhysics = gPhysics;
	this->gCooking = gCooking;
	this->gScene = gScene;
	this->gMaterial = gMaterial;
	this->name = filename.substr(filename.find_last_of("/") + 1);
	this->scale = scale;
	this->preLoad = preLoad;
	Logger::debug("打开模型文件：" + name);

	ifstream file(filename);
	string line;
	int count = 0;
	while (getline(file, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			struct vertices vtmp = { 0 };
			count++;
			istringstream s(line.substr(2));
			// v -1.038081 -0.8783139 -2.667491
			s >> vtmp.x >> vtmp.y >> vtmp.z;
			if (vtmp.x == 0 || vtmp.y == 0 || vtmp.z == 0) {
				cout << "0:index:" << count << "line:" << line.substr(2) << endl;;
			}
			v.push_back(vtmp);
		}
		else if (line.substr(0, 1) == "f")
		{
			vector<struct face> vface;
			count++;
			//vector<struct face> vface;
			istringstream s(line.substr(2));
			string tmpdata[3];
			// f 25/25/25     28/28/28           26/26/26
			s >> tmpdata[0] >> tmpdata[1] >> tmpdata[2];
			for (int i = 0; i < 3; i++)
			{
				struct face faceTmp;
				istringstream sdata(tmpdata[i]);
				string buffer;
				// sdata里面存储的   25/25/25
				int j = 0;
				while (getline(sdata, buffer, '/'))
				{
					// 得到的u 就是25
					faceTmp.u = atoi(buffer.c_str()) - 1;
					break;
				}
				vface.push_back(faceTmp);
			}
			f.push_back(vface);
		}
		if (count % 10000 == 0)
		{
			cout << "count : " << count << endl;
			count++;
		}

	}
	file.close();
	cout << "count : " << count << "     v num: " << v.size() << "     f num: " << f.size() << endl;

	if (preLoad) {
		Logger::info("开始写入cooking文件中...");
		writeMeshToCookingFile();
		Logger::info("完成");
	}
}

ObjLoader::~ObjLoader() {
	v.clear();
	f.clear();
}

physx::PxTriangleMesh* ObjLoader::createOjbMesh(int scale)
{

	const PxU32 numVertices = this->v.size();
	const PxU32 numTriangles = this->f.size();

	PxVec3* vertices = new PxVec3[numVertices];
	PxU32* indices = new PxU32[numTriangles * 3];

	// 加载顶点
	for (int i = 0; i < numVertices; ++i) {
		PxVec3 vectmp(this->v[i].x * scale, this->v[i].y * scale, this->v[i].z * scale);
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

	PxTriangleMeshDesc meshDesc1;
	meshDesc1.points.count = numVertices;
	meshDesc1.points.data = vertices;
	meshDesc1.points.stride = sizeof(PxVec3);

	meshDesc1.triangles.count = numTriangles;
	meshDesc1.triangles.data = indices;
	meshDesc1.triangles.stride = sizeof(PxU32) * 3;


	PxTriangleMesh* triMesh = gCooking->createTriangleMesh(meshDesc1, gPhysics->getPhysicsInsertionCallback());

	return triMesh;
}


PxRigidActor* ObjLoader::createActorAndAddToScene(PxVec3 &offset)
{
	PxTriangleMesh* mesh;
	if (preLoad) //从cooking中读取
		mesh = readCookingFile();
	else  //即时创建
		mesh = createOjbMesh(scale);


	// 创建出它的几何体
	PxTriangleMeshGeometry geom(mesh);
	// 创建网格面
	PxRigidStatic* TriangleMesh = gPhysics->createRigidStatic(PxTransform(offset));

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

	TriangleMesh->userData = new int;
	int testid = 88888888;
	memcpy(TriangleMesh->userData, &testid, sizeof(int));

	gScene->addActor(*TriangleMesh);

	return TriangleMesh;
}



int ObjLoader::writeMeshToCookingFile()
{
	if (!this)
		return -1;
	const PxU32 numVertices = this->v.size();
	const PxU32 numTriangles = this->f.size();

	PxVec3* vertices = new PxVec3[numVertices];
	PxU32* indices = new PxU32[numTriangles * 3];

	// 加载顶点
	for (int i = 0; i < numVertices; ++i) {
		PxVec3 vectmp(this->v[i].x * scale, this->v[i].y * scale, this->v[i].z * scale);
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


	PxTriangleMeshDesc meshDesc1;
	meshDesc1.points.count = numVertices;
	meshDesc1.points.data = vertices;
	meshDesc1.points.stride = sizeof(PxVec3);

	meshDesc1.triangles.count = numTriangles;
	meshDesc1.triangles.data = indices;
	meshDesc1.triangles.stride = sizeof(PxU32) * 3;



	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = gCooking->cookTriangleMesh(meshDesc1, writeBuffer, &result);
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

PxTriangleMesh* ObjLoader::readCookingFile() {
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
