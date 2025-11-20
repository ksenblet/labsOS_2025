#include <iostream>
#include <string>
#include <windows.h>

#include "matrix.h"
#include "threads.h"

void printUsage() {
    std::cout << "Usage: median_filter.exe -threads N" << std::endl;  // Использование: median_filter.exe -threads N
    std::cout << "  -threads N : maximum number of threads (required parameter)" << std::endl;  // максимальное количество потоков (обязательный параметр)
    std::cout << std::endl;
    std::cout << "Example: median_filter.exe -threads 4" << std::endl;  // Пример: median_filter.exe -threads 4
}

int main(int argc, char* argv[]) {
    // Парсинг аргументов командной строки - ТОЛЬКО threads
    int maxThreads = 0;
    
    std::cout << "=== Matrix Median Filter ===" << std::endl;  // Медианный фильтр для матрицы
    std::cout << "Laboratory work on thread management" << std::endl;  // Лабораторная работа по управлению потоками
    std::cout << "=====================================" << std::endl;
    
    // Проверяем аргументы командной строки
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-threads" && i + 1 < argc) {
            try {
                maxThreads = std::stoi(argv[++i]);
                if (maxThreads <= 0) {
                    std::cerr << "Error: number of threads must be positive" << std::endl;  // Ошибка: количество потоков должно быть положительным числом
                    return 1;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: invalid thread count format" << std::endl;  // Ошибка: неверный формат числа потоков
                return 1;
            }
        }
    }
    
    // Проверка обязательного параметра
    if (maxThreads <= 0) {
        std::cerr << "Error: must specify number of threads" << std::endl;  // Ошибка: необходимо указать количество потоков
        printUsage();
        return 1;
    }
    
    std::cout << "Set maximum number of threads: " << maxThreads << std::endl;  // Задано максимальное количество потоков
    
    try {
        // Ввод матрицы
        Matrix input;
        input.fillFromConsole();
        
        // Ввод параметров фильтра
        int windowSize, iterations;
        
        std::cout << "Enter window size for median filter (odd number): ";  // Введите размер окна для медианного фильтра (нечетное число)
        std::cin >> windowSize;
        
        if (windowSize % 2 == 0 || windowSize <= 0) {
            std::cerr << "Error: window size must be positive odd number!" << std::endl;  // Ошибка: размер окна должен быть положительным нечетным числом!
            return 1;
        }
        
        std::cout << "Enter number of filter applications (K): ";  // Введите количество применений фильтра (K)
        std::cin >> iterations;
        
        if (iterations <= 0) {
            std::cerr << "Error: number of iterations must be positive!" << std::endl;  // Ошибка: количество итераций должно быть положительным!
            return 1;
        }
        
        // Вывод исходной матрицы
        std::cout << "\n--- Original Matrix ---" << std::endl;  // Исходная матрица
        input.print();
        
        // Создание выходной матрицы
        Matrix output(input.getRows(), input.getCols());
        
        // Создание менеджера потоков и применение фильтра
        ThreadControl threadManager(maxThreads);
        
        std::cout << "\n--- Processing ---" << std::endl;  // Обработка
        std::cout << "Processing parameters:" << std::endl;  // Параметры обработки
        std::cout << " - Maximum threads: " << maxThreads << std::endl;  // Максимальное количество потоков
        std::cout << " - Window size: " << windowSize << std::endl;  // Размер окна
        std::cout << " - Iterations: " << iterations << std::endl;  // Количество итераций
        std::cout << " - Matrix size: " << input.getRows() << " x " << input.getCols() << std::endl;  // Размер матрицы
        
        bool success = threadManager.applyMedianFilter(input, output, windowSize, iterations);
        
        if (!success) {
            std::cerr << "Error applying filter!" << std::endl;  // Ошибка применения фильтра!
            return 1;
        }
        
        std::cout << "\n--- Result ---" << std::endl;  // Результат
        std::cout << "Processing completed successfully!" << std::endl;  // Обработка завершена успешно!
        
        // Вывод результирующей матрицы
        std::cout << "\nResult matrix:" << std::endl;  // Результирующая матрица
        output.print();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;  // Ошибка
        return 1;
    }
    return 0;
}