#ifndef THREADS_H
#define THREADS_H

#include <vector>
#include <windows.h>

#include "matrix.h"

typedef HANDLE ThreadHandle;

int threadCreate(ThreadHandle* handle, void* (*threadFunction)(void*), void* threadArgument);
void threadJoin(ThreadHandle handle);
void* processMatrixRegion(void* argument);  // static чтобы можно было использовать как callback
    

// класс семафора для синхронизации доступа к общим ресурсам
class Semaphore {
public:
    Semaphore(int initialCount);
    ~Semaphore();
    void wait();
    void signal();

private:
    HANDLE semaphore;  // дескриптор системного семафора Windows
};

// структура для передачи данных в рабочий поток
struct ThreadData {
    const Matrix* inputMatrix;    // указатель на входную матрицу (только для чтения)
    Matrix* outputMatrix;         // указатель на выходную матрицу (для записи)
    int windowSize;               // размер окна для медианного фильтра
    int threadId;                 // уникальный идентификатор потока (для отладки)
    int iterations;               // количество применений фильтра (K)
    
    // общие ресурсы для синхронизации между потоками
    Semaphore* matrixSemaphore;   // семафор для защиты доступа к общим данным
    int* nextRow;                 // указатель на общий счетчик текущей строки
    int totalRows;                // общее количество строк в матрице
};

// класс для управления созданием и синхронизацией потоков
class ThreadControl {
public:
    ThreadControl(int maxThreads);
    ~ThreadControl();

    // Основной метод применения медианного фильтра в многопоточном режиме
    bool applyMedianFilter(const Matrix& input, Matrix& output, int windowSize, int iterations);

private:
    std::vector<ThreadHandle> threads;      // Вектор дескрипторов созданных потоков
    std::vector<ThreadData*> threadData;    // Вектор структур данных для потоков
    int maxThreads;                         // Максимальное количество потоков
    
    // Общие ресурсы, защищаемые семафором
    Semaphore* matrixSemaphore;             // Семафор для синхронизации доступа к матрице
    int nextRow;                            // Текущая строка для обработки (общий ресурс)
    int totalRows;                          // Общее количество строк в матрице
};

#endif