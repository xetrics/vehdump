#include <Windows.h>
#include <iostream>
#include "process.h"

typedef struct _LDRP_VECTOR_HANDLER_LIST {
    PSRWLOCK LdrpVehLock;
    LIST_ENTRY LdrpVehList;
    PSRWLOCK LdrpVchLock;
    LIST_ENTRY LdrpVchList;
} LDRP_VECTOR_HANDLER_LIST, * PLDRP_VECTOR_HANDLER_LIST;

typedef struct _VECTOR_HANDLER_ENTRY {
    LIST_ENTRY ListEntry;
    PLONG64 pRefCount; // ProcessHeap allocated, initialized with 1
    DWORD unk_0; // always 0
    DWORD pad_0;
    PVOID EncodedHandler;
} VECTOR_HANDLER_ENTRY, * PVECTOR_HANDLER_ENTRY;

int main() {
    Process::Init("VEHDumpee.exe");

    printf("PID: %d\n", Process::pid);
    printf("Cookie: 0x%0X\n", Process::cookie);
    printf("Handle: 0x%0X\n", Process::handle);

    const auto p_RltAddVectoredExceptionHandler = (uintptr_t)GetProcAddress(GetModuleHandleA("kernel32.dll"), "AddVectoredExceptionHandler");

    printf("AddVectoredExceptionHandler: 0x%p\n", p_RltAddVectoredExceptionHandler);

    const auto p_LdrpVectorHandlerListRelative = Process::Read<DWORD>(p_RltAddVectoredExceptionHandler + 0x126);
    
    printf("pLdrpVectorHandlerListRelative: 0x%p\n", p_LdrpVectorHandlerListRelative);

    const auto p_LdrpVectorHandlerList = (p_RltAddVectoredExceptionHandler + 0x12A) + p_LdrpVectorHandlerListRelative;

    printf("pLdrpVectorHandlerList: 0x%p\n", p_LdrpVectorHandlerList);

    const auto LdrpVectorHandlerList = Process::Read<LDRP_VECTOR_HANDLER_LIST>(p_LdrpVectorHandlerList);

    printf("LdrpVectorHandlerList.LdrpVehLock: 0x%p\n", LdrpVectorHandlerList.LdrpVehLock);
    printf("LdrpVectorHandlerList.LdrpVehList: 0x%p\n", LdrpVectorHandlerList.LdrpVehList);
    printf("LdrpVectorHandlerList.LdrpVchLock: 0x%p\n", LdrpVectorHandlerList.LdrpVchLock);
    printf("LdrpVectorHandlerList.LdrpVchList: 0x%p\n", LdrpVectorHandlerList.LdrpVchList);

    printf("Dumping VEH Handlers...\n");
    
    const auto HeadEntry = Process::Read<LIST_ENTRY>(p_LdrpVectorHandlerList + offsetof(LDRP_VECTOR_HANDLER_LIST, LdrpVehList));
    printf("HeadEntry.Flink: 0x%p\n", HeadEntry.Flink);

    if((uintptr_t)HeadEntry.Flink == (p_LdrpVectorHandlerList + offsetof(LDRP_VECTOR_HANDLER_LIST, LdrpVehList))) {
        printf("No VEH Handlers found.\n");
        return 0;
    }

    VECTOR_HANDLER_ENTRY current = Process::Read<VECTOR_HANDLER_ENTRY>((uintptr_t)HeadEntry.Flink);
    uintptr_t lastFlink = 0;
    while(1) {
        if((uintptr_t)current.ListEntry.Flink == lastFlink)
            break;

        uintptr_t decodedHandler = Process::DecodePointer((uintptr_t)current.EncodedHandler);
        printf("VEH Handler @ 0x%p\n", decodedHandler);

        current = Process::Read<VECTOR_HANDLER_ENTRY>((uintptr_t)current.ListEntry.Flink);
        lastFlink = (uintptr_t)current.ListEntry.Flink;
    }

    return 0;
}