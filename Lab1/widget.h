#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

private slots:
    void browse();
    void find();

private:
    void showFiles(const QStringList &files);
    QLineEdit *fileComboBox; // A példakép alapján ComboBox is lehetne, de a LineEdit egyszerűbb
    QLineEdit *textComboBox;
    QLineEdit *directoryComboBox;
    QTableWidget *filesTable;
    QPushButton *findButton;
};

#endif
