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
  std::string read_shader_code_from_resource_file(QString filepath);
  QImage read_texture_from_resource_file(QString filepath);
  void printContextInformation();

  std::string vertexShaderCode;
  std::string fragmentShaderCode;
  QImage textureImage;

  unsigned int shaderProgram{0};
  unsigned int VBO{0};
  unsigned int VAO{0};
};

#endif // SCENEVIEW_HPP
