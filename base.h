#ifndef BASE_H
#define BASE_H

#include <QtCore>
#include <QtNetwork>

class Base : public QObject
{
private:
Q_OBJECT

	QVector<QString> files;
	//TODO: вынести Bearer из токена
	inline static constexpr char jwtToken[] = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiI5NTk2Mjc4Zi05MjczLTQ0NTgtODUxNi1iNzFjNTM5NTljOTIiLCJzdWIiOjM4LCJpYXQiOjE1NjIyMzMyNDUsIm5iZiI6MTU2MjIzMzI0NSwidHlwZSI6ImFjY2VzcyIsImZyZXNoIjpmYWxzZX0.TQVBvihhA2zzXDzV5VQD2Sz4Q_nR0IboyDc62S1nIPc";

	QNetworkAccessManager netManager;

public:
	Base();

	inline const QVector<QString> &getFiles() { return files; }

	void addFile( QString filename );

	void detectImage( QString filename );

private slots:

	void onFinish( QNetworkReply *rep );
};


#endif // BASE_H
