#include "../../headers/collisions/CollisionManager.h"

collisions::CollisionManager* collisions::CollisionManager::i_ = nullptr;

collisions::CollisionManager::CollisionManager()
{
    colliders_ = std::vector<s_ptr<components::Collider>>();
    for (int i = 0; i < 32; i++)
    {
        collision_layers[i] = (1 << i);
    }
}

void collisions::CollisionManager::AddCollider(s_ptr<components::Collider> collider)
{
    colliders_.push_back(collider);
}

s_ptr<components::Collider> collisions::CollisionManager::CreateCollider(int layer, int precision, s_ptr<Model> model, int mesh_index, s_ptr<components::Transform> transform)
{
    assert(layer > -1 && layer < 32);
    auto return_value = make_shared<components::Collider>(layer, precision, model, mesh_index, transform);
    AddCollider(return_value);
    return return_value;
}

s_ptr<components::Collider> collisions::CollisionManager::CreateCollider(json &j, s_ptr<GameObject> go)
{
    auto return_value = make_shared<components::Collider>(j, go);
    AddCollider(return_value);
    return return_value;
}

void collisions::CollisionManager::RemoveCollider(s_ptr<components::Collider> c)
{
    auto it = std::find(colliders_.begin(), colliders_.end(), c);
	if (it != colliders_.end())
	{
		colliders_.erase(it);
	}
}

void collisions::CollisionManager::Separation(s_ptr<components::Collider> a, s_ptr<components::Collider> b, float wa, float wb)
{
    auto atmp = a->transform_->get_predicted_position();
    auto btmp = b->transform_->get_predicted_position();

    auto separation_vector = GetSeparatingVector(a->np_collider_,
        a->transform_->get_predicted_position(),
        b->np_collider_,
        b->transform_->get_predicted_position());
   
    a->transform_->set_predicted_position(a->transform_->get_predicted_position() + 2.0f * wa * separation_vector.sep_a);
    b->transform_->set_predicted_position(b->transform_->get_predicted_position() + 2.0f * wb * separation_vector.sep_b);

    bool p1x = !isnormal(a->transform_->get_predicted_position().x);
    bool p1z = !isnormal(a->transform_->get_predicted_position().z);
    bool p2x = !isnormal(b->transform_->get_predicted_position().x);
    bool p2z = !isnormal(b->transform_->get_predicted_position().z);

    if (p1x || p1z || p2x || p2z)
    {
        cout << atmp.x << " " << atmp.z << endl;
        cout << btmp.x << " " << btmp.z << endl;
        cout << "UPS!" << endl;
    }

    a->PredictColliders();
    b->PredictColliders();

    /*bool a_moved = (a->transform_->get_predicted_position() != a->transform_->get_previous_position());
    bool b_moved = (b->transform_->get_predicted_position() != b->transform_->get_previous_position());

    if (a_moved && b_moved)
    {
        a->transform_->set_predicted_position(a->transform_->get_predicted_position() + 2.0f * wa * separation_vector.sep_a);
        b->transform_->set_predicted_position(b->transform_->get_predicted_position() + 2.0f * wb * separation_vector.sep_b);
        
        a->PredictColliders();
        b->PredictColliders();
    }
    else if (a_moved && !b_moved)
    {
        a->transform_->set_predicted_position(a->transform_->get_predicted_position() + 2.0f * separation_vector.sep_a);
        a->PredictColliders();
    }
    else if (!a_moved && b_moved)
    {
        b->transform_->set_predicted_position(b->transform_->get_predicted_position() + 2.0f * separation_vector.sep_b);
        b->PredictColliders();
    }*/

    
}

void collisions::CollisionManager::AddCollisionBetweenLayers(int layer_1, int layer_2)
{
    collision_layers[layer_1] = collision_layers[layer_1] | (1 << layer_2);
    collision_layers[layer_2] = collision_layers[layer_2] | (1 << layer_1);
}

void collisions::CollisionManager::RemoveCollisionBetweenLayers(int layer_1, int layer_2)
{
    collision_layers[layer_1] = collision_layers[layer_1] & ~(1 << layer_2);
    collision_layers[layer_2] = collision_layers[layer_2] & ~(1 << layer_1);
}

void collisions::CollisionManager::UpdateColliders()
{
    for (auto& c : colliders_)
    {
        if (c->active_)
        {
            c->UpdateColliders();
        }
    }
}

void collisions::CollisionManager::PredictColliders()
{
    for (auto& c : colliders_)
    {
        if (c->active_)
        {
            c->PredictColliders();
        }
    }
}

void collisions::CollisionManager::CollisionCheckPBD(std::vector<pbd::Contact>& contacts)
{
    static float time = 0;
    static int idx = 0;

    contacts.clear();

    for (int i = 0; i < colliders_.size() - 1; i++)
    {
        s_ptr<components::Collider> a = colliders_[i];
        if (!a->active_)
        {
            continue;
        }
        for (int j = i + 1; j < colliders_.size(); j++)
        {
            s_ptr<components::Collider> b = colliders_[j];
            if (!b->active_)
            {
                continue;
            }

            bool layer_check = LayerCheck(a->layer_, b->layer_);

            if (layer_check)
            {
                bool are_colliding = AABBCollisionCheck(a->bp_collider_, b->bp_collider_);
                if (are_colliding)
                {
                    are_colliding = ConvexHullCheckFaster(a->np_collider_, b->np_collider_);
                    if (are_colliding)
                    {
                        if (a->gameObject_.lock() == nullptr || b->gameObject_.lock() == nullptr)
                        {
                            continue;
                        }
                        auto particle_a = a->gameObject_.lock()->GetComponent<components::PBDParticle>();
                        auto particle_b = b->gameObject_.lock()->GetComponent<components::PBDParticle>();

                        if (particle_a != nullptr && particle_b != nullptr)
                        {
                            auto wa = particle_b->mass_ / (particle_a->mass_ + particle_b->mass_);
                            auto wb = particle_a->mass_ / (particle_a->mass_ + particle_b->mass_);
                           
                            if (!particle_a->friction_)
                            {
                                particle_a->friction_ = true;
                            }

                            if (!particle_b->friction_)
                            {
                                particle_a->friction_ = true;
                            }

                            Separation(a, b, wa, wb);
                            pbd::Contact contact(particle_a, particle_b);
                            contacts.push_back(contact);
                        }
                        else if(particle_a == nullptr && particle_b == nullptr)
                        {
                            Separation(a, b, 0.5f, 0.5f);
                        }
                        else if (particle_a != nullptr && particle_b == nullptr)
                        {
                            Separation(a, b, 1.0f * b->softness_, 0.0f);
                        }
                        else if (particle_a == nullptr && particle_b != nullptr)
                        {
                            Separation(a, b, 0.0f, 1.0f * a->softness_);
                        }
                    }
                }
            }
        }
    }
}
