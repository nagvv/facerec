#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include "base.h"

class Viewer : public QWidget
{
private:
	Q_OBJECT
	const ImgObj* target = nullptr;

protected:
	void paintEvent(QPaintEvent *event) override;
public:
	explicit Viewer(QWidget *parent = nullptr);

	void setTarget(const ImgObj* target);


signals:

public slots:
};

#endif // VIEWER_H
