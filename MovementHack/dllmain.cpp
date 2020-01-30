/*
	A movement hack for bzflag. Our injected thread allows us to directly modify the player's position and bypass the 
	game's move() function. Because of this, we can faster and through walls.

	This dll needs to be injected into bzflag.
*/
#include <Windows.h>

// The player structure, trimmed down from the bzflag source
struct Player {
	char unknown[0x1ec];
	//170 flag, 4 bytes
	short status;
	float pos[3];
};

Player *player = NULL;

// Since bzflag loads the player and class dynamically, the offsets are calculated
// based off a static pointer found in CE. This will need to be adjusted for different versions
// of the game.
void loadOffsets() {
	HMODULE baseOffset = NULL;
	DWORD *playerOffset = 0;

	baseOffset = GetModuleHandle("bzflag.exe");
	
	playerOffset = (DWORD*)((DWORD)baseOffset + 0x00253F44);
	player = (Player*)(*playerOffset);
}

// Our injected thread constantly loops and awaits input. When it receives the right or left arrow,
// it adjusts the player's position.
DWORD WINAPI injected_thread(LPVOID lpvParam) {
	while (true) {
		// Loop until we have valid player class
		// This only occurs when we join a game
		if (player == NULL) {
			Sleep(100);
			loadOffsets();
			continue;
		}

		// Our strafe functionality
		if (GetAsyncKeyState(VK_RIGHT)) {
			player->pos[0] += 0.1f;
			player->pos[1] += 0.1f;
		}
		else if (GetAsyncKeyState(VK_LEFT)) {
			player->pos[0] -= 0.1f;
			player->pos[1] -= 0.1f;
		}

		// Required so the game can run other threads
		Sleep(1);
	}

	return 0;
}

// Our DllMain function creates a thread within bzflag 
bool APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			CreateThread(NULL, 0, injected_thread, NULL, NULL, NULL);
			break;
	}

	return true;
}
