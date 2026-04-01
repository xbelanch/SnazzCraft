#include "snazzcraft-engine/world/world.hpp"

SnazzCraft::World* SnazzCraft::CurrentWorld = nullptr;

SnazzCraft::World::World(std::string Name, uint32_t Size, int Seed)
{
    this->Size = Size;
    this->Name = Name;
    this->Seed = Seed;

    this->ChunkMeshUpdateQueue = new std::vector<SnazzCraft::Chunk*>();
    this->Chunks = new std::unordered_map<uint32_t, SnazzCraft::Chunk*>();
    this->WorldHeightMap = new SnazzCraft::HeightMap(this->Size * SnazzCraft::Chunk::Width, 0, SnazzCraft:: Chunk::Height - 1, this->Seed, 1.0, 0.5, 2.0, 6);
    this->ApplyLightingQueue = new std::unordered_set<uint32_t>();
}

SnazzCraft::World::~World()
{
    delete this->ChunkMeshUpdateQueue;
    for (auto& ChunkPair : *this->Chunks) {
        delete ChunkPair.second;
    }

    delete this->Chunks;
    delete this->WorldHeightMap;
    delete this->ApplyLightingQueue;
}

void SnazzCraft::World::GenerateChunk(uint32_t X, uint32_t Z)
{
    auto InitializeNewChunk = [this, X, Z]() -> void
    {
        SnazzCraft::Chunk* NewChunk = new SnazzCraft::Chunk(X, Z);

        {
            std::lock_guard<std::mutex> LockGuard(this->HeightMapMutex);
            NewChunk->Generate(this->WorldHeightMap, this->Size * SnazzCraft::Chunk::Width);
        }
        
        NewChunk->CullVoxelFaces();

        {
            std::lock_guard<std::mutex> LockGuard(this->ChunkMeshUpdateQueueMutex);
            this->ChunkMeshUpdateQueue->push_back(NewChunk);
        }

        std::lock_guard<std::mutex> LockGuard(this->ChunkMutex);
        (*this->Chunks)[INDEX_2D(X, Z, this->Size)] = NewChunk;
        this->ApplyLightingChunk(NewChunk);
        NewChunk->UpdateVerticesAndIndices();
    };

    if (X >= SnazzCraft::World::Size || Z >= SnazzCraft::World::Size) return;

    {
        std::lock_guard<std::mutex> LockGuard(this->ChunkMutex);
        auto Iterator = this->Chunks->find(INDEX_2D(X, Z, this->Size));
        if (Iterator != this->Chunks->end()) return;
    }
    
    std::thread ChunkInitializationThread(InitializeNewChunk);
    ChunkInitializationThread.detach();
}

void SnazzCraft::World::RenderChunks(SnazzCraft::User* Player)
{ 
    int32_t PlayerChunkPosition[2];
    SnazzCraft::Chunk::GetChunkPosition(Player->Position, PlayerChunkPosition);

    this->UpdateChunkMeshesInQueue();

    std::lock_guard<std::mutex> LockGuard(this->ChunkMutex);
    for (int32_t X = PlayerChunkPosition[0] - static_cast<int32_t>(this->RenderDistance); X <= PlayerChunkPosition[0] + static_cast<int32_t>(this->RenderDistance); X++) {
    for (int32_t Z = PlayerChunkPosition[1] - static_cast<int32_t>(this->RenderDistance); Z <= PlayerChunkPosition[1] + static_cast<int32_t>(this->RenderDistance); Z++) {
        if (X < 0 || X >= this->Size || Z < 0 || Z >= this->Size) continue;

        auto ChunkIterator = this->Chunks->find(INDEX_2D(X, Z, static_cast<int32_t>(this->Size)));
        if (ChunkIterator == this->Chunks->end()) continue;

        if (ChunkIterator->second->ChunkMesh == nullptr) {
            {
                std::lock_guard<std::mutex> LockGuard(this->ChunkMeshUpdateQueueMutex);
                this->ChunkMeshUpdateQueue->push_back(ChunkIterator->second);
            }
            continue;
        }
        ChunkIterator->second->ChunkMesh->Draw();
    }
    }
}

SnazzCraft::Voxel* SnazzCraft::World::GetCollidingVoxel(const SnazzCraft::Hitbox* Hitbox) const
{
    const int32_t ChunkX = static_cast<int32_t>(Hitbox->Position.x / (SnazzCraft::Chunk::Width * SnazzCraft::Voxel::Size));
    const int32_t ChunkZ = static_cast<int32_t>(Hitbox->Position.z / (SnazzCraft::Chunk::Depth * SnazzCraft::Voxel::Size));

    for (int32_t X = ChunkX - 1; X <= ChunkX + 1; X++) {
    for (int32_t Z = ChunkZ - 1; Z <= ChunkZ + 1; Z++) {
        if (X < 0 || Z < 0 || X >= static_cast<int32_t>(this->Size) || Z >= static_cast<int32_t>(this->Size)) continue;
        
        auto ChunkIterator = this->Chunks->find(INDEX_2D(X, Z, this->Size));
        if (ChunkIterator == this->Chunks->end()) continue;

        SnazzCraft::Voxel* CollisionVoxel = ChunkIterator->second->GetCollidingVoxel(Hitbox);
        if (CollisionVoxel != nullptr) return CollisionVoxel;
    }
    }

    return nullptr;
}

SnazzCraft::Voxel* SnazzCraft::World::GetCollidingVoxel(const glm::vec3& Position) const
{
    const int32_t ChunkX = static_cast<int32_t>(Position.x / (SnazzCraft::Chunk::Width * SnazzCraft::Voxel::Size));
    const int32_t ChunkZ = static_cast<int32_t>(Position.z / (SnazzCraft::Chunk::Depth * SnazzCraft::Voxel::Size));

    if (ChunkX < 0 || ChunkZ < 0 || ChunkX >= static_cast<int32_t>(this->Size) || ChunkZ >= static_cast<int32_t>(this->Size)) return nullptr;

    auto ChunkIterator = this->Chunks->find(INDEX_2D(ChunkX, ChunkZ, this->Size));
    if (ChunkIterator == this->Chunks->end()) return nullptr;

    SnazzCraft::Voxel* CollisionVoxel = ChunkIterator->second->GetCollidingVoxel(Position);
    if (CollisionVoxel != nullptr) return CollisionVoxel;

    return nullptr;
}

void SnazzCraft::World::MoveEntity(SnazzCraft::Entity* Entity, const glm::vec3& Rotation, float Distance) const
{
    glm::vec3 NewPosition = Entity->Position;
    MoveVector3D(NewPosition, Entity->Rotation + Rotation, Distance);

    std::lock_guard<std::mutex> LockGuard(this->ChunkMutex);
    for (uint32_t I = 0; I < 3; I++) {
        float OldCoordinate = Entity->Position[I];

        Entity->Position[I] = NewPosition[I];
        Entity->EntityHitbox->Position[I] = NewPosition[I];

        SnazzCraft::Voxel* CollisionVoxel = this->GetCollidingVoxel(Entity->EntityHitbox);
        if (CollisionVoxel == nullptr) continue;

        Entity->Position[I] = OldCoordinate;
        Entity->EntityHitbox->Position[I] = OldCoordinate;
    }
}

void SnazzCraft::World::MoveEntity(glm::vec3 Translation, SnazzCraft::Entity* Entity) const
{
    std::lock_guard<std::mutex> LockGuard(this->ChunkMutex);
    for (uint32_t I = 0; I < 3; I++) {
        float OldCoordinate = Entity->Position[I];

        Entity->Position[I] += Translation[I];
        Entity->EntityHitbox->Position[I] = Entity->Position[I];

        SnazzCraft::Voxel* CollisionVoxel = this->GetCollidingVoxel(Entity->EntityHitbox);
        if (CollisionVoxel == nullptr) continue;

        Entity->Position[I] = OldCoordinate;
        Entity->EntityHitbox->Position[I] = OldCoordinate;
    }
}

void SnazzCraft::World::UpdateAndApplyAllLighting() const
{
    std::lock_guard<std::mutex> LockGuard(this->ChunkMutex);
    for (auto& ChunkPair : *this->Chunks) { ChunkPair.second->LightValues->clear(); }

    for (auto& ChunkPair : *this->Chunks) {
        this->ApplyLightingChunk(ChunkPair.second);
    }

    for (auto& ChunkPair : *this->Chunks) ChunkPair.second->UpdateMesh();
}

bool SnazzCraft::World::SaveWorldToFile(bool OverwriteExistingFile)
{
    constexpr const char* FileDirectory = "worlds/";
    constexpr const char* FileExtension = ".txt";

    std::string FilePath = FileDirectory + this->Name + FileExtension;
    std::ofstream File;

    if (std::filesystem::exists(FilePath) && !OverwriteExistingFile) return false;

    File.open(FilePath);

    File << WORLD_SAVE_FILE_DESCRIPTOR_NAME       << ": " << this->Name << "\n";
    File << WORLD_SAVE_FILE_DESCRIPTOR_SIZE       << ": " << this->Size << "\n";
    File << WORLD_SAVE_FILE_DESCRIPTOR_WORLD_SEED << ": " << this->Seed << "\n";

    for (auto& ChunkPair : *this->Chunks) {
        SnazzCraft::Chunk* Chunk = ChunkPair.second;

        File << WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_BEGIN << ": " << Chunk->Position[0] << " " << Chunk->Position[1] << "\n";

        for (auto& VoxelPair : *Chunk->Voxels) {
            File << WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_NEW_VOXEL << ": " << VoxelPair.second.Position[0] << " " << VoxelPair.second.Position[1] << " " << VoxelPair.second.Position[2] << " " << VoxelPair.second.ID << "\n";
        }

        File << WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_END << ":\n";
    }

    File.close();
    return true;
}

void SnazzCraft::World::ApplyLightingVoxel(int32_t LightOrigin[3], int32_t LightProducingLevel, bool AutoUpdateChunks) const
{
    auto IsOutsideWorld = [this](int32_t X, int32_t Y, int32_t Z) -> bool
    {
        return X < 0 || Y < 0 || Z < 0 || X >= static_cast<int32_t>(this->Size) * SnazzCraft::Chunk::Width || Y >= SnazzCraft::Chunk::Height || Z >= static_cast<int32_t>(this->Size) * SnazzCraft::Chunk::Depth;
    };

    auto GetLightValue = [LightOrigin, LightProducingLevel](int32_t X, int32_t Y, int32_t Z) -> int32_t
    {
        int32_t TranslationX = glm::abs(LightOrigin[0] - X);
        int32_t TranslationY = glm::abs(LightOrigin[1] - Y);
        int32_t TranslationZ = glm::abs(LightOrigin[2] - Z);

        return LightProducingLevel - (TranslationX + TranslationY + TranslationZ);
    };

    std::vector<SnazzCraft::Chunk*> ChunksToUpdate;

    for (int32_t X = LightOrigin[0] - LightProducingLevel; X <= LightOrigin[0] + LightProducingLevel; X++) {
    for (int32_t Y = LightOrigin[1] - LightProducingLevel; Y <= LightOrigin[1] + LightProducingLevel; Y++) {
    for (int32_t Z = LightOrigin[2] - LightProducingLevel; Z <= LightOrigin[2] + LightProducingLevel; Z++) {
        if (IsOutsideWorld(X, Y, Z)) continue;

        int32_t ChunkCoordinates[2];
        SnazzCraft::Chunk::GetChunkPosition(X, Z, ChunkCoordinates);
        auto ChunkIterator = this->Chunks->find(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));
        if (ChunkIterator == this->Chunks->end()) continue;

        int32_t LightValue = GetLightValue(X, Y, Z);
        if (LightValue <= 0) continue;

        int32_t Local[3];
        SnazzCraft::Chunk::GetLocalVoxelPosition(X, Y, Z, Local);
        int32_t LightIndex = INDEX_3D(Local[0], Local[1], Local[2], SnazzCraft::Chunk::Width, SnazzCraft::Chunk::Height);
        auto LightIterator = ChunkIterator->second->LightValues->find(LightIndex);

        if (LightIterator == ChunkIterator->second->LightValues->end() || LightIterator->second < LightValue) {
            ChunkIterator->second->LightValues->insert_or_assign(LightIndex, LightValue);

            if (!AutoUpdateChunks) continue;
            ChunksToUpdate.push_back(ChunkIterator->second);
        }
    }
    }
    }

    for (SnazzCraft::Chunk* Chunk : ChunksToUpdate) {
        std::lock_guard<std::mutex> LockGuard(this->ChunkMeshUpdateQueueMutex);
        this->ChunkMeshUpdateQueue->push_back(Chunk);
    }
}

SnazzCraft::World* SnazzCraft::World::CreateWorld(std::string Name, uint32_t Size, int32_t Seed)
{
    uint32_t GenerateSize = Size == 0 ? 
        SnazzCraft::World::MaxSize : 
        Size % SnazzCraft::World::MaxSize;

    SnazzCraft::World* NewWorld = new SnazzCraft::World(Name, GenerateSize, Seed);

    for (uint32_t X = 0; X < NewWorld->Size; X++) {
    for (uint32_t Z = 0; Z < NewWorld->Size; Z++) {
        NewWorld->GenerateChunk(X, Z);
    } 
    }
    NewWorld->UpdateAndApplyAllLighting();

    return NewWorld;
}

SnazzCraft::World* SnazzCraft::World::LoadWorldFromSaveFile(std::string FilePath)
{
    std::ifstream File(FilePath);
    if (!File.is_open()) return nullptr;

    std::vector<SnazzCraft::Chunk*> NewWorldChunks;
    std::string NewWorldName;
    uint32_t NewWorldSize;
    int32_t NewWorldSeed;

    std::string Line;

    SnazzCraft::Chunk* NewChunk;
    char* EmptyChar = (char*)malloc(sizeof(char));
    *EmptyChar = ' ';

    while (std::getline(File, Line))
    {
        if (Line.size() == 0) continue;
        if (Line[0] == ' ')   continue;

        uint32_t LineIndex = 3;
        std::string Data;

        SnazzCraft::ParseData(Data, Line, LineIndex, NULL);
       
        switch (Line[0])
        {
            case WORLD_SAVE_FILE_DESCRIPTOR_NAME:
            {
                uint32_t DataIndex = 0;
                SnazzCraft::ParseData(NewWorldName, Data, DataIndex, NULL);
                break;
            }

            case WORLD_SAVE_FILE_DESCRIPTOR_SIZE:
            {
                std::string Size;
                uint32_t DataIndex = 0;
                SnazzCraft::ParseData(Size, Data, DataIndex, EmptyChar);

                NewWorldSize = static_cast<uint32_t >(stoul(Size));
                break;
            }

            case WORLD_SAVE_FILE_DESCRIPTOR_WORLD_SEED:
            {
                std::string Seed;
                uint32_t DataIndex = 0;
                SnazzCraft::ParseData(Seed, Data, DataIndex, EmptyChar);

                NewWorldSeed = stoi(Seed);
                break;
            }

            case WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_BEGIN:
            {
                std::string NewCoordinate;
                int32_t NewCoordinates_Int[2];
                uint32_t DataIndex = 0;

                SnazzCraft::ParseData(NewCoordinate, Data, DataIndex, EmptyChar);

                NewCoordinates_Int[0] = stoi(NewCoordinate);
                NewCoordinate.clear();

                DataIndex++;
                SnazzCraft::ParseData(NewCoordinate, Data, DataIndex, EmptyChar);
                NewCoordinates_Int[1] = stoi(NewCoordinate);

                NewChunk = new SnazzCraft::Chunk(NewCoordinates_Int[0], NewCoordinates_Int[1]);
                break;
            }
                

            case WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_END:
            {
                NewChunk->CullVoxelFaces();
                NewChunk->UpdateMesh();

                NewWorldChunks.push_back(NewChunk);
                break;
            }    

            case WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_NEW_VOXEL:
            {
                uint32_t DataIndex = 0;
                std::string NewInfo;
                uint32_t NewVoxelInfo[4]; // X, Y, Z, ID

                for (uint32_t I = 0; I < 3; I++) {
                    SnazzCraft::ParseData(NewInfo, Data, DataIndex, EmptyChar);
                    NewVoxelInfo[I] = static_cast<uint32_t >(stoul(NewInfo));
                    NewInfo.clear();
                    DataIndex++;
                }

                SnazzCraft::ParseData(NewInfo, Data, DataIndex, EmptyChar); 
                NewVoxelInfo[3] = static_cast<uint32_t >(stoul(NewInfo));

                NewChunk->Voxels->insert({ LOCAL_VOXEL_INDEX(NewVoxelInfo[0], NewVoxelInfo[1], NewVoxelInfo[2]), SnazzCraft::Voxel(NewVoxelInfo[0], NewVoxelInfo[1], NewVoxelInfo[2], NewVoxelInfo[3]) }); 

                break;
            }
        }
    }

    SnazzCraft::World* NewWorld = new SnazzCraft::World(NewWorldName, NewWorldSize, NewWorldSeed);

    for (SnazzCraft::Chunk* ChunkPointer : NewWorldChunks) {
        NewWorld->Chunks->insert({ INDEX_2D(ChunkPointer->Position[0], ChunkPointer->Position[1], NewWorld->Size), ChunkPointer });
    }

    File.close();
    free(EmptyChar);

    return NewWorld;
}

