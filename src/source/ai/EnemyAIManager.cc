#include "../../headers/ai/EnemyAIManager.h"

ai::EnemyAIManager*			 ai::EnemyAIManager::i_ = nullptr;
std::shared_ptr<GameObject>  ai::EnemyAIManager::player_1_ = nullptr;
std::shared_ptr<GameObject>  ai::EnemyAIManager::player_2_ = nullptr;
bool						 ai::EnemyAIManager::players_set = false;

ai::EnemyAIManager::EnemyAIManager(const EnemyAIManagerInitStruct& init_struct, const Vehicle& vehicle)
{
	this->player_1_ = nullptr;
	this->player_2_ = nullptr;
	this->vehicle_template_ = vehicle;

	this->choked_tentacles_ = init_struct.choked_tentacles;
	this->multi_chokes_ = init_struct.multi_chokes;

	this->choke_threshold_ = init_struct.choke_threshold;
	this->multi_threshold_ = init_struct.multi_threshold;

	this->wall_proximity_threshold_ = init_struct.wall_proximity_threshold;

	this->attack_damage_ = init_struct.attack_damage;
	this->attack_range_ = init_struct.attack_range;

	this->sense_range_ = init_struct.sense_range;

	this->players_set = false;
}

void ai::EnemyAIManager::UpdateEnemyStateMachine(std::shared_ptr<EnemyStateMachine> machine)
{
	if (!players_set)
	{
		std::cout << "First set player_1_ and player_2_ variables of EnemyAIManager!!!\n";
	}

	assert(players_set);

	glm::vec3 to_p_1 = player_1_->transform_->get_position() - machine->transfrom_->get_position();
	float to_p_1_mag2 = glm::length2(to_p_1);

	glm::vec3 to_p_2 = player_2_->transform_->get_position() - machine->transfrom_->get_position();
	float to_p_2_mag2 = glm::length2(to_p_2);

	if (to_p_1_mag2 < to_p_2_mag2)
	{
		if (to_p_1_mag2 < sense_range_ * sense_range_)
		{
			machine->target_player_ = player_1_;
			machine->in_sense_range_ = (to_p_1_mag2 < sense_range_ * sense_range_);
			machine->in_attack_range_ = (to_p_1_mag2 < attack_range_ * attack_range_);
		}
		machine->in_sense_range_ = (to_p_1_mag2 < sense_range_ * sense_range_);
		machine->in_attack_range_ = (to_p_1_mag2 < attack_range_ * attack_range_);
	}
	else
	{
		if (to_p_2_mag2 < sense_range_ * sense_range_)
		{
			machine->target_player_ = player_2_;
			machine->in_attack_range_ = (to_p_2_mag2 < attack_range_ * attack_range_);
		}
		machine->in_sense_range_ = (to_p_2_mag2 < sense_range_ * sense_range_);
		machine->in_attack_range_ = (to_p_2_mag2 < attack_range_ * attack_range_);
	}

	machine->pursuit_ = (!(choked_tentacles_ > choke_threshold_) && !(multi_chokes_ > multi_threshold_));
	machine->extrapolation_ = (!machine->pursuit_ && ((choked_tentacles_ > choke_threshold_) && !(multi_chokes_ > multi_threshold_)));
	machine->evasive_manoeuvres_ = ((choked_tentacles_ > choke_threshold_) && (multi_chokes_ > multi_threshold_));

	
	machine->current_state_->Execute(machine.get());
}