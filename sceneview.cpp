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


//---------------------VERTEX SHADER INIT------------------------------
void SceneView::initVertexShader(){

    vertexShaderCode = read_shader_code_from_resource_file(":/shaders/pass_through.vert");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderString = vertexShaderCode.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderString, NULL);
    glCompileShader(vertexShader);

    checkVertShaderErrors();
}

void SceneView::checkVertShaderErrors(){
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
    }
}


//---------------------FRAGMENT SHADER INIT---------------------------------
void SceneView::initFragmentShader(){

    fragmentShaderCode = read_shader_code_from_resource_file(":/shaders/simple.frag");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderString = fragmentShaderCode.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderString, NULL);
    glCompileShader(fragmentShader);

    checkFragShaderErrors();
}


void SceneView::checkFragShaderErrors(){
    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    qDebug() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
    }
}

void SceneView::linkShaders(){

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkLinkIssues();

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void SceneView::checkLinkIssues(){
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
    }
}

void SceneView::populateVerticeArray(int count, float verticeArray[])
{
    verticeArray[0] = 0;
    verticeArray[1] = 0;
    verticeArray[2] = 0;

    verticeArray[3] = 4;
    verticeArray[4] = 0;
    verticeArray[5] = 0;

    verticeArray[6] = 4;
    verticeArray[7] = 4;
    verticeArray[8] = 0;

    verticeArray[9] = 0;
    verticeArray[10] = 4;
    verticeArray[11] = 0;

    verticeArray[12] = 8;
    verticeArray[13] = 0;
    verticeArray[14] = 0;

    verticeArray[15] = 8;
    verticeArray[16] = 4;
    verticeArray[17] = 0;

    verticeArray[18] = 8;
    verticeArray[19] = 8;
    verticeArray[20] = 0;

    verticeArray[21] = 4;
    verticeArray[22] = 8;
    verticeArray[23] = 0;

    verticeArray[24] = 0;
    verticeArray[25] = 8;
    verticeArray[26] = 0;
}

void SceneView::initializeGL()
{
    initializeOpenGLFunctions();
    printContextInformation();

    initVertexShader();
    initFragmentShader();
    linkShaders();


    // set up vertex data (and buffer(s)) and configure vertex attributes


    float vertices[27];
    populateVerticeArray(27, vertices);
    /*= {
        0, 0, 0,
        4, 0, 0,
        4, 4, 0,
        0, 4, 0,
        8, 0, 0,
        8, 4, 0,
        8, 8, 0,
        4, 8, 0,
        0, 8, 0
    };*/


    float colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,  // Red
        0.0f, 1.0f, 0.0f, 1.0f,  // Green
        1.0f, 0.0f, 0.0f, 1.0f,  // Red
        0.0f, 0.0f, 1.0f, 1.0f,   // Blue
        0.0f, 1.0f, 0.0f, 1.0f,  // Green
        0.0f, 1.0f, 0.0f, 1.0f,  // Green
        1.0f, 0.0f, 0.0f, 1.0f,  // Red
        0.0f, 0.0f, 1.0f, 1.0f,   // Blue
        0.0f, 0.0f, 1.0f, 1.0f   // Blue
    };


    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,
        0, 2, 3,
        1, 5, 2,
        1, 4, 5,
        2, 5, 6,
        2, 6, 7,
        2, 7, 3,
        3, 7, 8
    };

    // generate arrays and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &ColorVBO);
    glGenBuffers(1, &EBO);

    // bind array before doing anything else
    glBindVertexArray(VAO);

    // setup vertex buffer for positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // send position array to the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup vertex buffer for COLORS
    glBindBuffer(GL_ARRAY_BUFFER, ColorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    // send color array to the vertex shader
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // setup Element Buffer Object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void SceneView::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // transform stuff
    glm::mat4 trans = glm::mat4(1.0f);
    //trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));
    // YOU MUST SCALE BEFORE TRANSLATING
    trans = glm::scale(trans, glm::vec3(2.0f / 8.0f, 2.0f / 8.0f, 1.0f));
    trans = glm::translate(trans, glm::vec3(-4.0f, -4.0f, 0.0f));

    glUseProgram(shaderProgram);

    // tie the uniform variable 'trans' to the vertex shader
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glBindVertexArray(VAO);
    // finally, draw the triangles
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
    //glBindVertexArray(0);
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
