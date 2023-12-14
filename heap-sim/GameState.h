#pragma once

#include "HeapNode.h"

class GameState
{
public:
    GameState(string game, string version, unordered_map<std::string, bool> startFlags);
	~GameState();
    void LoadScene(int sceneId, int setupId, int roomId);
    void UnloadRoomsExcept(int roomId);
    HeapNode Alloc(int allocSize, string description);
    HeapNode AllocActor(int actorId, string rooms = "ALL", int actorParams = 0x0000, vector<int>position = { 0, 0, 0 });
    void Dealloc(u32 nodeAddr);

    nlohmann::json actorInfo;
    nlohmann::json sceneInfo;
    nlohmann::json versionInfo;
    nlohmann::json setupData;

    string console;
    string game;
    string version;
    u32 heapStart;
    size_t headerSize;
    unordered_map<std::string, bool> flags;
    int sceneId;
    int setupId;
    //std::vector<HeapNode> heap;
    std::unordered_set<int> loadedObjects;
    unordered_map<int, nlohmann::json> actorDefs;
    unordered_map<int, unordered_map<string, int>> actorStates; // basically just used for amount of each actor loaded. could jsut be map of int -> int
    unordered_map<u32, HeapNode> ram;
};

