#include "base.h"
#include "nlohmann/json.hpp"

Base::Base() : netManager( this )
{
	connect( &netManager, SIGNAL( finished( QNetworkReply * ) ), this, SLOT( onFinish( QNetworkReply * ) ) );
	//connect(&netManager, SIGNAL(finished(QNetworkReply*)), &netManager, SLOT(deleteLater()));
}

void Base::addFile( QString filename )
{
	files.push_back( filename );
}

#include <iostream>

void Base::detectImage( QString filename )
{
	/*QFile file(filename);

	//client.Post("api/v1/detect", params);

	httplib::Request req;
	req.path = "api/v1/detect";

	req.params.emplace("demographics", "true");
	req.params.emplace("attributes",  "true");
	req.params.emplace("landmarks",  "true");

	req.set_header("accept", "application/json");
	req.set_header("Authorization", jwtToken);
	req.set_header("Content-Type", "image/jpeg");

	file.open(QIODevice::ReadOnly);
	auto data = file.readAll();
	file.close();

	req.body = data.toStdString();

	httplib::Response res;

	req.progress = [](uint64_t len, uint64_t total) {
		printf("%lld / %lld bytes => %d%% complete\n", len, total, (int)((len/total)*100));
		return true; // return 'false' if you want to cancel the request.
	};
*/

	QFile *file = new QFile( filename );
	if ( !file->open( QFile::ReadOnly ) )
	{
		std::cout << "couldn't open a file" << std::endl;
		return;
	}

	QUrl url( "https://backend.facecloud.tevian.ru/api/v1/detect?demographics=true&attributes=true&landmarks=true" );

	QNetworkRequest req( url );
	req.setRawHeader( "Authorization", jwtToken );
	req.setRawHeader( "accept", "application/json" );
	req.setRawHeader( "Content-Type", "image/jpeg" );
	// TODO: convert png and so to jpeg

	netManager.post( req, file );
}

void Base::onFinish( QNetworkReply *rep )
{
	std::cout << rep->url().toString().toStdString() << " " << rep->error() << rep->readAll().toStdString()
	          << std::endl;
	rep->deleteLater();
}
