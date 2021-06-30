#include "Render.h"

#include "../Utils/Utils.h"

#include "models.h"
#include<ctime>

using namespace physx;

#define MAX_NUM_MESH_VEC3S  21474836
static PxVec3 gVertexBuffer[MAX_NUM_MESH_VEC3S];
extern PxScene* gScene;


Cube* cube = nullptr;
Sphere* sphere = nullptr;
PlainModel* bullet = nullptr;

void renderGeometry(PxRigidActor* actor, const PxGeometryHolder& h, Shader* shader)
{
	PxTransform& t = actor->getGlobalPose();
	glm::vec3& pos = pxVec3ToGlmVec3(t.p);
	//Logger::debug("pos:   x:" + to_string(position.x) + "   y:" + to_string(position.y) + "   z:" + to_string(position.z));

	switch (h.getType())
	{
	case PxGeometryType::eBOX:
	{
		glm::vec3 scale(2 * h.box().halfExtents.x, 2 * h.box().halfExtents.y, 2 * h.box().halfExtents.z);
		if (cube == nullptr) {
			cube = new Cube(pos, scale, "", shader, "images/textures/w200Bullet.png");
		}
		cube->setPosition(pos);
		cube->setScaleValue(scale);
		cube->setRotate(t.q);
		cube->updateShaderModel();
		cube->draw();
	}
	break;
	case PxGeometryType::eSPHERE:
	{
		glm::vec3 scale(h.sphere().radius, h.sphere().radius, h.sphere().radius);
		if (sphere == nullptr) {
			sphere = new Sphere(pos, scale, "", shader, 30, 30);
		}
		sphere->setPosition(pos);
		sphere->setScaleValue(scale);
		sphere->setRotate(t.q);
		sphere->updateShaderModel();
		sphere->setColor(glm::vec3(1.f, 0.f, 0.f));
		sphere->draw();
	}
	break;
	case PxGeometryType::eCAPSULE:
	{
		if (std::strcmp(actor->getName(), ACTOR_NAME_PLAYER_BULLET.c_str()) == 0) //子弹
		{
			glm::vec3 scale(h.capsule().halfHeight * 2, h.capsule().radius * 2, h.capsule().radius * 2);
			if (bullet == nullptr)
				bullet = new PlainModel(pos, scale, "model/bullet/bullet3/bullet.obj", shader);
			bullet->setPosition(pos);
			bullet->setScaleValue(scale);
			bullet->setRotate(t.q);
			bullet->updateShaderModel();
			bullet->draw();
		}
	}
	break;
	case PxGeometryType::eCONVEXMESH:
	{
		BaseModel* model = ObjLoader::meshToRenderModel[h.convexMesh().convexMesh];
		model->setPosition(pos);
		model->setRotate(t.q);

		model->updateShaderModel();
		model->draw();
	}
	break;
	case PxGeometryType::eTRIANGLEMESH:
	{
		BaseModel* model = ObjLoader::meshToRenderModel[h.triangleMesh().triangleMesh];
		model->setPosition(pos);
		model->setRotate(t.q);

		model->updateShaderModel();
		model->draw();
	}
	break;
	case PxGeometryType::eINVALID:
	case PxGeometryType::eHEIGHTFIELD:
	case PxGeometryType::eGEOMETRY_COUNT:
	case PxGeometryType::ePLANE:
		break;
	}
}

namespace Render
{

	void renderActors(PxRigidActor** actors, const PxU32 numActors, Shader* shader, bool shadows, const PxVec3 & color)
	{
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		for (PxU32 i = 0; i < numActors; i++)
		{
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
			actors[i]->getShapes(shapes, nbShapes);
			//bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;
			for (PxU32 j = 0; j < nbShapes; j++)
			{
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				PxGeometryHolder h = shapes[j]->getGeometry();

				if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE){
					glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
				// render object
				}
				//if (sleeping) {}
				renderGeometry(actors[i], h, shader);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				//if (shadows) {}
			}
		}
	}

	void renderParticles(list<PxParticleSystem*>& particleSystemList, list<BaseParticleCluster*>& renderParticleClusterList, glm::mat4 view, glm::mat4 projection) {

		//1、物理粒子的渲染
		//-----------------------------------------------------------------------
		vector<PxParticleSystem*> remove_list; //要移除的粒子系统列表
		list<PxParticleSystem*>::iterator pIt; //声明一个迭代器
		for (pIt = particleSystemList.begin(); pIt != particleSystemList.end(); pIt++) {
			PxParticleSystem* ps = *pIt;
			// lock SDK buffers of *PxParticleSystem* ps for reading
			PxParticleReadData* rd = ps->lockParticleReadData();

			bool stop_flag = true;
			ParticleSystemData* data = reinterpret_cast<ParticleSystemData*>(ps->userData);

			// access particle data from PxParticleReadData
			if (rd)
			{
				PxStrideIterator<const PxParticleFlags> flagsIt(rd->flagsBuffer);
				PxStrideIterator<const PxVec3> positionIt(rd->positionBuffer);
				PxStrideIterator<const PxVec3> vIt(rd->velocityBuffer);
				for (unsigned i = 0; i < rd->validParticleRange; ++i, ++flagsIt, ++positionIt, ++vIt)
				{
					if (*flagsIt & PxParticleFlag::eVALID)
					{
						// access particle position
						const PxVec3& position = *positionIt;
						const PxVec3& velocity = *vIt;
						/*std::cout << str << "th\t" << i << "\t号粒子位置：" << position.x << "\t" << position.y << "\t" << position.z;
						std::cout << "\tv:" << velocity.x << "\t" << velocity.y << "\t" << velocity.z;
						cout << "\n";*/
						if (velocity.x != 0.f || velocity.y != 0.f || velocity.z != 0.f)
							stop_flag = false; //如果速度不为0

						data->renderModel->update(position, velocity);
						data->renderModel->draw(i, view, projection);
					}
				}
				// return ownership of the buffers back to the SDK
				rd->unlock();
			}

			if (stop_flag || clock() - data->createTime >= data->deleteDelaySec * 1000) {//如果这个粒子系统中所有的粒子都停止运动了或粒子系统达到了删除时间，就移除它
				remove_list.push_back(ps);
			}

		}
		//1.1、物理粒子系统的回收
		for (size_t i = 0; i < remove_list.size(); i++)
		{
			PxParticleSystem* ps = remove_list[i];
			particleSystemList.remove(ps);
			ParticleSystemData* data = reinterpret_cast<ParticleSystemData*>(ps->userData);
			delete data;
			gScene->removeActor(*ps);
			ps->releaseParticles();
			ps->release();
		}

		//2、点精灵、纯渲染粒子的渲染
		//-----------------------------------------------------------------------
		vector<BaseParticleCluster*> remove_list_c; //要移除的粒子cluster列表
		list<BaseParticleCluster*>::iterator rIt;
		for (rIt = renderParticleClusterList.begin(); rIt != renderParticleClusterList.end(); rIt++) {
			(*rIt)->update();
			(*rIt)->draw(view, projection);
			if ((*rIt)->isRemoveable())
				remove_list_c.push_back(*rIt);
		}
		//2.1、点精灵、纯渲染粒子de回收
		for (size_t i = 0; i < remove_list_c.size(); i++) {
			BaseParticleCluster* pc = remove_list_c[i];
			renderParticleClusterList.remove(pc);
			delete pc;
		}


	}


} //namespace Render

PxFilterFlags testCCDFilterShader(
	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	pairFlags = PxPairFlag::eSOLVE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT;
	/*| PxPairFlag::eNOTIFY_TOUCH_FOUND
	| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
	| PxPairFlag::eNOTIFY_CONTACT_POINTS;*/
	// generate contacts for all that were not filtered above
	//pairFlags |= PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	cout << "fiterData0.word0:" << filterData0.word0 << "filterData1.word1:" << filterData1.word1
		<< "\tand:" << (filterData0.word0 & filterData1.word1) << "\n";
	if ((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlags();

}
