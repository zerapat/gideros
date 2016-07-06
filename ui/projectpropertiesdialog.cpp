#include "projectpropertiesdialog.h"
#include "ui_projectpropertiesdialog.h"
#include <QDebug>
#include <QIntValidator>
#include <QFileDialog>
#include <QPixmap>
#include <QTimer>
#include "projectpropertiesdialog.h"
#include "projectproperties.h"
#include <algorithm>

ProjectPropertiesDialog::ProjectPropertiesDialog(ProjectProperties* properties, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::ProjectPropertiesDialog),
	properties_(properties)
{
    ui->setupUi(this);

	ui->scaleMode->setCurrentIndex(properties_->scaleMode);

	ui->logicalWidth->setText(QString::number(properties_->logicalWidth));
	ui->logicalHeight->setText(QString::number(properties_->logicalHeight));

	ui->logicalWidth->setValidator(new QIntValidator());
	ui->logicalHeight->setValidator(new QIntValidator());

    ui->windowWidth->setText(QString::number(properties_->windowWidth));
    ui->windowHeight->setText(QString::number(properties_->windowHeight));

    ui->windowWidth->setValidator(new QIntValidator());
    ui->windowHeight->setValidator(new QIntValidator());

	for (size_t i = 0; i < properties_->imageScales.size(); ++i)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem;
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		item->setText(0, properties_->imageScales[i].first);
		item->setText(1, QString::number(properties_->imageScales[i].second));
		ui->imageScales->addTopLevelItem(item);
	}

	ui->orientation->setCurrentIndex(properties_->orientation);

	switch (properties_->fps)
	{
		case 60:
			ui->fps->setCurrentIndex(0);
			break;
		case 30:
			ui->fps->setCurrentIndex(1);
			break;
	}

    ui->retinaDisplay->setCurrentIndex(properties_->retinaDisplay);
	ui->autorotation->setCurrentIndex(properties_->autorotation);

    ui->mouseToTouch->setChecked(properties_->mouseToTouch);
    ui->touchToMouse->setChecked(properties_->touchToMouse);
    ui->mouseTouchOrder->setCurrentIndex(properties_->mouseTouchOrder);

    ui->version->setText(properties_->version);
    ui->version_code->setText(QString::number(properties_->version_code));

    connect(ui->add, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->remove, SIGNAL(clicked()), this, SLOT(remove()));

    connect(ui->appIcon, SIGNAL(clicked()), this, SLOT(addAppIcon()));
    connect(ui->tvIcon, SIGNAL(clicked()), this, SLOT(addTvIcon()));
    connect(ui->splashImage, SIGNAL(clicked()), this, SLOT(addSplashImage()));

	connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));

    //delay loading images
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loadImages()));
    timer->start(1);
}

ProjectPropertiesDialog::~ProjectPropertiesDialog()
{
    delete ui;
}


struct ItemScalesComp
{
	bool operator()(const std::pair<QString, double>& i1,
					const std::pair<QString, double>& i2) const
	{
		return i1.second > i2.second;
	}
};


void ProjectPropertiesDialog::onAccepted()
{
	properties_->scaleMode = ui->scaleMode->currentIndex();

	properties_->logicalWidth = ui->logicalWidth->text().toInt();
	properties_->logicalHeight = ui->logicalHeight->text().toInt();

    properties_->windowWidth = ui->windowWidth->text().toInt();
    properties_->windowHeight = ui->windowHeight->text().toInt();

	properties_->imageScales.clear();

	for (int i = 0; i < ui->imageScales->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem* item = ui->imageScales->topLevelItem(i);
		properties_->imageScales.push_back(std::make_pair(item->text(0), item->text(1).toDouble()));
	}

	std::sort(properties_->imageScales.begin(), properties_->imageScales.end(), ItemScalesComp());

	properties_->orientation = ui->orientation->currentIndex();

	switch (ui->fps->currentIndex())
	{
	case 0:
		properties_->fps = 60;
		break;
	case 1:
		properties_->fps = 30;
		break;
	}

    properties_->retinaDisplay = ui->retinaDisplay->currentIndex();
	properties_->autorotation = ui->autorotation->currentIndex();

    properties_->mouseToTouch = ui->mouseToTouch->isChecked();
    properties_->touchToMouse = ui->touchToMouse->isChecked();
    properties_->mouseTouchOrder = ui->mouseTouchOrder->currentIndex();

    properties_->version = ui->version->text();
    properties_->version_code = ui->version_code->text().toInt();

    if(!this->app_icon.isNull())
        properties_->app_icon = this->app_icon;
    if(!this->tv_icon.isNull())
        properties_->tv_icon = this->tv_icon;
    if(!this->splash_image.isNull())
        properties_->splash_image = this->splash_image;

}

void ProjectPropertiesDialog::loadImages()
{
    this->showImage(properties_->app_icon, ui->appIconLabel);
    this->showImage(properties_->tv_icon, ui->tvIconLabel);
    this->showImage(properties_->splash_image, ui->splashImageLabel);

}

void ProjectPropertiesDialog::add()
{
	QTreeWidgetItem* item = new QTreeWidgetItem;
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	item->setText(0, "suffix");
	ui->imageScales->addTopLevelItem(item);
	ui->imageScales->editItem(item, 0);
}

void ProjectPropertiesDialog::remove()
{
	QList<QTreeWidgetItem*> items = ui->imageScales->selectedItems();

	for (int i = 0; i < items.size(); ++i)
		delete items[i];
}

void ProjectPropertiesDialog::showImage(QString fileName, QLabel* label)
{
    if(!fileName.isNull()){
        int w = label->width();
        int h = label->height();
        QPixmap p = QPixmap(fileName);
        label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
    }
}

void ProjectPropertiesDialog::addAppIcon()
{
    this->app_icon = QFileDialog::getOpenFileName(0, QObject::tr("Select app icon"),"",QObject::tr("Images (*.png)"));
    this->showImage(this->app_icon, ui->appIconLabel);
}

void ProjectPropertiesDialog::addTvIcon()
{
    this->tv_icon = QFileDialog::getOpenFileName(0, QObject::tr("Select TV icon"),"",QObject::tr("Images (*.png)"));
    this->showImage(this->tv_icon, ui->tvIconLabel);
}

void ProjectPropertiesDialog::addSplashImage()
{
    this->splash_image = QFileDialog::getOpenFileName(0, QObject::tr("Select splash image"),"",QObject::tr("Images (*.png)"));
    this->showImage(this->splash_image, ui->splashImageLabel);
}
