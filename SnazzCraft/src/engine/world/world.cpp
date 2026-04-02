#include "snazzcraft-engine/world/world.hpp"

SnazzCraft::World* SnazzCraft::CurrentWorld = nullptr;

SnazzCraft::World::World(std::string Name, uint32_t Size, int Seed)
{
    this->Size = Size;
    this->Name = Name;
    this->Seed = Seed;

    this->Chunks = new std::unordered_map<uint32_t, SnazzCraft::Chunk*>();
    this->WorldHeightMap = new SnazzCraft::HeightMap(this->Size * SnazzCraft::Chunk::Width, 0, SnazzCraft:: Chunk::Height - 1, this->Seed, 1.0, 0.5, 2.0, 6);
}

SnazzCraft::World::~World()
{
    for (auto& ChunkPair : *this->Chunks) {
        delete ChunkPair.second;
    }

    delete this->Chunks;
    delete this->WorldHeightMap;
}

void SnazzCraft::World::GenerateChunk(uint32_t X, uint32_t Z, bool ApplyLighting) const
{
    if (X >= SnazzCraft::World::Size || Z >= SnazzCraft::World::Size) return;

    uint32_t ChunkIndex = INDEX_2D(X, Z, this->Size);
    auto Iterator = this->Chunks->find(ChunkIndex);
    if (Iterator != this->Chunks->end()) return;
    
    SnazzCraft::Chunk* NewChunk = new SnazzCraft::Chunk(X, Z);

    NewChunk->Generate(this->WorldHeightMap, this->Size * SnazzCraft::Chunk::Width);
    NewChunk->CullVoxelFaces();
    NewChunk->UpdateVerticesAndIndices();   

    (*this->Chunks)[ChunkIndex] = NewChunk;
    if (ApplyLighting) this->UpdateChunkLighting(NewChunk); 
    NewChunk->UpdateMesh();
}

void SnazzCraft::World::RenderChunks(SnazzCraft::User* Player)
{ 
    int32_t PlayerChunkPosition[2];
    SnazzCraft::Chunk::GetChunkPosition(Player->Position, PlayerChunkPosition);

    for (int32_t X = PlayerChunkPosition[0] - static_cast<int32_t>(this->RenderDistance); X <= PlayerChunkPosition[0] + static_cast<int32_t>(this->RenderDistance); X++) {
    for (int32_t Z = PlayerChunkPosition[1] - static_cast<int32_t>(this->RenderDistance); Z <= PlayerChunkPosition[1] + static_cast<int32_t>(this->RenderDistance); Z++) {
        if (X < 0 || X >= static_cast<int32_t>(this->Size) || Z < 0 || Z >= static_cast<int32_t>(this->Size)) continue;

        auto ChunkIterator = this->Chunks->find(INDEX_2D(X, Z, static_cast<int32_t>(this->Size)));
        if (ChunkIterator == this->Chunks->end()) continue;

        if (ChunkIterator->second->ChunkMesh == nullptr) continue; 
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


void SnazzCraft::World::ApplyLightingVoxel(int32_t LightOrigin[3], int32_t LightProducingLevel, std::unordered_set<uint32_t>& ChunksToUpdate) const
{
    auto IsOutsideWorld = [this](int32_t X, int32_t Y, int32_t Z) -> bool
    {
        return X < 0 || Y < 0 || Z < 0 || X >= static_cast<int32_t>(this->Size) * SnazzCraft::Chunk::Width || Y >= SnazzCraft::Chunk::Height || Z >= static_cast<int32_t>(this->Size) * SnazzCraft::Chunk::Depth;
    };

    auto AddLightNodes = [this](std::queue<SnazzCraft::World::LightNode>& Queue, const SnazzCraft::World::LightNode& OriginNode) -> void
    {
        if (OriginNode.LightValue <= 1) return;

        for (uint8_t I = 0; I < 3; I++) {
            for (int8_t T = -1; T < 2; T += 2) {
                int32_t NewPosition[3] = {
                    OriginNode.X,
                    OriginNode.Y,
                    OriginNode.Z
                };
                NewPosition[I] += T;

                int32_t NewLightValue = OriginNode.LightValue - 1;

                Queue.push(SnazzCraft::World::LightNode(NewLightValue, NewPosition));
            }
        }
    };

    std::queue<SnazzCraft::World::LightNode> Queue;
    SnazzCraft::World::LightNode LightOriginNode(LightProducingLevel, LightOrigin);
    Queue.push(LightOriginNode);

    bool First = true;
    while (!Queue.empty())
    {
        SnazzCraft::World::LightNode CurrentNode = Queue.front();
        Queue.pop();

        if (IsOutsideWorld(CurrentNode.X, CurrentNode.Y, CurrentNode.Z)) continue;

        int32_t ChunkCoordinates[2];
        SnazzCraft::Chunk::GetChunkPosition(CurrentNode.X, CurrentNode.Z, ChunkCoordinates);
        auto ChunkIterator = this->Chunks->find(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));
        
        if (ChunkIterator == this->Chunks->end()) {
            this->GenerateChunk(ChunkCoordinates[0], ChunkCoordinates[1], true);
            ChunkIterator = this->Chunks->find(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));
        }
        if (ChunkIterator == this->Chunks->end()) continue;

        int32_t Local[3];
        SnazzCraft::Chunk::GetLocalVoxelPosition(CurrentNode.X, CurrentNode.Y, CurrentNode.Z, Local);
        int32_t LightIndex = SnazzCraft::Chunk::LocalVoxelIndex(Local[0], Local[1], Local[2]);
        
        auto ExistingLightIterator = ChunkIterator->second->LightValues->find(LightIndex);
        int32_t CurrentExistingValue = (ExistingLightIterator == ChunkIterator->second->LightValues->end()) ? 0 : ExistingLightIterator->second;

        if (CurrentNode.LightValue > CurrentExistingValue) {
            ChunkIterator->second->LightValues->insert_or_assign(LightIndex, CurrentNode.LightValue);
            ChunksToUpdate.insert(INDEX_2D(ChunkCoordinates[0], ChunkCoordinates[1], this->Size));

            int32_t LocalVoxelPosition[3];
            SnazzCraft::Chunk::GetLocalVoxelPosition(CurrentNode.X, CurrentNode.Y, CurrentNode.Z, LocalVoxelPosition);
            auto VoxelIterator = ChunkIterator->second->OptimizedVoxels->find(SnazzCraft::Chunk::LocalVoxelIndex(LocalVoxelPosition[0], LocalVoxelPosition[1], LocalVoxelPosition[2]));
            
            if (CurrentNode.LightValue > 1 && (VoxelIterator == ChunkIterator->second->OptimizedVoxels->end() || First)) AddLightNodes(Queue, CurrentNode);
        }
        First = false; 
    }
}

SnazzCraft::World* SnazzCraft::World::CreateWorld(std::string Name, uint32_t Size, int32_t Seed)
{
    uint32_t GenerateSize = Size == 0 || Size > SnazzCraft::World::MaxSize ? 
        SnazzCraft::World::MaxSize : 
        Size;

    SnazzCraft::World* NewWorld = new SnazzCraft::World(Name, GenerateSize, Seed);

    for (uint32_t X = 0; X < NewWorld->Size; X++) {
    for (uint32_t Z = 0; Z < NewWorld->Size; Z++) {
        NewWorld->GenerateChunk(X, Z, true);
    } 
    }
    
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

                NewChunk->Voxels->insert({ SnazzCraft::Chunk::LocalVoxelIndex(NewVoxelInfo[0], NewVoxelInfo[1], NewVoxelInfo[2]), SnazzCraft::Voxel(NewVoxelInfo[0], NewVoxelInfo[1], NewVoxelInfo[2], NewVoxelInfo[3]) }); 

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

