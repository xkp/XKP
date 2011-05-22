#include "widget.h"
#include "ui_app.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    XKPPlacementLayout *layout;

	layout = addLayout();
	ui->pushButton_1->setProperty(placementName, XKPPlacementLayout::None);
    layout->addWidget(ui->pushButton_1, XKPPlacementLayout::None);
	ui->pushButton_2->setProperty(placementName, XKPPlacementLayout::None);
    layout->addWidget(ui->pushButton_2, XKPPlacementLayout::None);
	ui->label_1->setProperty(placementName, XKPPlacementLayout::None);
    layout->addWidget(ui->label_1, XKPPlacementLayout::None);
	ui->holder->setProperty(placementName, XKPPlacementLayout::None);
    layout->addWidget(ui->holder, XKPPlacementLayout::None);
	ui->pushButton_5->setProperty(placementName, XKPPlacementLayout::Bottom);
    layout->addWidget(ui->pushButton_5, XKPPlacementLayout::Bottom);
	setLayout(layout);
	layout = addLayout();
	ui->div_label_1->setProperty(placementName, XKPPlacementLayout::None);
    layout->addWidget(ui->div_label_1, XKPPlacementLayout::None);
	ui->pushButton_3->setProperty(placementName, XKPPlacementLayout::Right);
    layout->addWidget(ui->pushButton_3, XKPPlacementLayout::Right);
	ui->pushButton_4->setProperty(placementName, XKPPlacementLayout::Left);
    layout->addWidget(ui->pushButton_4, XKPPlacementLayout::Left);
	ui->edtText->setProperty(placementName, XKPPlacementLayout::None);
    layout->addWidget(ui->edtText, XKPPlacementLayout::None);
	ui->holder->setLayout(layout);
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

XKPPlacementLayout *Widget::addLayout()
{
    XKPPlacementLayout *plLayout = new XKPPlacementLayout;
    layouts.append(plLayout);

    return plLayout;
}


				void Widget::on_pushButton_1_clicked()
	{
    	ui->pushButton_1->setProperty(placementName, ((XKPPlacementLayout::Placement)ui->pushButton_1->property(placementName).toInt() + 1) % 7);
    	ui->pushButton_1->setText(QString(string("Clicked").c_str()));
    	application->reset("600");
    	ui->pushButton_2->setEnabled(true);
	}
	

		
				void Widget::on_pushButton_2_clicked()
	{
    	ui->pushButton_1->setText(QString(string("Old value: " + application->target_get()).c_str()));
    	application->target_set(application->target_get() + 10);
	}
	

		
			
		
			
		
				void Widget::on_pushButton_3_clicked()
	{
    	if (ui->holder->x() < 200)
    	{
    	    ui->holder->move(ui->holder->x() + 10, ui->holder->y());
    	}
	}
	

		
				void Widget::on_pushButton_4_clicked()
	{
    	if (ui->holder->x() > 10)
    	{
    	    ui->holder->move(ui->holder->x() - 10, ui->holder->y());
    	}
	}
	

		
			
		
			
		
				void Widget::on_pushButton_5_clicked()
	{
    	ui->edtText->setText(QString(string("pushButton_5 is clicked").c_str()));
    	ui->edtText->setReadOnly(!ui->edtText->isReadOnly());
    	if (ui->edtText->isReadOnly())
    	{
    	    ui->label_1->setText("editText is read only!");
    	}
    	else
    	{
    	    ui->label_1->setText("editText is for write");
    	}
	}
	

		
				void Widget::target_set(int value)
	{
    	if (xxx < 200)
    	{
    	    xxx = value;
    	}
    	else
    	{
    	    xxx = 60;
    	}
    	ui->pushButton_2->resize(xxx, ui->pushButton_2->height());
	}
	
	int Widget::target_get(void)
	{
    	return application->xxx;
	}
	
	void Widget::reset(string s)
	{
		target_set(60);
		ui->pushButton_1->setText(QString(string("...").c_str()));
		ui->pushButton_2->setText(QString(string("..." + target_get()).c_str()));
		ui->edtText->clear();
	}
	
	int Widget::returning_stuff()
	{
		int i = 0;
		return i + 23;
	}
	
	string Widget::declaring_error()
	{
		string s = "hello";
		return s;
	}
	
	int Widget::declaring_ok(int value)
	{
		return value;
	}
	
	int Widget::returned_real()
	{
		int pi = 3.1400001;
		return pi;
	}
	

		
