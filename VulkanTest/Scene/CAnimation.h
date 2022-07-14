#pragma once
#include "CGameObject.h"

class CAnimation
{
	public:
		const std::string	name;
		const Double		duriation;
		const Uint			ticks_per_secound;

		std::map<std::string, aiNodeAnim*> channels;
	public:

		CAnimation(aiAnimation* ai_animation_) : 
			name(ai_animation_->mName.data),
			duriation(ai_animation_->mDuration),
			ticks_per_secound(ai_animation_->mTicksPerSecond)
		{
			for (Uint i = 0; i < ai_animation_->mNumChannels ; i++)
			{
				aiNodeAnim* channel = new aiNodeAnim;
				*channel = *ai_animation_->mChannels[i];
				
				//Copy to new keys buffers...
				channel->mPositionKeys = new aiVectorKey[channel->mNumPositionKeys];
				channel->mRotationKeys = new aiQuatKey[channel->mNumRotationKeys];
				channel->mScalingKeys  = new aiVectorKey[channel->mNumScalingKeys];

				for (Uint j = 0 ; j < ai_animation_->mChannels[i]->mNumPositionKeys; j ++)
					channel->mPositionKeys[j] = ai_animation_->mChannels[i]->mPositionKeys[j];

				for (Uint j = 0; j < ai_animation_->mChannels[i]->mNumRotationKeys; j ++)
					channel->mRotationKeys[j] = ai_animation_->mChannels[i]->mRotationKeys[j];

				for (Uint j = 0; j < ai_animation_->mChannels[i]->mNumScalingKeys; j ++)
					channel->mScalingKeys[j] = ai_animation_->mChannels[i]->mScalingKeys[j];

				channels.insert(std::pair<std::string, aiNodeAnim*>(std::string(channel->mNodeName.data), channel));
			}
		}
};
