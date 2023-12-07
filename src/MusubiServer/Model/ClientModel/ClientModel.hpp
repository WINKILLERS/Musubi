#ifndef CLIENT_MODEL_HPP
#define CLIENT_MODEL_HPP
#include "qobject.h"
#include <QAbstractTableModel>
#include <QNetworkAccessManager>
#include <unordered_map>

namespace Network {
class Handler;
class Session;
} // namespace Network

namespace Model {
class ClientModel : public QAbstractTableModel {
  Q_OBJECT;

public:
  enum ColumnItem : uint8_t {
    location = 0,
    address,
    computer_name,
    user_name,
    os_name,
    cpu_model,
    max_item
  };

  explicit ClientModel(Network::Handler *handler_, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override {
    return (int)ColumnItem::max_item;
  };

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  inline std::string getHwid(int row) const { return hwid_mapping[row]; };

  Network::Session *getClient(int row) const;

public slots:
  void clientConnected(Network::Session *client);
  void clientDisconnected(Network::Session *client);

private:
  struct Location {
    QString location;
    QString error;
  };
  static QString getHeaderText(const ColumnItem section);
  QVariant getData(const ColumnItem column, Network::Session *client) const;
  QVariant getDataTip(const ColumnItem column, Network::Session *client) const;
  Location getLocation(Network::Session *client) const;
  uint32_t getRow(Network::Session *client) const;

  void emitRowChanged(uint32_t row);
  void emitItemChanged(uint32_t row, uint32_t column);

  QVector<std::string> hwid_mapping;
  std::unordered_map<std::string /*hwid*/, Location /*location*/>
      location_mapping;

  Network::Handler *handler;

  QNetworkAccessManager *manager;

private slots:
  void onLocationQueryFinished(Network::Session *client, QNetworkReply *reply);
};
} // namespace Model
#endif