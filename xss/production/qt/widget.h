#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui>
#include <QWidget>
#include <QVariant>
#include <string>

#include "xkpplacementlayout.h"

using namespace std;

namespace Ui {
    class Widget;
}

class Widget : public QWidget {
    Q_OBJECT
	
public:
    Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void changeEvent(QEvent *e);

private:
	XKPPlacementLayout *addLayout();

	QList<XKPPlacementLayout *> layouts;
    Ui::Widget *ui;

	string id;
	int width;
	int height;
	int xxx;
	int _target;

private slots:
	void on_pushButton_1_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_4_clicked();
	void on_pushButton_5_clicked();
	void target_set(int value);
	int target_get(void);
	void reset(string s);
	int returning_stuff();
	string declaring_error();
	int declaring_ok(int value);
	int returned_real();
};

#endif // WIDGET_H

