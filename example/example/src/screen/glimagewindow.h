#ifndef GLIMAGEWINDOW_H
#define GLIMAGEWINDOW_H

#include <QOpenGLWindow>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

// 网上很多示例使用的是这个类，这是Qt5.0添加的，编译时需要在pro文件中链接opengl库LIBS +=-lopengl32 -lglu32
// #include <QOpenGLFunctions>

// 由于使用到的glBegin等函数属于opengl1.0的API，在新版本中已经被删除了，所以这里需要使用Compatibility
// #include <QOpenGLFunctions_4_5_Compatibility>

#include <QOpenGLFunctions_4_5_Core>

class GLImageWindow : public QOpenGLWindow,
                      protected QOpenGLFunctions_4_5_Core {
    Q_OBJECT

public:

    enum DisplayMode {
        Original, // 原始大小居中
        Fit,      // 等比适应（居中全显示，可能有黑边）
        Cover,    // 等比填充（无黑边，可能裁剪）
        Stretch   // 拉伸填充（变形）
    };
    Q_ENUM(DisplayMode)
    explicit GLImageWindow(QWindow *parent = nullptr);

    // explicit GLImageWindow(QOpenGLContext *shareContext,
    //                        UpdateBehavior updateBehavior = NoPartialUpdate,
    //                        QWindow *parent = nullptr);
    ~GLImageWindow();

    // 设置显示模式
    void setDisplayMode(DisplayMode mode = Fit) {
        m_mode = mode;
    }

public slots:

    // 跨线程加载，OpenGL 操作必须在拥有 OpenGL 上下文的线程中执行。
    // 通常 QOpenGLWindow 的渲染是在主线程（GUI线程）进行的，所以直接调用
    // loadImage()（在主线程）是安全的
    void loadImage(const QImage& image);

protected:

    // 重写QGLWidget类的接口
    void initializeGL()override;
    void paintGL()override;
    void resizeGL(int w,
                  int h)override;

private:

    void getModelScale(float            & scaleWidth,
                       float            & scaleHeight,
                       const float      & windowWidth,
                       const float      & windowHeight,
                       const float      & imageWidth,
                       const float      & imageHeight,
                       const DisplayMode& mode);
    void setupShaders();
    void setupGeometry();
    void createTexture(const QImage& image); // 使用DSA创建纹理

    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLVertexArrayObject m_vao;

    // 使用原生OpenGL对象ID（4.5 DSA风格）
    GLuint m_vbo = 0;
    GLuint m_ibo = 0;
    GLuint m_texture = 0;

    int m_matrixUniform = -1;
    int m_textureWidth = 0;
    int m_textureHeight = 0;

    DisplayMode m_mode = Fit;

    QImage m_image;
};

#endif // GLIMAGEWINDOW_H
