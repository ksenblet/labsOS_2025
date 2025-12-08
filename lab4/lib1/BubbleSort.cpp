#include "../include/Sorter.hpp"


class BubbleSort : public Sorter {
public:
    int* Sort(int* array, int size) override {
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (array[j] > array[j + 1]) {
                    int temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
        return array;
    }
};

extern "C" __declspec(dllexport) Sorter* CreateArraySorter() {
    return new BubbleSort();
}

extern "C" __declspec(dllexport) void DeleteArraySorter(Sorter* p) {
    delete p;
}