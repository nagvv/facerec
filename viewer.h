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

#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include "base.h"

class Viewer : public QWidget
{
private:
Q_OBJECT
	const ImgObj *target = nullptr;

	double viewHeight;
	double viewX, viewY;

protected:
	void resizeEvent( QResizeEvent *event ) override;

	void paintEvent( QPaintEvent *event ) override;

public:
	explicit Viewer( QWidget *parent = nullptr );

	void setTarget( const ImgObj *target );
};

#endif // VIEWER_H
