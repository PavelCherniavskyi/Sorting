#include <iostream>
#include <vector>
#include <set>
#include <iterator>

class Object
{
    int a;
public:
    Object(int A) {
        std::cout << "constructor" << std::endl;
        a = A;
    }
    Object(const Object& other) {
        std::cout << "copy Object&" << std::endl;
        a = other.a;
    }
    Object(Object&& other) {
        std::cout << "copy Object&&" << std::endl;
        a = other.a;
        other.a = 0;
    }
    Object& operator=(const Object& other) {
        std::cout << "assignment Object&" << std::endl;
        a = other.a;
        return *this;
    }
    Object& operator=(Object&& other) {
        std::cout << "assignment Object&&" << std::endl;
        a = other.a;
        other.a = 0;
        return *this;
    }

    bool operator<(const Object& rhs) const {
        return a < rhs.a;
    }

    [[nodiscard]] int get() const {
        return a;
    }
};

template<class T>
void selectSort(std::vector<T>& arrOUT)
{
    for(size_t i = 0; i < arrOUT.size(); ++i) {
        size_t smallestIndx = i;
        for(size_t j = i + 1; j < arrOUT.size(); ++j) {
            if(arrOUT[j] < arrOUT[smallestIndx]) {
                smallestIndx = j;
            }
        }

        if(smallestIndx != i) {
            T tmp = std::move(arrOUT[i]);
            arrOUT[i] = std::move(arrOUT[smallestIndx]);
            arrOUT[smallestIndx] = std::move(tmp);
        }
    }
}

template<class T>
void insertSort(std::vector<T>& arrOUT)
{
    for(int i = 1; i < arrOUT.size(); ++i) {
        for(int j = i; j > 0; --j) {
            if(arrOUT[j] < arrOUT[j-1]) {
                T tmp = std::move(arrOUT[j-1]);
                arrOUT[j-1] = std::move(arrOUT[j]);
                arrOUT[j] = std::move(tmp);
            }
        }
    }
}

template<class T>
void bubbleSort(std::vector<T>& arrOUT)
{
    size_t lastSortedIndx = arrOUT.size();
    for(size_t i = 0; i < arrOUT.size(); ++i) {
        size_t currSortedIndx = lastSortedIndx;
        for(size_t j = 1; j < lastSortedIndx; ++j) { // lastSortedIndx -> avoid to check elements that already sorted
            if(arrOUT[j] < arrOUT[j-1]) {
                T tmp = std::move(arrOUT[j-1]);
                arrOUT[j-1] = std::move(arrOUT[j]);
                arrOUT[j] = std::move(tmp);
                currSortedIndx = j;
            }
        }

        if(lastSortedIndx == currSortedIndx) { //main advantage of bubble sort -> if arr is already sorted we have O(n)
            break;
        } else {
            lastSortedIndx = currSortedIndx;
        }
    }
}

int main()
{
    std::vector<Object> arr {4, 2, 5, 3, 1, 6, 9};
    std::vector<Object> arrSorted {1, 2, 3, 4, 5, 6, 9};
    std::vector<Object> arrBackSorted {4, 2, 1, 3, 5, 6, 9};
    std::vector<Object> arrFrontSorted {1, 2, 3, 9, 6, 4, 5};

    bubbleSort(arrFrontSorted);
    //insertSort(arr);
    //insertSort(arr);
    return 0;
}