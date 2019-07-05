#include "viewer.h"
#include <QPainter>
#include <QResizeEvent>

Viewer::Viewer(QWidget *parent) : QWidget(parent)
{
	viewHeight = this->height();
	viewX = 0.;
	viewY = 0.;
}

void Viewer::resizeEvent(QResizeEvent *event)
{
	//viewHeight = event->size().height();
}

void Viewer::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPainter painter(this);

	if ( target )
	{
		QImage img(target->filepath); // TODO: optimise (load from memory, for example)

		double sizeByX = double(height()) / img.height();
		double sizeByY = double(width()) / img.width();
		double size = std::min(sizeByX, sizeByY);

		QRectF rect( ( width() - img.width()*size )/2,
		            ( height() - img.height()*size )/2,
		            img.width() * size,
		            img.height() * size );

		painter.drawImage(rect, img);
	}
}

void Viewer::setTarget(const ImgObj* target)
{
	this->target = target;
}
