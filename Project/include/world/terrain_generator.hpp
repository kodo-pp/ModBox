#ifndef WORLD_TERRAIN_GENERATOR_HPP
#define WORLD_TERRAIN_GENERATOR_HPP

#include <set>
#include <map>
#include <world/world.hpp>
#include <geometry/game_position.hpp>

class TerrainGenerator {
public:
    TerrainGenerator(Seed _seed);

    std::map <ChunkId, Chunk> generate(std::set <ChunkId> chunks);
    std::map <ChunkId, Chunk> generateRange(GamePosition position, double range);

protected:
    Seed seed;
};

#endif /* end of include guard: WORLD_TERRAIN_GENERATOR_HPP */
