#include "HeapNode.h"
#include "ActorList.h"

HeapNode::HeapNode() {

}

HeapNode::HeapNode(u32 addr, size_t headerSize, size_t blockSize) {
    this->addr = addr;
    this->headerSize = headerSize;
    this->blockSize = blockSize;
    reset();
}

HeapNode::~HeapNode() {

}

void HeapNode::reset() {
    this->free = true;
    this->description = "Empty";
    this->rooms = "ALL";
    this->nodeType = "OTHER";
    this->actorId = NULL;
    this->actorParams = NULL;
}

string HeapNode::toString() {
    std::stringstream stream;

    stream << std::hex << this->addr;
    std::string header(stream.str());
    stream.str("");
    stream.clear();

    stream << std::hex << this->addr+this->headerSize;
    std::string data(stream.str());
    stream.str("");
    stream.clear();

    stream << hex << this->blockSize;
    std::string blockSize(stream.str());
    stream.str("");
    stream.clear();

    stream << std::hex << this->nextNodeAddr;
    std::string nextNodeAddr(stream.str());
    stream.str("");
    stream.clear();

    stream << std::hex << this->prevNodeAddr;
    std::string prevNodeAddr(stream.str());
    stream.str("");
    stream.clear();

    return "header:" + header + " data:" + data + " free:" + to_string(this->free) + " blockSize:" + blockSize + " next_addr:" + nextNodeAddr 
        + " prev_addr:" + prevNodeAddr + " - " + this->description;
}