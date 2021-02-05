//{
#include <fstream>
#include <thread>
#include <unistd.h>
#include <typeinfo>

#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QRandomGenerator>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shared.h"
#include "consts.h"
//}

SharedMem *shared;
QSettings *settings    = new QSettings("bsio.conf", QSettings::NativeFormat);
bool       isImitation = false;
int        currMod     = 0;
module    *activeModInfo;
int counter = 0;

//-------------------Всё что касается окна в целом и то, что можно уже не трогать--------------------//
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{   
    ui->setupUi(this);
    bool darktheme = settings->value("theme/darkmode",false).toBool();
    bool debugInfo = settings->value("dev/debugInfo" ,false).toBool();
    ui->dark_theme->setChecked(darktheme);
    ui->debInfView->setChecked(debugInfo);
    on_debInfView_toggled(debugInfo);
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
void MainWindow::debugInfoUpdate()
{
    if (isImitation)
        ui->debugInfo->setPalette(QColor(255,255,0));
    else
        ui->debugInfo->setPalette(QColor(255,0,255));

    ui->l1->setText(QString("Active m_pos: %1").arg(QString::number(currMod)));
    ui->l2->setText(QString("index.row(): %1").arg(QString::number(ui->modulesView->currentIndex().row())));
    ui->l3->setText(QString(""));
    ui->l4->setText(QString(""));
    ui->l5->setText(QString("UI updates: %1").arg(counter));
}
void MainWindow::on_debInfView_toggled(bool isActive)
{
    if(isActive){
        ui->debugInfo->setVisible(true);
    } else
        ui->debugInfo->setVisible(false);
    settings->setValue("dev/debugInfo",isActive);
    settings->sync();
}

void MainWindow::on_bsList_currentIndexChanged(const QString &currentValue)
{
    ui->statusbar->showMessage("Выбран "+currentValue);
}

void MainWindow::on_updateUI_triggered()
{
    debugInfoUpdate();
    ui->modulesView->reset();
    ui->modulesView->resizeColumnsToContents();
    if (ui->modulesView->currentIndex().row()==-1)
        ui->modulesView->selectRow(currMod);
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
    ui->modulesView->reset();
    ui->modulesView->resizeColumnsToContents();
}

void imitation()
{
    QRandomGenerator *rg = QRandomGenerator::global();
    SharedMem shm("/bs1");
    int i;
    while (isImitation){
        for (i=0; i<=15; i++){
            shm.region->BS_input[i].trace.count += 1;
            shm.region->BS_input[i].device.state = rg->bounded(0,20);
            shm.region->BS_input[i].device.type  = rg->bounded(1,13);
            shm.region->BS_input[i].AI.data[0]   = rg->bounded(0,100);
            shm.region->BS_input[i].AI.data[1]   = rg->bounded(0,100);
            shm.region->BS_input[i].AI.data[2]   = rg->bounded(0,100);
            shm.region->BS_input[i].AI.data[3]   = rg->bounded(0,100);
            shm.region->BS_input[i].DI.data[0]   = rg->bounded(0,2);
            shm.region->BS_input[i].DI.data[1]   = rg->bounded(0,2);
            shm.region->BS_input[i].DI.data[2]   = rg->bounded(0,2);
            shm.region->BS_input[i].DI.data[3]   = rg->bounded(0,2);
            shm.region->BS_input[i].AO.data[0]   = rg->bounded(0,100);
            shm.region->BS_input[i].AO.data[1]   = rg->bounded(0,100);
            shm.region->BS_input[i].AO.data[2]   = rg->bounded(0,100);
            shm.region->BS_input[i].AO.data[3]   = rg->bounded(0,100);
            shm.region->BS_input[i].DO.data[0]   = rg->bounded(0,2);
            shm.region->BS_input[i].DO.data[1]   = rg->bounded(0,2);
            shm.region->BS_input[i].DO.data[2]   = rg->bounded(0,2);
            shm.region->BS_input[i].DO.data[3]   = rg->bounded(0,2);
        }
        sleep(1);
    }
    shm.~SharedMem();
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
    while(isImitation){
        counter++;
        ui->updateUI->triggered();
        QApplication::processEvents();
        //sleep(1);
    }
}
template<typename Tin,typename Tout>
void MainWindow::fill_IO(Tin in, Tout out, bool isD)
{
    ui->valid_value->setText(valid_state[in.sign.valid]);
    ui->  opc_value->setText(  opc_state[in.sign.opc]);

    int  _count = activeModInfo->iCount;
    auto* _list = ui->in_list;
    auto  _data = in.data;

    for (int c=0;c<2;c++){
        for (int i=0;i<_count;i++) {
            _list->setItem(i,0,new QTableWidgetItem(QString::number(_data[i])));
            _list->item(i,0)->setTextAlignment(Qt::AlignCenter);
            if (isD){
                _list->item(i,0)->setTextColor(Qt::black);
                (bool)_data[i]?
                            _list->item(i,0)->setBackgroundColor(Qt::green):
                            _list->item(i,0)->setBackgroundColor(Qt::darkGreen);
            }
        }
        _count = activeModInfo->oCount;
        _list  = ui->out_list;
        _data  = out.data;
    }
}
void MainWindow::on_modulesView_activated(const QModelIndex &index)
{
    currMod = index.row();
    auto modInData  = &shared->region->BS_input[currMod];
    auto modOutData = &shared->region->BS_input[currMod]; //BS_output?
    activeModInfo   = &modules[modInData->device.type];

    ui->name_value->setText(activeModInfo->type);
    ui->type_value->setText(activeModInfo->iotype);
    ui-> out_list ->setRowCount(activeModInfo->oCount);
    ui->  in_list ->setRowCount(activeModInfo->iCount);

    if (activeModInfo->iotype == "A")
        MainWindow::fill_IO(modInData->AI, modOutData->AO, false);
    else if (activeModInfo->iotype == "D")
        MainWindow::fill_IO(modInData->DI, modOutData->DO, true);

    ui->updateUI->triggered();
}

void MainWindow::on_out_list_itemChanged(QTableWidgetItem *item)
{
    if (item-> isSelected()){
        auto val = item->text().toShort();
        auto channel  = item->row();
        item->setSelected(false);


        if (activeModInfo->iotype == "A")
            shared->region->BS_output[currMod].AO_MAN.data[channel] = val;
        else if (activeModInfo->iotype == "D")
            shared->region->BS_output[currMod].DO_MAN.data[channel] = val;

        ui->modulesView->selectRow(currMod);
        auto ind = ui->modulesView->currentIndex();
        ui->modulesView->activated(ind);

        QString msg = QString("Entered value '%1' to %2 channel in %3 module.")
                .arg(val)
                .arg(channel+1)
                .arg(activeModInfo->type);
        ui->statusbar->showMessage(msg);
    }
}


