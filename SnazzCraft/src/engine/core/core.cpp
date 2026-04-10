#include "snazzcraft-engine/core/core.hpp"
#include "snazzcraft-engine/vertice/voxel-vertice.hpp"
#include "snazzcraft-engine/core/window.hpp"
#include "snazzcraft-engine/texture/atlas.hpp"
#include "snazzcraft-engine/texture/texture.hpp"
#include "snazzcraft-engine/entity/entity-type.hpp"
#include "snazzcraft-engine/mesh/mesh.hpp"
#include "snazzcraft-engine/fps-tracker/fps-tracker.hpp"
#include "snazzcraft-engine/entity/user.hpp"
#include "snazzcraft-engine/world/world.hpp"
#include "snazzcraft-engine/entity/entity-ids.h"
#include "snazzcraft-engine/world/voxel-ids.h"

Shader* SnazzCraft::VoxelShader = nullptr;

glm::mat4 SnazzCraft::ProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000000.0f);
int SnazzCraft::ProjectionLock;

glm::mat4 SnazzCraft::ModelMatrix = glm::mat4(1.0f);
int SnazzCraft::ModelLock;

glm::mat4 SnazzCraft::ViewMatrix = glm::mat4(1.0f);
int SnazzCraft::ViewLock;

bool SnazzCraft::CloseApplication = false;

const std::vector<SnazzCraft::VoxelVertice> VoxelMeshVertices = {
    // Front
    { { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.2f } },
    { { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f } },
    { {  1.0f,  1.0f, -1.0f }, { 0.2f, 0.0f } },
    { {  1.0f, -1.0f, -1.0f }, { 0.2f, 0.2f } },

    // Left 
    { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.2f } },
    { { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
    { { -1.0f,  1.0f, -1.0f }, { 0.2f, 0.0f } },
    { { -1.0f, -1.0f, -1.0f }, { 0.2f, 0.2f } },

    // Right 
    { { 1.0f, -1.0f, -1.0f }, { 0.0f, 0.2f } },
    { { 1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f } },
    { { 1.0f,  1.0f,  1.0f }, { 0.2f, 0.0f } },
    { { 1.0f, -1.0f,  1.0f }, { 0.2f, 0.2f } },
    
    // Back
    { {  1.0f, -1.0f,  1.0f }, { 0.0f, 0.2f } },
    { {  1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },
    { { -1.0f,  1.0f,  1.0f }, { 0.2f, 0.0f } },
    { { -1.0f, -1.0f,  1.0f }, { 0.2f, 0.2f } },

    // Top
    { { -1.0f, 1.0f, -1.0f }, { 0.0f, 0.2f } },
    { { -1.0f, 1.0f,  1.0f }, { 0.0f, 0.0f } },
    { {  1.0f, 1.0f,  1.0f }, { 0.2f, 0.0f } },
    { {  1.0f, 1.0f, -1.0f }, { 0.2f, 0.2f } },

    // Bottom
    { { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.2f } },
    { { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f } },
    { {  1.0f, -1.0f, -1.0f }, { 0.2f, 0.0f } },
    { {  1.0f, -1.0f,  1.0f }, { 0.2f, 0.2f } },
};

const std::vector<uint32_t> VoxelMeshIndices = {
    0,  1,  2,    2,  3,  0,    // Front  (verts 0  .. 3 )
    4,  5,  6,    6,  7,  4,    // Left   (verts 4  .. 7 )
    8,  9,  10,   10, 11, 8,    // Right  (verts 8  .. 11)
    12, 13, 14,   14, 15, 12,   // Back   (verts 12 .. 15)
    16, 17, 18,   18, 19, 16,   // Top    (verts 16 .. 19)
    20, 21, 22,   22, 23, 20,   // Bottom (verts 20 .. 23)
};

void MainMenuCallback(SnazzCraft::Event* Event);

void WorldInputCallback(SnazzCraft::Event* Event);

void RenderWorld();

bool SnazzCraft::Initiate()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SnazzCraft::Window = glfwCreateWindow(900, 900, "SnazzCraft", NULL, NULL);
    if (SnazzCraft::Window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();

        return false;
    }

    glfwMakeContextCurrent(SnazzCraft::Window);
    glfwSetFramebufferSizeCallback(SnazzCraft::Window, &FrameBufferSizeCallBack);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

        return false;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    SnazzCraft::VoxelShader = new Shader("src/engine/shaders/voxel/vertex-shader.glsl", "src/engine/shaders/voxel/fragment-shader.glsl");
    SnazzCraft::VoxelShader->use();

    SnazzCraft::ProjectionLock = glGetUniformLocation(SnazzCraft::VoxelShader->ID, "projection");
    SnazzCraft::ModelLock = glGetUniformLocation(SnazzCraft::VoxelShader->ID, "model");
    SnazzCraft::ViewLock = glGetUniformLocation(SnazzCraft::VoxelShader->ID, "view");

    glUniformMatrix4fv(SnazzCraft::ProjectionLock, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
    glUniformMatrix4fv(SnazzCraft::ModelLock, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    SnazzCraft::VoxelTextureAtlas = new SnazzCraft::Texture("textures/voxel/atlas.png");
    stbi_set_flip_vertically_on_load(true);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glfwSetMouseButtonCallback(SnazzCraft::Window, MouseButtonCallback);

    SnazzCraft::EntityType::Initialize();
    
    SnazzCraft::VoxelMesh = new Mesh(VoxelMeshVertices, VoxelMeshIndices);

    SnazzCraft::VoxelMesh->ScaleVector = { (float)(SnazzCraft::Voxel::Size) / 2, (float)(SnazzCraft::Voxel::Size) / 2, (float)(SnazzCraft::Voxel::Size) / 2 };

    SnazzCraft::MenuGUI = new SnazzCraft::MainMenuGUI(900, 900, SnazzCraft::Window);
    SnazzCraft::MenuGUI->GUIInputHandler->Callback = MainMenuCallback;

    SnazzCraft::WorldGUI = new SnazzCraft::InWorldGUI(900, 900, SnazzCraft::Window);
    SnazzCraft::WorldGUI->GUIInputHandler->Callback = WorldInputCallback;

    return true;
}

void SnazzCraft::MainLoop()
{
    while (!glfwWindowShouldClose(SnazzCraft::Window) && !SnazzCraft::CloseApplication)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        SnazzCraft::GlobalFPSTracker->UpdateFPS();
        //system(CLEAR_COMMAND);
        //std::cout << "FPS| " << SnazzCraft::GlobalFPSTracker->FPS << "\n";

        switch (SnazzCraft::UserMode)
        {
            case SNAZZCRAFT_USER_MODE_WORLD:
                if (SnazzCraft::CurrentWorld == nullptr || SnazzCraft::WorldGUI == nullptr) break;

                SnazzCraft::CurrentWorld->ApplyGravityToAllEntities();

                // Poll and handle events
                SnazzCraft::WorldGUI->GUIInputHandler->PollEvents();
                SnazzCraft::WorldGUI->GUIInputHandler->HandleEvents();

                // Render
                SnazzCraft::VoxelShader->use(); 

                SnazzCraft::ViewMatrix = glm::lookAt(SnazzCraft::Player->Position, SnazzCraft::Player->Position + SnazzCraft::Player->Front, glm::vec3(0.0, 1.0, 0.0));
                glUniformMatrix4fv(SnazzCraft::ViewLock, 1, GL_FALSE, glm::value_ptr(SnazzCraft::ViewMatrix));
                
                RenderWorld();
                SnazzCraft::WorldGUI->Render();

                break;

            case SNAZZCRAFT_USER_MODE_MAIN_MENU:
                if (SnazzCraft::MenuGUI == nullptr) break;

                SnazzCraft::MenuGUI->GUIInputHandler->PollEvents();
                SnazzCraft::MenuGUI->GUIInputHandler->HandleEvents();
                SnazzCraft::MenuGUI->Render();

                break;
        }
      
        glfwSwapBuffers(SnazzCraft::Window);
        glfwPollEvents(); 
    }

    SnazzCraft::CloseApplication = true;
}

void SnazzCraft::FreeResources()
{
    SnazzCraft::CurrentWorld->SaveWorldToFile(true);

    SnazzCraft::EntityType::FreeResources();

    delete SnazzCraft::Player;
    delete SnazzCraft::VoxelShader;
    delete SnazzCraft::VoxelMesh;
    delete SnazzCraft::EngineVoxelTextureApplier;
    delete SnazzCraft::CurrentWorld;
    delete SnazzCraft::MenuGUI;
    delete SnazzCraft::WorldGUI;
    delete SnazzCraft::VoxelTextureAtlas;
    delete SnazzCraft::GlobalFPSTracker;
    
    glfwTerminate();
}

void RenderWorld()
{
    if (SnazzCraft::CurrentWorld->Entities.size() == 0) {
        SnazzCraft::CurrentWorld->Entities.push_back(new SnazzCraft::Entity(glm::vec3(10.0f, 95.0f, 10.0f), glm::vec3(0.0f, 45.0f, 0.0f), ID_ENTITY_SHEEP));
    } else {
        //SnazzCraft::CurrentWorld->MoveEntity(SnazzCraft::CurrentWorld->Entities[0], glm::vec3(0.0f), 0.01f);
        //SnazzCraft::CurrentWorld->Entities[0]->Rotation.x += 0.5f;
        //SnazzCraft::CurrentWorld->Entities[0]->Rotation.y += 1.0f;
        //SnazzCraft::CurrentWorld->Entities[0]->Rotation.z += 1.5f;
    }

    if (!SnazzCraft::VoxelTextureAtlas->BindTexture()) return;

    SnazzCraft::VoxelShader->use(); 

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT); 
    glFrontFace(GL_CW);  
    glEnable(GL_CULL_FACE);
    if (SnazzCraft::WireframeModeActive) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    SnazzCraft::CurrentWorld->UpdateVoxelPlacementDisplay();
    SnazzCraft::CurrentWorld->Render();
}

void WorldInputCallback(SnazzCraft::Event* Event)
{
    static uint8_t VoxelIDToPlace = 0;

    if (SnazzCraft::CurrentWorld == nullptr) return;

    if (!SnazzCraft::WorldGUI->InInventory) { // Not in inventory
        switch (Event->Type)
        {
            case SNAZZCRAFT_EVENT_KEY_DOWN:
            {
                unsigned char* Key = static_cast<unsigned char*>(Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_KEY));
                if (Key == nullptr) return;

                switch (*Key)
                {
                    case SNAZZCRAFT_KEY_0:
                        VoxelIDToPlace = 0;
                        break;
                    
                    case SNAZZCRAFT_KEY_1:
                        VoxelIDToPlace = 1;
                        break;
                    
                    case SNAZZCRAFT_KEY_2:
                        VoxelIDToPlace = 2;
                        break;
                    
                    case SNAZZCRAFT_KEY_3:
                        VoxelIDToPlace = 3;
                        break;

                    case SNAZZCRAFT_KEY_4:
                        VoxelIDToPlace = 4;
                        break;
                    
                    case SNAZZCRAFT_KEY_5:
                        VoxelIDToPlace = 5;
                        break;

                    case SNAZZCRAFT_KEY_6:
                        VoxelIDToPlace = 6;
                        break;
                    
                    case SNAZZCRAFT_KEY_7:
                        VoxelIDToPlace = 7;
                        break;

                    case SNAZZCRAFT_KEY_8:
                        VoxelIDToPlace = 8;
                        break;
                    
                    case SNAZZCRAFT_KEY_9:
                        VoxelIDToPlace = 9;
                        break;

                    case SNAZZCRAFT_KEY_T:
                        VoxelIDToPlace = ID_VOXEL_TORCH;
                        break;

                    case SNAZZCRAFT_KEY_ESCAPE:
                        SnazzCraft::CloseApplication = true;
                        break;

                    case SNAZZCRAFT_KEY_W:
                        SnazzCraft::CurrentWorld->MoveEntity(SnazzCraft::Player, glm::vec3(0.0f), 1.0f);
                        break;

                    case SNAZZCRAFT_KEY_A:
                        SnazzCraft::CurrentWorld->MoveEntity(SnazzCraft::Player, glm::vec3(0.0f, -90.0f, 0.0f), 1.0f);
                        break;

                    case SNAZZCRAFT_KEY_S:
                        SnazzCraft::CurrentWorld->MoveEntity(SnazzCraft::Player, glm::vec3(0.0f, 180.0f, 0.0f), 1.0f);
                        break;

                    case SNAZZCRAFT_KEY_D:
                        SnazzCraft::CurrentWorld->MoveEntity(SnazzCraft::Player, glm::vec3(0.0f, 90.0f, 0.0f), 1.0f);
                        break;

                    case SNAZZCRAFT_KEY_SPACE:
                        SnazzCraft::CurrentWorld->MoveEntity(glm::vec3(0.0f, 1.0f, 0.0f), SnazzCraft::Player);
                        break;

                    case SNAZZCRAFT_KEY_LEFT_SHIFT:
                        SnazzCraft::CurrentWorld->MoveEntity(glm::vec3(0.0f, -1.0f, 0.0f), SnazzCraft::Player);
                        break;

                    case SNAZZCRAFT_KEY_Q:
                        SnazzCraft::Player->Rotate({ 0.0f, -2.0f, 0.0f });
                        break;

                    case SNAZZCRAFT_KEY_E:
                        SnazzCraft::Player->Rotate({ 0.0f, 2.0f, 0.0f });
                        break;

                    case SNAZZCRAFT_KEY_X:
                        SnazzCraft::Player->Rotate({ 0.0f, 0.0f, -2.0f });
                        break;

                    case SNAZZCRAFT_KEY_C:
                        SnazzCraft::Player->Rotate({ 0.0f, 0.0f, 2.0f });
                        break;

                    default:
                        break;
                }

                break;
            }

            case SNAZZCRAFT_EVENT_MOUSE_CLICK_RIGHT_PRESS:
            {
                if (SnazzCraft::CurrentWorld == nullptr) break;

                SnazzCraft::CurrentWorld->PlaceVoxel(SnazzCraft::Player->Position, SnazzCraft::Player->Rotation, VoxelIDToPlace);

                break;
            }

            case SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT_PRESS:
            {
                Event->EventData->Items.push_back(SnazzCraft::WorldGUI);
                Event->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS);

                SnazzCraft::WorldGUI->SendEventToButtons(Event);

                SnazzCraft::CurrentWorld->DestroyVoxel(SnazzCraft::Player->Position, SnazzCraft::Player->Rotation);
            
                break;
            }

            default:
                break;
        }
    } else { // In inventory
        switch (Event->Type)
        {
            case SNAZZCRAFT_EVENT_KEY_DOWN:
            {
                unsigned char* Key = static_cast<unsigned char*>(Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_KEY));
                if (Key == nullptr) return;

                switch (*Key)
                {

                    default:
                        break;
                }

                break;
            }

            case SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT_PRESS:
            {
                Event->EventData->Items.push_back(SnazzCraft::WorldGUI);
                Event->EventData->Types.push_back(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS);

                SnazzCraft::WorldGUI->SendEventToButtons(Event);
            
                break;
            }

            default:
                break;
        }
    }
}

void MainMenuCallback(SnazzCraft::Event* Event)
{
    switch (Event->Type)
    {
        case SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT_PRESS:
        {
            SnazzCraft::MenuGUI->SendEventToButtons(Event);
            break;
        }
    }
}






