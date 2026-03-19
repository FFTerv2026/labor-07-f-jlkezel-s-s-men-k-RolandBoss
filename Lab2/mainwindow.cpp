#include "mainwindow.h"
#include <QTableWidget>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("CSV Olvasó");
    resize(800, 600);

    // --- 1. Táblázat létrehozása és beállítása ---
    tableWidget = new QTableWidget(this);
    // Beállítjuk, hogy a táblázat elemei ne legyenek módosíthatók
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // A QMainWindow közepére tesszük a táblázatot
    setCentralWidget(tableWidget);

    // --- 2. Menüsáv (QMenuBar) felépítése ---
    QMenu *fileMenu = menuBar()->addMenu("File");

    QAction *openAction = new QAction("Open File...", this);
    // Gyorsbillentyű hozzárendelése (opcionális, de hasznos)
    openAction->setShortcut(QKeySequence::Open);

    fileMenu->addAction(openAction);

    // A menüpont összekötése az openFile() függvénnyel
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile()
{
    // Állománymegnyitó dialógus, a filter csak a .csv fájlokat engedi
    QString fileName = QFileDialog::getOpenFileName(this, "CSV Fájl Megnyitása", "", "CSV Files (*.csv)");

    // Ha a felhasználó a "Mégse" gombra nyomott, a fileName üres lesz
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    // Fájl megnyitása csak olvasásra, szöveges módban
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Hiba", "Nem sikerült megnyitni a fájlt!");
        return;
    }

    QTextStream in(&file);

    // Előző adatok törlése a táblázatból
    tableWidget->clear();
    tableWidget->setRowCount(0);
    tableWidget->setColumnCount(0);

    bool isFirstLine = true;

    // Fájl beolvasása sorról sorra
    while (!in.atEnd()) {
        QString line = in.readLine();

        // Üres sorok átugrása
        if (line.trimmed().isEmpty()) {
            continue;
        }

        // A CSV sor felbontása (feltételezve, hogy vessző az elválasztó)
        QStringList fields = line.split(",");

        if (isFirstLine) {
            // Első sor feldolgozása: Oszlopok száma és fejlécek beállítása
            tableWidget->setColumnCount(fields.size());
            tableWidget->setHorizontalHeaderLabels(fields);
            isFirstLine = false;
        } else {
            // További sorok feldolgozása: Adatok beszúrása
            int currentRow = tableWidget->rowCount();
            tableWidget->insertRow(currentRow);

            for (int i = 0; i < fields.size(); ++i) {
                QTableWidgetItem *item = new QTableWidgetItem(fields[i]);
                tableWidget->setItem(currentRow, i, item);
            }
        }
    }

    file.close();
}
