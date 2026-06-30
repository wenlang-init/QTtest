#include "glimagewidget.h"
#include <QMatrix4x4>
#include <QDebug>
#include <QSurfaceFormat>
#include <QImage>
#include <QPainter>

// 顶点着色器 (4.5)
static const char *vertexShaderSource =
    "#version 450 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 uMatrix;\n"
    "void main() {\n"
    "    gl_Position = uMatrix * vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

// 片段着色器 (4.5)
static const char *fragmentShaderSource =
    "#version 450 core\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D uTexture;\n"
    "void main() {\n"
    "    FragColor = texture(uTexture, TexCoord);\n"
    "}\n";


GLImageWidget::GLImageWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    // setAutoFillBackground(false);
    auto version = this->format().version();

    // 请求 4.5 Core Profile
    QSurfaceFormat format;

    format.setVersion(4, 5);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);

    qDebug() << version << "->" << this->format().version();
}

GLImageWidget::~GLImageWidget()
{
    // 确保OpenGL资源在上下文销毁前释放
    makeCurrent();
    delete m_program;

    if (m_vbo) glDeleteBuffers(1, &m_vbo);

    if (m_ibo) glDeleteBuffers(1, &m_ibo);

    if (m_texture) glDeleteTextures(1, &m_texture);
    m_vao.destroy();
    doneCurrent();
}

void GLImageWidget::loadImage(const QImage& image)
{
    // if (!this->context()) return;
    if (!isValid()) return;

    m_image = image;

    // 必须在 OpenGL 上下文中执行
    makeCurrent();
    createTexture(image);
    doneCurrent();
    update(); // 触发重绘
}

void GLImageWidget::initializeGL()
{
    initializeOpenGLFunctions(); // 加载 4.5 函数

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    setupShaders();
    setupGeometry();

    // 设置默认纹理（蓝色背景）
    QImage defaultImg(256, 256, QImage::Format_RGBA8888);
    defaultImg.fill(Qt::blue);
    createTexture(defaultImg);
}

// 窗口绘制函数
void GLImageWidget::paintGL()
{
#if 0

    // 创建一个QPainter来绘制
    QPainter painter(this);

    // 在窗口的(0,0)位置绘制图片，并自动缩放以适应窗口
    painter.drawImage(QRect(0, 0, size().width(), size().height()), m_image);
    return;

#endif // if 0
    glClear(GL_COLOR_BUFFER_BIT);

    if (!m_program || (m_texture == 0)) return;

    m_program->bind();

    // 计算保持宽高比的矩阵
    QMatrix4x4 matrix;
    float scaleWidth = 1, scaleHeight = 1;

    // m_mode = Original;
    getModelScale(scaleWidth, scaleHeight,
                  (const float)width(), (const float)height(),
                  (const float)m_textureWidth, (const float)m_textureHeight,
                  m_mode);
    matrix.scale(scaleWidth, scaleHeight);

    m_program->setUniformValue(m_matrixUniform, matrix);

    // 绑定纹理 (传统方式，因为 sampler 使用绑定点)
    glBindTextureUnit(0, m_texture); // 4.5 新函数，直接绑定到单元
    m_program->setUniformValue("uTexture", 0);

    m_vao.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    m_vao.release();

    m_program->release();
}

void GLImageWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLImageWidget::getModelScale(float            & scaleWidth,
                                  float            & scaleHeight,
                                  const float      & windowWidth,
                                  const float      & windowHeight,
                                  const float      & imageWidth,
                                  const float      & imageHeight,
                                  const DisplayMode& mode)
{
    if (((windowWidth != 0) && (windowHeight != 0) &&
         (imageWidth != 0)) && (imageHeight != 0)) {
        switch (mode) {
        case Original:
        {
            // 原始大小
            scaleWidth = imageWidth / windowWidth;
            scaleHeight = imageHeight / windowHeight;
            break;
        }

        case Fit:
        {
            // 等比适应（全显示，可能有黑边）
            float scaleImage = imageWidth / imageHeight;
            float scaleWindow = windowWidth / windowHeight;
            float scaleWidth0 = imageWidth / windowWidth;
            float scaleHeight0 = imageHeight / windowHeight;

            if (scaleWidth0 > scaleHeight0) {
                scaleWidth = 1;
                scaleHeight = scaleWindow * scaleWidth / scaleImage;
            } else {
                scaleHeight = 1;
                scaleWidth = scaleImage * scaleHeight / scaleWindow;
            }
            break;
        }

        case Cover:
        {
            // 等比填充（无黑边，可能裁剪）
            float scaleImage = imageWidth / imageHeight;
            float scaleWindow = windowWidth / windowHeight;

            if (scaleImage > 1) {
                scaleWidth = 1;
                scaleHeight = scaleWindow * scaleWidth / scaleImage;
            } else {
                scaleHeight = 1;
                scaleWidth = scaleImage * scaleHeight / scaleWindow;
            }
            break;
        }

        // 拉伸填充（变形）
        case Stretch:
        default:
        {
            scaleWidth = 1;
            scaleHeight = 1;
            break;
        }
        }
    } else  {
        scaleWidth = 1;
        scaleHeight = 1;
    }
}

void GLImageWidget::setupShaders()
{
    m_program = new QOpenGLShaderProgram(this);

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                            vertexShaderSource) ||
        !m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                            fragmentShaderSource) ||
        !m_program->link()) {
        qDebug() << "Shader error:" << m_program->log();
        return;
    }
    m_matrixUniform = m_program->uniformLocation("uMatrix");
}

void GLImageWidget::setupGeometry()
{
    // 矩形顶点: 位置(x, y) + 纹理坐标(s, t)
    float vertices[] = {
        -1.0f, 1.0f,   0.0f,   0.0f,
        1.0f,  1.0f,   1.0f,   0.0f,
        1.0f,  -1.0f,  1.0f,   1.0f,
        -1.0f, -1.0f,  0.0f,   1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    // 使用 DSA 创建 VBO 和 IBO
    glCreateBuffers(1, &m_vbo);
    glNamedBufferStorage(m_vbo, sizeof(vertices), vertices, 0); // 不可变存储，但足够

    glCreateBuffers(1, &m_ibo);
    glNamedBufferStorage(m_ibo, sizeof(indices), indices, 0);

    // VAO 仍然需要用传统方式绑定（因为 VAO 无 DSA）
    m_vao.create();
    m_vao.bind();

    glBindBuffer(        GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    // 位置属性 (location=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

    // 纹理坐标 (location=1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));

    m_vao.release();
}

void GLImageWidget::createTexture(const QImage& image)
{
    // 将 QImage 转为 RGBA 格式，并修正 Y 轴
    QImage texImg = image // .flipped(Qt::Horizontal) /*mirrored()*/
                    .convertToFormat(QImage::Format_RGBA8888);
    int w = texImg.width();
    int h = texImg.height();

    // 如果已有纹理且尺寸相同，直接更新数据
    if ((m_texture != 0) && (m_textureWidth == w) && (m_textureHeight == h)) {
        glTextureSubImage2D(m_texture,
                            0,
                            0,
                            0,
                            w,
                            h,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            texImg.bits());
        return;
    }

    // 否则删除旧纹理并创建新纹理（DSA 方式）
    if (m_texture) glDeleteTextures(1, &m_texture);

    if (!this->context()) return;

    // if(!isValid())return;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
    glTextureStorage2D(m_texture, 1, GL_RGBA8, w, h); // 分配不可变存储
    glTextureSubImage2D(m_texture,
                        0,
                        0,
                        0,
                        w,
                        h,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        texImg.bits());

    // 设置纹理参数 (DSA 方式)
    glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER,        GL_LINEAR);
    glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER,        GL_LINEAR);
    glTextureParameteri(m_texture,     GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_texture,     GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_textureWidth = w;
    m_textureHeight = h;
}
