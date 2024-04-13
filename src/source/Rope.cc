#include "../headers/Rope.h"

void rope::Rope::CheckRestraints(std::shared_ptr<Components::RopeSegment> a, std::shared_ptr<Components::RopeSegment> b, float t)
{
	float distance = glm::distance(a->transform_->get_position(), b->transform_->get_position());
	auto particle_a = a->transform_->game_object_->GetComponent<Components::Particle>();
	auto particle_b = b->transform_->game_object_->GetComponent<Components::Particle>();
	assert(particle_a != nullptr);
	assert(particle_b != nullptr);
	
	if (distance >= kMaxDistance)
	{
		glm::vec3 force = particle_b->transform_->get_position();
		force -= particle_a->transform_->get_position();
		float magnitude = glm::length(force);
		magnitude = std::abs(magnitude - kMaxDistance);
		magnitude *= kSpringConstant;
		glm::normalize(force);
		force *= -magnitude;
		particle_b->AddForce(force);
		particle_a->AddForce(-force);	

		if (!a->is_puller_ && !b->is_puller_)
		{
			auto vl = left_puller_->gameObject_.lock()->GetComponent<Components::Particle>()->velocity_;
			auto rl = right_puller_->gameObject_.lock()->GetComponent<Components::Particle>()->velocity_;

			if(glm::dot(vl, rl) > 0.0f)
			{
				auto v = vl + rl;
				v *= kAdditionalPull;
				particle_a->velocity_ += v; 
				particle_b->velocity_ += v;
				physics::LogVec3(v);
			}
		}
	}
}

void rope::Rope::EnforceRestraints(float t)
{
	int idx = 0;
	std::shared_ptr<Components::RopeSegment> current = left_end_->right_;

	while (TRUE)
	{
		if (current == nullptr)
		{
			break;
		}

		if (current->transform_ == nullptr)
		{
			break;
		}

		if (current->right_ == nullptr)
		{
			break;
		}

		if (current->right_->transform_ == nullptr)
		{
			break;
		}

		CheckRestraints(current, current->right_, t);

		current = current->right_;
	}

	
}

rope::Rope::Rope()
{
	this->left_end_ = std::make_shared<Components::RopeSegment>(nullptr, nullptr, nullptr);
	this->right_end_ = std::make_shared<Components::RopeSegment>(nullptr, nullptr, nullptr);
	left_end_->right_ = right_end_;
	right_end_->left_ = left_end_;
	left_end_->name_ = "LEFT END";
	right_end_->name_ = "RIGHT END";

}

void rope::Rope::AddSegment(std::shared_ptr<Components::RopeSegment> segment)
{
	right_end_->left_->right_ = segment;
	right_end_->left_ = segment;

	segment->left_ = right_end_->left_;
	segment->right_ = right_end_;
}

bool rope::Rope::InsertSegment(std::shared_ptr<Components::RopeSegment> segment, int index)
{
	assert(index <= Size());

	int idx = 0;
	std::shared_ptr<Components::RopeSegment> current = left_end_;

	while (idx != index)
	{
		current = current->right_;
		idx++;
	}

	segment->left_ = current;
	segment->right_ = current->right_;

	current->right_ = segment;
	segment->right_->left_ = segment;

	return true;
}

int rope::Rope::Size()
{
	int return_value = 0;
	std::shared_ptr<Components::RopeSegment> current = left_end_;

	while (current->right_->right_ != nullptr)
	{
		return_value++;
		current = current->right_;
	}
	return return_value;
}

void Components::RopeSegment::Start()
{
}

void Components::RopeSegment::Update()
{
}
