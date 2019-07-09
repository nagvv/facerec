/*
 * Copyright 2019 nagvv ( https://github.com/nagvv )
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *    http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "viewer.h"
#include <QPainter>
#include <QResizeEvent>
#include <iostream>

Viewer::Viewer( QWidget *parent ) : QWidget( parent ) {}

void Viewer::fixZoom()
{
	double sizeByX = double( height() ) / currentImg.height();
	double sizeByY = double( width() ) / currentImg.width();
	double size = std::min( sizeByX, sizeByY );

	if ( viewZoom * size > 5 )
		viewZoom = 5 / size;

	if ( viewZoom < 0.4 && viewZoom < 0.8 / size )
		viewZoom = std::min( 0.4, 0.8 / size );
}

void Viewer::paintEvent( QPaintEvent *event )
{
	Q_UNUSED( event )
	QPainter painter( this );
	painter.translate( width() / 2 + viewX * viewZoom, height() / 2 + viewY * viewZoom);

	if ( target )
	{
		double sizeByX = double( height() ) / currentImg.height();
		double sizeByY = double( width() ) / currentImg.width();
		double size = std::min( sizeByX, sizeByY ) * viewZoom;

		QRectF rect( -currentImg.width() * size / 2, -currentImg.height() * size / 2,
		             currentImg.width() * size, currentImg.height() * size );

		painter.drawImage( rect, currentImg );

		painter.setPen( Qt::green );
		QFont font;
		font.setPointSize( 12 );
		painter.setFont( font );

		for ( auto face : target->faces )
		{
			painter.drawRect( QRectF( -currentImg.width() * size / 2 + face.boxX * size,
			                          -currentImg.height() * size / 2 + face.boxY * size,
			                          face.boxWidth * size,
			                          face.boxHeight * size ) );
			painter.drawText( QPointF(
			        -currentImg.width() * size / 2 + ( face.boxX + face.boxWidth ) * size + 5,
			        -currentImg.height() * size / 2 + face.boxY * size + 12 ),
			                  face.gender );

			painter.drawText( QPointF(
			        -currentImg.width() * size / 2 + ( face.boxX + face.boxWidth ) * size + 5,
			        -currentImg.height() * size / 2 + face.boxY * size + 26 ),
			                  QString::number( face.ageMean, 'f', 1 ) );

			// TODO: make text readable at any background
		}
	}
}

void Viewer::wheelEvent(QWheelEvent *event)
{
	double prevDeltaX = ( width() / 2 - event->x() ) / viewZoom;
	double prevDeltaY = ( height() / 2 - event->y() ) / viewZoom;

	double zoom = double(event->angleDelta().y()) / 1000;
	if ( abs( zoom ) > 0.8 )
		zoom = std::copysign( 0.8, zoom );

	viewZoom *= 1 + zoom;
	fixZoom();

	double nextDeltaX = ( width() / 2 - event->x() ) / viewZoom;
	double nextDeltaY = ( height() / 2 - event->y() ) / viewZoom;

	viewX -= ( nextDeltaX - prevDeltaX );
	viewY -= ( nextDeltaY - prevDeltaY );

	repaint();
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
	if ( !moving )
		return;

	int relX = event->x() - mouseOldX;
	int relY = event->y() - mouseOldY;

	mouseOldX = event->x();
	mouseOldY = event->y();

	viewX += relX / viewZoom;
	viewY += relY / viewZoom;
	repaint();
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
	if ( event->button() == Qt::LeftButton )
	{
		moving = true;
		mouseOldX = event->x();
		mouseOldY = event->y();
	}
}

void Viewer::mouseReleaseEvent(QMouseEvent *event)
{
	if ( event->button() == Qt::LeftButton )
		moving = false;
}

void Viewer::setTarget( const ImgObj *target )
{
	this->target = target;
	if ( target )
		currentImg.load( target->filepath );
	viewX = 0.;
	viewY = 0.;
	viewZoom = 1.;
	fixZoom();
}
