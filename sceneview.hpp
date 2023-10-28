#ifndef SCENEVIEW_HPP
#define SCENEVIEW_HPP

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <string>

class SceneView : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    SceneView(QWidget* parent=nullptr);
    ~SceneView();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void teardownGL();

private:

    int GridSize{0}; // nxn square
    void setGridSize(int newSize);
    int calcNumCells();
    int calcNumVertices();
    int calcVertArrayLength();
    int calcNumTriangleCorners();
    void populateVerticeArray(int count, float verticeArray[]);

    void initVertexShader();
    void checkVertShaderErrors();
    void initFragmentShader();
    void checkFragShaderErrors();
    void linkShaders();
    void checkLinkIssues();

    std::string read_shader_code_from_resource_file(QString filepath);
    std::string vertexShaderCode;
    std::string fragmentShaderCode;

    unsigned int vertexShader{0};
    unsigned int fragmentShader{0};
    unsigned int shaderProgram{0};
    unsigned int VBO{0};
    unsigned int ColorVBO{0};
    unsigned int VAO{0};
    unsigned int EBO{0};

    QImage read_texture_from_resource_file(QString filepath);
    void printContextInformation();

};

#endif // SCENEVIEW_HPP
