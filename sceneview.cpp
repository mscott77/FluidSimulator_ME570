#include "sceneview.hpp"
#include <QFile>
#include <QDebug>
#include <string>
//
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SceneView::SceneView(QWidget* parent): QOpenGLWidget(parent)
{
    setMinimumSize(775, 775);
    setMaximumSize(775, 775);
}

SceneView::~SceneView()
{
  teardownGL();
}

std::string SceneView::read_shader_code_from_resource_file(QString filepath)
{
  QString data;
  QFile file(filepath);

  if(!file.open(QIODevice::ReadOnly))
  {
    qDebug()<<"file not opened";
  }
  else
  {
    data = file.readAll();
  }

  return data.toStdString();
}

QImage SceneView::read_texture_from_resource_file(QString filepath)
{
  QImage texture(filepath);
  return texture;
}

void SceneView::initializeGL()
{
  initializeOpenGLFunctions();
  printContextInformation();

  vertexShaderCode = read_shader_code_from_resource_file(":/shaders/pass_through.vert");
  fragmentShaderCode = read_shader_code_from_resource_file(":/shaders/simple.frag");

  //Texture image reading from file using Qt
  //textureImage = read_texture_from_resource_file(":/textures/container.png");
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureImage.width(), textureImage.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
  //textureImage.bits());

  // build and compile our shader program
  // ------------------------------------
  // vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  const char* vertexShaderString = vertexShaderCode.c_str();
  glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
  }

  // fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragmentShaderString = fragmentShaderCode.c_str();
  glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    qDebug() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
  }

  // link shaders
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

  if (!success)
  {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float vertices[] =
  {
    -0.5f, -0.5f, 0.0f, // left
    0.5f, -0.5f, 0.0f, // right
    0.0f, 0.5f, 0.0f // top
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);

}

void SceneView::paintGL()
{
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // transform stuff
  glm::mat4 trans = glm::mat4(1.0f);
  trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
  trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

  // draw our first triangle
  glUseProgram(shaderProgram);

  // link the uniform variable to the vertex shader
  unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void SceneView::resizeGL(int width, int height)
{
}

void SceneView::teardownGL()
{
  makeCurrent();
}

void SceneView::printContextInformation()
{
  QString glType{(context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL"};
  QString glVersion{reinterpret_cast<const char*>(glGetString(GL_VERSION))};
  QString glProfile;

  // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break

  switch (format().profile())
  {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }

#undef CASE
  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
