#pragma once
#include "Protocols.h"
#include "Util/Util.h"
#include "qdialog.h"
#include "qstandarditemmodel.h"
#include "ui_ViewFile.h"

// Forward declare
namespace Network {
class FileChannel;
}

namespace Window {
namespace Control {
class ViewFile : public QDialog {
  Q_OBJECT

public:
  ViewFile(Network::FileChannel *channel, QWidget *parent = nullptr);
  ~ViewFile();

private slots:
  void update();
  void open(const QModelIndex &index);
  void download();
  void refresh();
  void upload();
  void remove();
  void execute();
  void forward();
  void back();

private:
  Ui::ViewFile *ui = nullptr;
  Network::FileChannel *channel = nullptr;

  std::vector<Packet::File> list;
  QStandardItemModel *model;

  QIcon ico_file;
  QIcon ico_folder;

  std::optional<Packet::File> getSelection();
};
} // namespace Control
} // namespace Window