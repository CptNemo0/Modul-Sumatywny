#include "../../headers/physics/Rope.h"

Rope::Rope(glm::vec3 start, glm::vec3 dir, int rope_length, float segment_mass, float segment_drag)
{

}

Rope::Rope(glm::vec3 start, glm::vec3 end, float segment_mass, float segment_drag, std::shared_ptr<GameObject> scene_root, std::shared_ptr<Model> model, std::shared_ptr<Shader> shader)
{
	this->segment_drag_ = segment_drag;
	this->segment_mass_ = segment_mass;

	CreateSegments(start, end, scene_root, model, shader);
}

void Rope::HelperConstrainer(std::shared_ptr<components::PBDParticle> p1, std::shared_ptr<components::PBDParticle> p2)
{
	pbd::RopeConstraint* constraint = pbd::PBDManager::i_->CreateRopeConstraint(p1, p2, kDistance + 0.001f);
	constraints_.push_back(constraint);
}



void Rope::CreateSegments(glm::vec3 start, glm::vec3 end, std::shared_ptr<GameObject> scene_root, std::shared_ptr<Model> model, std::shared_ptr<Shader> shader)
{
	float distance = glm::distance(start, end);
	glm::vec3 player_dir = glm::normalize(end - start);


	int i = 0;
	

	while (distance > kDistance)
	{
		auto rope_segment = GameObject::Create(scene_root);
		rope_segment->transform_->set_scale(glm::vec3(1.3f, 1.3f, 1.3f));
		rope_segment->transform_->TeleportToPosition(start + player_dir * kDistance * (float)i);
		rope_segment->AddComponent(make_shared<components::MeshRenderer>(model, shader));
		rope_segment->AddComponent(collisions::CollisionManager::i_->CreateCollider(2, gPRECISION, model->meshes_[0], rope_segment->transform_));
		rope_segment->AddComponent(pbd::PBDManager::i_->CreateParticle(segment_mass_, segment_drag_, rope_segment->transform_));

		if (i > 0)
		{
			auto constraint = pbd::PBDManager::i_->CreateRopeConstraint(rope_segments_.back()->GetComponent<components::PBDParticle>(), rope_segment->GetComponent<components::PBDParticle>(), kDistance + 0.001f);
			constraints_.push_back(constraint);
			//HelperConstrainer(rope_segments_.back()->GetComponent<components::PBDParticle>(), rope_segment->GetComponent<components::PBDParticle>());
		}

		rope_segments_.push_back(rope_segment);

		distance -= kDistance;
		i++;
	}
}

void Rope::AssignPlayerBegin(std::shared_ptr<GameObject> player_begin)
{
	auto front_segment = rope_segments_.front();
	auto segment_position = front_segment->transform_->get_position();
	auto player_position = player_begin->transform_->get_position();

	if (glm::distance(segment_position, player_position) != 0)
	{
		auto dir = glm::normalize(player_position - segment_position);
		player_begin->transform_->TeleportToPosition(segment_position + kDistance * dir);
	}

	auto player_particle = player_begin->GetComponent<components::PBDParticle>();
	auto segment_particle = front_segment->GetComponent<components::PBDParticle>();
	auto constraint = pbd::PBDManager::i_->CreateRopeConstraint(player_particle, segment_particle, kDistance + 0.001f);
	constraints_.insert(constraints_.begin(), constraint);
}

void Rope::AssignPlayerEnd(std::shared_ptr<GameObject> player_end)
{
	auto back_segment = rope_segments_.back();
	auto segment_position = back_segment->transform_->get_position();
	auto player_position = player_end->transform_->get_position();

	if (glm::distance(segment_position, player_position) != 0)
	{
		auto dir = glm::normalize(player_position - segment_position);
		player_end->transform_->TeleportToPosition(segment_position + kDistance * dir);
	}

	auto player_particle = player_end->GetComponent<components::PBDParticle>();
	auto segment_particle = back_segment->GetComponent<components::PBDParticle>();

	pbd::RopeConstraint* constraint = pbd::PBDManager::i_->CreateRopeConstraint(segment_particle, player_particle, kDistance + 0.001f);
	constraints_.push_back(constraint);
}

int Rope::Size()
{
	return rope_segments_.size();
}

void Rope::ApplyMass()
{
	float inverse_mass = 1.0f / segment_mass_;
	for (auto segment : rope_segments_)
	{
		segment->GetComponent<components::PBDParticle>()->mass_ = segment_mass_;
		segment->GetComponent<components::PBDParticle>()->inverse_mass_ = inverse_mass;
	}
}

void Rope::ApplyDrag()
{
	for (auto segment : rope_segments_)
	{
		segment->GetComponent<components::PBDParticle>()->damping_factor_ = segment_drag_;
	}
}

void Rope::AddSegment(std::shared_ptr<GameObject> scene_root, std::shared_ptr<Model> model, std::shared_ptr<Shader> shader)
{
	
	auto last_constraint = constraints_.back();

	auto last_segment_particle = last_constraint->p1_;
	auto player_particle = last_constraint->p2_;

	auto new_position = last_segment_particle->transform_->get_position() + (player_particle->transform_->get_position() - last_segment_particle->transform_->get_position()) * 0.5f;

	auto rope_segment = GameObject::Create(scene_root);
	rope_segment->transform_->set_scale(glm::vec3(1.3f, 1.3f, 1.3f));
	rope_segment->transform_->TeleportToPosition(new_position);
	rope_segment->AddComponent(make_shared<components::MeshRenderer>(model, shader));
	rope_segment->AddComponent(collisions::CollisionManager::i_->CreateCollider(2, gPRECISION, model->meshes_[0], rope_segment->transform_));
	rope_segment->AddComponent(pbd::PBDManager::i_->CreateParticle(segment_mass_, segment_drag_, rope_segment->transform_));
	rope_segment->PropagateStart();
	rope_segments_.push_back(rope_segment);

	auto new_particle = rope_segment->GetComponent<components::PBDParticle>();

	last_constraint->p1_ = last_segment_particle;
	last_constraint->p2_ = new_particle;

	pbd::RopeConstraint* constraint = pbd::PBDManager::i_->CreateRopeConstraint(new_particle, player_particle, kDistance + 0.001f);
	constraints_.push_back(constraint);
}

void Rope::RemoveSegment()
{
	int n = constraints_.size();
	
	auto plast = constraints_[n - 2];
	auto last = constraints_[n - 1];

	auto plast_particle = plast->p1_;
	auto last_particle = plast->p2_;
	auto player_particle = last->p2_;

	plast->p2_ = player_particle;

	auto last_particle_go = last_particle->gameObject_.lock();

	last_particle_go->Destroy();

	constraints_.erase(--constraints_.end());
	rope_segments_.erase(--rope_segments_.end());
}

Rope::~Rope()
{
	rope_segments_.clear();
	constraints_.clear();
}
