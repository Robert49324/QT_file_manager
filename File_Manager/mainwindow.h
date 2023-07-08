#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QtCore>
#include <QtGui>
#include <QFileSystemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_files_doubleClicked(const QModelIndex &index);

    void Path_in_layout(QString &sPath, QStringList &list);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_files_customContextMenuRequested(const QPoint &pos);

    void CopyAction();

    void PasteAction();

    void DeleteAction();

    void RenameAction();

    void ExtractAction();

    void OpenAction();

    void NewFile();

    void NewFolder();

    void SelectAll();

    void CreateArchiveAction();

    void on_about_programm_action_triggered();

    void on_actionDark_Mode_triggered();

    void on_actionLight_Mode_triggered();

    void on_dirs_clicked(const QModelIndex &index);

    void on_action_triggered();

private:
    Ui::MainWindow *ui;
    QFileSystemModel *dirmodel;
    QFileSystemModel *filemodel;
    QStack<QString> path_to_forward;
    QList<QString> CopyList;

};
#endif // MAINWINDOW_H
