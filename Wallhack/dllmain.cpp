/* 
	A wallhack for bzflag. It works by hooking glBegin and redirecting it to a codecave. This codecave is responsible
	for disabling depth testing (0xb71), and then redirecting flow to glBegin. As a result, glBegin will draw
	all objects on the same plane.

	This dll needs to be injected into bzflag.
*/
#include <Windows.h>

HMODULE openglBase = NULL;
unsigned char* glBeginAddress = 0;
DWORD* glDisableAddress = 0;
DWORD oldProt = 0;

// The codecave responsible for calling glDisable. Our hook in DllMain redirects program flow to this.
__declspec(naked) void hook() {
	__asm {
		mov eax, dword ptr fs:[0x18]	// the original instruction hooked
		pushad							// save all registers
		push 0xb71						
		call glDisableAddress			// glDisable(GL_DEPTH_TEST)
		popad							// restore all registers
		retn							// return to glBegin
	}
}

// Dllmain is responsible for patching glBegin and redirecting it to our codecave
bool APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			// Get the base address of OpenGL and glBegin and glDisable. These are loaded dynamically and change.
			openglBase = GetModuleHandle("opengl32.dll");
			glBeginAddress = (unsigned char*)GetProcAddress(openglBase, "glBegin");
			glDisableAddress = (DWORD*)GetProcAddress(openglBase, "glDisable");

			// Change the protection type of glBegin so we can modify the code
			VirtualProtect(glBeginAddress, 20, PAGE_EXECUTE_READWRITE, &oldProt);

			// Insert a call to our codecave
			// e8 = call
			// The offset is determined by subtracting the location of our hook from the calling location + 5
			glBeginAddress[5] = 0xe8;
			DWORD offset = (DWORD)&hook - ((DWORD)glBeginAddress + 5 + 5);
			memcpy(glBeginAddress + 6, &offset, 4);

			// Since the instruction we replaced is 6 bytes, nop out the last byte to restore the code
			glBeginAddress[10] = 0x90;
			break;
	}

	return true;
}
