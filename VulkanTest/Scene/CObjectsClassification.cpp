#include "CScene.h"

//The next function search object by name in forward order inside the entry's famely(That mean children + previous siblings)
CObject* SearchObjectForwardInFamely(std::string object_name_, CObject* entry_)
{
	CObject* found;

	if (object_name_ == entry_->GetName())
		return entry_;//Object is found!

	if (entry_->GetChildrenNumber() > 0)
	{
		found = SearchObjectForwardInFamely(object_name_, entry_->GetFirstChild());
		if (found != nullptr)
			return found;
	}

	CObject* next_sibling = entry_->GetNextSibling();
	if (next_sibling != nullptr)
	{
		found = SearchObjectForwardInFamely(object_name_, next_sibling);
		if (found)
			return found;
	}

	return nullptr;
}

//The next function search object by name in reverse order
CObject* SearchObjectForward(std::string object_name_, CObject* entry_)
{
	CObject* found;

	found = SearchObjectForwardInFamely(object_name_, entry_);
	if (found != nullptr)
		return found;

	CObject* next_uncle = entry_->GetParent()->GetNextSibling();
	if (next_uncle != nullptr)
	{
		found = SearchObjectForward(object_name_, next_uncle);//Now the uncle search for the object in his famely
		if (found)
			return found;
	}

	return nullptr;
}

Void CScene::ClassifyBones(CObject* entry_)
{
	for (Uint i = 0; i < entry_->GetMeshsNumber(); i++)
	{
		for (Uint j = 0; j < entry_->GetMesh(i)->GetBonesNumber(); j++)
		{
			CObject* found = SearchObjectForward(entry_->GetMesh(i)->GetBoneName(j), root_object);
			if (found != nullptr)
			{
				CBone* BoneClassification = dynamic_cast<CBone*>(found);
				if(BoneClassification == nullptr)//If the found object class is not CBone
					BoneClassification = new CBone(found);//Convert found class to CBone

				BoneClassification->influences.push_back(entry_);
			}
		}
	}

	for (Uint i = 0; i < entry_->GetChildrenNumber(); i ++)
		ClassifyBones(entry_->GetChild(i));
}



Void CScene::ClassifyArmatures(CObject* entry_, VkDevice device_)
{
	for (Uint i = 0; i < entry_->GetChildrenNumber(); i ++)
	{
		//If one of this entry's child is bone -> Convert entry's class to CArmature
		if (dynamic_cast<CBone*>(entry_->GetChild(i)) != nullptr)
		{
			CArmature* ArmatureClassification = new CArmature(entry_);
			ArmatureClassification->CreateBonesTransforms();
			return;
		}
		else
		{
			ClassifyArmatures(entry_->GetChild(i), device_);
		}
	}
}