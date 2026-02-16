#include "world.hpp"

SnazzCraft::World* SnazzCraft::CurrentWorld = nullptr;

SnazzCraft::World::World(std::string Name, unsigned int Size, int Seed)
{
    this->Size = Size;
    this->Name = Name;
    this->Seed = Seed;

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

bool SnazzCraft::World::GenerateChunk(unsigned int X, unsigned int Z)
{
    {
        std::lock_guard<std::mutex> Lock(this->ChunkMutex);

        auto Iterator = this->Chunks->find(INDEX_2D(X, Z, this->Size));
        if (Iterator != this->Chunks->end()) return false;
    }
    
    SnazzCraft::Chunk* NewChunk = new SnazzCraft::Chunk(X, Z);
    NewChunk->Generate(this->WorldHeightMap, this->Size * SnazzCraft::Chunk::Width);

    {
        std::lock_guard<std::mutex> Lock(this->ChunkMutex);
        (*this->Chunks)[INDEX_2D(X, Z, this->Size)] = NewChunk;
    }

    return true;
}

void SnazzCraft::World::RenderChunks()
{ 
    for (auto& ChunkPair : *this->Chunks) {
        if (ChunkPair.second->ChunkMesh == nullptr) continue;

        ChunkPair.second->ChunkMesh->Draw();
    }
}

void SnazzCraft::World::OptimizeChunks()
{
    for (auto& ChunkPair : *this->Chunks) {
        ChunkPair.second->CullVoxelFaces();
        ChunkPair.second->UpdateMesh();
    }
}

SnazzCraft::Voxel* SnazzCraft::World::IsCollidingVoxel(const SnazzCraft::Hitbox* Hitbox)
{
    int ChunkX = static_cast<int>(Hitbox->Position.x / (SnazzCraft::Chunk::Width * SnazzCraft::Voxel::Size));
    int ChunkZ = static_cast<int>(Hitbox->Position.z / (SnazzCraft::Chunk::Depth * SnazzCraft::Voxel::Size));

    for (int X = ChunkX - 1; X <= ChunkX + 1; X++) {
    for (int Z = ChunkZ - 1; Z <= ChunkZ + 1; Z++) {
        if (X < 0 || Z < 0 || X >= static_cast<int>(this->Size) || Z >= static_cast<int>(this->Size)) continue;

        auto ChunkIterator = this->Chunks->find(INDEX_2D(X, Z, this->Size));
        if (ChunkIterator == this->Chunks->end()) continue;

        SnazzCraft::Voxel* CollisionVoxel = ChunkIterator->second->IsCollidingVoxel(Hitbox);
        if (CollisionVoxel != nullptr) return CollisionVoxel;
    }
    }

    return nullptr;
}

void SnazzCraft::World::MoveEntity(SnazzCraft::Entity* Entity, const glm::vec3& Rotation, float Distance)
{
    glm::vec3 NewPosition = Entity->Position;
    MoveVector3D(NewPosition, Entity->Rotation + Rotation, Distance);

    for (unsigned int I = 0; I < 3; I++) {
        float OldCoordinate = Entity->Position[I];

        Entity->Position[I] = NewPosition[I];
        Entity->EntityHitbox->Position[I] = NewPosition[I];

        SnazzCraft::Voxel* CollisionVoxel = this->IsCollidingVoxel(Entity->EntityHitbox);
        if (CollisionVoxel == nullptr) continue;

        Entity->Position[I] = OldCoordinate;
        Entity->EntityHitbox->Position[I] = OldCoordinate;
    }
}

void SnazzCraft::World::MoveEntity(glm::vec3 Translation, SnazzCraft::Entity* Entity)
{
    for (unsigned int I = 0; I < 3; I++) {
        float OldCoordinate = Entity->Position[I];

        Entity->Position[I] += Translation[I];
        Entity->EntityHitbox->Position[I] = Entity->Position[I];

        SnazzCraft::Voxel* CollisionVoxel = this->IsCollidingVoxel(Entity->EntityHitbox);
        if (CollisionVoxel == nullptr) continue;

        Entity->Position[I] = OldCoordinate;
        Entity->EntityHitbox->Position[I] = OldCoordinate;
    }
}

void SnazzCraft::World::UpdateLighting()
{
    for (auto& ChunkPair : *this->Chunks) ChunkPair.second->LightValues->clear();

    for (auto& ChunkPair : *this->Chunks) {
    for (auto& VoxelPair : *ChunkPair.second->Voxels) {
        if (VoxelPair.second.LightProducingLevel <= 0) continue;

        int Position[3] = {
            static_cast<int>(VoxelPair.second.Position[0]) + ChunkPair.second->Position[0] * SnazzCraft::Chunk::Width,
            static_cast<int>(VoxelPair.second.Position[1]),
            static_cast<int>(VoxelPair.second.Position[2]) + ChunkPair.second->Position[1] * SnazzCraft::Chunk::Depth,
        };
        this->ApplyLighting(Position, VoxelPair.second.LightProducingLevel);
    }
    }
}

void SnazzCraft::World::ApplyLighting(int LightOrigin[3], int LightProducingLevel)
{
    auto OutsideWorld = [this](int X, int Y, int Z) -> bool
    {
        return X < 0 || Y < 0 || Z < 0 || X >= static_cast<int>(this->Size) * SnazzCraft::Chunk::Width || Y >= SnazzCraft::Chunk::Height || Z >= static_cast<int>(this->Size) * SnazzCraft::Chunk::Depth;
    };

    auto GetLightValue = [LightOrigin, LightProducingLevel](int X, int Y, int Z) -> int
    {
        int TranslationX = glm::abs(LightOrigin[0] - X);
        int TranslationY = glm::abs(LightOrigin[1] - Y);
        int TranslationZ = glm::abs(LightOrigin[2] - Z);

        return LightProducingLevel - (TranslationX + TranslationY + TranslationZ);
    };

    for (int X = LightOrigin[0] - LightProducingLevel; X <= LightOrigin[0] + LightProducingLevel; X++) {
    for (int Y = LightOrigin[1] - LightProducingLevel; Y <= LightOrigin[1] + LightProducingLevel; Y++) {
    for (int Z = LightOrigin[2] - LightProducingLevel; Z <= LightOrigin[2] + LightProducingLevel; Z++) {
        if (OutsideWorld(X, Y, Z)) continue;

        int ChunkX = X / SnazzCraft::Chunk::Width;
        int ChunkZ = Z / SnazzCraft::Chunk::Depth;
        auto ChunkIterator = this->Chunks->find(INDEX_2D(ChunkX, ChunkZ, this->Size));
        if (ChunkIterator == this->Chunks->end()) continue;

        int LocalX = X % SnazzCraft::Chunk::Width;
        int LocalY = Y % SnazzCraft::Chunk::Height;
        int LocalZ = Z % SnazzCraft::Chunk::Depth;

        int LightValue = GetLightValue(X, Y, Z);
        if (LightValue <= 0) continue;

        int LightIndex = INDEX_3D(LocalX, LocalY, LocalZ, SnazzCraft::Chunk::Width, SnazzCraft::Chunk::Height);
        auto LightIterator = ChunkIterator->second->LightValues->find(LightIndex);

        if (LightIterator == ChunkIterator->second->LightValues->end()) {
            ChunkIterator->second->LightValues->insert_or_assign(LightIndex, LightValue);
        } else {
            if (LightIterator->second >= LightValue) continue;
            ChunkIterator->second->LightValues->insert_or_assign(LightIndex, LightValue);
        }
    }
    }
    }
}

SnazzCraft::World* SnazzCraft::CreateWorld(std::string Name, unsigned int Size, int Seed)
{
    SnazzCraft::World* NewWorld = new SnazzCraft::World(Name, Size, Seed);

    for (unsigned int X = 0; X < NewWorld->Size; X++) {
    for (unsigned int Z = 0; Z < NewWorld->Size; Z++) {
        NewWorld->GenerateChunk(X, Z);
    }
    }

    return NewWorld;
}

SnazzCraft::World* SnazzCraft::LoadWorldFromSaveFile(std::string FilePath)
{
    std::ifstream File(FilePath);
    if (!File.is_open()) return nullptr;

    std::vector<SnazzCraft::Chunk*> NewWorldChunks;
    std::string NewWorldName;
    unsigned int NewWorldSize;
    int NewWorldSeed;

    std::string Line;

    SnazzCraft::Chunk* NewChunk;
    char* EmptyChar = (char*)malloc(sizeof(char));
    *EmptyChar = ' ';

    while (std::getline(File, Line))
    {
        if (Line.size() == 0) continue;
        if (Line[0] == ' ')   continue;

        unsigned int LineIndex = 3;
        std::string Data;

        SnazzCraft::ParseData(Data, Line, LineIndex, NULL);
       
        switch (Line[0])
        {
            case WORLD_SAVE_FILE_DESCRIPTOR_NAME:
            {
                unsigned int DataIndex = 0;
                SnazzCraft::ParseData(NewWorldName, Data, DataIndex, NULL);
                break;
            }

            case WORLD_SAVE_FILE_DESCRIPTOR_SIZE:
            {
                std::string Size;
                unsigned int DataIndex = 0;
                SnazzCraft::ParseData(Size, Data, DataIndex, EmptyChar);

                NewWorldSize = static_cast<unsigned int>(stoul(Size));
                break;
            }

            case WORLD_SAVE_FILE_DESCRIPTOR_WORLD_SEED:
            {
                std::string Seed;
                unsigned int DataIndex = 0;
                SnazzCraft::ParseData(Seed, Data, DataIndex, EmptyChar);

                NewWorldSeed = stoi(Seed);
                break;
            }

            case WORLD_SAVE_FILE_DESCRIPTOR_CHUNK_BEGIN:
            {
                std::string NewCoordinate;
                int NewCoordinates_Int[2];
                unsigned int DataIndex = 0;

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
                unsigned int DataIndex = 0;
                std::string NewInfo;
                unsigned int NewVoxelInfo[4]; // X, Y, Z, ID

                for (unsigned int I = 0; I < 3; I++) {
                    SnazzCraft::ParseData(NewInfo, Data, DataIndex, EmptyChar);
                    NewVoxelInfo[I] = static_cast<unsigned int>(stoul(NewInfo));
                    NewInfo.clear();
                    DataIndex++;
                }

                SnazzCraft::ParseData(NewInfo, Data, DataIndex, EmptyChar); 
                NewVoxelInfo[3] = static_cast<unsigned int>(stoul(NewInfo));

                NewChunk->Voxels->insert({ VOXEL_INDEX(NewVoxelInfo[0], NewVoxelInfo[1], NewVoxelInfo[2]), SnazzCraft::Voxel(NewVoxelInfo[0], NewVoxelInfo[1], NewVoxelInfo[2], NewVoxelInfo[3]) }); 

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

bool SnazzCraft::SaveWorldToFile(SnazzCraft::World* World, bool OverwriteExistingFile)
{
    if (World == nullptr) return false;

    const std::string FileDirectory = "worlds/";
    const std::string FileExtension = ".txt";

    std::string FilePath = FileDirectory + World->Name + FileExtension;
    std::ofstream File;

    if (std::filesystem::exists(FilePath) && !OverwriteExistingFile) {
        return false;
    } else {
        File.open(FilePath);
    }

    File << WORLD_SAVE_FILE_DESCRIPTOR_NAME       << ": " << World->Name << "\n";
    File << WORLD_SAVE_FILE_DESCRIPTOR_SIZE       << ": " << World->Size << "\n";
    File << WORLD_SAVE_FILE_DESCRIPTOR_WORLD_SEED << ": " << World->Seed << "\n";

    for (auto& ChunkPair : *World->Chunks) {
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

