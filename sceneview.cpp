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
    setMinimumSize(700, 700);
    setMaximumSize(700, 700);
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


//----------------------------VERTEX SHADER INIT------------------------------
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


//----------------------------FRAGMENT SHADER INIT---------------------------------
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

//-----------------------------DYNAMIC GRID STUFF---------------------------------

// specify n for an nxn grid
void SceneView::setGridSize(int newSize)
{
    this->GridSize = newSize;
}

int SceneView::calcNumCells()
{
    int n = this->GridSize;
    return n*n;
}

int SceneView::calcNumVertices()
{
    int n = this->GridSize;
    return (n+1)*(n+1);
}

int SceneView::calcVertArrayLength()
{
    return 3 * calcNumVertices();
}

int SceneView::calcNumTriangleCorners()
{
    return 6 * calcNumCells();
}

void SceneView::populateVerticeArray()
{
    for (int i = 0; i <= (this->GridSize); ++i)
    {
        for (int j = 0; j <= (this->GridSize); ++j)
        {
            this->vertices.push_back(i);
            this->vertices.push_back(j);
            this->vertices.push_back(0.0f);
        }
    }
}

void SceneView::populateIndices()
{
    int n = this->GridSize;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            int BL = i*(n+1) + j;
            int TL = BL + 1;
            int BR = BL + n + 1;
            int TR = BR + 1;

            this->indices.push_back(BL);
            this->indices.push_back(TL);
            this->indices.push_back(BR);
            //
            this->indices.push_back(TL);
            this->indices.push_back(TR);
            this->indices.push_back(BR);
        }
    }
}

//--------------------------------COLOR STUFF------------------------------------
int SceneView::getRedIndexAtPoint(int x, int y)
{
    int n = this->GridSize;
    return ((n+1)*4*x) + (4*y) + 0;
}
int SceneView::getGreenIndexAtPoint(int x, int y)
{
    int n = this->GridSize;
   return ((n+1)*4*x) + (4*y) + 1;
}
int SceneView::getBlueIndexAtPoint(int x, int y)
{
   int n = this->GridSize;
    return ((n+1)*4*x) + (4*y) + 2;
}
int SceneView::getOpacityIndexAtPoint(int x, int y)
{
    int n = this->GridSize;
    return ((n+1)*4*x) + (4*y) + 3;
}
void SceneView::setColorAtPoint(int x, int y, float R, float G, float B, float O)
{

}


void SceneView::populateColors()
{
    int numEntries = (this->calcNumVertices())*4;
    for (int i = 0; i < numEntries; i++)
    {
        this->colors.push_back(1.0f);
    }
}

//------------------------------INITIALIZE GL--------------------------------------
void SceneView::initializeGL()
{   
    initializeOpenGLFunctions();
    printContextInformation();

    initVertexShader();
    initFragmentShader();
    linkShaders();


    // set up vertex data (and buffer(s)) and configure vertex attributes
    setGridSize(2);                                                                                     // VERY IMPORTANT!!!!!
    populateVerticeArray();

    populateColors();
//    float colors[] = {
//        1.0f, 0.0f, 0.0f, 1.0f,  // Red
//        0.0f, 1.0f, 0.0f, 1.0f,  // Green
//        1.0f, 0.0f, 0.0f, 1.0f,  // Red
//        0.0f, 0.0f, 1.0f, 1.0f,   // Blue
//        0.0f, 1.0f, 0.0f, 1.0f,  // Green
//        0.0f, 1.0f, 0.0f, 1.0f,  // Green
//        1.0f, 0.0f, 0.0f, 1.0f,  // Red
//        0.0f, 0.0f, 1.0f, 1.0f,   // Blue
//        0.0f, 0.0f, 1.0f, 1.0f   // Blue
//    };

    populateIndices();

    // generate arrays and buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &ColorVBO);
    glGenBuffers(1, &EBO);

    // bind array before doing anything else
    glBindVertexArray(VAO);

    // setup vertex buffer for positions
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(float), this->vertices.data(), GL_STATIC_DRAW);
    // send position array to the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // setup vertex buffer for COLORS
    glBindBuffer(GL_ARRAY_BUFFER, ColorVBO);
    glBufferData(GL_ARRAY_BUFFER, this->colors.size()*sizeof(float), this->colors.data(), GL_DYNAMIC_DRAW);
    // send color array to the vertex shader
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // setup Element Buffer Object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size()*sizeof(float), this->indices.data(), GL_STATIC_DRAW);
}

//-------------------------------PAINT GL-------------------------------------------
void SceneView::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // transform stuff
    glm::mat4 trans = glm::mat4(1.0f);
    // YOU MUST SCALE BEFORE TRANSLATING
    trans = glm::scale(trans, glm::vec3(2.0f / (this->GridSize), 2.0f / (this->GridSize), 1.0f));
    trans = glm::translate(trans, glm::vec3( -((this->GridSize)/2.0f), -((this->GridSize)/2.0f), 0.0f));

    glUseProgram(shaderProgram);

    // tie the uniform variable 'trans' to the vertex shader
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

    glBindVertexArray(VAO);
    // finally, draw the triangles
    glDrawElements(GL_TRIANGLES, calcNumTriangleCorners(), GL_UNSIGNED_INT, 0);
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
