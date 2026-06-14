#include "simplefft.h"
#include <math.h>
#include <QDebug>

SimpleFFT::SimpleFFT(QObject *parent)
    : QObject{parent}
{}

void SimpleFFT::fft(complex *IN_X, int n, const complex *Wn)
{
    int row = log(n) / log(2); // row为FFT的N个输入对应的最大列数
    int i = 0, j = 0, k = 0, L = 0;
    complex top, bottom, product;

    // 调用变址函数
    Reverse(IN_X, n);

    for (i = 0; i < row; i++) /*第i级蝶形运算 */
    {
        // L等于2的i次方,表示每一级蝶形中组间的距离，即一组中蝶形个数
        L = 1 << i;

        for (j = 0; j < n; j += 2 * L) /*j为组偏移地址，每L个蝶形是一组，每级有N/2L组*/
        {
            for (k = 0; k < L; k++)    /*k为一组中蝶形的偏移地址，一个蝶形运算 每个group内的蝶形运算*/
            {
                product = mul(IN_X[j + k + L], Wn[n * k / 2 / L]);
                top = add(IN_X[j + k], product);
                bottom = sub(IN_X[j + k], product);
                IN_X[j + k] = top;
                IN_X[j + k + L] = bottom;
            }
        }
    }
}

void SimpleFFT::fft(const QVector<complex>& in, QVector<complex>& out, int N)
{
    if ((in.size() < 1) || (in.size() % 2 != 0)) {
        qDebug() << "输入数据需要为2的倍数";
    }
    out.resize(N);
    fft(out.data(), N, in.data());
}

/*变址计算，将x(n)码位倒置*/

/*
   码位倒序要解决两个问题：
    ①将t位二进制数倒序;②将倒序后的两个存储单元进行交换。
    如果输入序列的自然顺序号i用二进制数表示，
    例如若最大序号为15，即用4位就可表示n3n2n1n0，
    则其倒序后j对应的二进制数就是n0n1n2n3
 */
void SimpleFFT::Reverse(complex *IN_X, int n)
{
    int row = log(n) / log(2); // row为FFT的N个输入对应的最大列数
    complex temp;              // 临时交换变量
    unsigned short i = 0, j = 0, k = 0;
    double t;

    for (i = 0; i < row; i++) // 从第0个序号到第N-1个序号
    {
        k = i;                // 当前第i个序号
        j = 0;                // 存储倒序后的序号，先初始化为0
        t = row;              // 共移位t次

        while ((t--) > 0)     // 利用按位与以及循环实现码位颠倒
        {
            j = j << 1;
            j |= (k & 1);     // j左移一位然后加上k的最低位
            k = k >> 1;       // k右移一位，次低位变为最低位

            /*j为倒序，k为正序，
               从k右向左的值依次移位，
               j向左依次填入对应的k的右向位*/
        }

        if (j > i) // 如果倒序后大于原序数，就将两个存储单元进行交换(判断j>i是为了防止重复交换
        {
            temp = IN_X[i];
            IN_X[i] = IN_X[j];
            IN_X[j] = temp;
        }
    }
}

/*产生一个周期欧拉形式的Wn的值*/
void SimpleFFT::InitWn(complex *w, int n) // n为输入的个数,w为权值Wn
{
    int k;

    for (k = 0; k < n; k++)
    {
        w[k].real = cos(2 * PI / n * k); // 用欧拉公式计算旋转因子
        w[k].img = -1 * sin(2 * PI / n * k);
    }
}

/*输出傅里叶变换的结果*/
void SimpleFFT::output(int size_x, complex *x)
{
    int i;

    printf("The result are as follows：\n");

    for (i = 0; i < size_x; i++)
    {
        printf("%.4f", x[i].real);

        if (x[i].img >= 0.0001) printf("+%.4fj\n", x[i].img);
        else if (fabs(x[i].img) < 0.0001) printf("\n");
        else printf("%.4fj\n", x[i].img);
    }

    for (i = 0; i < size_x; i++)
    {
        int cnt = x[i].real * 10;

        while (cnt-- > 0) {
            printf("*");
        }
        printf("#\n");
    }
}

void SimpleFFT::test()
{
#if 0
    int i;                                      /*输出结果*/
    int size_x;                                 /*size_x为采样的信号个数，必须为2的倍数*/
    system("cls");                              // 调用系统命令，CLS的功能为清除屏幕输出
    printf("输出DIT方法实现的FFT结果\n");
    printf("Please input the size of x:\n");    // 输入序列的大小
    scanf("%d", &size_x);
    printf("Please input the data in x[N]:\n"); // 输入序列的实部和虚部

    for (i = 0; i < size_x; i++)
    {
        printf("请输入第%d个序列：", i);
        scanf("%lf%lf", &x[i].real, &x[i].img);
    }
    printf("输出倒序后的序列\n");
    Wn = (complex *)malloc(sizeof(complex) * size_x); // 分配变换后的值的内存空间
    InitWn(Wn, size_x);                               // 调用变换核
    fft(x, size_x);                                   // 调用快速傅里叶变换
    printf("输出FFT后的结果\n");
    output(size_x);                                   // 调用输出傅里叶变换结果函数
    scanf("%d", &size_x);
    free(Wn);
#else // if 0
    int i;
    int size_x = 100; /*size_x为采样的信号个数，必须为2的倍数*/
    complex x[1000];

    printf("输出DIT方法实现的FFT结果\n");
    printf("输入序列的大小 x:%d\n", size_x); // 输入序列的大小
    printf("输入序列的实部和虚部:\n");          // 输入序列的实部和虚部
    srand(time(NULL));

    for (i = 0; i < size_x; i++)
    {
        x[i].real = (rand() % 1000000) / 1000000.0;
        x[i].img = (rand() % 1000000) / 1000000.0;
        printf("输入第%d个序列：%lf+%lfj\n", i, x[i].real, x[i].img);
    }
    printf("输出倒序后的序列\n");
    complex *Wn;                                      /*输入序列,复数系数Wn*/
    Wn = (complex *)malloc(sizeof(complex) * size_x); // 分配变换后的值的内存空间
    InitWn(Wn, size_x);                               // 调用变换核
    fft(x, size_x, Wn);                               // 调用快速傅里叶变换
    free(Wn);
    printf("输出FFT后的结果\n");
    output(size_x, x);                                // 调用输出傅里叶变换结果函数
#endif // if 0
}
