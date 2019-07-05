#include "base.h"
#include "nlohmann/json.hpp"
#include <iostream>

Base::Base() : netManager( this )
{
	connect( &netManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( onFinish( QNetworkReply * ) ) );
	//connect(&netManager, SIGNAL(finished(QNetworkReply*)), &netManager, SLOT(deleteLater()));
}

bool Base::addFile( QString filepath )
{
	if ( !QFileInfo::exists(filepath) )
		return false;
	files.insert(filepath, new ImgObj(filepath));
	return true;
}

bool Base::detectImage( QString filepath )
{
	if ( !files.contains(filepath) )
		return false;

	if ( !files[filepath]->fileObj.open(QFile::ReadOnly) )
		return false;

	QNetworkRequest req( url );
	req.setRawHeader( "Authorization", jwtToken );
	req.setRawHeader( "accept", "application/json" );
	req.setRawHeader( "Content-Type", "image/jpeg" );
	// TODO: convert png and so to jpeg

	req.setOriginatingObject( files[filepath] );

	netManager.post( req, &files[filepath]->fileObj );
	// TODO: connect signal for progress bar
	return true;
}

void Base::onFinish( QNetworkReply *rep )
{
	if ( rep->error() == QNetworkReply::NoError )
	{
		auto *imgObj = dynamic_cast<ImgObj*>(rep->request().originatingObject());

		imgObj->fileObj.close();
		QJsonDocument json = QJsonDocument::fromJson(rep->readAll());

		imgObj->facialHair = json["data"]["attributes"]["facial_hair"].toString();
		imgObj->glasses = json["data"]["attributes"]["glasses"].toString();
		imgObj->hairColor = json["data"]["attributes"]["hair_color"].toString();
		imgObj->hairType = json["data"]["attributes"]["hair_type"].toString();
		imgObj->headwear = json["data"]["attributes"]["headwear"].toString();

		imgObj->boxX = json["data"]["bbox"]["x"].toInt();
		imgObj->boxY = json["data"]["bbox"]["y"].toInt();
		imgObj->boxHeight = json["data"]["bbox"]["height"].toInt();
		imgObj->boxWidth = json["data"]["bbox"]["width"].toInt();

		imgObj->ageMean = json["data"]["demographics"]["age"]["mean"].toDouble();
		imgObj->ageVariance = json["data"]["demographics"]["age"]["variance"].toDouble();
		imgObj->ethnicity = json["data"]["demographics"]["ethnicity"].toString();
		imgObj->gender = json["data"]["demographics"]["gender"].toString();

		for( auto item : json["data"]["landmarks"].toArray() )
			imgObj->landmarks.push_back( QPair( item.toObject()["x"].toInt(),
			                                    item.toObject()["y"].toInt()) );

		imgObj->score = json["data"]["score"].toDouble();

		imgObj->processed = true;
	}

	rep->deleteLater();
}
