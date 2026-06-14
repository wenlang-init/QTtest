#ifndef SIMPLEFFT_H
#define SIMPLEFFT_H

#include <QObject>

#ifndef PI
# define PI 3.1415926535897932384626433832795

// #define PI atan(1)* 4
#endif // !PI

typedef struct {
    double real;
    double img;
} complex;

class SimpleFFT : public QObject {
    Q_OBJECT

public:

    explicit SimpleFFT(QObject *parent = nullptr);

    void fft(complex       *IN_X,
             int            n,
             const complex *Wn);
    void fft(const QVector<complex>& in,
             QVector<complex>& out,
             int               N = 2048);

protected:

    void Reverse(complex *IN_X,
                 int      n);

    //复数加法的定义
    inline complex add(complex a, complex b)
    {
        complex c;

        c.real = a.real + b.real;
        c.img = a.img + b.img;
        return c;
    }

    //复数乘法的定义
    inline complex mul(complex a, complex b)
    {
        complex c;

        c.real = a.real * b.real - a.img * b.img;
        c.img = a.real * b.img + a.img * b.real;
        return c;
    }

    //复数减法的定义
    inline complex sub(complex a, complex b)
    {
        complex c;

        c.real = a.real - b.real;
        c.img = a.img - b.img;
        return c;
    }

private:

    void InitWn(complex *w,
                int      n);
    void output(int      size_x,
                complex *x);
    void test();

signals:
};

#endif // SIMPLEFFT_H
