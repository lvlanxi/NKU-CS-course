#include <iostream>
#include <immintrin.h> // AVX头文件
#include <Windows.h>
#include<ctime>
using namespace std;
const int N = 1024;
//还好输出都是英文
void SSE_elimitation(float** matrix) //SSE并行的高斯消去法
}

void AVX_elimitation(float** matrix) //AVX并行的高斯消去法
{
    __m256 t1, t2, t3, t4;//数据类型float
    for (int k = 0; k < N; k++)
    {
        float tmp[8] = { matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k], matrix[k][k] };
        t1 = _mm256_loadu_ps(tmp);  //加载到寄存器中，不要求地址与32字节对齐
        for (int j = N - 8; j >= k; j -= 8) //从后向前每次取八位
        {
            t2 = _mm256_loadu_ps(matrix[k] + j);
            t3 = _mm256_div_ps(t2, t1);    //除法，向量化处理使每次能除八位
            _mm256_storeu_ps(matrix[k] + j, t3);   //存储到内存中，不要求地址对齐
        }
        if (k % 8 != (N % 8)) //处理不能被8整除的元素
        {
            for (int j = k + 1; j % 8 != (N % 8); j++)
            {
                matrix[k][j] = matrix[k][j] / tmp[0];
            }
        }
        matrix[k][k] = 1.0;

        for (int i = k + 1; i < N; i++)
        {
            float tmp[8] = { matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k], matrix[i][k] };
            t1 = _mm256_loadu_ps(tmp);
            for (int j = N - 8; j > k; j -= 8)
            {
                t2 = _mm256_loadu_ps(matrix[i] + j);
                t3 = _mm256_loadu_ps(matrix[k] + j);
                t4 = _mm256_sub_ps(t2, _mm256_mul_ps(t1, t3)); //减法
                _mm256_storeu_ps(matrix[i] + j, t4);
            }
            for (int j = k + 1; j % 8 != (N % 8); j++)
            {
                matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}

void normal_elimitation(float** matrix) //串行高斯消去法
{
    for (int k = 0; k < N; k++)
    {
        float tmp = matrix[k][k];
        for (int j = k + 1; j < N; j++)
        {
            matrix[k][j] = matrix[k][j] / tmp;
        }
        matrix[k][k] = 1.0;
        for (int i = k + 1; i < N; i++)
        {
            float tmp2 = matrix[i][k];
            for (int j = k + 1; j < N; j++)
            {
                matrix[i][j] = matrix[i][j] - tmp2 * matrix[k][j];
            }
            matrix[i][k] = 0;
        }
    }
}

void print(float** matrix) //输出
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    long long head, tail, freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    double Mytime=0.0;

    srand((unsigned)time(NULL));
    float** matrix = new float* [N];
    float** matrix2 = new float* [N];
    float** matrix3 = new float* [N];

    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
        matrix2[i] = new float[N];
        matrix3[i] = new float[N];

    }//绝不能共地址

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            matrix[i][j] = rand() % 100;
            matrix2[i][j] = matrix[i][j];
            matrix3[i][j] = matrix[i][j];

        }
    }

    cout <<"normal" << endl;
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    normal_elimitation(matrix);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    
    Mytime=((tail - head) * 1000.0 / freq);

    cout << Mytime << "ms" << endl;


    cout << endl << "AVX" << endl;
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    AVX_elimitation(matrix2);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    
    Mytime=((tail - head) * 1000.0 / freq);

    cout << Mytime << "ms" << endl;



    cout << endl << "SSE" << endl;
    QueryPerformanceCounter((LARGE_INTEGER*)&head);
    SSE_elimitation(matrix3);
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);
    
    Mytime=((tail - head) * 1000.0 / freq);

    cout << Mytime << "ms" << endl;

   
    //print(matrix3);


    system("pause");
    return 0;
}
