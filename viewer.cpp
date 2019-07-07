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

Viewer::Viewer( QWidget *parent ) : QWidget( parent )
{
	viewHeight = this->height();
	viewX = 0.;
	viewY = 0.;
}

void Viewer::paintEvent( QPaintEvent *event )
{
	Q_UNUSED( event )
	QPainter painter( this );

	if ( target )
	{
		double sizeByX = double( height() ) / currentImg.height();
		double sizeByY = double( width() ) / currentImg.width();
		double size = std::min( sizeByX, sizeByY );

		QRectF rect( ( width() - currentImg.width() * size ) / 2,
		             ( height() - currentImg.height() * size ) / 2,
		             currentImg.width() * size,
		             currentImg.height() * size );

		painter.drawImage( rect, currentImg );

		painter.setPen( Qt::green );
		for ( auto face : target->faces )
		{
			painter.drawRect( QRectF( ( width() - currentImg.width() * size ) / 2 + face.boxX * size,
			                          ( height() - currentImg.height() * size ) / 2 + face.boxY * size,
			                          face.boxWidth * size,
			                          face.boxHeight * size ) );
			painter.drawText( QPointF(
			        ( width() - currentImg.width() * size ) / 2 + ( face.boxX + face.boxWidth ) * size + 5,
			        ( height() - currentImg.height() * size ) / 2 + face.boxY * size + 5 ), face.gender );
		}
	}
}

void Viewer::setTarget( const ImgObj *target )
{
	this->target = target;
	if ( target )
		currentImg.load( target->filepath );
}
