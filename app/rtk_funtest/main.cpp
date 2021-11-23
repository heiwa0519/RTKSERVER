/*
 * rtklib函数测试与分析
 *
 *
*/

#include <QCoreApplication>
#include<QDebug>
#include "rtklib.h"


/* multiply matrix -----------------------------------------------------------
* multiply matrix by matrix (C=alpha*A*B+beta*C)
* args   : char   *tr       I  transpose flags ("N":normal,"T":transpose)
*          int    n,k,m     I  size of (transposed) matrix A,B
*          double alpha     I  alpha
*          double *A,*B     I  (transposed) matrix A (n x m), B (m x k)
*          double beta      I  beta
*          double *C        IO matrix C (n x k)
* return : none
*-----------------------------------------------------------------------------*/
//n代表第一个矩阵的行，k代表第二个矩阵的列，m代表第一个矩阵的列和第二个矩阵的行。作者之所以把n和k放在m前面，看起来顺序反逻辑，其实是矩阵相乘一共要循环n*k次，从编程的顺序来排的。
extern void matmul2(const char *tr, int n, int k, int m, double alpha,
                   const double *A, const double *B, double beta, double *C)
{
    double d;
    int i,j,x,f=tr[0]=='N'?(tr[1]=='N'?1:2):(tr[1]=='N'?3:4);//NN:1  NT:2 TN:3 TT:4   N表示原始矩阵，T表示转置

    for (i=0;i<n;i++) for (j=0;j<k;j++) {
        d=0.0;
        switch (f) {
            case 1: for (x=0;x<m;x++) {d+=A[i+x*n]*B[x+j*m];qDebug()<<i+x*n<<x+j*m<<i+j*n;} break;
            case 2: for (x=0;x<m;x++) {d+=A[i+x*n]*B[j+x*k];qDebug()<<i+x*n<<j+x*k<<i+j*n;} break;
            case 3: for (x=0;x<m;x++) {d+=A[x+i*m]*B[x+j*m];qDebug()<<x+i*m<<x+j*m<<i+j*n;} break;
            case 4: for (x=0;x<m;x++) {d+=A[x+i*m]*B[j+x*k];qDebug()<<x+i*m<<j+x*k<<i+j*n;} break;
        }
        if (beta==0.0) C[i+j*n]=alpha*d; else C[i+j*n]=alpha*d+beta*C[i+j*n];
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


   double *A,*B,*C;
   A=zeros(3,3);B=zeros(2,3);
   C=zeros(3,3);

   matmul2("NT",3,2,3,1.0,A,B,0.0,C);



    return a.exec();
}



