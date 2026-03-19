#include "mainwindow.h"
#include <QtWidgets>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), lastIndex(-1) {
    setWindowTitle("Napi Motiváció");
    resize(500, 350);

    // Felület felépítése
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    quoteLabel = new QLabel("Töltsd be az idézeteket...");
    quoteLabel->setWordWrap(true);
    quoteLabel->setAlignment(Qt::AlignCenter);

    newQuoteButton = new QPushButton("Új Idézet Generálása");

    layout->addStretch();
    layout->addWidget(quoteLabel);
    layout->addStretch();
    layout->addWidget(newQuoteButton);

    // --- STYLESHEET (Díszítés) ---
    this->setStyleSheet("QMainWindow { background-color: #f0f2f5; }");

    quoteLabel->setStyleSheet(
        "QLabel { "
        "   font-size: 22px; "
        "   font-style: italic; "
        "   color: #2c3e50; "
        "   padding: 20px; "
        "   font-family: 'Georgia', serif; "
        "}"
        );

    newQuoteButton->setStyleSheet(
        "QPushButton { "
        "   background-color: #3498db; "
        "   color: white; "
        "   border-radius: 10px; "
        "   padding: 12px; "
        "   font-size: 16px; "
        "   font-weight: bold; "
        "   border: none; "
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:pressed { background-color: #1c5980; }"
        );

    // Funkciók
    loadQuotes();
    connect(newQuoteButton, &QPushButton::clicked, this, &MainWindow::showRandomQuote);

    // Kezdő idézet megjelenítése
    showRandomQuote();
}

void MainWindow::loadQuotes() {
    QFile file("quotes.txt");
    // Megjegyzés: A fájlnak a futtatható .exe mellett (vagy a shadow build mappában) kell lennie.
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                quotes << line;
            }
        }
        file.close();
    }

    if (quotes.isEmpty()) {
        quotes << "Hiba: A quotes.txt fájl nem található vagy üres!";
    }
}

void MainWindow::showRandomQuote() {
    if (quotes.size() < 2) {
        if (!quotes.isEmpty()) quoteLabel->setText(quotes[0]);
        return;
    }

    int newIndex;
    // Addig generálunk újat, amíg meg nem egyezik az előzővel
    do {
        newIndex = QRandomGenerator::global()->bounded(quotes.size());
    } while (newIndex == lastIndex);

    lastIndex = newIndex;
    quoteLabel->setText(quotes[newIndex]);
}
