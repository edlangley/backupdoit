#ifndef BDLOGIC_H
#define BDLOGIC_H

#include <QObject>
#include <QGraphicsObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#define BDLOGIC_STATUS_DOWNLOADING                  1
#define BDLOGIC_STATUS_OK                           0
#define BDLOGIC_STATUS_NETWORK_ERROR               -1
#define BDLOGIC_STATUS_JSON_PARSE_ERROR            -2



class BdLogic : public QObject
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

    BdLogic();

    Q_INVOKABLE int ConnectAndDownload(const QString &username, const QString &password);
    Q_INVOKABLE int GetDownloadStatus() { return m_statusCode; }
    Q_INVOKABLE int SetDataModelOrdering(int order);
    Q_INVOKABLE QVariantList GetDataModel();
    Q_INVOKABLE int SaveDataToFile(QString &filename, int fileType);
    Q_INVOKABLE QString GetErrorString() { return m_errorString; }

    void SetQmlObject(QGraphicsObject *qmlObject) { m_qmlObject = qmlObject; }

private slots:
    void ReplyFinished();
    void ReplyError(QNetworkReply::NetworkError code);
    void ReplySSLError(const QList<QSslError> & errors);

private:
    QString getProjectNameFromJsonAction(QVariantMap actionFromJson);
    QString getContextNameFromJsonAction(QVariantMap actionFromJson);

    int m_dlState;
    QVariantList m_boxListOrderedForQML;
    QVariantMap m_boxMapParsedJson;
    QMap<QString, QByteArray> m_boxMapRawJson;
    int m_statusCode;
    QString m_errorString;

    QNetworkAccessManager *m_netManager;
    QNetworkReply *m_reply;
    bool m_replyGotError;

    QGraphicsObject *m_qmlObject;
};

#endif // BDLOGIC_H
