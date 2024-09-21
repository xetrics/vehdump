# VEHDump

A simple tool to dump registered VEH handlers of an external x64 process. I spent no time cleaning this up once I got it working, and there is definetely room for improvement. However, it works as a POC for anyone who wants to adapt it to their own project.

## Explaination

VEH handlers are stored in a doubly linked list in a global struct called `LdrpVectorHandlerList (LDRP_VECTOR_HANDLER_LIST)` located in the `.mrdata` section of `ntdll.dll`.

There are no API functions to access this struct, and it is meant for internal win32 use only. To get a pointer to `LdrpVectorHandlerList`, I reverse engineered x-ref `AddVectoredExceptionHandler`. At `AddVectoredExceptionHandler+0x123` at the time of making this tool (subject to change with windows updates), it accesses this struct like so:

```x86asm
    lea rdi, qword ptr [rip + 0x1168be]
```

`0x1168be` is a rip relative offset to the start of the `LdrpVectorHandlerList` struct. Therefore, we read said 8 byte pointer from `+0x126` and add that to the start of the next function, in this case, `AddVectoredExceptionHandler+0x12A`.

From there on, we just need to traverse the linked list and dump the address of each VEH handler entry.

**HOWEVER**, each VEH handler entry is an encoded pointer.

Pointers are encoded by xor'ing it with the process cookie, then rotating it to the right by the lower 6 bits of the process cookie.

You can see how this is done in `ntdll.(RtlDecodePointer|RtlEncodePointer)`

You can use the aforementioned module functions, but the logic is pretty simple so I just implemented it myself in `math.h`.

This is by no means a "release" or a tool that you should even use, but it's a good reference for something that is very sparsely documented.
