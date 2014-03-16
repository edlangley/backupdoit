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
    viewer.setMainQmlFile(QLatin1String("qml/backupdoit_qml/backupdoit.qml"));

    backupDoitLogic.SetQmlObject(viewer.rootObject());

    viewer.showExpanded();

    return app->exec();
}
