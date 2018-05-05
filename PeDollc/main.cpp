#include "pedollc.h"
#include <QTranslator>
#include <QSplashScreen>
#include <QDateTime>
#include <QtGui/QApplication>

#define  REMOTESHELL_PROCESS_GUID (L"14B599DD-E9D3-4300-BF5D-4228B15441515")
HANDLE G_hMutex;

BOOL IsAlreadyRunning()
{
	G_hMutex = ::CreateMutex(NULL,TRUE,REMOTESHELL_PROCESS_GUID);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(G_hMutex);
		return TRUE;
	}
	return FALSE;
}


int main(int argc, char *argv[])
{
	if (IsAlreadyRunning())
	{
		return 0;
	}
	QApplication a(argc, argv);

	QSplashScreen *splash = new QSplashScreen;
	splash->setPixmap(QPixmap(":/PeDollc/Resources/PeDoll.png"));
	splash->show();

	DWORD m_time=timeGetTime();
	do{   
		splash->update();
	} while (timeGetTime()-m_time<1200);

	QTranslator qtTranslator;
	qtTranslator.load("Language.qm","./");
	a.installTranslator(&qtTranslator);
	PeDollc w;
	w.show();
	splash->finish(&w);
	delete splash;
	return a.exec();
}
