#include <QApplication>
#include <QDebug>
#include <QDeclarativeContext>

#include "qmlapplicationviewer.h"
#include "bdlogic.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;
    BdLogic backupDoitLogic;

    viewer.rootContext()->setContextProperty("bdLogic", &backupDoitLogic);
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
#ifdef QMLBUILTIN
    viewer.setSource(QUrl("qrc:/qml/backupdoit_qml/backupdoit.qml"));
#else
    viewer.setMainQmlFile(QLatin1String("qml/backupdoit_qml/backupdoit.qml"));
#endif
    backupDoitLogic.SetQmlObject(viewer.rootObject());

    viewer.showExpanded();

    return app->exec();
}
