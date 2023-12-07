#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <chrono>

using namespace std;
using namespace chrono;

int** result;
int ** matrix1;
int ** matrix2;
int cols1,cols2,rows1,rows2;
struct params{
    int i;
    int j;
    int n;

};
void* dotproduct(void* arg){
    auto *p =(params*)arg;
    int i = p->i;
    int j = p->j;
    int n = p->n;
    result[i][j] = 0;
    for (int k=0; k<n; k++){
        result[i][j] += (matrix1[i][k] * matrix2[k][j]);
    }
    pthread_exit(0);

}

int** createMatrix(int rows, int cols){
    int** arr = new int*[rows];
    for (int i=0; i<rows;i++){
        arr[i]= new int[cols];
    }
    return arr;
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* multiply(void* arg) {
    auto p = (params*)arg;
    int i =p->i;

    for (int j = 0; j < cols2; ++j) {
        int partial =0;
        for (int k = 0; k < cols1; ++k) {
            partial += matrix1[i][k] * matrix2[k][j];
        }
        pthread_mutex_lock(&mutex);
        result[i][j] = partial;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(nullptr);
}




int main() {

    ifstream inputFile1("/home/eriksie/Documents/Lab4_TestCases/Lab4_TestCases/input3-matrix.txt");
    //ifstream inputFile2("/home/eriksie/Documents/Lab4_TestCases/Lab4_TestCases/input2-matrix.txt");

    if (!inputFile1.is_open() ) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    inputFile1 >> rows1 >> cols1;
    matrix1 = createMatrix(rows1,cols1);


    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols1; ++j) {
            inputFile1 >> matrix1[i][j];
        }
    }



    inputFile1 >> rows2 >> cols2;
    matrix2 = createMatrix(rows2,cols2);


    for (int i = 0; i < rows2; ++i) {
        for (int j = 0; j < cols2; ++j) {
            inputFile1 >> matrix2[i][j];

        }
    }

    inputFile1.close();

    // Check if matrices can be multiplied
    if (cols1 != rows2) {
        cerr << "Error: Matrices cannot be multiplied." << endl;
        return 1;
    }

    pthread_t threads[rows1][cols2];

    result = createMatrix(rows1,cols2);
    auto start = chrono::high_resolution_clock::now();
    for (int i=0; i<rows1;i++){
        for (int j=0; j<cols2;j++){
            auto *p = new params();

            p->i =i;
            p->j =j;
            p->n =cols1;

            pthread_create(&threads[i][j],NULL,dotproduct, (void*)p);
        }
    }

    for (int i=0; i<rows1;i++) {
        for (int j = 0; j < cols2; j++) {
            pthread_join(threads[i][j],NULL);

        }
    }
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Result Matrix using Element-Wise multiplication:" << endl;
    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols2; ++j) {
            cout << result[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Time taken by the code: " << duration.count() << " microseconds" << endl;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize pthread variables
    pthread_t threads2[rows1];
    start = chrono::high_resolution_clock::now();
    // Create threads
    for (int i = 0; i < rows1; ++i) {
        auto *p = new params();
        p->i =i;
        pthread_create(&threads2[i], nullptr, multiply, (void*)p);
    }

    // Join threads
    for (int i = 0; i < rows1; ++i) {
        pthread_join(threads2[i], nullptr);
    }
    stop = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout << "Result Matrix:" << endl;
    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols2; ++j) {
            cout << result[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Time taken by the code: " << duration.count() << " microseconds" << endl;

}