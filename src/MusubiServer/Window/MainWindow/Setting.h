#pragma once
#include "ActivationLevel.h"
#include "qdialog.h"
#include "qsettings.h"
#include "spdlog/spdlog.h"
#include "ui_Setting.h"

const std::string MusubiVersion = "V1.0.0.13";
const std::string MusubiBuildTime = __TIMESTAMP__;

namespace Window {
namespace MainWindow {
class Setting : public QDialog {
  Q_OBJECT

public:
  explicit Setting(QWidget *parent = nullptr);
  ~Setting();

  // Get listen port
  inline uint16_t getPort() const { return port; };

  // Get activation level
  inline ActivationLevel getActivationLevel() const {
    return activation_level;
  };

  // Get log level
  inline spdlog::level::level_enum getLogLevel() const { return log_level; };

  // Get user name
  inline QString getUser() const { return user; };

  // Get language file path
  inline QString getLanguageFile() const { return language_file; };

  void updateGui();
  void reset();
  void retranslate();

  void save();

protected:
  void closeEvent(QCloseEvent *ev) override;
  void timerEvent(QTimerEvent *ev) override;

private:
  void getFromRegistry();

  Ui::Setting *ui = nullptr;

  QSettings *settings = nullptr;

  uint16_t port = 0;
  QString activation_code;
  ActivationLevel activation_level = ActivationLevel::NotActivated;
  spdlog::level::level_enum log_level = spdlog::level::info;
  QString user = "Unknown";
  QString language_file;
  bool set_language = false;

  const std::string pub_key = R"(-----BEGIN PUBLIC KEY-----
MEMwBQYDK2VxAzoA3FWPh5/qGXoaNfQzeUCXpJEybbXneKo+R+Ahery1KPukFs80
pjGN7W6uRRAAZb3qAxt8NByCrlCA
-----END PUBLIC KEY-----)";

private slots:
  void onSaveClicked(bool);

  void onPortEdited(QString port_);
  void onCodeEdited(QString code);
  void onLogLevelChanged(int level);
  void onLanguageFileEdited(QString file);
  void onBrowseLanguageFile(bool);

  bool validateCode(QString code);
};
} // namespace MainWindow
} // namespace Window