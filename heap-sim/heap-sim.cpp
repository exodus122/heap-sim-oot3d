#include "GameState.h"

enum versions { MM_JP0, MM_JP1, MM_US, OOT_3D };
const vector<int> LINK_SIZE = { 0x30, 0x30, 0x10, 0x10 };
const vector<int> HEAP_START_ADDR = { 0x40B3B0, 0x40B670, 0x40b140, 0x9907450 };

int main()
{
    std::cout << "Hello World!\n";
    GameState state("OoT3D", "OoT3D-US", {});
    state.LoadScene(0, 0, 0);
}
