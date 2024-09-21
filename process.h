#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include "ntdll.h"
#include "math.h"

namespace Process {
    DWORD pid = 0;
    DWORD cookie = 0;
    HANDLE handle = NULL;

    // init functions ==========================================
    void GetProcessID(const char* processName) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(snapshot, &entry)) {
            do {
                if (!strcmp(entry.szExeFile, processName)) {
                    pid = entry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
    }

    void OpenHandle() {
        handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    }

    void GetCookie() {
        DWORD temp;
        NtQueryInformationProcess(handle, ProcessCookie, &temp, sizeof(DWORD), NULL);
        cookie = temp;
    }

    void Init(const char* processName) {
        GetProcessID(processName);
        OpenHandle();
        GetCookie();
    }
    // =========================================================

    template <class T>
	T Read(uintptr_t address, size_t bytesToRead = sizeof(T))
	{
		T buf;
		ReadProcessMemory(handle, reinterpret_cast<LPCVOID>(address), &buf, bytesToRead, nullptr);
		return buf;
	}

    uintptr_t DecodePointer(uintptr_t pointer) {
        return __ROR8__(pointer, 64 - (cookie & 0x3F)) ^ cookie;
    }
}