#include "Render.h"

#include "../Utils/Utils.h"

#include "models.h"

using namespace physx;

#define MAX_NUM_MESH_VEC3S  21474836
static PxVec3 gVertexBuffer[MAX_NUM_MESH_VEC3S];

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

namespace Snippets
{

	void renderActors(PxRigidActor** actors, const PxU32 numActors, Shader* shader, bool shadows, const PxVec3 & color)
	{
		PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
		for (PxU32 i = 0; i < numActors; i++)
		{
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
			actors[i]->getShapes(shapes, nbShapes);
			bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

			for (PxU32 j = 0; j < nbShapes; j++)
			{
				const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
				PxGeometryHolder h = shapes[j]->getGeometry();

				if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				// render object


				if (sleeping)
				{
					//PxVec3 darkColor = color * 0.25f;
					//glColor4f(darkColor.x, darkColor.y, darkColor.z, 1.0f);
				}
				else {
					// glColor4f(color.x, color.y, color.z, 1.0f); 
				}
				renderGeometry(actors[i], h, shader);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				if (shadows)
				{
					//const PxVec3 shadowDir(0.0f, -0.7071067f, -0.7071067f);
					//const PxReal shadowMat[] = { 1,0,0,0, -shadowDir.x / shadowDir.y,0,-shadowDir.z / shadowDir.y,0, 0,0,1,0, 0,0,0,1 };
					//glPushMatrix();
					//glMultMatrixf(shadowMat);
					//glMultMatrixf(reinterpret_cast<const float*>(&shapePose));
					////glDisable(GL_LIGHTING);
					////glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
					//renderGeometry(h);
					////glEnable(GL_LIGHTING);
					//glPopMatrix();
				}
			}
		}
	}



} //namespace Snippets

PxFilterFlags testCCDFilterShader(
	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0,
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize)
{
	pairFlags = PxPairFlag::eSOLVE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
	pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT
		| PxPairFlag::eNOTIFY_TOUCH_FOUND
		| PxPairFlag::eNOTIFY_TOUCH_PERSISTS
		| PxPairFlag::eNOTIFY_CONTACT_POINTS;

	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags |= PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlags();

}
