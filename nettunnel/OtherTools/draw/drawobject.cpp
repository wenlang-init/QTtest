#include "drawobject.h"
#include <QImage>
drawObject::drawObject(QObject *parent)
    : QObject{parent}
{}

int drawObject::getGraphData(char *data, int width, int height, DRAWTYPE type)
{
    if (!data) {
        return -1;
    }

    static drawObject obj;

    switch (type) {
    case DRAWTYPE_HOLLOW:
        obj.getHollowData(data, width, height);
        break;

    case DRAWTYPE_RECT:
        obj.getRectData(data, width, height);
        break;

    case DRAWTYPE_FILLHEART:
        obj.getFillHeartData(data, width, height);
        break;

    case DRAWTYPE_PENTAGRAM:
        obj.getPentagramData(data, width, height);
        break;

    default:
        break;
    }
    return 0;
}

int drawObject::getGraphQImage(QImage& image, int width, int height,
                               DRAWTYPE type)
{
    QByteArray data(width * height, 0);

    if (0 != getGraphData(data.data(), width, height, type))
    {
        return -1;
    }
    QByteArray imagedata(width * height * 3, 0);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int v = data[i * width + j];

            if (v) {
                imagedata[(i * width + j) * 3 + 0] = rand() % 255; // B
                imagedata[(i * width + j) * 3 + 1] = rand() % 255; // G
                imagedata[(i * width + j) * 3 + 2] = rand() % 255; // R
            } else {
                imagedata[(i * width + j) * 3 + 0] = 0;            // B
                imagedata[(i * width + j) * 3 + 1] = 0;            // G
                imagedata[(i * width + j) * 3 + 2] = 0;            // R
            }
        }
    }
    image =
        QImage((uchar *)imagedata.data(), width, height, QImage::Format_BGR888);
    return 0;
}

// 心型曲线: (x^2 + y^2 -1)^3 - x^2y^3 <= 0的区域(x,y为区域宽高,[-1.5,1.5])

int drawObject::getHollowData(char *data, int width, int height)
{
    float max = 3.0;
    float xstep = max / (float)(width);
    float ystep = -max / (float)(height);
    float xstart = -max / 2;
    float ystart = max / 2;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            float x = xstart + i * xstep;
            float y = ystart + j * ystep;
            float v = x * x + y * y - 1;
            v = v * v * v - x * x * y * y * y;

            if (abs(v) <= 0.001) {
                data[j * width + i] = 1;
            } else {
                data[j * width + i] = 0;
            }
        }
    }

    return 0;
}

int drawObject::getRectData(char *data, int width, int height)
{
    int fl1 = 0;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (fl1 % 2 == 0) {
                if (j % 2 == 0) {
                    data[j * width + i] = 1;
                } else {
                    data[j * width + i] = 0;
                }
            } else {
                if (j % 2 != 0) {
                    data[j * width + i] = 1;
                } else {
                    data[j * width + i] = 0;
                }
            }
        }
        fl1++;
    }
    return 0;
}

// y=x^(2/3) + 4/5*(9/10 - sin(aπx)*x^2)^(1/2) 的区域
int drawObject::getFillHeartData(char *data, int width, int height)
{
    float a = 1;
    float xstep = 3.0 / (float)(width);
    float ystep = -3.0 / (float)(height);
    float xstart = -1.5;
    float ystart = 1.5;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            float x = xstart + i * xstep;
            float y = ystart + j * ystep;
            float v =
                pow(x, 2 / 3.0) + 4 / 5.0 * pow(9 / 10.0 - sin(
                                                    a * M_PI * x) * x * x,
                                                0.5) - y;

            if (abs(v) <= 0.001) {
                data[j * width + i] = 1;
            } else {
                data[j * width + i] = 0;
            }
        }
    }
    return 0;
}

// 使用Bresenham算法画线
void drawObject::draw_line(int x0, int y0, int x1, int y1, char *data, int w) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        // lcd_put_pixel(x0, y0, color);
        data[y0 * w + x0] = 1;

        if ((x0 == x1) && (y0 == y1)) break;
        e2 = 2 * err;

        if (e2 >= dy) { err += dy; x0 += sx; }

        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

int drawObject::getPentagramData(char *data, int width, int height)
{
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            data[j * width + i] = 0;
        }
    }

    int centerX = width / 2;
    int centerY = height / 2;
    int radius = width < height ? width : height;

    radius = radius / 2; // 大小
    int x[5], y[5];

    for (int i = 0; i < 5; i++) {
        // 计算每个顶点的坐标
        x[i] = centerX + radius * cos(i * 72 * M_PI / 180);
        y[i] = centerY + radius * sin(i * 72 * M_PI / 180);
    }

    // 绘制五角星的每一条边
    for (int i = 0; i < 5; i++) {
        draw_line(x[i], y[i], x[(i + 2) % 5], y[(i + 2) % 5], data, width);
    }
    return 0;
}
