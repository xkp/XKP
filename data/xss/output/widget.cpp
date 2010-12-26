#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


				void Widget::on_pushButton_1_clicked()
				{
				pushButton_2.setText("Clicked");
	caption = "Other Clicked";
	pushButton_2.resize(pushButton_2.width() + pushButton_1.width(), pushButton_2.height());

				}
			
				void Widget::on_pushButton_2_clicked()
				{
				pushButton_1.setText("Old value: " + application.target);
	application.target_set(300);

				}
			
