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

#include "base.h"
#include <QImageReader>

Base::Base() : netManager( this )
{
	connect( &netManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( onFinish( QNetworkReply * ) ) );
}

bool Base::addFile( QString filepath )
{
	if ( !QFileInfo::exists( filepath ) )
		return false;
	files.insert( filepath, new ImgObj( filepath ) );
	return true;
}

void Base::removeFile( QString filepath )
{
	files.remove( filepath );
}

bool Base::detectImage( QString filepath, std::function<void( qint64, qint64 )> uploadProgress,
                        std::function<void( qint64, qint64 )> downloadProgress )
{
	if ( !files.contains( filepath ) )
		return false;

	if ( !files[filepath]->fileObj.open( QFile::ReadOnly ) )
		return false;

	QNetworkRequest req( url );
	req.setRawHeader( "Authorization", jwtToken );
	req.setRawHeader( "accept", "application/json" );
	req.setRawHeader( "Content-Type", "image/jpeg" );

	req.setOriginatingObject( files[filepath] );

	files[filepath]->buffer = new QBuffer();
	files[filepath]->buffer->open( QIODevice::ReadWrite );

	if ( QImageReader::imageFormat( &files[filepath]->fileObj ) == "jpeg" )
	{
		files[filepath]->buffer->write( files[filepath]->fileObj.readAll() );
	}
	else
	{
		QImage timg( filepath );
		timg.save( files[filepath]->buffer, "JPG" );
	}

	files[filepath]->buffer->reset();
	files[filepath]->fileObj.close();

	auto rep = netManager.post( req, files[filepath]->buffer );

	if ( uploadProgress )
		connect( rep, &QNetworkReply::uploadProgress, uploadProgress );
	if ( downloadProgress )
		connect( rep, &QNetworkReply::downloadProgress, downloadProgress );

	return true;
}

void Base::onFinish( QNetworkReply *rep )
{
	auto *imgObj = dynamic_cast<ImgObj *>(rep->request().originatingObject());

	delete imgObj->buffer;
	imgObj->buffer = nullptr;

	if ( rep->error() == QNetworkReply::NoError )
	{
		QJsonDocument json = QJsonDocument::fromJson( rep->readAll() );

		auto faces = json["data"].toArray();

		for ( auto faceRef : faces )
		{
			QJsonValue face( faceRef );
			ImgObj::Face temp;

			temp.facialHair = face["attributes"]["facial_hair"].toString();
			temp.glasses = face["attributes"]["glasses"].toString();
			temp.hairColor = face["attributes"]["hair_color"].toString();
			temp.hairType = face["attributes"]["hair_type"].toString();
			temp.headwear = face["attributes"]["headwear"].toString();

			temp.boxX = face["bbox"]["x"].toInt();
			temp.boxY = face["bbox"]["y"].toInt();
			temp.boxHeight = face["bbox"]["height"].toInt();
			temp.boxWidth = face["bbox"]["width"].toInt();

			temp.ageMean = face["demographics"]["age"]["mean"].toDouble();
			temp.ageVariance = face["demographics"]["age"]["variance"].toDouble();
			temp.ethnicity = face["demographics"]["ethnicity"].toString();
			temp.gender = face["demographics"]["gender"].toString();

			for ( auto item : face["landmarks"].toArray() )
				temp.landmarks.push_back( QPair( item.toObject()["x"].toInt(), item.toObject()["y"].toInt() ) );

			temp.score = face["score"].toDouble();

			imgObj->faces.push_back( temp );
		}
		imgObj->success = true;
	}

	emit imgObjUpdated( imgObj );

	rep->deleteLater();
}
