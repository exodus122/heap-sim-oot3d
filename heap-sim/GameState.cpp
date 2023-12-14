#include "GameState.h"

void OutputJsonExceptionInformation(nlohmann::json::parse_error& error)
{
	// output exception information
	std::cout << "message: " << error.what() << '\n'
		<< "exception id: " << error.id << '\n'
		<< "byte position of error: " << error.byte << std::endl;
}

nlohmann::json ParseJson(const std::string& filename)
{
	try
	{
		std::ifstream f(filename);
		std::cout << "loading " << filename << "..." << std::endl;
		return nlohmann::json::parse(f);
	}
	catch (nlohmann::json::parse_error& e)
	{
		OutputJsonExceptionInformation(e);
		return NULL;
	}
}

// Constructor. Initialize the empty heap.
GameState::GameState(string game, string version, unordered_map<std::string, bool> startFlags) {
	this->actorInfo = ParseJson("./actors.json");
	this->sceneInfo = ParseJson("./scenes.json");
	this->versionInfo = ParseJson("./versions.json");

    this->game = game;
    this->version = version;
    this->heapStart = versionInfo[version]["heapStart"];
	this->console = to_string(versionInfo[version]["console"]);
	//this->game = to_string(versionInfo[version]["game"]);
    this->headerSize = 0x10;
    this->flags = startFlags;
}

// Destructor
GameState::~GameState() {
	//heapNodes.clear();
}

void GameState::LoadScene(int sceneId, int setupId, int roomId) {
	this->setupData = sceneInfo[this->version][sceneId][setupId];
	this->sceneId = sceneId;
	this->setupId = setupId;
	this->loadedObjects.insert(0x0001);

	if (setupId != 2 && setupId != 3)
		this->loadedObjects.insert(0x0015); // object_link_child
	if (setupId != 0 && setupId != 1)
		this->loadedObjects.insert(0x0014); // object_link_boy

	int specialObject = this->setupData["specialObject"];
	if (specialObject != 0)
		this->loadedObjects.insert(specialObject);

	/*for (auto x : loadedObjects)
		cout << hex << x << endl;*/

	this->actorDefs.clear();
	this->actorStates.clear();
	for (int actorId = 0; actorId < actorInfo[this->game].size(); actorId++) {
		//cout << actorId << endl;
		this->actorDefs[actorId] = actorInfo[this->game][actorId];
	}

	/*for (auto x : actorDefs)
		cout << hex << x << endl;*/

	this->ram.clear();
	this->ram.insert(std::make_pair(this->heapStart, HeapNode(this->heapStart, this->headerSize, 0x100000000 - this->headerSize)));

	/*for (auto x : ram)
		cout << x.second.toString() << endl;*/



	/*this->allocActor(actors.Player, 'ALL') # Link
	this->alloc(0x2010, 'Get Item Object')
	this->allocActor(actors.En_Elf, 'ALL') # Navi

	this->loadedRooms = set()

	this->loadRoom(roomId)*/

}

void GameState::UnloadRoomsExcept(int roomId) {
	//assert roomId in self.loadedRooms
	/*self.loadedRooms = set([roomId]);

	for node in self.heap() :
		if not node.freeand node.rooms != 'ALL' :
			for actorRoomId in node.rooms :
				if actorRoomId in self.loadedRooms :
					break
			else :
				self.dealloc(node.addr)*/
}

HeapNode GameState::Alloc(int allocSize, string description) {
	allocSize = allocSize + ((-allocSize) % 0x10);

	u32 nodeAddr = this->heapStart;
	while (nodeAddr != 0) {
		HeapNode node = this->ram[nodeAddr];

		if (node.free && node.blockSize >= allocSize) {
			if (node.blockSize > allocSize + this->headerSize) {
				HeapNode newNode(node.addr + this->headerSize + allocSize, this->headerSize, size_t(node.blockSize - allocSize - this->headerSize));
				this->ram[newNode.addr] = newNode;
				node.blockSize = allocSize;
				newNode.prevNodeAddr = node.addr;
				newNode.nextNodeAddr = node.nextNodeAddr;
				if (node.nextNodeAddr)
					this->ram[node.nextNodeAddr].prevNodeAddr = newNode.addr;
				node.nextNodeAddr = newNode.addr;
			}
			node.free = false;
			node.description = description;
			return node;
		}

		nodeAddr = this->ram[nodeAddr].nextNodeAddr;
	}

	throw("Alloc failed");
}

void GameState::Dealloc(u32 nodeAddr) {
	HeapNode node = this->ram[nodeAddr];
	if (node.free)
		cout << "node is free. tried to delloc. " << hex << nodeAddr << endl;

	if(node.nodeType == "INSTANCE") {
		nlohmann::json actorDef = this->actorDefs[node.actorId];
		unordered_map<string, int> actorState = this->actorStates[node.actorId];
		actorState["numLoaded"] -= 1;
	}
	if (this->ram[node.nextNodeAddr].free) {
		node.blockSize += this->headerSize + this->ram[node.nextNodeAddr].blockSize;
		node.nextNodeAddr = this->ram[node.nextNodeAddr].nextNodeAddr;
		if(node.nextNodeAddr > 0)
			this->ram[node.nextNodeAddr].prevNodeAddr = node.addr;
	}
	if (this->ram[node.prevNodeAddr].free) {
		this->ram[node.prevNodeAddr].blockSize += this->headerSize + node.blockSize;
		this->ram[node.prevNodeAddr].nextNodeAddr = node.nextNodeAddr;
		if (node.nextNodeAddr > 0)
			this->ram[node.nextNodeAddr].prevNodeAddr = node.prevNodeAddr;
	}

	node.reset();
}

HeapNode GameState::AllocActor(int actorId, string rooms, int actorParams, vector<int> position) {
	if (!this->actorStates.count(actorId))
		this->actorStates[actorId]["numLoaded"] = 0;
	unordered_map<string, int> actorState = this->actorStates[actorId];
	nlohmann::json actorDef = this->actorDefs[actorId];

	std::stringstream stream;

	stream << std::hex << actorId;
	std::string actorId2(stream.str());
	stream.str("");
	stream.clear();

	stream << std::hex << actorParams;
	std::string actorParams2(stream.str());
	stream.str("");
	stream.clear();

	HeapNode instanceNode = this->Alloc(actorDef["instanceSize"], "Actor " + actorId2 + " " + to_string(actorDef["name"]) + " (" + actorParams2 + ")");
	instanceNode.rooms = rooms;
	instanceNode.nodeType = "INSTANCE";
	instanceNode.actorId = actorId;
	instanceNode.actorParams = actorParams;
	instanceNode.position = position;

	actorState["numLoaded"] += 1;

	return instanceNode;
}