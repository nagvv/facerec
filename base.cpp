#include "base.h"
#include "nlohmann/json.hpp"
#include <iostream>

Base::Base() : netManager( this )
{
	connect( &netManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( onFinish( QNetworkReply * ) ) );
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

		auto faces = json["data"].toArray();

		for ( auto faceRef : faces )
		{
			QJsonValue face(faceRef);
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

			for( auto item : face["landmarks"].toArray() )
				temp.landmarks.push_back( QPair( item.toObject()["x"].toInt(),
				                                    item.toObject()["y"].toInt()) );

			temp.score = face["score"].toDouble();

			imgObj->faces.push_back(temp);
		}

		imgObj->processed = true;
		emit imgObjUpdated(imgObj);
	}

	rep->deleteLater();
}
