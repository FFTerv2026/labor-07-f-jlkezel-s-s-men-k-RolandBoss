#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browse();
    void find();
    void openFileOfItem(int row, int column);

private:
    void createUI();
    void findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);

    QComboBox *fileComboBox;
    QComboBox *textComboBox;
    QComboBox *directoryComboBox;
    QTableWidget *filesTable;
    QLabel *filesFoundLabel;
    QPushButton *findButton;
};

#endif // MAINWINDOW_H
