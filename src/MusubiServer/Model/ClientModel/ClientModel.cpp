#include "ClientModel.hpp"
#include "Network/Handler.hpp"
#include <QNetworkReply>
#include <algorithm>
#include <cassert>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

namespace Model {
ClientModel::ClientModel(Network::Handler *handler_, QObject *parent)
    : QAbstractTableModel(parent), handler(handler_) {
  manager = new QNetworkAccessManager(this);

  connect(handler, &Network::Handler::clientConnected, this,
          &ClientModel::clientConnected);
  connect(handler, &Network::Handler::clientDisconnected, this,
          &ClientModel::clientDisconnected);
}

int ClientModel::rowCount(const QModelIndex &parent) const {
  return hwid_mapping.size();
}

QVariant ClientModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      return getHeaderText((ColumnItem)section);
    } else if (role == Qt::TextAlignmentRole) {
      return Qt::AlignCenter;
    }
  }

  return QVariant();
}

QVariant ClientModel::data(const QModelIndex &index, int role) const {
  auto row = index.row();
  auto column = (ColumnItem)index.column();

  if (row > hwid_mapping.size() ||
      index.column() >= (int)ColumnItem::max_item) {
    return QVariant();
  }

  auto client = handler->getClient(hwid_mapping[row]);

  if (role == Qt::DisplayRole) {
    return getData(column, client);
  } else if (role == Qt::ToolTipRole) {
    return getDataTip(column, client);
  } else if (role == Qt::TextAlignmentRole) {
    return Qt::AlignCenter;
  }

  return QVariant();
}

Network::Session *ClientModel::getClient(int row) const {
  return handler->getClient(getHwid(row));
}

void ClientModel::clientConnected(Network::Session *client) {
  auto row = hwid_mapping.size();

  beginInsertRows(QModelIndex(), row, row);
  hwid_mapping.emplace_back(client->getHwid());
  endInsertRows();

  emitRowChanged(row);
}

void ClientModel::clientDisconnected(Network::Session *client) {
  auto pos =
      std::find(hwid_mapping.cbegin(), hwid_mapping.cend(), client->getHwid());
  auto row = pos - hwid_mapping.cbegin();

  beginRemoveRows(QModelIndex(), row, row);
  hwid_mapping.erase(pos);
  endRemoveRows();

  location_mapping.erase(client->getHwid());

  emitRowChanged(row);

  client->deleteLater();
}

QString ClientModel::getHeaderText(const ColumnItem section) {
  QString header;

  switch (section) {
  case ColumnItem::location:
    header = tr("Location");
    break;
  case ColumnItem::address:
    header = tr("Address");
    break;
  case ColumnItem::computer_name:
    header = tr("Computer Name");
    break;
  case ColumnItem::user_name:
    header = tr("User Name");
    break;
  case ColumnItem::os_name:
    header = tr("OS Name");
    break;
  case ColumnItem::cpu_model:
    header = tr("CPU Model");
    break;
  default:
    header = tr("Unknown Header");
    assert(false);
    break;
  }

  return header;
}

QVariant ClientModel::getData(const ColumnItem column,
                              Network::Session *client) const {
  QVariant data;

  switch (column) {
  case ColumnItem::location:
    data = getLocation(client).location;
    break;
  case ColumnItem::address:
    data = QString::fromStdString(client->getRemoteAddress());
    break;
  case ColumnItem::computer_name:
    data = QString::fromStdString(client->getComputerName());
    break;
  case ColumnItem::user_name:
    data = QString::fromStdString(client->getUserName());
    break;
  case ColumnItem::os_name:
    data = QString::fromStdString(client->getOsName());
    break;
  case ColumnItem::cpu_model:
    data = QString::fromStdString(client->getCpuModel());
    break;
  default:
    assert(false);
    break;
  }

  return data;
}

QVariant ClientModel::getDataTip(const ColumnItem column,
                                 Network::Session *client) const {
  QVariant data;

  switch (column) {
  case ColumnItem::location:
    data = getLocation(client).error;
    break;
  default:
    break;
  }

  return data;
}

ClientModel::Location ClientModel::getLocation(Network::Session *client) const {
  if (location_mapping.contains(client->getHwid())) {
    return location_mapping.at(client->getHwid());
  }

  auto reply = manager->get(QNetworkRequest(
      QString("http://ip-api.com/json/%1?lang=zh-CN")
          .arg(QString::fromStdString(client->getRemoteAddress()))));

  connect(reply, &QNetworkReply::finished, this, [this, client, reply]() {
    ((ClientModel *)this)->onLocationQueryFinished(client, reply);
  });

  return ClientModel::Location(tr("Unknown Location"),
                               tr("Query operation not completed"));
}

uint32_t ClientModel::getRow(Network::Session *client) const {
  auto pos =
      std::find(hwid_mapping.cbegin(), hwid_mapping.cend(), client->getHwid());
  return pos - hwid_mapping.cbegin();
}

void ClientModel::emitRowChanged(uint32_t row) {
  QModelIndex top_left = index(row, 0);
  QModelIndex bottom_right = index(row, columnCount() - 1);
  emit dataChanged(top_left, bottom_right);
}

void ClientModel::emitItemChanged(uint32_t row, uint32_t column) {
  QModelIndex top_left = index(row, column);
  QModelIndex bottom_right = index(row, column);
  emit dataChanged(top_left, bottom_right);
}

void ClientModel::onLocationQueryFinished(Network::Session *client,
                                          QNetworkReply *reply) {
  auto row = getRow(client);
  auto body = reply->readAll().toStdString();
  nlohmann::json json;

  try {
    json = nlohmann::json::parse(body);
  } catch (const std::exception &) {
    location_mapping[client->getHwid()] = ClientModel::Location(
        tr("Unknown Location"), tr("Failed to parse query json"));

    emitItemChanged(row, (uint32_t)ColumnItem::location);

    reply->deleteLater();
    return;
  }

  if (json["status"] != "success") {
    location_mapping[client->getHwid()] = ClientModel::Location(
        tr("Unknown Location"),
        tr("Query operation failed, message: %1")
            .arg(QString::fromStdString(json["message"].get<std::string>())));

    emitItemChanged(row, (uint32_t)ColumnItem::location);

    reply->deleteLater();
    return;
  }

  auto country = QString::fromStdString(json["country"]);
  auto region = QString::fromStdString(json["regionName"]);
  auto city = QString::fromStdString(json["city"]);

  location_mapping[client->getHwid()] = ClientModel::Location(
      QString("%1 %2 %3").arg(country).arg(region).arg(city), "");

  emitItemChanged(row, (uint32_t)ColumnItem::location);

  reply->deleteLater();
}
} // namespace Model