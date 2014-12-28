#include <QtCore>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "raft.h"

QHash<int, RaftNode*> nodes;
int node_num = 5;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    qDebug()<<"hjjjjjjjjjjjjjjjjj";
    for (int i = 0; i < node_num; i++) {
        RaftNode *rn = new RaftNode(i);
        nodes.insert(i, rn);
        rn->start();
    }

}

void MainWindow::on_pushButton_2_clicked()
{
    qDebug()<<"select me.....";
    foreach (auto *rn, nodes) {
        rn->on_state_machine();
    }
}
