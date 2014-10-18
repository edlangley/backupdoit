#ifndef BDLOGIC_H
#define BDLOGIC_H

#include <QObject>
#include <QWidget>
#include <QGraphicsObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>

#define BDLOGIC_STATUS_DOWNLOAD_FINISHED            2
#define BDLOGIC_STATUS_DOWNLOADING                  1
#define BDLOGIC_STATUS_OK                           0
#define BDLOGIC_STATUS_NETWORK_ERROR               -1
#define BDLOGIC_STATUS_JSON_PARSE_ERROR            -2
#define BDLOGIC_STATUS_FILE_ERROR                  -3


class BdLogic : public QWidget
{
    Q_OBJECT

public:
    enum ActionOrder
    {
        OrderByBox = 0,
        OrderByContext,
        OrderByProject,
        OrderByPriority
    };

    enum SaveFileType
    {
        FileTypeOrderedList = 0,
        FileTypeJson
    };

    BdLogic();

    Q_INVOKABLE int ConnectAndDownload(const QString &username, const QString &password);
    Q_INVOKABLE int SetDataModelOrdering(int order);
    Q_INVOKABLE QVariantList GetDataModel();
    Q_INVOKABLE QString GetSaveFileName(int fileType);
    Q_INVOKABLE int SaveDataToFile(QString filename, int fileType);

    void SetQmlObject(QGraphicsObject *qmlObject) { m_qmlObject = qmlObject; }

signals:
    void downloadStatusUpdated(int status, QString message);

private slots:
    void replyFinished();
    void replyError(QNetworkReply::NetworkError code);
    void replySSLError(const QList<QSslError> & errors);
    void ignoreSSLErrors(QNetworkReply* reply, QList<QSslError> errors);

private:
    QString getProjectNameFromJsonAction(QVariantMap actionFromJson);
    QString getContextNameFromJsonAction(QVariantMap actionFromJson);

    int m_dlState;
    QVariantList m_actionListOrderedForQML;
    QVariantMap m_boxMapParsedJson;
    QMap<QString, QByteArray> m_boxMapRawJson;
    int m_statusCode;
    QString m_statusString;

    QNetworkAccessManager *m_netManager;
    QNetworkReply *m_reply;
    bool m_replyGotError;

    QGraphicsObject *m_qmlObject;
};

#endif // BDLOGIC_H
