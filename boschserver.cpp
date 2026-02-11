#include "boschserver.h"
#include <QRegularExpression>

BoschServer::BoschServer(QObject *parent) : QObject(parent) {
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &BoschServer::onNewConnection);

    // Initialize 10 Seats
    for(int i=1; i<=10; i++) {
        // LOGIC: Seat 1 is the Chairman
        bool isBoss = (i == 1);
        QString name = isBoss ? "Chairman" : QString("Seat %1").arg(i);

        // Pass 'isBoss' to our struct
        m_seats.append({i, name, false, isBoss});
    }
}

void BoschServer::startServer(int port) {
    if (m_server->listen(QHostAddress::Any, port)) {
        log(QString("Bosch Simulator Running. Listening on http://localhost:%1").arg(port));
        log("Ready to accept connections from App.");
    } else {
        log("ERROR: Could not start server! Port might be in use.");
    }
}

void BoschServer::log(const QString &msg) {
    QString time = QDateTime::currentDateTime().toString("HH:mm:ss");
    m_log.append(QString("[%1] %2\n").arg(time, msg));
    emit serverLogChanged();
}

void BoschServer::onNewConnection() {
    QTcpSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &BoschServer::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void BoschServer::onReadyRead() {
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QString request = QString::fromUtf8(data);
    QStringList lines = request.split("\r\n");
    if (lines.isEmpty()) return;

    QString header = lines.first();

    QByteArray body;
    int blankLineIndex = lines.indexOf("");
    if (blankLineIndex != -1 && blankLineIndex < lines.size() - 1) {
        body = request.split("\r\n\r\n").last().toUtf8();
    }

    handleRequest(socket, header, body);
}

void BoschServer::handleRequest(QTcpSocket *socket, const QString &header, const QByteArray &body) {
    // --- 1. LOGIN ---
    if (header.contains("POST /api/login")) {
        log("API LOGIN Request Received");
        QJsonObject response;
        response["sid"] = "simulated-session-id-123";
        response["id"] = 1;
        sendResponse(socket, "200 OK", QJsonDocument(response).toJson());
    }
    // --- 2. GET SEATS ---
    else if (header.contains("GET /api/seats")) {
        QJsonArray arr;
        for (const auto &seat : m_seats) {
            QJsonObject s;
            s["id"] = seat.id;
            s["name"] = seat.name;
            s["connected"] = true;
            arr.append(s);
        }
        sendResponse(socket, "200 OK", QJsonDocument(arr).toJson());
    }
    // --- 3. GET ACTIVE SPEAKERS ---
    else if (header.contains("GET /api/speakers")) {
        QJsonArray arr;
        for (const auto &seat : m_seats) {
            if (seat.isOn) {
                QJsonObject s;
                s["id"] = seat.id;
                s["name"] = seat.name;
                s["micOn"] = true;
                arr.append(s);
            }
        }
        sendResponse(socket, "200 OK", QJsonDocument(arr).toJson());
    }
    // --- 4. TURN OFF MIC (DELETE) ---
    else if (header.contains("DELETE /api/speakers")) {
        QRegularExpression re("DELETE /api/speakers/(\\d+)");
        QRegularExpressionMatch match = re.match(header);

        if (match.hasMatch()) {
            int idToRemove = match.captured(1).toInt();

            // FIX: Manually turn off instead of calling toggleMic
            // This ensures DELETE always sets state to FALSE (not toggle)
            bool found = false;
            for(int i=0; i<m_seats.size(); ++i) {
                if(m_seats[i].id == idToRemove) {
                    if (m_seats[i].isOn) {
                        m_seats[i].isOn = false;
                        emit activeSeatsChanged();
                        log(QString("API turned OFF Mic %1").arg(idToRemove));
                    }
                    found = true;
                    break;
                }
            }

            if (found) sendResponse(socket, "200 OK", "{}");
            else sendResponse(socket, "404 Not Found", "{}");

        } else {
            sendResponse(socket, "400 Bad Request", "{}");
        }
    }
    // --- 5. CORS PREFLIGHT ---
    else if (header.contains("OPTIONS")) {
        sendResponse(socket, "204 No Content", "");
    }
    else if (header.contains("GET /api/seats")) {
        QJsonArray arr;
        for (const auto &seat : m_seats) {
            QJsonObject s;
            s["id"] = seat.id;
            s["name"] = seat.name;
            s["prio"] = seat.isChairman; // <--- TELL APP THIS IS CHAIRMAN
            s["connected"] = true;
            arr.append(s);
        }
        sendResponse(socket, "200 OK", QJsonDocument(arr).toJson());
    }
    else {
        sendResponse(socket, "404 Not Found", "{}");
    }
}

void BoschServer::sendResponse(QTcpSocket *socket, const QString &status, const QByteArray &body) {
    QByteArray response;
    response.append(QString("HTTP/1.1 %1\r\n").arg(status).toUtf8());
    response.append("Content-Type: application/json\r\n");
    response.append("Access-Control-Allow-Origin: *\r\n");
    response.append("Access-Control-Allow-Methods: GET, POST, DELETE, OPTIONS\r\n");
    response.append("Access-Control-Allow-Headers: Content-Type, Bosch-Sid, Cookie\r\n");
    response.append(QString("Content-Length: %1\r\n").arg(body.size()).toUtf8());
    response.append("\r\n");
    response.append(body);

    socket->write(response);
}

// FIX: Removed 'QString name' parameter and logic
void BoschServer::toggleMic(int seatId) {
    for (int i=0; i<m_seats.size(); ++i) {
        if (m_seats[i].id == seatId) {
            m_seats[i].isOn = !m_seats[i].isOn;
            log(QString("Physical Button: Mic %1 is now %2").arg(seatId).arg(m_seats[i].isOn ? "ON" : "OFF"));
            emit activeSeatsChanged();
            break;
        }
    }
}

void BoschServer::pressPriority(int seatId) {
    bool somethingChanged = false;

    // Iterate through all seats
    for (int i = 0; i < m_seats.size(); ++i) {
        // If this is the chairman who pressed the button:
        if (m_seats[i].id == seatId && m_seats[i].isChairman) {
            if (!m_seats[i].isOn) {
                m_seats[i].isOn = true; // Force Chairman ON
                somethingChanged = true;
            }
        }
        // If this is anyone else:
        else {
            if (m_seats[i].isOn) {
                m_seats[i].isOn = false; // Force everyone else OFF
                somethingChanged = true;
            }
        }
    }

    if (somethingChanged) {
        log(QString("PRIORITY PRESSED! Chairman (Seat %1) took control.").arg(seatId));
        emit activeSeatsChanged();
    }
}

QVariantList BoschServer::activeSeats() const {
    QVariantList list;
    for (const auto &seat : m_seats) {
        if (seat.isOn) list.append(seat.id);
    }
    return list;
}
