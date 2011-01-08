#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
    class Widget;
}

// remember to personalize the name of this class
class Widget : public QWidget {
    Q_OBJECT
public:
    Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Widget *ui;

private slots:
	// remember to implement the returned type of events
        void on_pushButton_1_clicked();
        void on_pushButton_2_clicked();
};

#endif // WIDGET_H


