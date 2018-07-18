#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QLocale>

#include "GUI/QWindowMain.h"
#include "GUIController/QWindowMainController.h"

// Enable memory leak detection
#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// Must be done after pre-build lirary
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif // _DEBUG

int main(int argc, char *argv[])
{
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	bool bGoOn = true;

	// Initialize the QT application
	QApplication app(argc, argv);
	QApplication::setApplicationName(APPLICATION_NAME);
	QApplication::setApplicationVersion(APPLICATION_VERSION);
	QApplication::setOrganizationName(APPLICATION_VENDOR_NAME);
	QApplication::setOrganizationDomain(APPLICATION_VENDOR_DOMAIN);
	QApplication::setWindowIcon(QIcon(":/" APPLICATION_PACKAGE_NAME ".png"));

	qDebug("[Main] Starting application");

	QStringList listPathsTheme;
	listPathsTheme = QIcon::themeSearchPaths();
#ifdef QT_DEBUG
	listPathsTheme.append("./data/theme/icons"); // Add from the working directory
	listPathsTheme.append(app.applicationDirPath() + "/data/theme/icons"); // Add from the binary directory
#if defined(APPLE)
	listPathsTheme.append(app.applicationDirPath() + "/../data/theme/icons"); // Add from the binary directory
#endif
#endif

#if defined(WIN32)
	listPathsTheme.append("./icons_themes"); // Add from the working directory
	listPathsTheme.append(app.applicationDirPath() + "/icons_themes");
#elif defined(APPLE)
	listPathsTheme.append(app.applicationDirPath() + "/../Resources/icons_themes"); // Add from bundle
#else
	listPathsTheme.append(app.applicationDirPath() + "/../share/" + APPLICATION_PACKAGE_NAME + "/icons_themes"); // Add from the binary directory
#endif
	QIcon::setThemeSearchPaths(listPathsTheme);
	qDebug("[Main] Theme paths: %s", qPrintable(QIcon::themeSearchPaths().join(";")));
	//QIconThemeFallback::setThemeName("app_default");
	//qDebug("[Main] Using theme: %s, fallback theme: %s", qPrintable(QIcon::themeName()), qPrintable(QIconThemeFallback::themeName()));

	// Initialize translation
	QTranslator qtTranslator;
	qDebug("[Main] Current locale is %s", qPrintable(QLocale::system().name()));
	// qDebug("[Main] Current locale path is %s", qPrintable(QLibraryInfo::location(QLibraryInfo::TranslationsPath)));
	qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&qtTranslator);
	QTranslator appTranslator;
	appTranslator.load(QLocale::system().name(), ":/ts/");
	app.installTranslator(&appTranslator);

	//QSettingsManager::getInstance().loadConfigSettings();

	// Init GUI
	QWindowMain* pWindowMain = NULL;
	QWindowMainController controller;
	if(bGoOn){
		qDebug("[Main] Initializing GUI");
		pWindowMain = new QWindowMain();
		controller.init(pWindowMain);
	}

	// Show GUI
	if(bGoOn && pWindowMain){
		qDebug("[Main] Show application");
		//if (ApplicationSettings::getWindowMaximized()){
		//			pWindowMain->showMaximized();
		//}else{
			pWindowMain->show();
		//}
#ifdef WIN32
		pWindowMain->setWindowIcon(QIcon(":/" APPLICATION_PACKAGE_NAME ".png"));
#else
		pWindowMain->setWindowIconText(APPLICATION_PACKAGE_NAME);
#endif
	}

	// Run the event loop
	if(bGoOn && pWindowMain){
		qDebug("[Main] Starting the main loop");
		app.exec();
		qDebug("[Main] Exiting the main loop");
	}else{
		QMessageBox msgBox(QMessageBox::Critical, QT_TRANSLATE_NOOP("Main", "Error"), QT_TRANSLATE_NOOP("Main", "Error to initialize application"), QMessageBox::Ok);
		msgBox.exec();
	}

	// Delete the UI
	if(pWindowMain){
		delete pWindowMain;
		pWindowMain = NULL;
	}

	qDebug("[Main] Shutdown application");

	return 0;
}
