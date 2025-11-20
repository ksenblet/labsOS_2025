#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include <string>

#ifdef _WIN32
    #include <windows.h>
    // #define PIPE_HANDLE HANDLE
    // #define INVALID_PIPE_HANDLE INVALID_HANDLE_VALUE
    #define MMF_HANDLE HANDLE
    #define INVALID_MMF_HANDLE NULL
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    // #define PIPE_HANDLE int
    // #define INVALID_PIPE_HANDLE -1
    #define MMF_HANDLE int
    #define INVALID_MMF_HANDLE -1
#endif

// typedef struct {
//     PIPE_HANDLE read_end;
//     PIPE_HANDLE write_end;
// } pipeT;

typedef struct {
    MMF_HANDLE handle;
    void* data;
    size_t size;
} mmfT;

typedef struct {
#ifdef _WIN32
    PROCESS_INFORMATION process_info;
#else
    pid_t pid;
#endif
    bool is_valid;
} process;

// bool PipeCreate(pipeT* pipe);
// void PipeClose(pipeT* pipe);
// void PipeCloseWriteEnd(pipeT* pipe);

// process ProcessCreate(const char* program, pipeT* stdin_pipe, pipeT* stdout_pipe);
bool MMFCreate(mmfT* mmf, const char* name, size_t size);
bool MMFOpen(mmfT* mmf, const char* name, size_t size);
void MMFClose(mmfT* mmf);
bool WriteToMMF(mmfT& mmf, const std::string& data);
std::string ReadFromMMF(mmfT& mmf);
void ClearMMF(mmfT& mmf);

process ProcessCreateWithMMF(const char* program, const char* inputName, const char* outputName);
int ProcessTerminate(process* process_info);

// bool ReadStringFromPipe(PIPE_HANDLE pipe, std::string& output);
// bool WriteStringToPipe(PIPE_HANDLE pipe, const std::string& input);

#endif