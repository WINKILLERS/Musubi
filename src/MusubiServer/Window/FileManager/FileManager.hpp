#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP
#include "Network/Session.hpp"
#include "ui_FileManager.h"
#include <QDialog>
#include <filesystem>
#include <stack>

namespace Window {
class FileManager : public QDialog {
  Q_OBJECT;

public:
  enum ColumnItem : uint8_t { name = 0, size, last_write_time, max_item };

  FileManager(Network::Session *session_, QWidget *parent = nullptr);
  ~FileManager();

  inline bool hasPrevious() { return previous_directories.empty() == false; }

  inline bool hasNext() { return next_directories.empty() == false; }

signals:
  void refreshing(const Bridge::RequestGetFiles packet);
  void removing(const Bridge::RequestRemoveFiles packet);

private:
  enum class Operation { previous, next, user_open };

  static QString getColumnText(const ColumnItem section);
  static QString getData(const Bridge::File &file, const ColumnItem section);
  static QString getTime(uint64_t timestamp);
  QIcon getIcon(const Bridge::File &file);
  std::vector<std::string> getSelections();

  Ui::FileManager *ui;
  Network::Session *session;
  std::filesystem::path current_directory;
  std::stack<std::filesystem::path> previous_directories;
  std::stack<std::filesystem::path> next_directories;
  Operation operation = Operation::user_open;
  std::vector<Bridge::File> files;
  QIcon folder_icon;
  QIcon file_icon;

private slots:
  void openDirectoryByRow(int row, int column);
  void openDirectoryByPath();
  void openRoot();
  void openAbsolute(const std::filesystem::path &path);
  void refresh();
  void previous();
  void next();
  void remove();
  DECLARE_SLOT(ResponseGetFiles);
  DECLARE_SLOT(ResponseRemoveFiles);

  void selectionChanged();
};
} // namespace Window
#endif
