#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QStringList>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void showRandomQuote();

private:
    void loadQuotes();

    QLabel *quoteLabel;
    QPushButton *newQuoteButton;
    QStringList quotes;
    int lastIndex; // Az előző idézet indexe az ismétlés elkerüléséhez
};

#endif
