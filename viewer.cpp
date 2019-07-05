#include "viewer.h"
#include <QPainter>

Viewer::Viewer(QWidget *parent) : QWidget(parent)
{

}



void Viewer::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPainter painter(this);

	if ( target )
	{
		QImage img(target->filepath); // TODO: optimise (load from memory, for example)

		painter.drawImage(0, 0, img);
	}
}

void Viewer::setTarget(const ImgObj* target)
{
	this->target = target;
}
