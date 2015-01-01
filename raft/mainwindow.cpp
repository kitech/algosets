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

    QTimer *tmer = new QTimer();
    connect(tmer, &QTimer::timeout, this, &MainWindow::onStatusUpdateTimeout);
    tmer->start(100);
}

void MainWindow::on_pushButton_3_clicked()
{
    foreach (auto *rn, nodes) {
        // rn->on_state_machine();
        rn->dumpState();
    }
}

void MainWindow::onStatusUpdateTimeout()
{
    for (auto it = nodes.begin(); it != nodes.end(); it++) {
        int nid = it.key();
        RaftNode *rn = it.value();

        int leader_id = rn->_cms._leader_id +1;
        switch (nid) {
        case 0:
            ui->toolButton_1->setText(QString::number(rn->_cms._term));
            ui->lineEdit->setText(QString::number(leader_id));
            ui->lineEdit_6->setText(rn->ntname(rn->_cms._ntype));
            break;
        case 1:
            ui->toolButton_2->setText(QString::number(rn->_cms._term));
             ui->lineEdit_2->setText(QString::number(leader_id));
             ui->lineEdit_7->setText(rn->ntname(rn->_cms._ntype));
            break;
        case 2:
            ui->toolButton_3->setText(QString::number(rn->_cms._term));
            ui->lineEdit_3->setText(QString::number(leader_id));
            ui->lineEdit_8->setText(rn->ntname(rn->_cms._ntype));
            break;
        case 3:\
            ui->toolButton_4->setText(QString::number(rn->_cms._term));
            ui->lineEdit_4->setText(QString::number(leader_id));
            ui->lineEdit_9->setText(rn->ntname(rn->_cms._ntype));
            break;
        case 4:
            ui->toolButton_5->setText(QString::number(rn->_cms._term));
            ui->lineEdit_5->setText(QString::number(leader_id));
            ui->lineEdit_10->setText(rn->ntname(rn->_cms._ntype));
            break;
        default:
            assert(false);
            break;
        }
    }
}
