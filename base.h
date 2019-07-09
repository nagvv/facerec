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

#ifndef BASE_H
#define BASE_H

#if defined(SHARED_LIBRARY)
#  define SHARED_EXPORT Q_DECL_EXPORT
#else
#  define SHARED_EXPORT Q_DECL_IMPORT
#endif

#include <QtCore>
#include <QtNetwork>
#include <functional>

class SHARED_EXPORT ImgObj : public QObject
{
Q_OBJECT
public:
	struct Face
	{
		QString facialHair;
		QString glasses;
		QString hairColor;
		QString hairType;
		QString headwear;

		int boxX, boxY, boxHeight, boxWidth;

		double ageMean, ageVariance;
		QString ethnicity, gender;

		QVector<QPair<int, int>> landmarks;

		double score;
	};

	QString filepath;
	QFile fileObj;
	QBuffer *buffer;

	QVector<Face> faces;
	bool success = false;

	ImgObj( QString filepath ) : filepath( filepath ), fileObj( filepath ) {}
};

class SHARED_EXPORT Base : public QObject
{
private:
Q_OBJECT

	QHash<QString, ImgObj *> files;
	//TODO: вынести Bearer из токена
	QString jwtToken;

	QNetworkAccessManager netManager;

public:
	Base();

	inline const QHash<QString, ImgObj *> &getFiles() { return files; }

	bool addFile( QString filepath );

	void removeFile( QString filepath );

	const ImgObj *getImgObj( QString filepath ) { return files.value( filepath, nullptr ); }

	bool detectImage( QString filepath, std::function<void( qint64, qint64 )> uploadProgress = nullptr,
	                  std::function<void( qint64, qint64 )> downloadProgress = nullptr);

	void createDemoUser( const QString &email, const QString &password );

	void getToken( const QString &email, const QString &password );

	void setToken( const QString &token );

	bool isAuthorized();


private slots:

	void onFinish( QNetworkReply *rep );

	void onGetTokenFinish( QNetworkReply *rep );

	void onCreateUserFinish( QNetworkReply *rep );

signals:

	void imgObjUpdated( const ImgObj * );

	void getTokenFinished( const QString &token );

	void createUserFinished( bool success );
};


#endif // BASE_H
