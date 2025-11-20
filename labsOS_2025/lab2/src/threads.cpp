#include <iostream>
#include <vector>

#include "threads.h"

Semaphore::Semaphore(int initialCount) {
    // NULL - атрибуты безопасности по умолчанию
    // initialCount - начальное значение счетчика разрешений
    // initialCount - макс значение
    // NULL - семафор без имени
    semaphore = CreateSemaphore(NULL, initialCount, initialCount, NULL);
}

Semaphore::~Semaphore() {
    CloseHandle(semaphore);
}

// блокирует поток если счетчик = 0
void Semaphore::wait() {
    WaitForSingleObject(semaphore, INFINITE); // INFINITE - бесконечное ожидание
}

// счетчик разрешений + 1
void Semaphore::signal() {
    // 1 - на сколько +
    // NULL - не важно предыдущее значение счетчика
    ReleaseSemaphore(semaphore, 1, NULL);
}

int ThreadCreate(ThreadHandle* handle, void* (*threadFunction)(void*), void* threadArgument) {
    // nullptr - атрибуты безопасности по умолчанию
    // 0 - размер стека по умолчанию
    // (LPTHREAD_START_ROUTINE)threadFunction - функция, которую будет выполнять поток
    // threadArgument - аргумент для функции потока
    // 0 - флаги создания (0 = выполнение сразу после создания)
    // nullptr - не сохраняем идентификатор потока
    *handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)threadFunction, 
                          threadArgument, 0, nullptr);
    if (*handle == NULL) 
    {
        std::cerr << "ERROR: CreateThread failed:(" << std::endl;
        return -1;
    }
    return 0;
}

void ThreadJoin(ThreadHandle handle) {
    // WaitForSingleObject блокирует текущий поток до завершения указанного
    WaitForSingleObject(handle, INFINITE);
    CloseHandle(handle); // handle - дескриптор потока, которого ждем
}

void* ProcessMatrixRegion(void* argument) {
    ThreadData* data = static_cast<ThreadData*>(argument);

    // для проверки количества потоков через диспетчер задач
    std::cout << "Check Task Manager NOW to see threads! You have 30 seconds!" << std::endl;
    Sleep(30000);

    // обрабатываем указанное количество итераций
    for (int i = 0; i < data->iterations; ++i) {
        // Обрабатываем строки пока они не закончатся
        bool workRemaining = true;
        while (workRemaining) {
            int currentRow = -1;  // номер текущей обрабатываемой стр
            // захватываем семафор
            data->matrixSemaphore->wait();
            // остались ли строки для обработки
            if (*data->nextRow < data->totalRows) {
                // след строка и счетчик + 1
                currentRow = (*data->nextRow);
                (*data->nextRow)++;
            } else {
                workRemaining = false;
            }
            // свобода семафору
            data->matrixSemaphore->signal();

            if (currentRow != -1) {
                for (int col = 0; col < data->inputMatrix->getCols(); ++col) {
                    int filteredValue = data->inputMatrix->applyMedianFilter(currentRow, col, data->windowSize, *(data->inputMatrix));
                    data->outputMatrix->setValue(currentRow, col, filteredValue);
                }
            }
        }
        // сброс счетчика строк для след итерации
        if (i < data->iterations - 1) {
            data->matrixSemaphore->wait();
            *data->nextRow = 0;  // сброс только если будут еще итерации
            data->matrixSemaphore->signal();
        }
    }
    return nullptr;
}

ThreadControl::ThreadControl(int maxThreads) 
    : maxThreads(maxThreads), nextRow(0), totalRows(0) {
    // семафор с 1 разрешением
    // только 1 поток может одновременно изменять общие данные!!
    matrixSemaphore = new Semaphore(1);
}

ThreadControl::~ThreadControl() {
    for (auto data : threadData) {
        delete data;
    }
    delete matrixSemaphore;
}

bool ThreadControl::applyMedianFilter(const Matrix& input, Matrix& output, int windowSize, int iterations) {
    totalRows = input.getRows();
    
    for (auto data : threadData) {
        delete data;
    }
    threadData.clear();
    
    for (auto handle : threads) {
        if (handle) {
            CloseHandle(handle);
        }
    }
    threads.clear();
    
    // сброс счетчика стр для новой обработки
    nextRow = 0;

    for (int i = 0; i < maxThreads; ++i) {
        ThreadData* data = new ThreadData;
        data->inputMatrix = &input;
        data->outputMatrix = &output;
        data->windowSize = windowSize;
        data->threadId = i + 1;  // нумерация потоков с 1
        data->iterations = iterations;
        data->matrixSemaphore = matrixSemaphore;
        data->nextRow = &nextRow;
        data->totalRows = totalRows;
        
        threadData.push_back(data); // сохраняем указатель для последующего удаления

        ThreadHandle threadHandle;
        int createResult = ThreadCreate(&threadHandle, ProcessMatrixRegion, data);
        if (createResult != 0) {
            std::cerr << "Error: failed to create thread " << i << std::endl;
            return false; 
        }
        
        // дескриптор созданного потока
        threads.push_back(threadHandle);
    }
    
    for (auto handle : threads) {
        ThreadJoin(handle);
    }
    
    return true;
}