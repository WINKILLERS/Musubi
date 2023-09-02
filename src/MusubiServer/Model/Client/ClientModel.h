#pragma once
#include "Network/Controller.h"
#include "Network/HeartbeatChannel.h"
#include "Protocols.h"
#include "qabstractitemmodel.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkreply.h"


namespace Model {
namespace Client {
class ClientModel : public QAbstractTableModel {
  Q_OBJECT

public:
  explicit ClientModel(QObject *parent = nullptr);
  ~ClientModel();

  Network::Controller *getController(size_t row);

public slots:
  void addClient(Network::Controller *controller);
  void deleteClient(Network::Controller *controller);
  void updateClientLag(uint32_t lag);
  void updateClientInfo(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseInformation> packet);
  void updateHeartbeat(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::ResponseHeartbeat> packet);
  void updateProcess(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::ResponseGetProcess> packet);
  void onDoubleClicked(const QModelIndex &index_);

protected:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

private slots:
  void onQueryLocationCompleted(QNetworkReply *reply);

private:
  struct client_info {
    QString location;
    QString address;
    QString cpu_model;
    QString os_name;
    QString user_name;
    QString computer_name;
    QString current_window;
    QString note;
    QString anti_virus;
    QString online_at;
    qint32 lag;
    Network::Controller *controller;
  };
  static constexpr uint16_t member_count = 11;

  void updateClient(const client_info &info);
  void readNote();
  void saveNote();
  void emitRowChanged(size_t row);
  std::optional<size_t> getRow(Network::Controller *controller);

  QList<client_info> container;

  QNetworkAccessManager *network;

  std::map<QNetworkReply *, Network::Controller *> pending_query;
  std::unordered_map<std::string, std::string> note;
};
} // namespace Client
} // namespace Model