#include "../../Headers/Entity/Behavior.h"
#include "../../Headers/UI/Billboard.h"
#include "../../Headers/World/GameObject.h"
#include "../../Headers/Resource/Resources.h"
#include "../../Headers/Helper/StringUtils.h"
using namespace osomi;

Behavior::~Behavior() {
	for (u32 i = 0; i < bill.size(); i++)
		helper->remove(bill[i]->getName());
}
Billboard *Behavior::addBillboard(GameObject *go, std::string tex, vec3 pos, vec3 scale, vec4 uv, vec3 colorizer) {
	if (!Resources::hasTexture(tex))return nullptr;
	Billboard *bb = helper->add(StringUtils::numstring(bill.size(), go->getName() + "." + getName() + " #", ""), Resources::fetchTexture(tex), pos, scale, vec3(), uv, colorizer);
	if (bb == nullptr)return nullptr;
	bb->link(go);
	bill.push_back(bb);
	return bb;
}