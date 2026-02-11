#ifndef BOSCHSERVER_H
#define BOSCHSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

class BoschServer : public QObject
{
    Q_OBJECT
    // Expose active speakers to QML so wires turn red
    Q_PROPERTY(QString serverLog READ serverLog NOTIFY serverLogChanged)
    Q_PROPERTY(QVariantList activeSeats READ activeSeats NOTIFY activeSeatsChanged)

public:
    explicit BoschServer(QObject *parent = nullptr);
    Q_INVOKABLE void startServer(int port);

    // QML calls this when you click a simulated mic button
    Q_INVOKABLE void toggleMic(int seatId);

    Q_INVOKABLE void pressPriority(int seatId);

    QString serverLog() const { return m_log; }
    QVariantList activeSeats() const;

signals:
    void serverLogChanged();
    void activeSeatsChanged();

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *m_server;
    QString m_log;

    // Simulating Hardware State
    struct Seat {
        int id;
        QString name;
        bool isOn;
        bool isChairman;
    };
    QList<Seat> m_seats;

    void log(const QString &msg);
    void sendResponse(QTcpSocket *socket, const QString &status, const QByteArray &body);
    void handleRequest(QTcpSocket *socket, const QString &header, const QByteArray &body);
};

#endif // BOSCHSERVER_H
