#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <chrono>


using namespace std;

int n;
int* arr;

struct params {
    int low;
    int high;
};


int* createArray(){
    int* array = new int[n];
    return array;
}

// Merge two sorted subarrays into a single sorted array
void merge(int low, int mid, int high) {
    int n1 = mid - low + 1;
    int n2 = high - mid;

    vector<int> left(n1);
    vector<int> right(n2);

    for (int i = 0; i < n1; i++)
        left[i] = arr[low + i];

    for (int j = 0; j < n2; j++)
        right[j] = arr[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = low;

    while (i < n1 && j < n2) {
        if (left[i] <= right[j]) {
            arr[k] = left[i];
            i++;
        } else {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = left[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = right[j];
        j++;
        k++;
    }
}


void* mergeSort(void* arg) {
    auto p = (params*)arg;

    if (p->low < p->high) {
        int mid = p->low + (p->high - p->low) / 2;

        // Create parameters for the left and right subarrays
        params leftParams{p->low, mid};
        params rightParams{mid + 1, p->high};

        // Create threads for the left and right subarrays
        pthread_t leftThread, rightThread;
        pthread_create(&leftThread, NULL, mergeSort, &leftParams);
        pthread_create(&rightThread, NULL, mergeSort, &rightParams);

        // Wait for both threads to finish
        pthread_join(leftThread, NULL);
        pthread_join(rightThread, NULL);

        // Merge the sorted left and right subarrays
        merge(p->low, mid, p->high);
    }

    pthread_exit(NULL);
}


void parallelMergeSort(int* arr) {
   // int arrSize = arr.size();

    params p{ 0, n-1};

    // Create a thread for the initial call to mergeSort
    pthread_t initialThread;
    pthread_create(&initialThread, NULL, mergeSort, &p);

    // Wait for the initial thread to finish
    pthread_join(initialThread, NULL);
}

int main() {
    ifstream inputFile1("/home/eriksie/Documents/Lab4_TestCases/Lab4_TestCases/input-sort.txt");

    if (!inputFile1.is_open() ) {
        cerr << "Error opening file." << endl;
        return 1;
    }
    inputFile1 >> n;
    arr = createArray();
    for (int i=0;i<n;i++){
        inputFile1 >> arr[i];
    }
    parallelMergeSort(arr);
    cout << "SORTED ARRAY: ";
    for (int i=0; i<n;i++){
        cout << arr[i] << " ";
    }
    return 0;
}
