#include "widget.h"
#include <QtWidgets>

Widget::Widget(QWidget *parent) : QWidget(parent) {
    // Felület felépítése
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *topLayout = new QGridLayout();

    topLayout->addWidget(new QLabel("Named:"), 0, 0);
    fileComboBox = new QLineEdit("*");
    topLayout->addWidget(fileComboBox, 0, 1, 1, 2);

    topLayout->addWidget(new QLabel("Containing text:"), 1, 0);
    textComboBox = new QLineEdit();
    topLayout->addWidget(textComboBox, 1, 1, 1, 2);

    topLayout->addWidget(new QLabel("In directory:"), 2, 0);
    directoryComboBox = new QLineEdit(QDir::currentPath());
    topLayout->addWidget(directoryComboBox, 2, 1);

    QPushButton *browseButton = new QPushButton("Browse...");
    connect(browseButton, &QPushButton::clicked, this, &Widget::browse);
    topLayout->addWidget(browseButton, 2, 2);

    filesTable = new QTableWidget(0, 2);
    filesTable->setHorizontalHeaderLabels({"Filename", "Size"});
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    findButton = new QPushButton("Find");
    connect(findButton, &QPushButton::clicked, this, &Widget::find);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(filesTable);
    mainLayout->addWidget(findButton, 0, Qt::AlignRight);

    setWindowTitle("Find Files");
    resize(600, 400);
}

void Widget::browse() {
    QString directory = QFileDialog::getExistingDirectory(this, "Select Directory", directoryComboBox->text());
    if (!directory.isEmpty()) {
        directoryComboBox->setText(directory);
    }
}

void Widget::find() {
    filesTable->setRowCount(0);
    QString fileName = fileComboBox->text();
    QString text = textComboBox->text();
    QString path = directoryComboBox->text();

    QDir directory(path);
    QStringList files;

    // Fájlok listázása a név alapján (pl. *.txt)
    QDirIterator it(path, QStringList() << fileName, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        files << it.next();
    }

    if (text.isEmpty()) {
        showFiles(files);
        return;
    }

    // Szűrés tartalom alapján ProgressDialog-gal
    QStringList foundFiles;
    QProgressDialog progress("Searching files...", "Cancel", 0, files.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    for (int i = 0; i < files.size(); ++i) {
        progress.setValue(i);
        if (progress.wasCanceled()) break;

        QFile file(files[i]);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            if (in.readAll().contains(text, Qt::CaseInsensitive)) {
                foundFiles << files[i];
            }
        }
    }
    progress.setValue(files.size());
    showFiles(foundFiles);
}

void Widget::showFiles(const QStringList &files) {
    for (const QString &filePath : files) {
        QFileInfo fileInfo(filePath);
        int row = filesTable->rowCount();
        filesTable->insertRow(row);

        filesTable->setItem(row, 0, new QTableWidgetItem(fileInfo.fileName()));
        filesTable->setItem(row, 1, new QTableWidgetItem(QString::number(fileInfo.size() / 1024) + " KB"));
    }
}
