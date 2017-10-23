// ac_hp_changer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cstddef>
#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <TlHelp32.h> //PROCESSENTRY
#include "ProcessMemoryReaderLib"

#define UNINITIALIZED 0xFFFFFFFF

using namespace std;

/* The name of the process */
const char* processName_ = "ac_client.exe";


int main()
{
	DWORD  processID_ = NULL;
	DWORD  processBaseAddress_ = UNINITIALIZED;

	/* Get the process ID  */
		PROCESSENTRY32 processEntry_; // Entry into process you wish to inject to
		HANDLE hProcSnapshot_ = NULL;
		/* Takes a snapshot of the system's processes */
		hProcSnapshot_ = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //?

		/* While process has not been found, keep looking for it */
		while (!processID_)
		{
			/* If a process on the system exists */
			if (Process32First(hProcSnapshot_, &processEntry_)) //?
			{
				/* Check all processes in the system's processes snapshot */
				do
				{
					/* Compare the name of the process to the one we want */
					if (!strcmp(processEntry_.szExeFile, processName_)) //?
					{
						/* Save the processID and break out */
						processID_ = processEntry_.th32ProcessID;
						break;
					}
				} while (Process32Next(hProcSnapshot_, &processEntry_));
			}

			/* Didnt find process, sleep for a bit */
			if (!processID_)
			{
				system("CLS");
				cout << "Make sure " << processName_ << " is running." << endl;
				Sleep(200);
			}
		}

		/* Process found */
		cout << "Found Process: " << processName_ << endl;
		cout << "Found Process: " << processID_ << endl;



	/* Find Base Address of process */
	{
		HANDLE moduleSnapshotHandle_ = INVALID_HANDLE_VALUE;
		MODULEENTRY32 moduleEntry_;

		/* Take snapshot of all the modules in the process */
		moduleSnapshotHandle_ = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID_);

		/* Snapshot failed */
		if (moduleSnapshotHandle_ == INVALID_HANDLE_VALUE)
		{
			cout << "Module Snapshot error" << endl;
			return 0;
		}

		/* Size the structure before usage */
		moduleEntry_.dwSize = sizeof(MODULEENTRY32);

		/* Retrieve information about the first module */
		if (!Module32First(moduleSnapshotHandle_, &moduleEntry_))
		{
			cout << "First module not found" << endl;
			CloseHandle(moduleSnapshotHandle_);
			return 0;
		}

		/* Find base address */
		while (processBaseAddress_ == UNINITIALIZED)
		{
			/* Find module of the executable */
			do
			{

				/* Compare the name of the process to the one we want */
				if (!strcmp(moduleEntry_.szModule, processName_)) //?
				{
					/* Save the processID and break out */
					processBaseAddress_ = (unsigned int)moduleEntry_.modBaseAddr;
					break;
				}

			} while (Module32Next(moduleSnapshotHandle_, &moduleEntry_));


			if (processBaseAddress_ == UNINITIALIZED)
			{
				system("CLS");
				cout << "Failed to find module" << processName_ << endl;
				Sleep(200);
			}
		}

		/* Found module and base address successfully */
		cout << "Base Address: " << hex << processBaseAddress_ << dec << endl;
		CloseHandle(moduleSnapshotHandle_);
	}

	//ReadProcessMemory(handle, (PBYTE*)jump , &readInValue, sizeof(readInValue), 0);
	//WriteProcessMemory(handle, (LPVOID)0x???????, &newValue, sizeof(newValue), 0);
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID_);

	while (1) {

		const int baseAddress = 0x400000;
		const int off_localPlayer = 0x50F4F4;
		const int off_health = 0xF8;
		const int off_grenade = 0x158;

		const int localPlayer = off_localPlayer;

		int playerBase = Mem.

		int healthValue; // Hold health Value

		// Press F1 to turn off
		if (GetKeyState(VK_F1)) {
			// Read in the players health value from memory
			ReadProcessMemory(handle, (PBYTE*)(localPlayer + off_health), &healthValue, sizeof(healthValue), 0);
			cout << healthValue << endl;

			// If health falls bellow 80 increase to 100
			if (healthValue < 80) {
				int newHP = 100;
				WriteProcessMemory(handle, (LPVOID)(localPlayer + off_health), &newHP, sizeof(newHP), 0);
				Sleep(10); //Ease off CPU
			}
		}

		// Press F2 to turn off
		if (GetKeyState(VK_F2)) {
			int noGrenades;
			// Read in the players health value from memory
			ReadProcessMemory(handle, (PBYTE*)(localPlayer + off_grenade), &noGrenades, sizeof(noGrenades), 0);

			// If health falls bellow 80 increase to 100
			if (noGrenades < 3) {
				int newGrenadeCount = 3;
				WriteProcessMemory(handle, (LPVOID)(localPlayer + off_grenade), &newGrenadeCount, sizeof(newGrenadeCount), 0);
				Sleep(10); //Ease off CPU
			}
		}
	}
	return 0;
}


