
#include <QDebug>
#include <QFile>
#include <qjson/parser.h>

#include "bdlogic.h"

#define DOIT_LOGIN_URL           "https://i.doit.im/signin"
#define DOIT_RESOURCES_DATA_URL  "https://i.doit.im/api/resources_init"
#define DOIT_BASE_DATA_URL       "https://i.doit.im/api/tasks/"

enum
{
    DLSTATE_LOGIN,
    DLSTATE_RESOURCES,
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
    "resources",
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
    m_actionListOrderedForQML.clear();
    m_boxMapParsedJson.clear();
    m_boxMapRawJson.clear();
    m_replyGotError = false;

    loginPostData = "username=";
    loginPostData += username;
    loginPostData += "&password=";
    loginPostData += password;

    QNetworkRequest request;
    request.setUrl(QUrl(DOIT_LOGIN_URL));
    m_reply = m_netManager->post(request, loginPostData.toUtf8());

    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(replyError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(replySSLError(QList<QSslError>)));

    return 0;
}

void BdLogic::replyFinished()
{
    QByteArray replyData(m_reply->readAll());

    if(m_replyGotError)
    {
        return;
    }

    qDebug() << BoxNames[m_dlState] << " reply body:\n" << replyData;

    if(m_dlState != DLSTATE_LOGIN)
    {
        QJson::Parser parser;
        bool parsedOk = true;

        m_boxMapRawJson[BoxNames[m_dlState]] = replyData;

        m_boxMapParsedJson[BoxNames[m_dlState]] = parser.parse(replyData, &parsedOk);
        if(!parsedOk)
        {
            m_errorString = QString("Error parsing JSON from URL: ");

            if(m_dlState == DLSTATE_RESOURCES)
            {
                m_errorString += DOIT_RESOURCES_DATA_URL;
            }
            else
            {
                m_errorString += DOIT_BASE_DATA_URL;
                m_errorString += BoxNames[m_dlState];
            }
            // User doesn't really need to know line number of error,
            // but print during development:
            qDebug() << m_errorString << ": " << parser.errorString() << ", on line: " << parser.errorLine();

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
        QString boxUrl;
        if(m_dlState == DLSTATE_RESOURCES)
        {
            boxUrl = DOIT_RESOURCES_DATA_URL;
        }
        else
        {
            boxUrl = DOIT_BASE_DATA_URL;
            boxUrl += BoxNames[m_dlState];
        }
        request.setUrl(boxUrl);
        m_reply = m_netManager->get(request);

        connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(replyError(QNetworkReply::NetworkError)));
        connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)),
                this, SLOT(replySSLError(QList<QSslError>)));
    }
    else
    {
        m_statusCode = BDLOGIC_STATUS_OK;

        SetDataModelOrdering(0);

        // TODO: Emit a "download completed" signal.

        qDebug() << "Downloading finished";
    }

}

void BdLogic::replyError(QNetworkReply::NetworkError code)
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

void BdLogic::replySSLError(const QList<QSslError> & errors)
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

int BdLogic::SetDataModelOrdering(int order)
{
//    QString boxName;
    QVariantMap actionMapFromJson;
    QVariantList actionListFromJson;
    QVariantMap actionFromJson;
    QVariantMap actionForQml;

    m_actionListOrderedForQML.clear();

    switch(order)
    {
    case BdLogic::OrderByBox:
    default:
        for(int boxIx = DLSTATE_INBOX; boxIx < DLSTATE_FINISHED; boxIx++)
        {
//            boxName = BoxNames[boxIx];
            actionMapFromJson = m_boxMapParsedJson[BoxNames[boxIx]].toMap();
            actionListFromJson = actionMapFromJson["entities"].toList();
            //actionListFromJson = m_boxMapParsedJson[BoxNames[boxIx]]["entities"];

            actionForQml.clear();
            actionForQml["itemType"] = 1;
            actionForQml["name"] = BoxNames[boxIx];
            m_actionListOrderedForQML.push_back(actionForQml);

            for(int actionIx = 0; actionIx < actionListFromJson.length(); actionIx++)
            {
                actionFromJson = actionListFromJson[actionIx].toMap();

                actionForQml.clear();
                actionForQml["itemType"] = 0;
                actionForQml["name"] = actionFromJson["title"];
                actionForQml["box"] = actionFromJson["attribute"];
                actionForQml["project"] = getProjectNameFromJsonAction(actionFromJson);
                actionForQml["context"] = getContextNameFromJsonAction(actionFromJson);
                actionForQml["priority"] = actionFromJson["priority"].toInt();
                m_actionListOrderedForQML.push_back(actionForQml);

            }
        }
        break;
    case BdLogic::OrderByContext:
        // TODO:
        qDebug() << "BdLogic::OrderByContext not done yet";
        break;
    case BdLogic::OrderByProject:
        // TODO:
        qDebug() << "BdLogic::OrderByProject not done yet";
        break;
    case BdLogic::OrderByPriority:
        // TODO:
        qDebug() << "BdLogic::OrderByPriority not done yet";
        break;
    }

    return 0;
}

QString BdLogic::getProjectNameFromJsonAction(QVariantMap actionFromJson)
{
    QVariantMap resourcesMapFromJson;
    QVariantList projectListFromJson;
    QVariantMap projectFromJson;

    resourcesMapFromJson = m_boxMapParsedJson[BoxNames[DLSTATE_RESOURCES]].toMap();
    resourcesMapFromJson = resourcesMapFromJson["resources"].toMap();
    projectListFromJson = resourcesMapFromJson["projects"].toList();

    for(int projectIx = 0; projectIx < projectListFromJson.length(); projectIx++)
    {
        projectFromJson = projectListFromJson[projectIx].toMap();

        if(actionFromJson["project"].toString() == projectFromJson["uuid"].toString())
        {
            return projectFromJson["name"].toString();
        }
    }

    return QString("");
}

QString BdLogic::getContextNameFromJsonAction(QVariantMap actionFromJson)
{
    QVariantMap resourcesMapFromJson;
    QVariantList contextListFromJson;
    QVariantMap contextFromJson;

    resourcesMapFromJson = m_boxMapParsedJson[BoxNames[DLSTATE_RESOURCES]].toMap();
    resourcesMapFromJson = resourcesMapFromJson["resources"].toMap();
    contextListFromJson = resourcesMapFromJson["contexts"].toList();

    for(int contextIx = 0; contextIx < contextListFromJson.length(); contextIx++)
    {
        contextFromJson = contextListFromJson[contextIx].toMap();

        if(actionFromJson["context"].toString() == contextFromJson["uuid"].toString())
        {
            return contextFromJson["name"].toString();
        }
    }

    return QString("");
}

QVariantList BdLogic::GetDataModel()
{
    return m_actionListOrderedForQML;
}

int BdLogic::SaveDataToFile(QString filename, int fileType)
{

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return BDLOGIC_STATUS_FILE_ERROR;

    QTextStream out(&file);

    switch(fileType)
    {
    case BdLogic::FileTypeOrderedList:
    default:
        // TODO:
        qDebug() << "BdLogic::FileTypeOrderedList not done yet";

        for(int actionIx = 0; actionIx < m_actionListOrderedForQML.length(); actionIx++)
        {
            QVariantMap actionForQml = m_actionListOrderedForQML[actionIx].toMap();

            if(actionForQml["itemType"].toInt())
            {
                out << actionForQml["name"].toString() << ":\n";
            }
            else
            {
                out << "Name:     " << actionForQml["name"].toString() << "\n";
                out << "Box:      " << actionForQml["box"].toString() << "\n";
                out << "Project:  " << actionForQml["project"].toString() << "\n";
                out << "Context:  " << actionForQml["name"].toString() << "\n";
                out << "Priority: " << actionForQml["name"].toString() << "\n\n";
            }
        }

        break;
    case BdLogic::FileTypeJson:
        for(int boxIx = DLSTATE_INBOX; boxIx < DLSTATE_FINISHED; boxIx++)
        {
            out << BoxNames[boxIx] << ":\n" << m_boxMapRawJson[BoxNames[boxIx]] << "\n\n";
        }
        break;
    }

    file.close();

    return BDLOGIC_STATUS_OK;
}

