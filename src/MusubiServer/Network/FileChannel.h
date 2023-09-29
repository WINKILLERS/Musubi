#pragma once
#include "AbstractChannel.h"
#include "Protocols.h"
#include "filesystem"
#include "qfuture.h"
#include "stack"

namespace Window {
namespace Control {
class ViewFile;
}
} // namespace Window

namespace Network {
// Forward declare
class AbstractSession;
class Controller;

class FileChannel : public AbstractChannel {
  Q_OBJECT

public:
  FileChannel(AbstractSession *session, AbstractChannel *parent);
  virtual ~FileChannel();

signals:
  void updateCompleted();

  CHANNEL_FORWARD signals
      : void
        recvDeleteFile(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::ResponseDeleteFile> packet);
  void recvDownloadFile(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseDownloadFile> packet);
  void recvExecuteFile(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::ResponseExecuteFile> packet);
  void recvQueryFile(std::shared_ptr<Packet::Header> header,
                     std::shared_ptr<Packet::ResponseQueryFile> packet);
  void recvUploadFile(std::shared_ptr<Packet::Header> header,
                      std::shared_ptr<Packet::ResponseUploadFile> packet);

public slots:
  bool refresh();
  // directory_name: UTF-8 string
  bool open(const std::string &directory_name);
  // Open root directory
  void open();
  // Open absolute path
  void openAbsolute(const std::filesystem::path &path);
  // Go back previous directory
  bool previous();
  // Go back next directory
  bool next();
  // Go back previous directory
  bool hasPrevious();
  // Go back next directory
  bool hasNext();

  // file_name: UTF-8 string
  std::optional<QFuture<std::shared_ptr<Packet::ResponseDownloadFile>>>
  download(const std::string &file_name);
  // Download absolute path
  QFuture<std::shared_ptr<Packet::ResponseDownloadFile>>
  downloadAbsolute(const std::filesystem::path &path);

  // file_name: UTF-8 string
  std::optional<QFuture<std::shared_ptr<Packet::ResponseUploadFile>>>
  upload(const std::wstring &local_path, const std::string &file_name);
  // Upload absolute path
  std::optional<QFuture<std::shared_ptr<Packet::ResponseUploadFile>>>
  uploadAbsolute(const std::wstring &local_path,
                 const std::filesystem::path &path);

  // file_name: UTF-8 string
  std::optional<QFuture<std::shared_ptr<Packet::ResponseExecuteFile>>>
  execute(const std::string &file_name);
  // Execute absolute path
  QFuture<std::shared_ptr<Packet::ResponseExecuteFile>>
  executeAbsolute(const std::filesystem::path &path);

  // file_name: UTF-8 string
  std::optional<QFuture<std::shared_ptr<Packet::ResponseDeleteFile>>>
  remove(const std::string &file_name);
  // Remove absolute path
  QFuture<std::shared_ptr<Packet::ResponseDeleteFile>>
  removeAbsolute(const std::filesystem::path &path);

  std::vector<Packet::File> getDirectories() const noexcept {
    return directories;
  }
  std::vector<Packet::File> getFiles() const noexcept { return files; }
  std::string getCurrentDirectory() const;

  bool showWindow() noexcept override;

private slots:
  void update(std::shared_ptr<Packet::Header> header,
              std::shared_ptr<Packet::ResponseQueryFile> packet);
  void downloadCompleted(std::shared_ptr<Packet::Header> header,
                         std::shared_ptr<Packet::ResponseDownloadFile> packet);
  void uploadCompleted(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::ResponseUploadFile> packet);
  void executeCompleted(std::shared_ptr<Packet::Header> header,
                        std::shared_ptr<Packet::ResponseExecuteFile> packet);
  void removeCompleted(std::shared_ptr<Packet::Header> header,
                       std::shared_ptr<Packet::ResponseDeleteFile> packet);

private:
  enum class Operation { previous, next, user_open };

  std::vector<Packet::File> directories;
  std::vector<Packet::File> files;
  std::filesystem::path current_directory;
  std::stack<std::filesystem::path> previous_directories;
  std::stack<std::filesystem::path> next_directories;
  Operation operation = Operation::user_open;

  std::unordered_map<std::string,
                     QPromise<std::shared_ptr<Packet::ResponseDownloadFile>>>
      download_tasks;
  std::unordered_map<std::string,
                     QPromise<std::shared_ptr<Packet::ResponseUploadFile>>>
      upload_tasks;
  std::unordered_map<std::string,
                     QPromise<std::shared_ptr<Packet::ResponseExecuteFile>>>
      execute_tasks;
  std::unordered_map<std::string,
                     QPromise<std::shared_ptr<Packet::ResponseDeleteFile>>>
      remove_tasks;

  static void sortCaseInsensitive(std::vector<Packet::File> &files);
};
} // namespace Network