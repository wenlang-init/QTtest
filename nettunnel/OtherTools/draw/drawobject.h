#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H

#include <QObject>
class QImage;
enum DRAWTYPE {
    DRAWTYPE_HOLLOW = 0, // 空心心型
    DRAWTYPE_RECT,       // 矩形
    DRAWTYPE_FILLHEART,  // 填充心型
    DRAWTYPE_PENTAGRAM,  // 填充心型

    DRAWTYPE_END,
};

class drawObject : public QObject {
    Q_OBJECT

public:

    explicit drawObject(QObject *parent = nullptr);
    static int getGraphData(char    *data,
                            int      width,
                            int      height,
                            DRAWTYPE type);
    static int getGraphQImage(QImage & image,
                              int      width,
                              int      height,
                              DRAWTYPE type);

protected:

    int getHollowData(char *data,
                      int   width,
                      int   height);
    int getRectData(char *data,
                    int   width,
                    int   height);
    int getFillHeartData(char *data,
                         int   width,
                         int   height);
    int getPentagramData(char *data,
                         int   width,
                         int   height);

private:

    void draw_line(int   x0,
                   int   y0,
                   int   x1,
                   int   y1,
                   char *data,
                   int   w);

signals:
};

#endif // DRAWOBJECT_H
