#include "core.hpp"

Shader* SnazzCraft::VoxelShader = nullptr;

glm::mat4 SnazzCraft::ProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
int SnazzCraft::ProjectionLock;

glm::mat4 SnazzCraft::ModelMatrix = glm::mat4(1.0f);
int SnazzCraft::ModelLock;

glm::mat4 SnazzCraft::ViewMatrix = glm::mat4(1.0f);
int SnazzCraft::ViewLock;

bool SnazzCraft::CloseApplication = false;

const std::vector<SnazzCraft::Vertice3D> VoxelMeshVertices = {
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

const std::vector<unsigned int> VoxelMeshIndices = {
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
        std::cout << "Failed to create GLFW window" << std::endl;
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

    glfwSetMouseButtonCallback(SnazzCraft::Window, &MouseButtonCallback);
    
    SnazzCraft::VoxelMesh = new Mesh(VoxelMeshVertices, VoxelMeshIndices);

    SnazzCraft::VoxelMesh->ScaleVector = { (float)(SnazzCraft::Voxel::Size) / 2, (float)(SnazzCraft::Voxel::Size) / 2, (float)(SnazzCraft::Voxel::Size) / 2 };

    SnazzCraft::MenuGUI = new SnazzCraft::MainMenuGUI(900, 900, SnazzCraft::Window);
    SnazzCraft::MenuGUI->GUIInputHandler->Callback = &MainMenuCallback;

    SnazzCraft::WorldGUI = new SnazzCraft::InWorldGUI(900, 900, SnazzCraft::Window);
    SnazzCraft::WorldGUI->GUIInputHandler->Callback = &WorldInputCallback;

    SnazzCraft::Player->SetHitbox({ 4.0f, 8.0f, 4.0f });

    return true;
}

void SnazzCraft::MainLoop()
{
    while (!glfwWindowShouldClose(SnazzCraft::Window) && !SnazzCraft::CloseApplication)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        SnazzCraft::GlobalFPSTracker->UpdateFPS();
        system(CLEAR_COMMAND);
        std::cout << "FPS| " << SnazzCraft::GlobalFPSTracker->FPS << "\n";

        switch (SnazzCraft::UserMode)
        {
            case SNAZZCRAFT_USER_MODE_WORLD:
                if (SnazzCraft::CurrentWorld == nullptr || SnazzCraft::WorldGUI == nullptr) break;

                SnazzCraft::CurrentWorld->ApplyGravityToEntities({ SnazzCraft::Player });

                // Poll and handle events
                SnazzCraft::WorldGUI->GUIInputHandler->PollEvents();
                SnazzCraft::WorldGUI->GUIInputHandler->HandleEvents();

                // Render
                SnazzCraft::WorldGUI->Render();
                SnazzCraft::VoxelShader->use(); 

                glUniformMatrix4fv(SnazzCraft::ProjectionLock, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

                SnazzCraft::ViewMatrix = glm::lookAt(SnazzCraft::Player->Position, SnazzCraft::Player->Position + SnazzCraft::Player->Front, glm::vec3(0.0, 1.0, 0.0));
                glUniformMatrix4fv(SnazzCraft::ViewLock, 1, GL_FALSE, glm::value_ptr(SnazzCraft::ViewMatrix));
                
                RenderWorld();

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
    SaveWorldToFile(SnazzCraft::CurrentWorld, true); 

    delete SnazzCraft::VoxelShader;
    delete SnazzCraft::VoxelMesh;
    delete SnazzCraft::Player;
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

    glUniformMatrix4fv(SnazzCraft::ModelLock, 1, GL_FALSE, glm::value_ptr(SnazzCraft::ModelMatrix));
    SnazzCraft::ModelMatrix = glm::mat4(1.0f); // Chunk vertices are stored in world space so no transformation is needed

    SnazzCraft::CurrentWorld->RenderChunks();
}

void WorldInputCallback(SnazzCraft::Event* Event)
{
    if (SnazzCraft::CurrentWorld == nullptr) return;

    switch (Event->Type)
    {
        case SNAZZCRAFT_EVENT_KEY_DOWN:
        {
            unsigned char* Key = static_cast<unsigned char*>(Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_KEY));
            if (Key == nullptr) return;

            switch (*Key)
            {
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
                    SnazzCraft::CurrentWorld->MoveEntity(glm::vec3(0.0f, 1.0f, 0.0f), SnazzCraft::Player, glm::vec3(0.0f));
                    break;

                case SNAZZCRAFT_KEY_LEFT_SHIFT:
                    SnazzCraft::CurrentWorld->MoveEntity(glm::vec3(0.0f, -1.0f, 0.0f), SnazzCraft::Player, glm::vec3(0.0f));
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

                case SNAZZCRAFT_KEY_F:
                    SnazzCraft::Player->Position.y += 0.2f;
                    break;
            }

            break;
        }

        case SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT:
        {
            SnazzCraft::GUI* WorldGUI = static_cast<SnazzCraft::GUI*>((Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS)));
            if (WorldGUI == nullptr) return;

            WorldGUI->HandleLeftMouseClick(Event);
            
            break;
        }
    }
}

void MainMenuCallback(SnazzCraft::Event* Event)
{
    switch (Event->Type)
    {
        case SNAZZCRAFT_EVENT_MOUSE_CLICK_LEFT:
        {
            SnazzCraft::GUI* MenuGUI = static_cast<SnazzCraft::GUI*>((Event->EventData->AccessDataType(SNAZZCRAFT_DATA_TYPE_GUI_ADDRESS)));
            if (MenuGUI == nullptr) return;
           
            MenuGUI->HandleLeftMouseClick(Event);
            break;
        }
    }
}






