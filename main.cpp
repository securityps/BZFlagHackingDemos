/*
	A DLL injector that uses CreateRemoteThread to call LoadLibrary within the target process.

	It requires the Dll to be in the same directory.

	Usage: ./Injector WindowTitle Dll
*/

#include <Windows.h>
#include <iostream>

int main(int argc, char** argv) {
	char full_dll_path[512] = { 0 };

	DWORD processId = 0;
	HANDLE process = NULL;
	void* lpBaseAddress;
	DWORD exitCode;

	if (argc < 3) {
		std::cout << "Need a process name and dll name" << std::endl;
		return 1;
	}

	// Get our full dll path. Since we will be writing this within the processes memory, we cannot use the relative path.
	GetModuleFileName(NULL, full_dll_path, sizeof(full_dll_path));
	for (int i = strlen(full_dll_path); full_dll_path[i] != '\\'; i--)
		full_dll_path[i] = 0;
	strcat_s(full_dll_path, sizeof(full_dll_path), argv[2]);

	// FindWindow is considered bad due to changing titles. Better to enumerate the processes and use that to select the target.
	HWND handle = FindWindow(NULL, argv[1]);
	if (handle == NULL) {
		std::cout << "Couldn't find window" << std::endl;
		return 1;
	}

	// OpenProcess requires a processID, so we use this to get one from the window handle.
	GetWindowThreadProcessId(handle, &processId);
	if (!processId) {
		std::cout << "Couldn't get process id" << std::endl;
		return 1;
	}

	// Open the process with all permissions. May fail if not run as admin.
	if(!(process = OpenProcess(PROCESS_ALL_ACCESS, true, processId))) {
		std::cout << "No process" << std::endl;
		return 1;
	}

	// Since we are calling LoadLibrary from within the application's memory, we need to allocate memory within the application to
	// hold the full path of our dll
	if (!(lpBaseAddress = VirtualAllocEx(process, NULL, strlen(full_dll_path) + 1, MEM_COMMIT, PAGE_READWRITE))) {
		std::cout << "No memory" << std::endl;
		return false;
	}

	// Write our full path into the memory just allocated.
	WriteProcessMemory(process, lpBaseAddress, full_dll_path, strlen(full_dll_path) + 1, NULL);

	// Get the base of kernel32 module loaded within the application.
	HMODULE kernel32base = GetModuleHandle("kernel32.dll");
	if (kernel32base == NULL) {
		std::cout << "Couldn't get kernel32 base" << std::endl;
		return false;
	}
	
	// Create a thread within the application. This thread will call LoadLibraryA and pass in the base address of our allocated module name,
	// loading our dll within the process. The DLL_PROCESS_ATTACH switch will then be called within the DLL.
	HANDLE thread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32base, "LoadLibraryA"), lpBaseAddress, 0, NULL);
	if (thread == NULL) {
		std::cout << "Couldn't create thread" << std::endl;
		return false;
	}

	// Verify that the thread created and exited successfully.
	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &exitCode);

	if (!exitCode) {
		std::cout << "Couldn't inject" << std::endl;
		return false;
	}

	// Free memory and close all the handles we created.
	VirtualFreeEx(process, lpBaseAddress, 0, MEM_RELEASE);
	CloseHandle(thread);
	CloseHandle(process);

	return 0;
}
