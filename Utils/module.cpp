#include "module.h"
#include <PsString.h>
module::module():shape(NULL),body0(NULL) {
	//printf("Ĭ�Ϲ��캯��1\n");
}

module::module(PxTransform o, PxShape* s, PxTransform l, PxRigidActor* b0):
	ori(o), 
	shape(s), 
	local(l), 
	body0(b0) {
	//printf("Ĭ�Ϲ��캯��2\n");
}

module::~module() {

}

//void module::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
//{
//	PX_UNUSED(pairHeader);
//	PX_UNUSED(pairs);
//	PX_UNUSED(nbPairs);
//}

