#include "mainwindow.h"
#include <QtWidgets>
#include <QDirIterator>
#include <QProgressDialog>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createUI();
    setWindowTitle("Find Files");
    resize(700, 500);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QGridLayout *topLayout = new QGridLayout();

    // -- 1. Sor: Named --
    topLayout->addWidget(new QLabel("Named:"), 0, 0);
    fileComboBox = new QComboBox();
    fileComboBox->setEditable(true);
    fileComboBox->addItem("*");
    fileComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topLayout->addWidget(fileComboBox, 0, 1, 1, 2);

    // -- 2. Sor: Containing text --
    topLayout->addWidget(new QLabel("Containing text:"), 1, 0);
    textComboBox = new QComboBox();
    textComboBox->setEditable(true);
    textComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topLayout->addWidget(textComboBox, 1, 1, 1, 2);

    // -- 3. Sor: In directory --
    topLayout->addWidget(new QLabel("In directory:"), 2, 0);
    directoryComboBox = new QComboBox();
    directoryComboBox->setEditable(true);
    directoryComboBox->addItem(QDir::currentPath());
    directoryComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topLayout->addWidget(directoryComboBox, 2, 1);

    QPushButton *browseButton = new QPushButton("Browse...");
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::browse);
    topLayout->addWidget(browseButton, 2, 2);

    // -- Táblázat --
    filesTable = new QTableWidget(0, 2);
    filesTable->setHorizontalHeaderLabels(QStringList() << "Filename" << "Size");
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    filesTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Csak olvasható
    connect(filesTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::openFileOfItem);

    // -- Alsó sáv --
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    filesFoundLabel = new QLabel("0 file(s) found (Double click on a file to open it)");
    findButton = new QPushButton("Find");
    connect(findButton, &QPushButton::clicked, this, &MainWindow::find);

    bottomLayout->addWidget(filesFoundLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(findButton);

    // -- Layoutok összerakása --
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(filesTable);
    mainLayout->addLayout(bottomLayout);
}

void MainWindow::browse()
{
    // A feladat kéri a QFileDialog használatát
    QString directory = QFileDialog::getExistingDirectory(this, "Select Directory", directoryComboBox->currentText());
    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}

void MainWindow::find()
{
    filesTable->setRowCount(0); // Táblázat ürítése

    QString fileName = fileComboBox->currentText();
    QString text = textComboBox->currentText();
    QString path = directoryComboBox->currentText();

    if (fileName.isEmpty())
        fileName = "*";

    // 1. lépés: Könyvtár bejárása és fájlok összegyűjtése név alapján
    QDir directory(path);
    QStringList nameFilters;
    nameFilters << fileName;

    QStringList collectedFiles;
    // A feladat kéri a QDir és QDirIterator használatát
    QDirIterator it(path, nameFilters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        collectedFiles << it.next();
    }

    // 2. lépés: Keresés a tartalom alapján
    findFiles(collectedFiles, text);
}

void MainWindow::findFiles(const QStringList &files, const QString &text)
{
    QStringList foundFiles;

    // Ha nincs megadva keresendő szöveg, minden fájlt visszaadunk
    if (text.isEmpty()) {
        foundFiles = files;
    } else {
        // A feladat kéri a QProgressDialog használatát
        QProgressDialog progressDialog("Searching files...", "Cancel", 0, files.size(), this);
        progressDialog.setWindowModality(Qt::WindowModal);
        progressDialog.setMinimumDuration(0); // Azonnal jelenjen meg

        for (int i = 0; i < files.size(); ++i) {
            progressDialog.setValue(i);

            // Ha a felhasználó rányom a Cancel-re, megszakítjuk
            if (progressDialog.wasCanceled())
                break;

            // A feladat kéri a QFile és QTextStream használatát
            QFile file(files.at(i));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                QString content = in.readAll();

                // Keresés a fájl tartalmában (kis/nagybetű független)
                if (content.contains(text, Qt::CaseInsensitive)) {
                    foundFiles << files.at(i);
                }
                file.close();
            }
        }
        progressDialog.setValue(files.size());
    }

    // Eredmények megjelenítése
    showFiles(foundFiles);
}

void MainWindow::showFiles(const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        // A feladat kéri a QFileInfo használatát
        QFileInfo fileInfo(files.at(i));

        int row = filesTable->rowCount();
        filesTable->insertRow(row);

        // Fájlnév oszlop
        QTableWidgetItem *fileNameItem = new QTableWidgetItem(fileInfo.fileName());
        // Eltároljuk a teljes elérési utat, hogy dupla kattintáskor meg tudjuk nyitni
        fileNameItem->setData(Qt::UserRole, fileInfo.absoluteFilePath());

        // Méret oszlop (KB-ba számolva, minimum 1 KB a képi példa alapján)
        qint64 sizeKB = (fileInfo.size() + 1023) / 1024;
        if (sizeKB == 0 && fileInfo.size() > 0) sizeKB = 1; // Ha van benne valami, legyen 1 KB
        QTableWidgetItem *sizeItem = new QTableWidgetItem(QString::number(sizeKB) + " KB");
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, sizeItem);
    }

    filesFoundLabel->setText(QString("%1 file(s) found (Double click on a file to open it)").arg(files.size()));
}

void MainWindow::openFileOfItem(int row, int /* column */)
{
    QTableWidgetItem *item = filesTable->item(row, 0);
    if (item) {
        QString filePath = item->data(Qt::UserRole).toString();
        // Fájl megnyitása az operációs rendszer alapértelmezett programjával
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}
