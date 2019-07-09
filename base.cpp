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
	connect( &netManager, &QNetworkAccessManager::finished, this, &Base::onFinish );
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

	QUrl url( "https://backend.facecloud.tevian.ru/api/v1/detect?demographics=true" );
	QNetworkRequest req( url );
	req.setRawHeader( "Authorization", QString( "Bearer %1" ).arg( jwtToken ).toUtf8() );
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

void Base::createDemoUser( const QString &email, const QString &password )
{
	QJsonObject request;
	request.insert( "billing_type", "demo" );
	request.insert( "email", email );
	request.insert( "password", password );

	QJsonObject someData;
	QJsonArray someDataArr{ "to", "save" };
	someData.insert( "some_data", someDataArr );
	request.insert( "data", someData );

	QJsonDocument json( request );

	QUrl url( "https://backend.facecloud.tevian.ru/api/v1/users" );
	QNetworkRequest req( url );

	req.setRawHeader( "accept", "application/json" );
	req.setRawHeader( "Content-Type", "application/json" );

	netManager.post( req, json.toJson() );
}

void Base::getToken( const QString &email, const QString &password )
{
	QJsonObject request;
	request.insert( "email", email );
	request.insert( "password", password );

	QJsonDocument json( request );

	QUrl url( "https://backend.facecloud.tevian.ru/api/v1/login" );
	QNetworkRequest req( url );

	req.setRawHeader( "accept", "application/json" );
	req.setRawHeader( "Content-Type", "application/json" );

	netManager.post( req, json.toJson() );
}

void Base::setToken( const QString &token )
{
	jwtToken = token;
}

bool Base::isAuthorized()
{
	return !jwtToken.isEmpty();
}

void Base::onFinish( QNetworkReply *rep )
{
	if ( rep->url().path() == "/api/v1/login" )
		return onGetTokenFinish( rep );

	if ( rep->url().path() == "/api/v1/users" )
		return onCreateUserFinish( rep );

	if ( rep->url().path() != "/api/v1/detect" )
		return; // TODO add loggging

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
				temp.landmarks.push_back( QPair<int, int>( item.toObject()["x"].toInt(), item.toObject()["y"].toInt() ) );

			temp.score = face["score"].toDouble();

			imgObj->faces.push_back( temp );
		}
		imgObj->success = true;
	}

	emit imgObjUpdated( imgObj );

	rep->deleteLater();
}

void Base::onGetTokenFinish( QNetworkReply *rep )
{
	QString token;
	if ( rep->error() == QNetworkReply::NoError )
	{
		QJsonDocument json = QJsonDocument::fromJson( rep->readAll() );
		token = json["data"]["access_token"].toString();
	}

	emit getTokenFinished( token );
	rep->deleteLater();
}

void Base::onCreateUserFinish( QNetworkReply *rep )
{
	bool success = ( rep->error() == QNetworkReply::NoError );
	emit createUserFinished( success );
	rep->deleteLater();
}


