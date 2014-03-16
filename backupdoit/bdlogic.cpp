
#include <QDebug>
#include <qjson/parser.h>

#include "bdlogic.h"

#define DOIT_LOGIN_URL      "https://i.doit.im/signin"
#define DOIT_BASE_DATA_URL  "https://i.doit.im/api/tasks/"

enum
{
    DLSTATE_LOGIN,
    DLSTATE_INBOX,
    DLSTATE_TODAY,
    DLSTATE_NEXT,
    DLSTATE_TOMORROW,
    DLSTATE_SCHEDULED,
    DLSTATE_SOMEDAY,
    DLSTATE_WAITING,
    DLSTATE_FINISHED
};

// Order must match above enum
const char *const BoxNames[] =
{
    "login",
    "inbox",
    "today",
    "next",
    "tomorrow",
    "scheduled",
    "someday",
    "waiting",
};

BdLogic::BdLogic()
{
    m_netManager = new QNetworkAccessManager(this);

    m_dlState = DLSTATE_LOGIN;
    m_statusCode = BDLOGIC_STATUS_OK;
}

int BdLogic::ConnectAndDownload(const QString &username, const QString &password)
{
    QString loginPostData;

    m_dlState = DLSTATE_LOGIN;
    m_statusCode = BDLOGIC_STATUS_DOWNLOADING;
    m_boxMap.clear();
    m_boxRawJsonMap.clear();

    loginPostData = "username=";
    loginPostData += username;
    loginPostData += "&password=";
    loginPostData += password;

    QNetworkRequest request;
    request.setUrl(QUrl(DOIT_LOGIN_URL));
    m_reply = m_netManager->post(request, loginPostData.toUtf8());

    connect(m_reply, SIGNAL(finished()), this, SLOT(ReplyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(ReplyError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(ReplySSLError(QList<QSslError>)));

    return 0;
}

/*
int BdLogic::SetDataModelOrdering(int order)
{

    return 0;
}
*/

QVariantMap BdLogic::GetDataModel()
{



    // Test data:

    QVariantMap testBoxMap;
    QVariantMap testInboxMap;
    QVariantList testInboxEntitiesList;
    QVariantMap testTask1, testTask2;

    testTask1["project"] = QVariant("Paint shed");
    testTask1["title"] = QVariant("Get paint");
    testInboxEntitiesList.push_back(testTask1);

    testTask2["project"] = QVariant("Make app");
    testTask2["title"] = QVariant("Write code");
    testTask2["priority"] = QVariant(0);
    testInboxEntitiesList.push_back(testTask2);

    testInboxMap["entities"] = testInboxEntitiesList;

    testBoxMap["inbox"] = testInboxMap;

    return testBoxMap;
    // End test data version
}

int BdLogic::SaveDataToFile(QString &filename, int fileType)
{

    return 0;
}

void BdLogic::ReplyFinished()
{
    QString replyData(m_reply->readAll());

    if(m_replyGotError)
    {
        return;
    }

    if(m_dlState != DLSTATE_LOGIN)
    {
        QJson::Parser parser;
        bool parsedOk = true;

        m_boxRawJsonMap[BoxNames[m_dlState]] = m_reply->readAll();

        m_boxMap[BoxNames[m_dlState]] = parser.parse(m_reply->readAll(), &parsedOk);
        if(!parsedOk)
        {
            m_errorString = QString("Error parsing JSON from URL: ");
            m_errorString += DOIT_BASE_DATA_URL;
            m_errorString += BoxNames[m_dlState];
            // User doesn't really need to know line number of error

            m_statusCode = BDLOGIC_STATUS_JSON_PARSE_ERROR;

            m_reply->deleteLater();
            return;
        }

    }

    // After this, I can re-use my QNetworkReply pointer
    m_reply->deleteLater();

    m_dlState++;
    if(m_dlState != DLSTATE_FINISHED)
    {
        QNetworkRequest request;
        QString boxUrl(DOIT_BASE_DATA_URL);
        boxUrl += BoxNames[m_dlState];

        request.setUrl(boxUrl);
        m_reply = m_netManager->get(request);

        connect(m_reply, SIGNAL(finished()), this, SLOT(ReplyFinished()));
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(ReplyError(QNetworkReply::NetworkError)));
        connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(ReplySSLError(QList<QSslError>)));
    }
    else
    {
        m_statusCode = BDLOGIC_STATUS_OK;

        qDebug() << "Downloading finished";
    }

}

void BdLogic::ReplyError(QNetworkReply::NetworkError code)
{
    m_replyGotError = true;

    m_errorString = QString("Network error accessing URL: ");
    m_errorString += DOIT_BASE_DATA_URL;
    m_errorString += BoxNames[m_dlState];
    m_errorString += ", error: ";
    m_errorString += code;
    m_errorString += ": ";
    m_errorString += m_reply->errorString();

    m_statusCode = BDLOGIC_STATUS_NETWORK_ERROR;

    qDebug() << m_errorString;
}

void BdLogic::ReplySSLError(const QList<QSslError> & errors)
{
    m_replyGotError = true;

    m_errorString = QString("SSL network error accessing URL: ");
    m_errorString += DOIT_BASE_DATA_URL;
    m_errorString += BoxNames[m_dlState];
    m_errorString += ", error: ";
    m_errorString += m_reply->errorString();

    m_statusCode = BDLOGIC_STATUS_NETWORK_ERROR;

    qDebug() << m_errorString;
}


