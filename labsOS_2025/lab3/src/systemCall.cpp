#include <iostream>

#include "systemCall.h"

#ifdef _WIN32
#include <tchar.h>
#endif

bool MMFCreate(mmfT* mmf, const char* name, size_t size) {
    if (!mmf) {
        std::cerr << "Error: Null pointer in MMFCreate" << std::endl;
        return false;
    }

#ifdef _WIN32
    //File Mapping 
    mmf->handle = CreateFileMappingA(
        INVALID_HANDLE_VALUE,  // Используем системный файл подкачки, а не реальный файл
        NULL,                  // Безопасность по умолчанию
        PAGE_READWRITE,        // Права: чтение и запись
        0,                     // Старшие 32 бита размера
        size,                  // Младшие 32 бита размера (общий размер)
        name                   // Имя для общего доступа
    );
    
    if (mmf->handle == NULL) {
        std::cerr << "Error: CreateFileMapping failed for " << name << std::endl;
        return false;
    }

    // проецируем файл в адресное пространство процесса
    mmf->data = MapViewOfFile(
        mmf->handle,           // Handle созданного mapping object
        FILE_MAP_ALL_ACCESS,   // Полный доступ к отображению
        0, 0,                  // Смещение в файле (0,0 - с начала)
        size                   // Размер отображаемой области
    );

#else
    mmf->handle = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (mmf->handle == -1) {
        std::cerr << "Error: shm_open failed for " << name << std::endl;
        return false;
    }
    
    if (ftruncate(mmf->handle, size) == -1) {
        std::cerr << "Error: ftruncate failed for " << name << std::endl;
        close(mmf->handle);
        return false;
    }
    
    mmf->data = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mmf->handle,
        0
    );
    
    if (mmf->data == MAP_FAILED) {
        std::cerr << "Error: mmap failed for " << name << std::endl;
        close(mmf->handle);
        return false;
    }
#endif

    if (!mmf->data) {
        std::cerr << "Error: Memory mapping failed for " << name << std::endl;
#ifdef _WIN32
        CloseHandle(mmf->handle);
#else
        close(mmf->handle);
#endif
        return false;
    }
    
    mmf->size = size;
    memset(mmf->data, 0, size);
    std::cout << "Created MMF: " << name << " size: " << size << std::endl;
    return true;
}

// bool PipeCreate(pipeT* pipe) {
//     if (!pipe) {
//         return false;
//     }
// #ifdef _WIN32
//     SECURITY_ATTRIBUTES sa;
//     sa.nLength = sizeof(SECURITY_ATTRIBUTES);
//     sa.bInheritHandle = TRUE;
//     sa.lpSecurityDescriptor = NULL;
//     if (!CreatePipe(&pipe->read_end, &pipe->write_end, &sa, 0)) {
//         return false;
//     }
//     return true;
// #else
//     int fds[2];
//     if (::pipe(fds) == -1) {
//         return false;
//     }
//     pipe_ptr->read_end = fds[0];
//     pipe_ptr->write_end = fds[1];
//     return true;
// #endif
// }

// void PipeClose(pipeT* pipe) {
//     if (!pipe) {
//         return;
//     }
// #ifdef _WIN32
//     if (pipe->read_end != INVALID_PIPE_HANDLE) {
//         CloseHandle(pipe->read_end);
//         pipe->read_end = INVALID_PIPE_HANDLE;
//     }
//     if (pipe->write_end != INVALID_PIPE_HANDLE) {
//         CloseHandle(pipe->write_end);
//         pipe->write_end = INVALID_PIPE_HANDLE;
//     }
// #else
//     if (pipe->read_end != INVALID_PIPE_HANDLE) {
//         close(pipe->read_end);
//         pipe->read_end = INVALID_PIPE_HANDLE;
//     }
//     if (pipe->write_end != INVALID_PIPE_HANDLE) {
//         close(pipe->write_end);
//         pipe->write_end = INVALID_PIPE_HANDLE;
//     }
// #endif
// }

// void PipeCloseWriteEnd(pipeT* pipe) {
//     if (!pipe) {
//         return;
//     }
// #ifdef _WIN32
//     if (pipe->write_end != INVALID_PIPE_HANDLE) {
//         CloseHandle(pipe->write_end);
//         pipe->write_end = INVALID_PIPE_HANDLE;
//     }
// #else
//     if (pipe->write_end != INVALID_PIPE_HANDLE) {
//         close(pipe->write_end);
//         pipe->write_end = INVALID_PIPE_HANDLE;
//     }
// #endif
// }

bool MMFOpen(mmfT* mmf, const char* name, size_t size) {
    if (!mmf) {
        std::cerr << "Error: Null pointer in MMFOpen" << std::endl;
        return false;
    }
#ifdef _WIN32
    mmf->handle = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        name
    );
    
    if (mmf->handle == NULL) {
        std::cerr << "Error: OpenFileMapping failed for " << name << std::endl;
        return false;
    }
    
    mmf->data = MapViewOfFile(
        mmf->handle,
        FILE_MAP_ALL_ACCESS,
        0, 0,
        size
    );
#else
    mmf->handle = shm_open(name, O_RDWR, 0666);
    if (mmf->handle == -1) {
        std::cerr << "Error: shm_open failed for " << name << std::endl;
        return false;
    }
    
    mmf->data = mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        mmf->handle,
        0
    );
    
    if (mmf->data == MAP_FAILED) {
        std::cerr << "Error: mmap failed for " << name << std::endl;
        close(mmf->handle);
        return false;
    }
#endif

    if (!mmf->data) {
        std::cerr << "Error: Memory mapping failed for " << name << std::endl;
#ifdef _WIN32
        CloseHandle(mmf->handle);
#else
        close(mmf->handle);
#endif
        return false;
    }
    
    mmf->size = size;
    std::cout << "Opened MMF: " << name << std::endl;
    return true;
}

void MMFClose(mmfT* mmf) {
    if (!mmf || !mmf->data) {
        return;
    }

#ifdef _WIN32
    UnmapViewOfFile(mmf->data);
    if (mmf->handle != INVALID_MMF_HANDLE) {
        CloseHandle(mmf->handle);
    }
#else
    munmap(mmf->data, mmf->size);
    if (mmf->handle != INVALID_MMF_HANDLE) {
        close(mmf->handle);
    }
#endif
    
    mmf->data = nullptr;
    mmf->handle = INVALID_MMF_HANDLE;
    mmf->size = 0;
    
    std::cout << "Closed MMF" << std::endl;
}

bool WriteToMMF(mmfT& mmf, const std::string& data) {
    if (!mmf.data || data.length() >= mmf.size) {
        return false;
    }
    // копируем стр в разделяемую память
    strcpy(static_cast<char*>(mmf.data), data.c_str());
    return true;
}

std::string ReadFromMMF(mmfT& mmf) {
    if (!mmf.data) {
        return "";
    }
    // создаем стр из данных в разделяемой памяти
    return std::string(static_cast<char*>(mmf.data));
}

// очистка разделяемой памяти (заполнение нулями)
void ClearMMF(mmfT& mmf) {
    if (mmf.data) {
        // заполняем
        memset(mmf.data, 0, mmf.size);
    }
}

process ProcessCreateWithMMF(const char* program, const char* inputName, const char* outputName) {
    process process_info;
    process_info.is_valid = false;

#ifdef _WIN32
    std::string cmd = program;
    cmd += " ";
    cmd += inputName;
    cmd += " ";
    cmd += outputName;
    
    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        process_info.process_info = pi;
        process_info.is_valid = true;
        CloseHandle(pi.hThread);
        std::cout << "Created process: " << program << std::endl;
    } else {
        std::cerr << "Error: CreateProcess failed for " << program << std::endl;
    }
#else
    pid_t pid = fork();
    if (pid == 0) {
        execl(program, program, inputName, outputName, NULL);
        std::cerr << "Error: execl failed for " << program << std::endl;
        exit(1);
    } else if (pid > 0) {
        process_info.pid = pid;
        process_info.is_valid = true;
        std::cout << "Created process: " << program << " PID: " << pid << std::endl;
    } else {
        std::cerr << "Error: fork failed for " << program << std::endl;
    }
#endif

    return process_info;
}

int ProcessTerminate(process* process_info) {
    if (!process_info || !process_info->is_valid) {
        return 0;
    }

#ifdef _WIN32
    TerminateProcess(process_info->process_info.hProcess, 0);
    WaitForSingleObject(process_info->process_info.hProcess, 1000);
    CloseHandle(process_info->process_info.hProcess);
#else
    kill(process_info->pid, SIGTERM);
    waitpid(process_info->pid, NULL, 0);
#endif

    process_info->is_valid = false;
    std::cout << "Process terminated" << std::endl;
    return 1;
}

// bool ReadStringFromPipe(PIPE_HANDLE pipe, std::string& output) {
//     char buffer[1024];
// #ifdef _WIN32
//     DWORD bytes_read;
//     if (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytes_read, NULL) && bytes_read > 0) {
//         buffer[bytes_read] = '\0';
//         output = buffer;
//         return true;
//     }
// #else
//     ssize_t bytes_read = read(pipe, buffer, sizeof(buffer) - 1);
//     if (bytes_read > 0) {
//         buffer[bytes_read] = '\0';
//         output = buffer;
//         return true;
//     }
// #endif
//     return false;
// }

// bool WriteStringToPipe(PIPE_HANDLE pipe, const std::string& input) {
//     if (input.empty()) {
//         return true;
//     } 
// #ifdef _WIN32
//     DWORD bytes_written;
//     return WriteFile(pipe, input.c_str(), input.length(), &bytes_written, NULL) && 
//            bytes_written == input.length();
// #else
//     ssize_t bytes_written = write(pipe, input.c_str(), input.length());
//     return bytes_written == (ssize_t)input.length();
// #endif
// }