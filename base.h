#ifndef BASE_H
#define BASE_H

#include <QtCore>
#include <QtNetwork>

class ImgObj : public QObject{
	Q_OBJECT
public:
	QString filepath;
	QFile fileObj;
	QString result;

	ImgObj(QString filepath) : filepath(filepath), fileObj(filepath) {}
};

class Base : public QObject
{
private:
Q_OBJECT

	QHash<QString, ImgObj*> files;
	//TODO: вынести Bearer из токена
	inline static constexpr char jwtToken[] = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiI5NTk2Mjc4Zi05MjczLTQ0NTgtODUxNi1iNzFjNTM5NTljOTIiLCJzdWIiOjM4LCJpYXQiOjE1NjIyMzMyNDUsIm5iZiI6MTU2MjIzMzI0NSwidHlwZSI6ImFjY2VzcyIsImZyZXNoIjpmYWxzZX0.TQVBvihhA2zzXDzV5VQD2Sz4Q_nR0IboyDc62S1nIPc";

	QNetworkAccessManager netManager;
	QUrl url = QUrl("https://backend.facecloud.tevian.ru/api/v1/detect?demographics=true&attributes=true&landmarks=true");

public:
	Base();

	inline const QHash<QString, ImgObj*> &getFiles() { return files; }

	bool addFile( QString filepath );

	bool detectImage( QString filepath );

private slots:

	void onFinish( QNetworkReply *rep );
};


#endif // BASE_H
