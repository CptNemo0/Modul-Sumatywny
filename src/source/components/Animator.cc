#include "./headers/components/Animator.h"

components::Animator::Animator(anim::Animation* currentAnimation)
{
    m_CurrentTime = 0.0;
    m_CurrentAnimation = currentAnimation;

    m_FinalBoneMatrices.reserve(MAX_BONES);

    for (int i = 0; i < MAX_BONES; i++)
        m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}


void components::Animator::UpdateAnimation(float dt)
{
	m_CurrentTime += dt;

	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void components::Animator::PlayAnimation(anim::Animation* animation)
{
	m_CurrentAnimation = animation;
	m_CurrentTime = 0.0f;
}

void components::Animator::CalculateBoneTransform(const anim::AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}