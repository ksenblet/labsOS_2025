#include "../include/Sorter.hpp"


class QuickSort : public Sorter {
private:
    void quickSort(int* array, int low, int high) {
        if (low < high) {
            int pivot = array[high];
            int i = low - 1;
            
            for (int j = low; j < high; j++) {
                if (array[j] <= pivot) {
                    i++;
                    int temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
            }
            
            int temp = array[i + 1];
            array[i + 1] = array[high];
            array[high] = temp;
            
            int pi = i + 1;
            quickSort(array, low, pi - 1);
            quickSort(array, pi + 1, high);
        }
    }
    
public:
    int* Sort(int* array, int size) override {
        if (size > 1) {
            quickSort(array, 0, size - 1);
        }
        return array;
    }
};

extern "C" __declspec(dllexport) Sorter* CreateArraySorter() {
    return new QuickSort();
}

extern "C" __declspec(dllexport) void DeleteArraySorter(Sorter* p) {
    delete p;
}