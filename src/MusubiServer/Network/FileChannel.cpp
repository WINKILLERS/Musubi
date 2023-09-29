#include "FileChannel.h"
#include "AApch.h"
#include "AbstractProtocol.h"
#include "Util/Util.h"
#include "Window/Control/File/ViewFile.h"
#include "fstream"

Network::FileChannel::FileChannel(AbstractSession *session,
                                  AbstractChannel *parent)
    : AbstractChannel(session, parent) {
  connect(&session->notifier, &IPacketNotify::recvDeleteFile, this,
          &FileChannel::recvDeleteFile);
  connect(&session->notifier, &IPacketNotify::recvDownloadFile, this,
          &FileChannel::recvDownloadFile);
  connect(&session->notifier, &IPacketNotify::recvExecuteFile, this,
          &FileChannel::recvExecuteFile);
  connect(&session->notifier, &IPacketNotify::recvQueryFile, this,
          &FileChannel::recvQueryFile);
  connect(&session->notifier, &IPacketNotify::recvUploadFile, this,
          &FileChannel::recvUploadFile);

  // Connect private slots
  connect(&session->notifier, &IPacketNotify::recvDownloadFile, this,
          &FileChannel::downloadCompleted);
  connect(&session->notifier, &IPacketNotify::recvUploadFile, this,
          &FileChannel::uploadCompleted);
  connect(&session->notifier, &IPacketNotify::recvExecuteFile, this,
          &FileChannel::executeCompleted);
  connect(&session->notifier, &IPacketNotify::recvDeleteFile, this,
          &FileChannel::removeCompleted);
  connect(&session->notifier, &IPacketNotify::recvQueryFile, this,
          &FileChannel::update);
}

Network::FileChannel::~FileChannel() {
  directories.clear();
  files.clear();
  current_directory.clear();
}

bool Network::FileChannel::refresh() {
  return session->sendJsonPacket(Packet::Generator<Packet::RequestQueryFile>(
      current_directory.u8string()));
}

bool Network::FileChannel::open(const std::string &directory_name) {
  // Check if we need to go back root
  if (current_directory == current_directory.root_path() &&
      directory_name == "..") {
    // Open the root
    open();
    return true;
  }

  // Check if directory_name is in directories
  auto find_ret = std::find_if(
      directories.cbegin(), directories.cend(),
      [&](const Packet::File &f) { return f.name == directory_name; });
  if (find_ret == directories.cend()) {
    spdlog::error("trying to open a non-exist directory");
    return false;
  }

  auto absolute = current_directory / utf8to8(directory_name);
  absolute = absolute.lexically_normal();
  openAbsolute(absolute);

  return true;
}

void Network::FileChannel::open() { openAbsolute(""); }

void Network::FileChannel::openAbsolute(const std::filesystem::path &path) {
  // Clear next directories
  next_directories = std::stack<std::filesystem::path>();

  previous_directories.push(current_directory);
  current_directory = path;
  operation = Operation::user_open;
  session->sendJsonPacket(Packet::Generator<Packet::RequestQueryFile>(
      current_directory.u8string()));
}

bool Network::FileChannel::previous() {
  // If we have no step back
  if (previous_directories.empty()) {
    spdlog::error("can not step back directory");
    return false;
  }

  // Save current directory
  next_directories.push(current_directory);

  // Pop previous directory
  current_directory = previous_directories.top();
  previous_directories.pop();

  // Request update
  operation = Operation::previous;
  session->sendJsonPacket(Packet::Generator<Packet::RequestQueryFile>(
      current_directory.u8string()));

  return true;
}

bool Network::FileChannel::next() {
  // If we have no step next
  if (next_directories.empty()) {
    spdlog::error("can not step next directory");
    return false;
  }
  // Save current directory
  previous_directories.push(current_directory);

  // Pop next directory
  current_directory = next_directories.top();
  next_directories.pop();

  // Request update
  operation = Operation::next;
  session->sendJsonPacket(Packet::Generator<Packet::RequestQueryFile>(
      current_directory.u8string()));

  return true;
}

bool Network::FileChannel::hasPrevious() {
  return previous_directories.empty() == false;
}

bool Network::FileChannel::hasNext() {
  return next_directories.empty() == false;
}

std::optional<QFuture<std::shared_ptr<Packet::ResponseDownloadFile>>>
Network::FileChannel::download(const std::string &file_name) {
  // Check if file_name is in files
  auto find_ret =
      std::find_if(files.cbegin(), files.cend(),
                   [&](const Packet::File &f) { return f.name == file_name; });
  if (find_ret == files.cend()) {
    spdlog::error("trying to download a non-exist file");
    return std::nullopt;
  }

  auto absolute = current_directory / utf8to8(file_name);
  return downloadAbsolute(absolute);
}

QFuture<std::shared_ptr<Packet::ResponseDownloadFile>>
Network::FileChannel::downloadAbsolute(const std::filesystem::path &path) {

  auto generator =
      Packet::Generator<Packet::RequestDownloadFile>(utf8to8(path.u8string()));

  auto id = generator.getId();
  auto &task = download_tasks[id];

  session->sendJsonPacket(generator);

  return task.future();
}

std::optional<QFuture<std::shared_ptr<Packet::ResponseUploadFile>>>
Network::FileChannel::upload(const std::wstring &local_path,
                             const std::string &file_name) {
  auto absolute = current_directory / utf8to8(file_name);

  return uploadAbsolute(local_path, absolute);
}

std::optional<QFuture<std::shared_ptr<Packet::ResponseUploadFile>>>
Network::FileChannel::uploadAbsolute(const std::wstring &local_path,
                                     const std::filesystem::path &path) {
  // Check if we can upload file here
  if (current_directory.empty()) {
    spdlog::error("trying to upload to root directory");
    return std::nullopt;
  }

  // Open local file
  std::ifstream file(local_path, std::ios::binary);
  if (file.is_open() == false) {
    spdlog::error("can not open the file to be uploaded");
    return std::nullopt;
  }

  // Get local file size
  file.seekg(0, std::ios::end);
  auto file_size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Allocate memory
  std::string buffer;
  buffer.resize(file_size);

  // Read file
  file.read(buffer.data(), file_size);

  // Construct packet
  auto generator = Packet::Generator<Packet::RequestUploadFile>(
      utf8to8(path.u8string()), std::move(buffer));

  // Set up task
  auto id = generator.getId();
  auto &task = upload_tasks[id];

  session->sendJsonPacket(generator);

  return task.future();
}

std::optional<QFuture<std::shared_ptr<Packet::ResponseExecuteFile>>>
Network::FileChannel::execute(const std::string &file_name) {
  bool is_found = false;

  // Check if file_name is in files
  auto find_ret =
      std::find_if(files.cbegin(), files.cend(),
                   [&](const Packet::File &f) { return f.name == file_name; });
  if (find_ret != files.cend()) {
    is_found = true;
  }

  // Check if file_name is in directories
  find_ret =
      std::find_if(directories.cbegin(), directories.cend(),
                   [&](const Packet::File &f) { return f.name == file_name; });
  if (find_ret != directories.cend()) {
    is_found = true;
  }

  if (is_found == false) {
    spdlog::error("trying to execute a non-exist file or directory");
    return std::nullopt;
  }

  auto absolute = current_directory / utf8to8(file_name);
  return executeAbsolute(absolute);
}

QFuture<std::shared_ptr<Packet::ResponseExecuteFile>>
Network::FileChannel::executeAbsolute(const std::filesystem::path &path) {

  auto generator =
      Packet::Generator<Packet::RequestExecuteFile>(utf8to8(path.u8string()));

  auto id = generator.getId();
  auto &task = execute_tasks[id];

  session->sendJsonPacket(generator);

  return task.future();
}

std::optional<QFuture<std::shared_ptr<Packet::ResponseDeleteFile>>>
Network::FileChannel::remove(const std::string &file_name) {
  bool is_found = false;

  // Check if file_name is in files
  auto find_ret =
      std::find_if(files.cbegin(), files.cend(),
                   [&](const Packet::File &f) { return f.name == file_name; });
  if (find_ret != files.cend()) {
    is_found = true;
  }

  // Check if file_name is in directories
  find_ret =
      std::find_if(directories.cbegin(), directories.cend(),
                   [&](const Packet::File &f) { return f.name == file_name; });
  if (find_ret != directories.cend()) {
    is_found = true;
  }

  if (is_found == false) {
    spdlog::error("trying to remove a non-exist file or directory");
    return std::nullopt;
  }

  auto absolute = current_directory / utf8to8(file_name);
  return removeAbsolute(absolute);
}

QFuture<std::shared_ptr<Packet::ResponseDeleteFile>>
Network::FileChannel::removeAbsolute(const std::filesystem::path &path) {

  auto generator =
      Packet::Generator<Packet::RequestDeleteFile>(utf8to8(path.u8string()));

  auto id = generator.getId();
  auto &task = remove_tasks[id];

  session->sendJsonPacket(generator);

  return task.future();
}

std::string Network::FileChannel::getCurrentDirectory() const {
  return utf8to8(current_directory.u8string());
}

bool Network::FileChannel::showWindow() noexcept {
  if (window == nullptr) {
    window = new Window::Control::ViewFile(this, nullptr);
  }

  return AbstractChannel::showWindow();
}

void Network::FileChannel::downloadCompleted(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseDownloadFile> packet) {
  auto task = std::move(download_tasks[header->id]);
  download_tasks.erase(header->id);
  task.addResult(packet);
  task.finish();
}

void Network::FileChannel::uploadCompleted(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseUploadFile> packet) {
  auto task = std::move(upload_tasks[header->id]);
  upload_tasks.erase(header->id);
  task.addResult(packet);
  task.finish();
}

void Network::FileChannel::executeCompleted(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseExecuteFile> packet) {
  auto task = std::move(execute_tasks[header->id]);
  execute_tasks.erase(header->id);
  task.addResult(packet);
  task.finish();
}

void Network::FileChannel::removeCompleted(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseDeleteFile> packet) {
  auto task = std::move(remove_tasks[header->id]);
  remove_tasks.erase(header->id);
  task.addResult(packet);
  task.finish();
}

void Network::FileChannel::update(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::ResponseQueryFile> packet) {
  spdlog::debug("received response, updated files");

  // Check whether the operation is succeed
  if (packet->is_error == true) {
    spdlog::error("open directory failed");

    // If the request is failed because next
    if (operation == Operation::next) {
      previous();
    }
    // Or by previous or user open
    else if (operation == Operation::previous ||
             operation == Operation::user_open) {
      next();
    } else {
      spdlog::error("invalid operation occurred: {}",
                    magic_enum::enum_name(operation));
      open();
    }

    return;
  }

  // Succeed, clear previous
  directories.clear();
  files.clear();

  for (const auto &item : packet->list) {
    if (item.is_directory) {
      directories.push_back(item);
    } else {
      files.push_back(item);
    }
  }

  // Sort list
  sortCaseInsensitive(directories);
  sortCaseInsensitive(files);

  // Add ".." directory
  directories.insert(directories.begin(), {"..", true});

  emit updateCompleted();
}

void Network::FileChannel::sortCaseInsensitive(
    std::vector<Packet::File> &files) {
  std::sort(files.begin(), files.end(), [](Packet::File &p1, Packet::File &p2) {
    return std::lexicographical_compare(
        begin(p1.name), end(p1.name), begin(p2.name), end(p2.name),
        [](const char &char1, const char &char2) {
          return tolower(char1) < tolower(char2);
        });
  });
}