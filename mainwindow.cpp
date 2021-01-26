//{
#include <fstream>
//#include <iostream>
#include <thread>
//#include <cstdlib>

#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QRandomGenerator>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shared.cc"
#include "shared.h"
#include "consts.h"
//}

SharedMem *shared;
QSettings *settings    = new QSettings("bsio.conf", QSettings::NativeFormat);
bool       isImitation = false;

//-------------------Всё что касается окна в целом и то, что можно уже не трогать--------------------//
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{   
    ui->setupUi(this);
    bool darktheme = settings->value("theme/darkmode",false).toBool();
    ui->dark_theme->setChecked(darktheme);
    QApplication::setPalette(QPalette(darktheme?
                                          QColor(38,39,40):
                                          QColor(220,220,220)));
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_dark_theme_toggled(bool darkThemeChecked)
{
    if (darkThemeChecked)
        QApplication::setPalette(QPalette(QColor(38,39,40)));
    else
        QApplication::setPalette(QPalette(QColor(220,220,220)));
    settings->setValue("theme/darkmode",darkThemeChecked);
    settings->sync();
}
void MainWindow::on_bsList_currentIndexChanged(const QString &currentValue)
{
    ui->statusbar->showMessage("Выбран "+currentValue);
}

//-------------------Модели таблиц и всё что с ними связано------------------------------------------//
QVariant ModulesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("#");
        case 1:
            return QString("Тип модуля");
        case 2:
            return QString("Статус модуля");
        case 3:
            return QString("Счётчик");
        }
    }
    return QVariant();
}
QVariant ModulesTableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()){
        switch (role) {
        case Qt::DisplayRole:
            switch (index.column()){
            case 0:
                return QString::number(index.row());
            case 1:
                return modules[shared->region->BS_input[index.row()].device.type].type;
            case 2:
                return module_states[shared->region->BS_input[index.row()].device.state];
            case 3:
                return QString::number(shared->region->BS_input[index.row()].trace.count);
            }
        case Qt::TextAlignmentRole:
            return int(Qt::AlignCenter | Qt::AlignVCenter);
        }
    }
    return QVariant();
}

//-------------------Прочие функции------------------------------------------------------------------//
void MainWindow::on_create_shm_triggered()
{
    QString shm_name = QInputDialog::getText(
                this, "Input SHM region name",
                "Name", QLineEdit::Normal, "bs1");
    SharedMem new_shared(shm_name.toStdString().c_str());
    new_shared.~SharedMem();
}

QStringList searchSharedRegs()
{
    const std::string path = "/dev/shm/bs";

    std::string fullname;
    QStringList bs_list;

    for (int i=1;;i++) {
        fullname = path + std::to_string(i);
        if (std::ifstream(fullname.c_str()).good())
            bs_list.append("БС-"+QString::number(i));
        else
            break;
    }
    return bs_list;
}
void MainWindow::on_updateListButton_clicked()
{
    QStringList bs_list = searchSharedRegs();
    ui->bsList->clear();
    ui->bsList->addItems(bs_list);
    ui->statusbar->showMessage(
                "Найдено БС: "+
                QString::number(bs_list.length()));
}
void MainWindow::on_bsList_activated(int index)
{
    std::string bs_name = "/bs"+std::to_string(index+1);
    shared = new SharedMem(bs_name.c_str());
    ModulesTableModel *md = new ModulesTableModel(this);
    ui->modulesView->setModel(md);
    ui->modulesView->resizeColumnsToContents();
}

void imitation()
{
    QRandomGenerator *rg = QRandomGenerator::global();
    SharedMem shm("/bs1");
    int i;
    while (isImitation){
        for (i=0; i<=15; i++){
            shm.region->BS_input[i].device.state = rg->bounded(0,20);
            shm.region->BS_input[i].device.type  = rg->bounded(1,13);
            shm.region->BS_input[i].trace.count += 1;
            shm.region->BS_input[i].AI.data[0] = rg->bounded(0,100);
            shm.region->BS_input[i].AI.data[1] = rg->bounded(0,100);
            shm.region->BS_input[i].AI.data[2] = rg->bounded(0,100);
            shm.region->BS_input[i].AI.data[3] = rg->bounded(0,100);
            shm.region->BS_input[i].DI.data[0] = rg->bounded(0,100);
            shm.region->BS_input[i].DI.data[1] = rg->bounded(0,100);
            shm.region->BS_input[i].DI.data[2] = rg->bounded(0,100);
            shm.region->BS_input[i].DI.data[3] = rg->bounded(0,100);
        }
        sleep(2);
    }
}
void MainWindow::on_imitationBox_stateChanged(int imitation_checked)
{
    std::thread *imit_thread;
    if ((bool)imitation_checked){
        isImitation = true;
        imit_thread = new std::thread(imitation);
        ui->statusbar->showMessage("Имитация запущена");
    } else {
        if (isImitation){
            isImitation = false;
            ui->statusbar->showMessage("Имитация остановлена");
        }
    }
}
template<typename Tin,typename Tout, typename Tmod>
void MainWindow::fill_IO(Tin in, Tout out, Tmod modInfo){
    ui->valid_value->setText(valid_state[in.sign.valid]);
    ui-> opc_value ->setText(opc_state[in.sign.opc]);
    for (int i=0;i<modInfo.iCount;i++) {
        ui->in_list ->setItem(i,0,new QTableWidgetItem(QString::number(in.data[i])));
    }
    for (int i=0;i<modInfo.oCount;i++) {
        ui->in_list ->setItem(i,0,new QTableWidgetItem(QString::number(out.data[i])));
    }
}
void MainWindow::on_modulesView_activated(const QModelIndex &index)
{
    auto modInData  = &shared->region->BS_input [index.row()];
    auto modOutData = &shared->region->BS_output[index.row()];
    module modInfo  = modules[modInData->device.type];

    ui->name_value->setText(modInfo.type);
    ui->type_value->setText(modInfo.iotype);
    ui-> out_list ->setRowCount(modInfo.oCount);
    ui->  in_list ->setRowCount(modInfo.iCount);

    if (modInfo.iotype == "A")
        MainWindow::fill_IO(modInData->AI, modOutData->AO, modInfo);
    else if (modInfo.iotype == "D")
        MainWindow::fill_IO(modInData->DI, modOutData->DO, modInfo);
}
