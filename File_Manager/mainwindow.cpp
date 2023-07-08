#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QTableWidget"
#include <QFileSystemModel>
#include <QString>
#include <QSplitter>
#include <QFileDialog>
#include <QDebug>
#include <Shellapi.h>
#include <windows.h>
#include <QInputDialog>
#include "about.h"
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QString sPath = "C:/";
    {
    dirmodel = new QFileSystemModel(this);
    dirmodel->setRootPath(sPath);
    dirmodel->setFilter(QDir::NoDotAndDotDot |  QDir::AllDirs);
    ui->dirs->setModel(dirmodel);
    ui->dirs->header()->hide();
    ui->dirs->hideColumn(1);
    ui->dirs->hideColumn(2);
    ui->dirs->hideColumn(3);
    ui->dirs->expandAll();
    }

    {
    filemodel = new QFileSystemModel(this);
    filemodel->setRootPath(sPath);
    ui->files->setModel(filemodel);
    ui->files->setRootIndex(filemodel->index(sPath));
    }

    {
    QPushButton* button = new QPushButton(sPath, this);

    QFont font("MS Shell Dlg 2", 8);
    QFontMetrics metrics(font);
    QRect rect = metrics.boundingRect(sPath);

    button->setMaximumWidth(rect.width() + 10);

    QWidget *form = this;
    QHBoxLayout *layout = form->findChild<QHBoxLayout*>("horizontalLayout_4");
    layout->setAlignment(Qt::AlignLeft);

    ui->horizontalLayout_4->addWidget(button);

    connect(button, &QPushButton::clicked, [=]() {
        ui->files->setRootIndex(filemodel->setRootPath(sPath));
    });
    }

    {
    QDir dir = {sPath};
    QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    int count = entries.count();
    ui->statusbar->showMessage("Элементов: " + QString::number(count));
    }
    {
    QShortcut *copyshotcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    connect(copyshotcut, &QShortcut::activated, this, &MainWindow::CopyAction);
    QShortcut *pasteshortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_V), this);
    connect(pasteshortcut, &QShortcut::activated, this, &MainWindow::PasteAction);
    QShortcut *renameshotcut = new QShortcut(QKeySequence(Qt::Key_F2), this);
    connect(renameshotcut, &QShortcut::activated, this, &MainWindow::RenameAction);
    QShortcut *deleteshotcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(deleteshotcut, &QShortcut::activated, this, &MainWindow::DeleteAction);
    QShortcut *newfoldershotcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N), this);
    connect(newfoldershotcut, &QShortcut::activated, this, &MainWindow::NewFolder);
    QShortcut *selectall = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_A), this);
    connect(selectall, &QShortcut::activated, this, &MainWindow::SelectAll);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_files_doubleClicked(const QModelIndex &index) {
    QString sPath = filemodel->fileInfo(index).absoluteFilePath();
    QDir dir = {sPath};

    if (QFileInfo(sPath).isFile()) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(sPath));
    return;
    }

    ui->files->setRootIndex(filemodel->setRootPath(sPath));

    QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    int count = entries.count();
    ui->statusbar->showMessage("Элементов: " + QString::number(count));

    QStringList list = sPath.split("/", Qt::SkipEmptyParts);
    list[0] = list[0].append("/");
    Path_in_layout(sPath, list);
}

void MainWindow::Path_in_layout(QString &sPath, QStringList &list) {
    QLayout* layout1 = ui->horizontalLayout_4;
    QLayoutItem* item;

    while ((item = layout1->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (QString str : list) {
        QPushButton *button = new QPushButton(str, this);

        QFont font("MS Shell Dlg 2", 8);
        QFontMetrics metrics(font);
        QRect rect = metrics.boundingRect(str);

        button->setMaximumWidth(rect.width() + 10);
        button->setMinimumWidth(1);

        ui->horizontalLayout_4->addWidget(button);

        connect(button, &QPushButton::clicked, [=]() {
            QString buttonPath = sPath.mid(0, sPath.indexOf(str) + str.length());

            ui->files->setRootIndex(filemodel->setRootPath(buttonPath));
            QDir dir{buttonPath};
            QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
            int count = entries.count();
            ui->statusbar->showMessage("Элементов: " + QString::number(count));
            QWidget *form = this;
            QHBoxLayout *layout =
                form->findChild<QHBoxLayout *>("horizontalLayout_4");
            int index = layout->indexOf(button);
            QLayoutItem *item;

            while ((item = layout->takeAt(index + 1)) != nullptr) {
                delete item->widget();
                delete item;
            }
        });
    }
}

void MainWindow::on_dirs_clicked(const QModelIndex &index)
{
    QString sPath = dirmodel->fileInfo(index).absoluteFilePath();
    QDir dir = {sPath};

    if (QFileInfo(sPath).isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(sPath));
        return;
    }

    ui->files->setRootIndex(filemodel->setRootPath(sPath));

    QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    int count = entries.count();
    ui->statusbar->showMessage("Элементов: " + QString::number(count));

    QStringList list = sPath.split("/",Qt::SkipEmptyParts);
    list[0] = list[0].append("/");
    Path_in_layout(sPath, list);
}

void MainWindow::on_pushButton_clicked() {
    QString path = filemodel->rootPath();
    QDir dir {path};
    path_to_forward.push(path);
    dir.cdUp();
    ui->files->setRootIndex(filemodel->setRootPath(dir.path()));
    path = dir.path();



    QStringList list = path.split("/",Qt::SkipEmptyParts);
    list[0] = list[0].append("/");
    Path_in_layout(path, list);
    QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    int count = entries.count();
    ui->statusbar->showMessage("Элементов: " + QString::number(count));
}

void MainWindow::on_pushButton_2_clicked() {
    if(!path_to_forward.isEmpty()){
        QString path = path_to_forward.pop();
        ui->files->setRootIndex(filemodel->setRootPath(path));

        QStringList list = path.split("/",Qt::SkipEmptyParts);
        list[0] = list[0].append("/");
        Path_in_layout(path, list);
        QDir dir = {path};
        QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
        int count = entries.count();
        ui->statusbar->showMessage("Элементов: " + QString::number(count));
    }
}

void MainWindow::on_pushButton_3_clicked() {
    QString path = filemodel->rootPath();
    QDir dir {path};
    if(!dir.isRoot()){
        while(!dir.isRoot()){
            dir.cdUp();
        }
        ui->files->setRootIndex(filemodel->setRootPath(dir.path()));
        QStringList list;
        foreach (QString l, list) {
        }
        list.append(dir.path());
        path = dir.path();
        Path_in_layout(path, list);
        QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
        int count = entries.count();
        ui->statusbar->showMessage("Элементов: " + QString::number(count));
    }
}

void MainWindow::on_files_customContextMenuRequested(const QPoint &pos) {
    QModelIndex index = ui->files->indexAt(pos);
    QString filePath = filemodel->filePath(index);
    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();

    if (extension == "zip" || extension == "rar" || extension == "7z" || extension == "tar" || extension == "gz"){
        QMenu *menu = new QMenu(this);
        QAction *Copy =menu->addAction("Копировать");
        QAction *Paste = menu->addAction("Вставить");
        QAction *Delete = menu->addAction("Удалить");
        QAction *Rename = menu->addAction("Переименовать");
        QAction *Extract = menu->addAction("Извлечь");
        connect(Copy, SIGNAL(triggered()), this, SLOT(CopyAction()));
        connect(Paste, SIGNAL(triggered()), this, SLOT(PasteAction()));
        connect(Delete, SIGNAL(triggered()), this, SLOT(DeleteAction()));
        connect(Rename, SIGNAL(triggered()), this, SLOT(RenameAction()));
        connect(Extract, SIGNAL(triggered()), this, SLOT(ExtractAction()));
        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(OnContextMenuSelected(QAction*)));
        menu->popup(ui->files->viewport()->mapToGlobal(pos));
    }
    else if(extension == "exe" || extension == "url" || extension == "lnk"){
        QMenu *menu = new QMenu(this);
        QAction *Open = menu->addAction("Открыть");
        QAction *Delete = menu->addAction("Удалить");
        QAction *Rename = menu->addAction("Переименовать");
        connect(Open, SIGNAL(triggered()), this, SLOT(OpenAction()));
        connect(Delete, SIGNAL(triggered()), this, SLOT(DeleteAction()));
        connect(Rename, SIGNAL(triggered()), this, SLOT(RenameAction()));
        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(OnContextMenuSelected(QAction*)));
        menu->popup(ui->files->viewport()->mapToGlobal(pos));
    }
    else {
        QMenu *menu = new QMenu(this);
        QAction *Copy =menu->addAction("Копировать");
        QAction *Paste = menu->addAction("Вставить");
        QAction *Delete = menu->addAction("Удалить");
        QMenu *submenu = new QMenu("Создать");
        menu->addMenu(submenu);
        QAction *NewFile = submenu->addAction("Создай файл");
        QAction *NewFolder = submenu->addAction("Создать папку");
        QAction *Rename = menu->addAction("Переименовать");
        QAction *Archive = menu->addAction("Заархивировать");
        connect(Copy, SIGNAL(triggered()), this, SLOT(CopyAction()));
        connect(Paste, SIGNAL(triggered()), this, SLOT(PasteAction()));
        connect(Delete, SIGNAL(triggered()), this, SLOT(DeleteAction()));
        connect(Rename, SIGNAL(triggered()), this, SLOT(RenameAction()));
        connect(NewFile, SIGNAL(triggered()), this, SLOT(NewFile()));
        connect(NewFolder, SIGNAL(triggered()), this, SLOT(NewFolder()));
        connect(Archive, SIGNAL(triggered()), this, SLOT(CreateArchiveAction()));
        connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(OnContextMenuSelected(QAction*)));
        menu->popup(ui->files->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::OpenAction(){
    QModelIndex index = ui->files->currentIndex();
    QString sPath = dirmodel->fileInfo(index).absoluteFilePath();
    QDir dir = {sPath};

    if (QFileInfo(sPath).isFile()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(sPath));
        return;
    }
}

void MainWindow::CopyAction() {
    CopyList.clear();
    QString path = filemodel->rootPath();
    QList<QVariant> datalist;
    QList<QString> paths;

    QItemSelectionModel *selectionModel = ui->files->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    foreach (QModelIndex index, selectedIndexes) {
        if (index.isValid()) {
            QVariant data = ui->files->model()->data(index);
            datalist.append(data);
        }
    }
    foreach (QVariant data, datalist) {
        QDir parentDir(path);
        QString fullPath = parentDir.absoluteFilePath(data.toString());
        paths.append(fullPath);
    }
    CopyList = paths;

}

void MainWindow::PasteAction() {
    QModelIndex rootIndex = filemodel->index(filemodel->rootPath());
    QString path = filemodel->rootPath();

    foreach (QString path, CopyList) {
        QFileInfo fileInfo(path);
        QString destPath = filemodel->filePath(rootIndex) + QDir::separator() + fileInfo.fileName();

        if (fileInfo.isDir()) {
            if (!QDir(destPath).mkpath(".")) {
                continue;
            }

            QDirIterator it(path, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                it.next();
                QString subFilePath = it.filePath();
                QString newSubFilePath = destPath + QDir::separator() + it.fileName();
                if (it.fileInfo().isDir()) {
                    if (!QDir().mkpath(newSubFilePath)) {
                        continue;
                    }
                } else {
                    if (!QFile::copy(subFilePath, newSubFilePath)) {
                            continue;
                    }
                }
            }
        } else {
            if (!QFile::copy(path, destPath)) {
                continue;
            }
        }
    }
    QDir dir = {path};
    QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    int count = entries.count();
    ui->statusbar->showMessage("Элементов: " + QString::number(count));
}

void MainWindow::DeleteAction() {
    QString path = filemodel->rootPath();
    QList<QVariant> datalist;
    QList<QString> paths;

    QItemSelectionModel *selectionModel = ui->files->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();

    foreach (QModelIndex index, selectedIndexes) {
        if (index.isValid()) {
            QVariant data = ui->files->model()->data(index);
            datalist.append(data);
        }
    }
    foreach (QVariant data, datalist) {
        QDir parentDir(path);
        QString fullPath = parentDir.absoluteFilePath(data.toString());
        paths.append(fullPath);
    }

    foreach (QString path, paths) {
        QFileInfo info(path);
        if (info.isDir()) {
            QDir dir(path);
            if (dir.removeRecursively()) {
            } else {
            }
        } else if (info.isFile()) {
            QFile file(path);
            if (file.remove()) {
            } else {
            }
        }
    }

    QDir dir = {path};
    QStringList entries = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
    int count = entries.count();
    ui->statusbar->showMessage("Элементов: " + QString::number(count));
}

void MainWindow::NewFile() {
    QString path = filemodel->rootPath();
    QString fileName = QInputDialog::getText(this, "Create file", "File name");
    if (!fileName.isEmpty()) {
        QFile file(path + "/" + fileName);
        if (file.open(QIODevice::NewOnly)) {
        } else {
        }
    }
}

void MainWindow::NewFolder() {
    QString dirPath = filemodel->rootPath();
    QString dirName = QInputDialog::getText(this, "Create directory", "Directory name");
    if (!dirName.isEmpty()) {
        QDir dir;
        if (dir.mkdir(dirPath + "/" + dirName)) {
        } else {
        }
    }
}

void MainWindow::RenameAction() {
    QString dirPath = filemodel->rootPath();
    QString NewName = QInputDialog::getText(this, "Rename file or directory", "New name");

    QModelIndex index = ui->files->currentIndex();
    QString oldFile = dirmodel->fileInfo(index).absoluteFilePath();
    QFile file(oldFile);
    if (!NewName.isEmpty()) {
        QString newFileName = dirPath + "/" + NewName;
        file.rename(newFileName);
    }
}

void MainWindow::ExtractAction() {
    QModelIndex index = ui->files->currentIndex();
    QString archiveFilePath = filemodel->filePath(index);
    QString directory = QFileInfo(archiveFilePath).absolutePath();
    // Создаем папку с именем архива (без расширения)
    QString archiveName = QFileInfo(archiveFilePath).baseName();
    QString extractPath = directory + QDir::separator() + archiveName;
    // Создаем процесс для выполнения команды архиватора
    QProcess process;
    // Получаем команду распаковки архива для текущей операционной системы
    QString extractCommand;
#ifdef Q_OS_WIN
    extractCommand = "tar";
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
    extractCommand = "tar";
#else
    return;
#endif
    // Устанавливаем аргументы командной строки для распаковки архива
    QStringList arguments;    arguments << "-xf"; // Распаковать архив
    arguments << archiveFilePath; // Путь к архиву
    // Устанавливаем рабочую директорию для процесса
    process.setWorkingDirectory(directory);
    // Запускаем процесс архиватора
    process.start(extractCommand, arguments);
    // Ждем завершения процесса
    if (!process.waitForFinished(-1)) {
        return;
    }
}

void MainWindow::CreateArchiveAction() {
    // Получаем список выбранных элементов в ListView
    QModelIndexList selectedIndexes = ui->files->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) {
        return;
    }

    // Получаем путь к директории, в которой находятся выбранные файлы
    QString directory = QFileInfo(filemodel->filePath(selectedIndexes.first())).absolutePath();

    // Создаем папку для архива
    QString archiveName = QInputDialog::getText(this, "Add to archive", "Archive name");
    QString archivePath = directory + QDir::separator() + archiveName + ".tar";

    // Создаем процесс для выполнения команды архиватора
    QProcess process;

    // Получаем команду архивации для текущей операционной системы
    QString archiveCommand;
#ifdef Q_OS_WIN
    archiveCommand = "tar";
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
    archiveCommand = "tar";
#else
    return;
#endif

    // Устанавливаем аргументы командной строки для архивации файлов
    QStringList arguments;
    arguments << "-cf";  // Создать архив
    arguments << archivePath;  // Путь к архиву
    foreach (const QModelIndex& index, selectedIndexes) {
        QString filePath = filemodel->filePath(index);
        arguments << "-C" << directory << QFileInfo(filePath).fileName();  // Добавить выбранный файл в архив
    }

    // Устанавливаем рабочую директорию для процесса
    process.setWorkingDirectory(directory);

    // Запускаем процесс архиватора
    process.start(archiveCommand, arguments);

    // Ждем завершения процесса
    if (!process.waitForFinished(-1)) {
        return;
    }
}

void MainWindow::SelectAll() {
    ui->files->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->files->selectAll();
    ui->files->show();
    ui->files->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void MainWindow::on_about_programm_action_triggered() {
    about *aboutWindow = new about(this);
    aboutWindow->show();
}

void MainWindow::on_actionDark_Mode_triggered() {
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(77, 77, 77));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}

void MainWindow::on_actionLight_Mode_triggered() {
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::WindowText, Qt::black);
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    lightPalette.setColor(QPalette::ToolTipBase, Qt::white);
    lightPalette.setColor(QPalette::ToolTipText, Qt::black);
    lightPalette.setColor(QPalette::Text, Qt::black);
    lightPalette.setColor(QPalette::Button, QColor(220, 220, 220));
    lightPalette.setColor(QPalette::ButtonText, Qt::black);
    lightPalette.setColor(QPalette::BrightText, Qt::red);
    lightPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    lightPalette.setColor(QPalette::Highlight, QColor(77, 77, 77));
    lightPalette.setColor(QPalette::HighlightedText, Qt::white);
    qApp->setPalette(lightPalette);
    qApp->setStyleSheet("QToolTip { color: #000000; background-color: #ffffff; border: 1px solid black; }");
}

void MainWindow::on_action_triggered()
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette purplePalette;
    purplePalette.setColor(QPalette::Window, QColor(120, 81, 169));
    purplePalette.setColor(QPalette::WindowText, Qt::white);
    purplePalette.setColor(QPalette::Base, QColor(140, 101, 189));
    purplePalette.setColor(QPalette::AlternateBase, QColor(150, 111, 199));
    purplePalette.setColor(QPalette::ToolTipBase, Qt::white);
    purplePalette.setColor(QPalette::ToolTipText, Qt::black);
    purplePalette.setColor(QPalette::Text, Qt::white);
    purplePalette.setColor(QPalette::Button, QColor(160, 121, 209));
    purplePalette.setColor(QPalette::ButtonText, Qt::white);
    purplePalette.setColor(QPalette::BrightText, Qt::red);
    purplePalette.setColor(QPalette::Link, QColor(42, 130, 218));
    purplePalette.setColor(QPalette::Highlight, QColor(77, 77, 77));
    purplePalette.setColor(QPalette::HighlightedText, Qt::white);
    qApp->setPalette(purplePalette);
    qApp->setStyleSheet("QToolTip { color: #000000; background-color: #ffffff; border: 1px solid black; }");
}

