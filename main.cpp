#include <iostream>
#include <vector>
#include <set>
#include <iterator>
#include <algorithm>
#include <random>
#include <chrono>

class Object
{
    int a;
public:
    Object() {
        std::cout << "constructor" << std::endl;
        a = 0;
    }
    explicit Object(int A) {
        std::cout << "constructor int" << std::endl;
        a = A;
    }
    Object(const Object& other) {
        std::cout << "copy Object&" << std::endl;
        a = other.a;
    }
    Object(Object&& other) noexcept {
        std::cout << "copy Object&&" << std::endl;
        a = other.a;
        other.a = 0;
    }
    Object& operator=(const Object& other) {
        std::cout << "assignment Object&" << std::endl;
        a = other.a;
        return *this;
    }
    Object& operator=(Object&& other) noexcept {
        std::cout << "assignment Object&&" << std::endl;
        a = other.a;
        other.a = 0;
        return *this;
    }

    bool operator<(const Object& rhs) const {
        std::cout << "operator<" << std::endl;
        return a < rhs.a;
    }

    bool operator>(const Object& rhs) const {
        std::cout << "operator>" << std::endl;
        return a > rhs.a;
    }

    [[nodiscard]] int get() const {
        return a;
    }
};

//is less effective than insertion sort as if arr is partly sorted -> selectSort will iterarate over unsorted part anyway
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
            std::swap(arrOUT[i], arrOUT[smallestIndx]);
        }
    }
}

template<class T>
void insertSort(std::vector<T>& arrOUT)
{
    for(size_t i = 1; i < arrOUT.size(); ++i) {
        for(int j = i; j > 0; --j) {
            if(arrOUT[j] < arrOUT[j-1]) {
                //the option to use binary search std::upper_bound won't work out
                // because one insert and one erase will shift (copy) all elements twice
                // so swapping with O(n) here will be better.
                std::swap(arrOUT[j-1],arrOUT[j]);
            } else {
                break; //avoiding to iterate over sorted part
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
                std::swap(arrOUT[j-1],arrOUT[j]);
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

//possible improvements:
// 1. algorithm to search best pivot
// 2. rework to not create two vectors every time and consume memory.
template <class T>
void quickSort(std::vector<T>& arrOUT)
{
    if(arrOUT.size() < 2) {
        return;
    }

    int pivot = arrOUT.size() / 2;

    std::vector<T> left;
    left.reserve(arrOUT.size() - 1);
    for(size_t i = 0; i < arrOUT.size(); ++i) {
        if(arrOUT[i] < arrOUT[pivot]) {
            left.emplace_back(std::move(arrOUT[i]));
        }
    }

    std::vector<T> right;
    right.reserve(arrOUT.size() - 1);
    for(size_t i = 0; i < arrOUT.size(); ++i) {
        if(arrOUT[i] > arrOUT[pivot]) {
            right.emplace_back(std::move(arrOUT[i]));
        }
    }

    // to not override value when pivot < value
    T middle = std::move(arrOUT[pivot]);

    quickSort(left);
    quickSort(right);

    size_t i = 0, j = 0;
    for(; i < left.size(); ++i) {
        arrOUT[i] = std::move(left[i]);
    }
    arrOUT[i++] = std::move(middle);
    for(; j < right.size(); ++j, ++i) {
        arrOUT[i] = std::move(right[j]);
    }

}

//possible improvements:
// 1. rework to not create two vectors every time and consume memory.
template <class T>
void mergeSort(std::vector<T>& arrOUT)
{
    if(arrOUT.size() < 2) {
        return;
    }

    const int arrIndexSize = arrOUT.size();
    const int pivot = arrIndexSize / 2;

    std::vector<T> left(std::make_move_iterator(arrOUT.begin()), std::make_move_iterator(arrOUT.begin() + pivot));
    std::vector<T> right(std::make_move_iterator(arrOUT.begin() + pivot), std::make_move_iterator(arrOUT.end()));

    mergeSort(left);
    mergeSort(right);

    int leftSize = left.size();
    int rightSize = right.size();
    int l = 0, r = 0, i = 0;
    while(r < rightSize && l < leftSize) {
        if(left[l] < right[r]) {
            arrOUT[i++] = std::move(left[l++]);
        } else {
            arrOUT[i++] = std::move(right[r++]);
        }
    }
    if (l == leftSize) {
        while (r != rightSize) { arrOUT[i++] = std::move(right[r++]); }
    } else if (r == rightSize) {
        while (l != rightSize) { arrOUT[i++] = std::move(left[l++]); }
    }
}

template <class T>
struct HeapNode
{
   explicit HeapNode(T& val) : value(val), right(nullptr), left(nullptr) {}
   HeapNode(T& val, HeapNode* l, HeapNode* r) : value(val), right(r), left(l) {}
   T& value;
   HeapNode* right;
   HeapNode* left;
};

template <class T>
HeapNode<T>* heapSortInsertNodes(HeapNode<T>* node, T& val, const int& level, bool& isInserted)
{
   if(level == 0 || !node) {
      return node;
   }

   HeapNode<T>* left = heapSortInsertNodes(node->left, val, level - 1, isInserted);
   HeapNode<T>* right = heapSortInsertNodes(node->right, val, level - 1, isInserted);

   if(!left && level == 1 && !isInserted) {
      node->left = new HeapNode(val);
      isInserted = true;
   } else if(!right && level == 1 && !isInserted) {
      node->right = new HeapNode(val);
      isInserted = true;
   }

   if(node->left && node->left->value > node->value) {
      std::swap(node->left->value, node->value);
   }
   if(node->right && node->right->value > node->value) {
      std::swap(node->right->value, node->value);
   }

   return node;
}

template <class T>
HeapNode<T>* heapSortDeleteAndSortNodes(HeapNode<T>* node, T& val, bool& isDeleted)
{
   if(!node) {
      return nullptr;
   }

   if(node->left) {
      if(node->left->value == val && !isDeleted) {
         delete node->left;
         node->left = nullptr;
         isDeleted = true;
      } else if (node->left->value > node->value) {
         std::swap(node->left->value, node->value);
      }

   }
   if (node->right) {
      if(node->right->value == val) {
         delete node->right;
         node->right = nullptr;
      } else if (node->right->value > node->value) {
         std::swap(node->right->value, node->value);
      }
   }

   node->left = heapSortDeleteAndSortNodes(node->left, val, isDeleted);
   node->right = heapSortDeleteAndSortNodes(node->right, val, isDeleted);

   return node;
}

template <class T>
HeapNode<T>* heapSortMakeTree(std::vector<T>& arrOUT)
{
   if(arrOUT.empty()) {
      return nullptr;
   }
   auto* head = new HeapNode(arrOUT[0]);
   int powCount = 1;
   for(size_t i = 1, j = 1; i < arrOUT.size(); ++i, ++powCount) {
      bool inserted = false;
      head = heapSortInsertNodes(head, arrOUT[i], j, inserted);
      if(powCount == std::pow(2,j)) {
         j++;
         powCount = 0;
      }
   }

   return head;
}

template <class T>
void heapSort(std::vector<T>& arrOUT)
{
   HeapNode<T>* tree = heapSortMakeTree(arrOUT);
   for(int i = arrOUT.size() - 1; i >= 0; --i) {
      bool isDeleted = false; // to avoid double delete of duplicates
      std::swap(arrOUT[0], arrOUT[i]);
      heapSortDeleteAndSortNodes(tree, arrOUT[i], isDeleted);
   }
}

// To heapify a subtree rooted with node i which is
// an index in arr[]. n is size of heap
template<class T>
void heapify(std::vector<T>& arr, int n, int i)
{
   int largest = i; // Initialize largest as root
   int l = 2 * i + 1; // left = 2*i + 1
   int r = 2 * i + 2; // right = 2*i + 2

   // If left child is larger than root
   if (l < n && arr[l] > arr[largest])
      largest = l;

   // If right child is larger than largest so far
   if (r < n && arr[r] > arr[largest])
      largest = r;

   // If largest is not root
   if (largest != i) {
      std::swap(arr[i], arr[largest]);

      // Recursively heapify the affected sub-tree
      heapify(arr, n, largest);
   }
}

// main function to do heap sort
template <class T>
void heapSortExport(std::vector<T>& arr, int n)
{
   // Build heap (rearrange array)
   for (int i = n / 2 - 1; i >= 0; i--)
      heapify(arr, n, i);

   // One by one extract an element from heap
   for (int i = n - 1; i > 0; i--) {
      // Move current root to end
      std::swap(arr[0], arr[i]);

      // call max heapify on the reduced heap
      heapify(arr, i, 0);
   }
}

int main()
{
    const int SIZE = 50;

    std::vector<int> arr1(SIZE);
    std::vector<int> arr2(SIZE);
    std::vector<int> arr3(SIZE);
    std::vector<int> arr4(SIZE);
    std::vector<int> arr5(SIZE);
    for(size_t i = 0; i < arr1.size(); i++){
        arr1[i] = i;
        arr2[i] = i;
        arr3[i] = i;
        arr4[i] = i;
        arr5[i] = i;
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(arr1.begin(), arr1.end(), std::default_random_engine(seed));
    std::shuffle(arr2.begin(), arr2.end(), std::default_random_engine(seed));
    std::shuffle(arr3.begin(), arr3.end(), std::default_random_engine(seed));
    std::shuffle(arr4.begin(), arr4.end(), std::default_random_engine(seed));
    std::shuffle(arr5.begin(), arr5.end(), std::default_random_engine(seed));

//    std::vector<Object> arrSorted {1, 2, 3, 4, 5, 6, 7, 8, 9};
//    std::vector<Object> arrSorted2 {1, 2, 3, 4, 5, 6, 7, 8, 9};
//
//    std::vector<Object> arrBackSorted {4, 2, 5, 1, 3, 6, 7, 8, 9};
//    std::vector<Object> arrBackSorted2 {4, 2, 5, 1, 3, 6, 7, 8, 9};
//
//    std::vector<Object> arrFrontSorted {1, 2, 3, 4, 7, 9, 6, 8, 5};
//    std::vector<Object> arrFrontSorted2 {1, 2, 3, 4, 7, 9, 6, 8, 5};

    auto begin = std::chrono::high_resolution_clock::now();
    quickSort(arr1);
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "##################################### quickSort: " << elapsed.count() << " mks\n";

    begin = std::chrono::high_resolution_clock::now();
    mergeSort(arr2);
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "##################################### mergeSort: " << elapsed.count() << " mks\n";

    begin = std::chrono::high_resolution_clock::now();
    insertSort(arr3);
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "##################################### insrtSort: " << elapsed.count() << " mks\n";

    begin = std::chrono::high_resolution_clock::now();
    heapSortExport(arr4, arr4.size());
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "#####################################  heapSort: " << elapsed.count() << " mks\n";

    begin = std::chrono::high_resolution_clock::now();
    std::sort(arr5.begin(), arr5.end());
    end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "##################################### std::sort: " << elapsed.count() << " mks\n";

    return 0;
}