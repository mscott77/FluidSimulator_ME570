#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QGridLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include "sceneview.hpp"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(800,800);
    scene = new SceneView();

    QFormLayout* layout = new QFormLayout(ui->frame);
    //layout->setContentsMargins(0, 0, 0, 0);
    //layout->setSpacing(0);

    QPushButton* button_1 = new QPushButton("Button1");
    QPushButton* button_2 = new QPushButton("Button2");

    QCheckBox* box_1 = new QCheckBox();
    QLabel* label_1 = new QLabel("label1");


    layout->addRow(button_1, button_2);
    layout->addRow(scene);
    layout->addRow(label_1, box_1);
    ui->frame->setLayout(layout);
}

MainWindow::~MainWindow()
{
  delete ui;
}
