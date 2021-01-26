#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QAbstractTableModel>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ModulesTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ModulesTableModel(QObject *parent = nullptr){};
    int rowCount(const QModelIndex &parent = QModelIndex())     const override
    {
        return 16;
    }
    int columnCount(const QModelIndex &parent = QModelIndex())  const override
    {
        return 4;
    }
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole)                       const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    template<typename Tin,typename Tout, typename Tmod>
    void fill_IO(Tin, Tout, Tmod);

private slots:

    void on_updateListButton_clicked();

    void on_create_shm_triggered();

    void on_aboutQt_triggered()
    {
        QMessageBox::aboutQt(this, "About Qt");
    }

    void on_bsList_currentIndexChanged(const QString &arg1);

    void on_bsList_activated(int index);

    void on_modulesView_activated(const QModelIndex &index);

    void on_dark_theme_toggled(bool arg1);

    void on_app_exit_triggered()
    {
        QApplication::exit();
    }

    void on_imitationBox_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
