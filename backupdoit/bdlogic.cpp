
#include <QDebug>
#include <QFile>
#include <QFileDialog>
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
    m_statusString = QString("Logging in");
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

    emit downloadStatusUpdated(m_statusCode, m_statusString);

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

    // TODO: if state is login, check if it worked ....

    if(m_dlState != DLSTATE_LOGIN)
    {
        QJson::Parser parser;
        bool parsedOk = true;

        m_boxMapRawJson[BoxNames[m_dlState]] = replyData;

        m_boxMapParsedJson[BoxNames[m_dlState]] = parser.parse(replyData, &parsedOk);
        if(!parsedOk)
        {
            m_statusString = QString("Error parsing JSON from URL: ");

            if(m_dlState == DLSTATE_RESOURCES)
            {
                m_statusString += DOIT_RESOURCES_DATA_URL;
            }
            else
            {
                m_statusString += DOIT_BASE_DATA_URL;
                m_statusString += BoxNames[m_dlState];
            }
            // User doesn't really need to know line number of error,
            // but print during development:
            qDebug() << m_statusString << ": " << parser.errorString() << ", on line: " << parser.errorLine();

            m_statusCode = BDLOGIC_STATUS_JSON_PARSE_ERROR;

            emit downloadStatusUpdated(m_statusCode, m_statusString);

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

        m_statusString = QString("Downloading data for box: ");
        m_statusString += BoxNames[m_dlState];

        emit downloadStatusUpdated(m_statusCode, m_statusString);
    }
    else
    {
        m_statusCode = BDLOGIC_STATUS_DOWNLOAD_FINISHED;
        m_statusString = QString("Downloading finished");
        SetDataModelOrdering(0);

        emit downloadStatusUpdated(m_statusCode, m_statusString);
        qDebug() << m_statusString;
    }

}

void BdLogic::replyError(QNetworkReply::NetworkError code)
{
    m_replyGotError = true;

    m_statusString = QString("Network error accessing URL: ");
    m_statusString += DOIT_BASE_DATA_URL;
    m_statusString += BoxNames[m_dlState];
    m_statusString += ", error: ";
    m_statusString += code;
    m_statusString += ": ";
    m_statusString += m_reply->errorString();

    m_statusCode = BDLOGIC_STATUS_NETWORK_ERROR;

    emit downloadStatusUpdated(m_statusCode, m_statusString);
    qDebug() << m_statusString;
}

void BdLogic::replySSLError(const QList<QSslError> & errors)
{
    m_replyGotError = true;

    m_statusString = QString("SSL network error accessing URL: ");
    m_statusString += DOIT_BASE_DATA_URL;
    m_statusString += BoxNames[m_dlState];
    m_statusString += ", error: ";
    m_statusString += m_reply->errorString();

    m_statusCode = BDLOGIC_STATUS_NETWORK_ERROR;

    emit downloadStatusUpdated(m_statusCode, m_statusString);
    qDebug() << m_statusString;
}

int BdLogic::SetDataModelOrdering(int order)
{
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
            actionMapFromJson = m_boxMapParsedJson[BoxNames[boxIx]].toMap();
            actionListFromJson = actionMapFromJson["entities"].toList();

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
        {
            QVariantMap resourcesMapFromJson;
            QVariantList contextListFromJson;
            QVariantMap contextFromJson;

            resourcesMapFromJson = m_boxMapParsedJson[BoxNames[DLSTATE_RESOURCES]].toMap();
            resourcesMapFromJson = resourcesMapFromJson["resources"].toMap();
            contextListFromJson = resourcesMapFromJson["contexts"].toList();

            // Add an extra context entry with an empty name string
            // to match against actions with no context
            contextFromJson["name"] = QString("No context");
            contextListFromJson.push_back(contextFromJson);

            for(int contextIx = 0; contextIx < contextListFromJson.length(); contextIx++)
            {
                contextFromJson = contextListFromJson[contextIx].toMap();

                actionForQml.clear();
                actionForQml["itemType"] = 1;
                actionForQml["name"] = contextFromJson["name"].toString();
                m_actionListOrderedForQML.push_back(actionForQml);

                for(int boxIx = DLSTATE_INBOX; boxIx < DLSTATE_FINISHED; boxIx++)
                {
                    actionMapFromJson = m_boxMapParsedJson[BoxNames[boxIx]].toMap();
                    actionListFromJson = actionMapFromJson["entities"].toList();

                    for(int actionIx = 0; actionIx < actionListFromJson.length(); actionIx++)
                    {
                        actionFromJson = actionListFromJson[actionIx].toMap();

                        if( (actionFromJson["context"].toString() == contextFromJson["uuid"].toString()) ||
                            ((actionIx == contextListFromJson.length()) && (actionFromJson["context"].toString() == QString())) )
                        {
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
                }
            }
        }
        break;
    case BdLogic::OrderByProject:
        {
            QVariantMap resourcesMapFromJson;
            QVariantList projectListFromJson;
            QVariantMap projectFromJson;

            resourcesMapFromJson = m_boxMapParsedJson[BoxNames[DLSTATE_RESOURCES]].toMap();
            resourcesMapFromJson = resourcesMapFromJson["resources"].toMap();
            projectListFromJson = resourcesMapFromJson["projects"].toList();

            // Add an extra project entry with an empty name string
            // to match against actions with no project
            projectFromJson["name"] = QString("No project");
            projectListFromJson.push_back(projectFromJson);

            for(int projectIx = 0; projectIx < projectListFromJson.length(); projectIx++)
            {
                projectFromJson = projectListFromJson[projectIx].toMap();

                actionForQml.clear();
                actionForQml["itemType"] = 1;
                actionForQml["name"] = projectFromJson["name"].toString();
                m_actionListOrderedForQML.push_back(actionForQml);

                for(int boxIx = DLSTATE_INBOX; boxIx < DLSTATE_FINISHED; boxIx++)
                {
                    actionMapFromJson = m_boxMapParsedJson[BoxNames[boxIx]].toMap();
                    actionListFromJson = actionMapFromJson["entities"].toList();

                    for(int actionIx = 0; actionIx < actionListFromJson.length(); actionIx++)
                    {
                        actionFromJson = actionListFromJson[actionIx].toMap();

                        if( (actionFromJson["project"].toString() == projectFromJson["uuid"].toString()) ||
                            ((actionIx == projectListFromJson.length()) && (actionFromJson["project"].toString() == QString())) )
                        {
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
                }
            }
        }
        break;
    case BdLogic::OrderByPriority:
        for(int priority = 3; priority >= 0; priority--)
        {
            actionForQml.clear();
            actionForQml["itemType"] = 1;
            QString prioHeaderName("Priority: ");
            switch(priority)
            {
            case 3:
                prioHeaderName += "High";
                break;
            case 2:
                prioHeaderName += "Medium";
                break;
            case 1:
                prioHeaderName += "Low";
                break;
            case 0:
            default:
                prioHeaderName += "None";
                break;
            }
            actionForQml["name"] = prioHeaderName;
            m_actionListOrderedForQML.push_back(actionForQml);

            for(int boxIx = DLSTATE_INBOX; boxIx < DLSTATE_FINISHED; boxIx++)
            {
                actionMapFromJson = m_boxMapParsedJson[BoxNames[boxIx]].toMap();
                actionListFromJson = actionMapFromJson["entities"].toList();

                for(int actionIx = 0; actionIx < actionListFromJson.length(); actionIx++)
                {
                    actionFromJson = actionListFromJson[actionIx].toMap();

                    if( actionFromJson["priority"].toInt() == priority )
                    {
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
            }
        }
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

QString BdLogic::GetSaveFileName(int fileType)
{
    QString selectedFilter;

    switch(fileType)
    {
    case BdLogic::FileTypeOrderedList:
    default:
        selectedFilter = "Ordered list (*.txt)";
        break;
    case BdLogic::FileTypeJson:
        selectedFilter = "Raw JSON (*.json)";
        break;
    }

    return QFileDialog::getSaveFileName(this, tr("Save File"),
                                "",
                                tr("Ordered list (*.txt);;Raw JSON (*.json)"),
                                &selectedFilter);
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

