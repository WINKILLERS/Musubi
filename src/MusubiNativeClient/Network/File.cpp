#include "File.h"
#include "AApch.h"
#include "Util/Util.h"
#include "bitset"
#include "filesystem"
#include "fstream"
#include "shellapi.h"

Network::File::~File() {}

void Network::File::run() noexcept {
  performHandshake(Packet::Handshake::Role::file);

  registerCallback(Packet::Type::request_query_file, *this,
                   &Network::File::onQuery);
  registerCallback(Packet::Type::request_delete_file, *this,
                   &Network::File::onRemove);
  registerCallback(Packet::Type::request_upload_file, *this,
                   &Network::File::onUpload);
  registerCallback(Packet::Type::request_execute_file, *this,
                   &Network::File::onExecute);
  registerCallback(Packet::Type::request_download_file, *this,
                   &Network::File::onDownload);

  while (true) {
    auto parser = readJsonPacket();

    if (parser.has_value() == false) {
      return;
    }

    dispatch(parser.value());
  }
}

bool Network::File::onQuery(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  auto packet = std::dynamic_pointer_cast<Packet::RequestQueryFile>(param);

  bool is_error = false;
  std::vector<Packet::File> list;

  if (packet->path.empty()) {
    std::bitset<32> volumes = GetLogicalDrives();
    for (char i = 0; i < volumes.size(); i++) {
      if (volumes[i]) {
        Packet::File f;
        f.name = fmt::format("{}:\\", (char)('A' + i));
        f.is_directory = true;
        list.push_back(std::move(f));
      }
    }
  } else {
    try {
      for (auto &entry : std::filesystem::directory_iterator(
               utf8to16(packet->path),
               std::filesystem::directory_options::skip_permission_denied)) {
        auto u8name = entry.path().filename().u8string();
        Packet::File f;
        f.name.assign(u8name.cbegin(), u8name.cend());
        f.is_directory = entry.is_directory();
        list.push_back(std::move(f));
      }
    } catch (const std::exception &) {
      is_error = true;
    }
  }

  auto generator = Packet::Generator<Packet::ResponseQueryFile>(list, is_error);
  generator.setId(header->id);
  return sendJsonPacket(generator);
}

bool Network::File::onRemove(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  auto packet = std::dynamic_pointer_cast<Packet::RequestDeleteFile>(param);

  uint64_t removed_count = 0;

  try {
    removed_count = std::filesystem::remove_all(utf8to16(packet->path));
  } catch (const std::filesystem::filesystem_error &) {
  }

  auto generator = Packet::Generator<Packet::ResponseDeleteFile>(removed_count);
  generator.setId(header->id);
  return sendJsonPacket(generator);
}

bool Network::File::onUpload(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  auto packet = std::dynamic_pointer_cast<Packet::RequestUploadFile>(param);

  std::ofstream file(utf8to16(packet->remote_path), std::ios::binary);

  if (file.is_open()) {
    file.write(packet->content.data(), packet->content.size());
    file.close();
  }

  auto generator = Packet::Generator<Packet::ResponseUploadFile>(file.fail());
  generator.setId(header->id);
  return sendJsonPacket(generator);
}

bool Network::File::onExecute(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  auto packet = std::dynamic_pointer_cast<Packet::RequestExecuteFile>(param);

  ShellExecuteW(nullptr, L"open", utf8to16(packet->path).c_str(), nullptr,
                nullptr, SW_SHOW);

  auto generator =
      Packet::Generator<Packet::ResponseExecuteFile>(GetLastError());
  generator.setId(header->id);
  return sendJsonPacket(generator);
}

bool Network::File::onDownload(
    std::shared_ptr<Packet::Header> header,
    std::shared_ptr<Packet::AbstractPacket> param) noexcept {
  auto packet = std::dynamic_pointer_cast<Packet::RequestDownloadFile>(param);

  std::ifstream file(utf8to16(packet->path), std::ios::binary);
  std::string content;
  uint8_t error_code = 0;
  auto file_size = 0;

  if (file.is_open()) {
    file.seekg(0, std::ios::end);
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    content.resize(file_size);
    file.read((char *)content.data(), file_size);

    file.close();
  }

  auto generator =
      Packet::Generator<Packet::ResponseDownloadFile>(file.fail(), content);
  generator.setId(header->id);
  return sendJsonPacket(generator);
}