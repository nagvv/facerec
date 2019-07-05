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
		imgObj->result = rep->readAll();
		imgObj->fileObj.close();
	}

	rep->deleteLater();
}
