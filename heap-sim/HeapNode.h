#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include "json.hpp"
using namespace std;

typedef unsigned int u32;

class HeapNode
{
public:
    HeapNode();
    HeapNode(u32 addr, size_t headerSize, size_t blockSize);
    ~HeapNode();
    
    void reset();
    string toString();

    u32 addr;
    size_t headerSize = headerSize;
    size_t blockSize = blockSize;
    u32 prevNodeAddr = 0;
    u32 nextNodeAddr = 0;

    bool free = true;
    string description = "Empty";
    string rooms = "ALL";
    string nodeType = "OTHER";
    int actorId = NULL;
    int actorParams = NULL;
    vector<int> position;
};

