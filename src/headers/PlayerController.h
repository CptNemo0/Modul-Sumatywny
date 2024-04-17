#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "GameObject.h"
#include "InputObserver.h"
#include "InputManager.h"
#include "Physics.h"
#include "PBD.h"
#include "Timer.h"

namespace Components
{

class PlayerController : public Component, public Input::InputObserver, public std::enable_shared_from_this<PlayerController>
{
private:
    int gamepadID_;
    std::shared_ptr<pbd::BasicGenerator> move_generator_;
    std::shared_ptr<pbd::BasicGenerator> pull_generator_;
    int speed_ = 2000;
    int pull_power_ = 3000;
    bool is_pulling_ = false, pulling_cooldown_ = false;
    glm::vec3 direction_ = glm::vec3(0.0f);
public:
    PlayerController(int gamepadID);

    void Start() override;
    void Update() override;
    void Destroy() override;

    void OnAction(Action action, Input::State state) override;
};

} // namespace Components

#endif // !PLAYERCONTROLLER_H