//{
#include <fstream>
#include <iostream>
#include <thread>
#include <cstdlib>

#include "QMessageBox"
#include "QInputDialog"
#include "QSettings"

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
                                          Qt::GlobalColor::darkGray:
                                          Qt::GlobalColor::lightGray));
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_dark_theme_toggled(bool arg1)
{
    if (arg1)
        QApplication::setPalette(QPalette(Qt::GlobalColor::darkGray));
    else
        QApplication::setPalette(QPalette(Qt::GlobalColor::lightGray));
    settings->setValue("theme/darkmode",arg1);
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
                        return module_types[shared->region->BS_input[index.row()].device.type];
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
    std::srand(std::time(nullptr));
    SharedMem shm("/bs1");
    int i;
    while (isImitation){
        for (i=0; i<=15; i++){
            shm.region->BS_input[i].device.color = 50 + (rand() % static_cast<int>(120 - 50 + 1));;
            shm.region->BS_input[i].device.func  = 20 + (rand() % static_cast<int>(70 - 20 + 1));
            shm.region->BS_input[i].device.state = 0  + (rand() % static_cast<int>(20));
            shm.region->BS_input[i].device.type  = 1  + (rand() % static_cast<int>(13 - 1 + 1));
            shm.region->BS_input[i].trace.count += 1;
        }
        sleep(1);
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

void MainWindow::on_modulesView_activated(const QModelIndex &index)
{
    auto module = shared->region -> BS_input[index.row()];
    ui->name_value->setText(module_types[module.device.type]);

    switch (module.device.type){
        /*default:
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);          //quality
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);    //valid
            for (int i=0; i< 4; i++)
            {
                ui->in_list->setItem(i, 0, new QTableWidgetItem(QString::number(module.DI.data[i])));
            }*/
        case 1://SS-01t
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 2://USB/RS-485
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 3://ID-8k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 4://ID-16k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 5://OD-5k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 6://OD-16k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 7://OA-4k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 8://IA-4k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 9://IA-8k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 10://IF-1k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 11://IF-3k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 12://D-1k
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);

        case 13://БУ ВНА
            ui->opc_value->setText(opc_state[module.DI.sign.opc]);
            ui->valid_value->setText(valid_state[module.DI.sign.valid]);
    }
}
