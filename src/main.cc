#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#include <chrono>
#include <ctime>
#include <iostream>
#include <memory>
#include <ratio>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "stb_easy_font.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "headers/Camera.h"
#include "headers/collisions/Collider.h"
#include "headers/collisions/Collisions.h"
#include "headers/collisions/CollisionManager.h"
#include "headers/collisions/Raycast.h"
#include "headers/GameObject.h"
#include "headers/Model.h"
#include "headers/components/MeshRenderer.h"
#include "headers/physics/Physics.h"
#include "headers/physics/PBD.h"
#include "headers/physics/Rope.h"
#include "headers/Shader.h"
#include "headers/Texture.h"
#include "headers/utility.h"
#include "headers/input/InputManager.h"
#include "headers/components/HUDRenderer.h"
#include "headers/components/TextRenderer.h"
#include "headers/components/PlayerController.h"
#include "headers/HDRCubemap.h"
#include "headers/Font.h"
#include "headers/components/ParticleEmitter.h"

#include "headers/SteeringBehaviors.h"
#include "headers/Vehicle.h"
#include "headers/ai/EnemyAIManager.h"
#include "headers/ai/EnemyState.h"
#include "headers/ai/EnemyStateMachine.h"

void FixOrientation(std::shared_ptr<GameObject> go)
{
    auto current_forward = go->transform_->get_position() - go->transform_->get_previous_position();
    if (glm::length(current_forward))
    {
        current_forward = glm::normalize(current_forward);
    }
    float angle = glm::degrees(glm::orientedAngle(glm::vec3(0.0f, 0.0f, 1.0f), current_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    go->transform_->set_rotation(glm::vec3(0.0f, angle, 0.0f));
}


int main()
{
    std::cout << "Byc czy nie byc oto jest pytanie.\n";
    const std::string kWindowTitle = "Modul Sumatywny";

    const std::string kVertexShaderPath = "res/shaders/Basic.vert";
    const std::string kFragmentShaderPath = "res/shaders/Basic.frag";

    const std::string kHUDVertexShaderPath = "res/shaders/HUD.vert";
    const std::string kHUDFragmentShaderPath = "res/shaders/HUD.frag";

    const std::string kHUDTextVertexShaderPath = "res/shaders/HUDText.vert";
    const std::string kHUDTextFragmentShaderPath = "res/shaders/HUDText.frag";

    const std::string kPBRVertexShaderPath = "res/shaders/PBR.vert";
    const std::string kPBRFragmentShaderPath = "res/shaders/PBR.frag";

    const std::string kHDRCubemapVertexShaderPath = "res/shaders/HDRCubemap.vert";
    const std::string kHDREquirectangularToCubemapFragmentShaderPath = "res/shaders/HDREquirectangularToCubemap.frag";
    const std::string kIrradianceFragmentShaderPath = "res/shaders/IrradianceConvolution.frag";

    const std::string kBackgroundVertexShaderPath = "res/shaders/Background.vert";
    const std::string kBackgroundFragmentShaderPath = "res/shaders/Background.frag";

    const std::string kParticleVertexShaderPath = "res/shaders/Particle.vert";
    const std::string kParticleGeometryShaderPath = "res/shaders/Particle.geom";
    const std::string kParticleFragmentShaderPath = "res/shaders/Particle.frag";

    const std::string kGreenTexturePath = "res/textures/green_texture.png";
    const std::string kRedTexturePath = "res/textures/red_texture.png";
    const std::string kHUDTexturePath = "res/textures/placeholder_icon.png";
    const std::string kHUDTexturePath2 = "res/textures/staly_elmnt.png";

    const std::string kHDREquirectangularPath = "res/cubemaps/HDR_placeholder.hdr";

    const std::string kCubeMeshPath = "res/models/cube_2.obj";
    const std::string kPlayerMeshPath = "res/models/player.obj";
    const std::string kDebugMeshPath = "res/models/debug_thingy.obj";
    const std::string kEnemyMeshPath = "res/models/enemy.obj";
    const std::string kTestPath = "res/models/test2.obj";
    const std::string kWallPath = "res/models/simple_wall.obj";
    const std::string kModule1Path = "res/models/module1.obj";
    const std::string kModule2Path = "res/models/enviroment/modules/module2.obj";
    const std::string kSimpleFloodPath = "res/models/enviroment/floor/floor.obj";
    const std::string kTestBallPath = "res/models/test_ball.obj";

    const std::string kFontPath = "res/fonts/CourierPrime-Regular.ttf";

    const float kFov = 90.0f;
    const float kNear = 0.1f;
    const float kFar = 1000.0f;

    srand(static_cast <unsigned> (time(0)));

    Vehicle enemy_vehicle_template;

    enemy_vehicle_template.rest_lenght = 4.0f;

    enemy_vehicle_template.max_speed = 2000.0f;

    enemy_vehicle_template.wander_target = glm::vec3(0.0f);
    enemy_vehicle_template.wander_distance = 2.0f;
    enemy_vehicle_template.wander_radius = 2.0f;
    enemy_vehicle_template.wander_jitter = 0.5f;
    enemy_vehicle_template.wander_weight = 1.0f;
    enemy_vehicle_template.wander_speed_ = 1000.f;

    enemy_vehicle_template.wall_avoidance_distance = 2.0f;
    enemy_vehicle_template.wall_avoidance_weight = 3.0f;

    enemy_vehicle_template.pursuit_distance = 0.5f;
    enemy_vehicle_template.pursuit_weight = 1.0f;
    enemy_vehicle_template.pursuit_speed_ = 1500.f;

    enemy_vehicle_template.extrapolation_distance = 5.0f;
    enemy_vehicle_template.extrapolation_weight = 1.0f;
    enemy_vehicle_template.extrapolation_speed_ = 1600.f;

    enemy_vehicle_template.evade_distance = 5.0f;
    enemy_vehicle_template.evade_weight = 1.0f;
    enemy_vehicle_template.evade_speed_ = 1700.f;

    ai::EnemyAIManagerInitStruct enemy_ai_init;
    enemy_ai_init.choked_tentacles = 10;
    enemy_ai_init.multi_chokes = 0;
    enemy_ai_init.choke_threshold = 5;
    enemy_ai_init.multi_threshold = 5;
    enemy_ai_init.wall_proximity_threshold = 1.0f;
    enemy_ai_init.attack_damage = 1.0f;
    enemy_ai_init.attack_range = 2.5f;
    enemy_ai_init.sense_range = 7.0f;

    GLFWwindow* window = nullptr;
    GLFWmonitor* monitor = nullptr;
    GLFWvidmode* mode = nullptr;

    if (int return_value = utility::InitGLFW(window, monitor, mode, kWindowTitle))
    {
        exit(return_value);
    }
    std::cout << "GLFW Initialized.\n";

    if (int return_value = utility::InitGlad())
    {   
        exit(return_value);
    }
    std::cout << "GLAD Initialized.\n";

    input::InputManager::Initialize(window);
    collisions::CollisionManager::Initialize();
    physics::PhysicsManager::Initialize();
    pbd::PBDManager::Initialize(3, 0.5f, 0.8f);
    ai::EnemyAIManager::Initialize(enemy_ai_init, enemy_vehicle_template);

    auto camera = std::make_shared<llr::Camera>();
    camera->set_fov(kFov);
    camera->set_near(kNear);
    camera->set_far(kFar);
    camera->set_aspect_ratio(((float)mode->width / (float)mode->height));
    camera->set_position(glm::vec3(0.0f, 25.0f, 0.0f));
    camera->set_pitch(-90.0f);
    camera->set_yaw(+90.0f);

    auto projection_matrix = glm::perspective(glm::radians(camera->get_fov()), camera->get_aspect_ratio(), camera->get_near(), camera->get_far());
    auto ortho_matrix = glm::ortho(0.0f, (float)mode->width, 0.0f, (float)mode->height);

    auto shader = std::make_shared<Shader>(kVertexShaderPath, kFragmentShaderPath);
    auto HUDshader = std::make_shared<Shader>(kHUDVertexShaderPath, kHUDFragmentShaderPath);
    auto HUDTextShader = std::make_shared<Shader>(kHUDTextVertexShaderPath, kHUDTextFragmentShaderPath);
    auto PBRShader = std::make_shared<Shader>(kPBRVertexShaderPath, kPBRFragmentShaderPath);
    auto EquirectangularToCubemapShader = std::make_shared<Shader>(kHDRCubemapVertexShaderPath, kHDREquirectangularToCubemapFragmentShaderPath);
    auto BackgroundShader = std::make_shared<Shader>(kBackgroundVertexShaderPath, kBackgroundFragmentShaderPath);
    auto IrradianceShader = std::make_shared<Shader>(kHDRCubemapVertexShaderPath, kIrradianceFragmentShaderPath);
    auto ParticleShader = std::make_shared<Shader>(kParticleVertexShaderPath, kParticleGeometryShaderPath, kParticleFragmentShaderPath);

    auto cubemap = std::make_shared<HDRCubemap>(kHDREquirectangularPath, BackgroundShader, EquirectangularToCubemapShader, IrradianceShader);

    PointLight point_light;
    point_light.intensity = 200.0f;
    point_light.position = glm::vec3(-3.0f, 3.0f, -3.0f);
    point_light.ambient_colour = glm::vec3(0.6f, 0.6f, 0.6f);
    point_light.diffuse_colour = glm::vec3(0.5f, 0.7f, 0.5f);
    point_light.specular_colour = glm::vec3(0.5f, 0.7f, 0.5f);

    // lights
    // ------
    glm::vec3 light_Positions[] = {
        glm::vec3(0.0f,  10.0f, 0.0f),
        glm::vec3(10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3(10.0f, -10.0f, 10.0f),
    };
    glm::vec3 light_Colors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };

    auto test_model = std::make_shared<Model>(kTestPath);

    auto cube_model = std::make_shared<Model>(kCubeMeshPath);
    auto player_model = std::make_shared<Model>(kPlayerMeshPath);
    auto debug_model = std::make_shared<Model>(kDebugMeshPath);
    auto enemy_model = std::make_shared<Model>(kEnemyMeshPath);
    auto wall_model = std::make_shared<Model>(kWallPath);
    auto module_1_model = std::make_shared<Model>(kModule1Path);
    auto module_2_model = std::make_shared<Model>(kModule2Path);
    auto simple_floor_model = std::make_shared<Model>(kSimpleFloodPath);
    auto test_ball_model = std::make_shared<Model>(kTestBallPath);

    auto HUD_texture = std::make_shared<tmp::Texture>(kHUDTexturePath);
    auto HUD_texture2 = std::make_shared<tmp::Texture>(kHUDTexturePath2);

    collisions::CollisionManager::i_->AddCollisionBetweenLayers(0, 1);
    collisions::CollisionManager::i_->AddCollisionBetweenLayers(0, 2);
    collisions::CollisionManager::i_->RemoveCollisionBetweenLayers(2, 2);

    auto scene_root = GameObject::Create();

    auto wall_up_1 = GameObject::Create(scene_root);
    wall_up_1->transform_->set_position(glm::vec3(-8.0f, 0.0f, -16.0f));
    wall_up_1->AddComponent(std::make_shared<components::MeshRenderer>(module_2_model, PBRShader));
    auto wall_up_2 = GameObject::Create(scene_root);
    wall_up_2->transform_->set_position(glm::vec3(8.0f, 0.0f, -16.0f));
    wall_up_2->AddComponent(std::make_shared<components::MeshRenderer>(module_2_model, PBRShader));

    auto wall_right_1 = GameObject::Create(scene_root);
    wall_right_1->transform_->set_position(glm::vec3(-16.0f, 0.0f, -8.0f));
    wall_right_1->transform_->set_rotation(glm::vec3(0.0f, 90.0f, 0.0f));
    wall_right_1->AddComponent(std::make_shared<components::MeshRenderer>(module_2_model, PBRShader));

    auto wall_right_2 = GameObject::Create(scene_root);
    wall_right_2->transform_->set_position(glm::vec3(-16.0f, 0.0f, 8.0f));
    wall_right_2->transform_->set_rotation(glm::vec3(0.0f, 90.0f, 0.0f));
    wall_right_2->AddComponent(std::make_shared<components::MeshRenderer>(module_2_model, PBRShader));


    auto floor = GameObject::Create(scene_root);
    floor->transform_->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    floor->transform_->set_scale(glm::vec3(2.0f, 0.0f, 2.0f));
    floor->AddComponent(std::make_shared<components::MeshRenderer>(simple_floor_model, PBRShader));

    pbd::WallConstraint walls = pbd::WallConstraint(glm::vec3(-17.0f, 0.0f, 17.0f), glm::vec3(17.0f, 0.0f, -17.0f), 1.0f);
    pbd::PBDManager::i_->set_walls(walls);


    auto enemy_1 = GameObject::Create(scene_root);
  
    enemy_1->transform_->set_position(glm::vec3(-10.0f, 0.0f, -10.0f));    
    enemy_1->transform_->set_position(glm::vec3(-10.0f, 0.0f, -10.0f));
    enemy_1->AddComponent(std::make_shared<components::MeshRenderer>(enemy_model, shader));
    enemy_1->AddComponent(collisions::CollisionManager::i_->CreateCollider(0, gPRECISION, enemy_model->meshes_[0], enemy_1->transform_));
    enemy_1->AddComponent(pbd::PBDManager::i_->CreateParticle(3.0f, 0.88f, enemy_1->transform_));

    auto enemy_movement_generator = std::make_shared<pbd::BasicGenerator>();
    pbd::PBDManager::i_->CreateFGRRecord(enemy_1->GetComponent<components::PBDParticle>(), enemy_movement_generator);

    auto enemy_state_machine = std::make_shared<ai::EnemyStateMachine>(enemy_1, enemy_movement_generator, enemy_vehicle_template);

    auto enemy_2 = GameObject::Create(scene_root);

    enemy_2->transform_->set_position(glm::vec3(-8.0f, 0.0f, -10.0f));
    enemy_2->transform_->set_position(glm::vec3(-8.0f, 0.0f, -10.0f));
    enemy_2->AddComponent(std::make_shared<components::MeshRenderer>(enemy_model, shader));
    enemy_2->AddComponent(collisions::CollisionManager::i_->CreateCollider(0, gPRECISION, enemy_model->meshes_[0], enemy_2->transform_));
    enemy_2->AddComponent(pbd::PBDManager::i_->CreateParticle(3.0f, 0.88f, enemy_2->transform_));

    ////test
    /*auto test = GameObject::Create(scene_root);
    test->transform_->set_position(glm::vec3(-3.0f, 2.0f, -3.0f));
    test->AddComponent(std::make_shared<components::MeshRenderer>(test_model, PBRShader));*/


    auto player_1 = GameObject::Create(scene_root);
    player_1->transform_->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    player_1->transform_->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    player_1->AddComponent(std::make_shared<components::MeshRenderer>(player_model, PBRShader));
    player_1->AddComponent(collisions::CollisionManager::i_->CreateCollider(1, gPRECISION, player_model->meshes_[0], player_1->transform_));
    player_1->AddComponent(pbd::PBDManager::i_->CreateParticle(2.0f, 0.9f, player_1->transform_));
    player_1->AddComponent(std::make_shared<components::PlayerController>(GLFW_JOYSTICK_1));

    auto player_2 = GameObject::Create(scene_root);
    player_2->transform_->set_position(glm::vec3(10.0f + (1.0f/5.0f), 0.0f, 0.0f));
    player_2->transform_->set_position(glm::vec3(10.0f + (1.0f / 5.0f), 0.0f, 0.0f));
    player_2->AddComponent(std::make_shared<components::MeshRenderer>(player_model, PBRShader));
    player_2->AddComponent(collisions::CollisionManager::i_->CreateCollider(1, gPRECISION, player_model->meshes_[0], player_2->transform_));
    player_2->AddComponent(pbd::PBDManager::i_->CreateParticle(2.0f, 0.9f, player_2->transform_));
    player_2->AddComponent(std::make_shared<components::PlayerController>(GLFW_JOYSTICK_2));

    ai::EnemyAIManager::SetPlayers(player_1, player_2);
    //ai::EnemyAIManager::SetEnemies(enemies) //jakis vector i potem metoda ktora go zmienia na cos innego moze zadziala

    /*auto start = glm::vec3(-0.2, 0.0f, 2.0f);
    auto end = glm::vec3(0.2, 0.0f, -2.0f);
    auto dir = glm::normalize(end - start);

    auto hit = collisions::Raycast(start, dir, 20.0f, 0);*/

    std::vector<std::shared_ptr<GameObject>> rope_segments;

    for (int i = 0; i < 50; i++)
    {
        auto rope_segment = GameObject::Create(scene_root);
        rope_segment->transform_->set_scale(glm::vec3(1.1f, 1.1f, 1.1f));
        rope_segment->transform_->set_position(glm::vec3(((float)i + 1.0f) / 5.0f, 0.0f, 0.0f));
        rope_segment->transform_->set_position(glm::vec3(((float)i + 1.0f) / 5.0f, 0.0f, 0.0f));
        rope_segment->AddComponent(std::make_shared<components::MeshRenderer>(test_ball_model, shader));
        rope_segment->AddComponent(collisions::CollisionManager::i_->CreateCollider(2, gPRECISION, test_ball_model->meshes_[0], rope_segment->transform_));
        rope_segment->AddComponent(pbd::PBDManager::i_->CreateParticle(0.25f, 0.99f, rope_segment->transform_));

        if (i == 0)
        {
            pbd::PBDManager::i_->CreateRopeConstraint(player_1->GetComponent<components::PBDParticle>(), rope_segment->GetComponent<components::PBDParticle>(), 0.31f);
        }
        else
        {
            pbd::PBDManager::i_->CreateRopeConstraint(rope_segments.back()->GetComponent<components::PBDParticle>(), rope_segment->GetComponent<components::PBDParticle>(), 0.31f);
        }

        rope_segments.push_back(rope_segment);
    }

    pbd::PBDManager::i_->CreateRopeConstraint(rope_segments.back()->GetComponent<components::PBDParticle>(), player_2->GetComponent<components::PBDParticle>(), 0.21f);


    auto HUD_root = GameObject::Create();

    auto HUD_object = GameObject::Create(HUD_root);
    HUD_object->AddComponent(std::make_shared<components::HUDRenderer>(HUD_texture, HUDshader));
    HUD_object->transform_->set_scale(glm::vec3(0.25f, 0.25f, 1.0f));
    HUD_object->transform_->set_position(glm::vec3(-0.75f, -0.75f, 0.0f));

    auto HUD_object2 = GameObject::Create(HUD_root);
    HUD_object2->AddComponent(std::make_shared<components::HUDRenderer>(HUD_texture2, HUDshader));
    HUD_object2->transform_->set_scale(glm::vec3(0.25f, 0.25f, 1.0f));
    HUD_object2->transform_->set_position(glm::vec3(0.75f, -0.75f, 0.0f));

    auto HUDText_root = GameObject::Create();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }
    auto maturasc_font = std::make_shared<Font>(ft, kFontPath.c_str());

    auto HUDText_object = GameObject::Create(HUDText_root);
    HUDText_object->AddComponent(std::make_shared<components::TextRenderer>(HUDTextShader, maturasc_font, "TEST", glm::vec3(1.0f)));
    HUDText_object->transform_->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    HUDText_object->transform_->set_position(glm::vec3(50.0f, 900.0f, 0.0f));

    auto particle_root = GameObject::Create();

    auto particle_emitter = GameObject::Create(particle_root);
    particle_emitter->transform_->set_position(glm::vec3(0.0f, 10.0f, 0.0f));
    particle_emitter->AddComponent(std::make_shared<components::ParticleEmitter>(HUD_texture, ParticleShader));

    scene_root->PropagateStart();
    HUD_root->PropagateStart();
    HUDText_root->PropagateStart();
    particle_root->PropagateStart();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    PBRShader->Use();
    PBRShader->SetInt("irradiance_map", 5);

    BackgroundShader->Use();
    BackgroundShader->SetInt("environmentMap", 0);

    cubemap->LoadHDRimg(window, camera);

    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    glm::mat4 projection = glm::perspective(glm::radians(camera->get_fov()), camera->get_aspect_ratio(), camera->get_near(), camera->get_far());
    PBRShader->Use();
    PBRShader->SetMatrix4("projection_matrix", projection);

    ParticleShader->Use();
    ParticleShader->SetMatrix4("projection_matrix", projection);

    BackgroundShader->Use();
    BackgroundShader->SetMatrix4("projection_matrix", projection);

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

    Timer::Timer fixed_update_timer = Timer::CreateTimer(1.0f / 120.0f, [enemy_state_machine, &fixed_update_timer]()
    {
        //ai::EnemyAIManager::i_->UpdateEnemyStateMachine(enemy_state_machine);

        pbd::PBDManager::i_->GeneratorUpdate();
        pbd::PBDManager::i_->Integration(pbd::kMsPerUpdate);
        collisions::CollisionManager::i_->PredictColliders();
        collisions::CollisionManager::i_->CollisionCheckPBD(pbd::PBDManager::i_->contacts_);
        pbd::PBDManager::i_->ProjectConstraints(pbd::kMsPerUpdate);
        pbd::PBDManager::i_->UpdatePositions(pbd::kMsPerUpdate);
        pbd::PBDManager::i_->ClearContacts();
    }, nullptr, true);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static float lag = 0.0f;
        static float previous_time = glfwGetTime();
        float current_time = glfwGetTime();
        float delta_time = current_time - previous_time;
        lag += delta_time;
        #ifdef _DEBUG
            delta_time = glm::clamp(delta_time, 0.0f, (1.0f / 30.0f));
        #endif

        previous_time = current_time;

        Timer::Update(delta_time);
        collisions::ChokeCheck(enemy_1, gPRECISION, gPRECISION * 0.75f, 2.0f);
        collisions::ChokeCheck(enemy_2, gPRECISION, gPRECISION * 0.75f, 2.0f);
        utility::DebugCameraMovement(window, camera, delta_time);
        input::InputManager::i_->Update();

#pragma region Collisions and Physics
        
        Timer::UpdateTimer(fixed_update_timer, delta_time);

        FixOrientation(enemy_1);
        FixOrientation(player_1);
        FixOrientation(player_2);

#pragma endregion
#pragma region GO Update and Draw

       /* shader->Use();

        shader->SetVec3("camera_position", camera->get_position());
        shader->SetFloat("shininess", 50.0f);

        shader->SetPointLight("light", point_light);
        shader->SetMatrix4("projection_matrix", projection_matrix);
        shader->SetMatrix4("view_matrix", camera->GetViewMatrix());*/

        PBRShader->Use();
        PBRShader->SetMatrix4("view_matrix", camera->GetViewMatrix());
        PBRShader->SetVec3("camera_position", camera->get_position());

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->irradianceMap);

        glm::vec3 newPos = light_Positions[0] /* + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0)*/;
        PBRShader->SetVec3("light_positions[0]", newPos);
        PBRShader->SetVec3("light_colors[0]", light_Colors[0]);

        glm::mat4 model = glm::mat4(1.0f);
        PBRShader->SetMatrix4("model_matrix", model);

        scene_root->PropagateUpdate();

        BackgroundShader->Use();
        BackgroundShader->SetMatrix4("view_matrix", camera->GetViewMatrix());

        cubemap->RenderCube();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        ParticleShader->Use();
        ParticleShader->SetMatrix4("view_matrix", camera->GetViewMatrix());

        particle_root->PropagateUpdate();

        glDisable(GL_BLEND);

#pragma endregion
#pragma region Interface

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        HUDshader->Use();

        HUD_object->transform_->add_rotation(glm::vec3(133.0f * delta_time, 100.0f * delta_time, 66.0f * delta_time));
        HUD_root->PropagateUpdate();

        HUDTextShader->Use();
        HUDTextShader->SetMatrix4("projection_matrix", ortho_matrix);

        HUDText_object->GetComponent<components::TextRenderer>()->text_ = "fps: " + std::to_string(1.0f / delta_time);
        HUDText_root->PropagateUpdate();

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

#pragma endregion

        glfwSwapBuffers(window);

#pragma endregion
    }

    ai::EnemyAIManager::Destroy();
    pbd::PBDManager::Destroy();
    collisions::CollisionManager::Destroy();
    input::InputManager::Destroy();

    shader->End();
    glfwTerminate();
    return 0;
}
