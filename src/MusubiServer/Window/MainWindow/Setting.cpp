#include "Setting.h"
#include "AApch.h"
#include "cpr/cpr.h"
#include "jwt-cpp/jwt.h"
#include "qfile.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "spdlog/spdlog.h"

Window::MainWindow::Setting::Setting(QWidget *parent)
    : QDialog(parent), ui(new Ui::Setting()) {
  spdlog::debug("initializing setting");

  ui->setupUi(this);

  settings = new QSettings("Mitsuha", "Musubi", this);

  settings->beginGroup("Common");
  getFromRegistry();

  connect(ui->save_button, &QPushButton::clicked, this,
          &Window::MainWindow::Setting::onSaveClicked);
  connect(ui->port, &QLineEdit::textEdited, this,
          &Window::MainWindow::Setting::onPortEdited);
  connect(ui->activation_code, &QLineEdit::textEdited, this,
          &Window::MainWindow::Setting::onCodeEdited);
  connect(ui->log_level, &QComboBox::currentIndexChanged, this,
          &Window::MainWindow::Setting::onLogLevelChanged);
  connect(ui->language_file, &QLineEdit::textEdited, this,
          &Window::MainWindow::Setting::onLanguageFileEdited);
  connect(ui->browse_language, &QPushButton::clicked, this,
          &Window::MainWindow::Setting::onBrowseLanguageFile);

  startTimer(1min);
}

Window::MainWindow::Setting::~Setting() {
  spdlog::debug("shutting down setting");

  delete ui;
  settings->deleteLater();
}

void Window::MainWindow::Setting::updateGui() {
  ui->port->setText(QString::number(port));
  ui->activation_code->setText(activation_code);
  ui->log_level->setCurrentIndex(log_level);
  ui->language_file->setText(language_file);
}

void Window::MainWindow::Setting::reset() {
  port = 11451;
  activation_code.clear();
  log_level = spdlog::level::info;
  language_file.clear();

  save();
  updateGui();
}

void Window::MainWindow::Setting::retranslate() {
  ui->retranslateUi(this);
  getFromRegistry();
}

void Window::MainWindow::Setting::getFromRegistry() {
  port = settings->value("port", qint16()).toInt();
  if (port == 0)
    port = 11451;

  activation_code = settings->value("activation_code", QString()).toString();
  validateCode(activation_code);

  log_level = (spdlog::level::level_enum)(
      settings->value("log_level", qint16()).toInt());
  if (log_level >= spdlog::level::n_levels)
    log_level = spdlog::level::info;

  language_file = settings->value("language_file", QString()).toString();

  updateGui();
}

void Window::MainWindow::Setting::save() {
  spdlog::debug("saving settings");
  settings->setValue("port", port);
  settings->setValue("activation_code", activation_code);
  settings->setValue("log_level", log_level);
  settings->setValue("language_file", language_file);
  QMessageBox::information(nullptr, tr("Restart Required"),
                           tr("Setting changed, restart program is required"));
}

void Window::MainWindow::Setting::closeEvent(QCloseEvent *ev) {
  auto button =
      QMessageBox::question(this, tr("Save Requested"),
                            QString(tr("Do you want to save the setting")),
                            QMessageBox::Yes | QMessageBox::No);
  if (button == QMessageBox::Yes)
    save();

  QDialog::closeEvent(ev);
}

void Window::MainWindow::Setting::timerEvent(QTimerEvent *ev) {
  if (validateCode(activation_code) == false) {
    spdlog::error("token expired");
    exit(0);
  }

  QDialog::timerEvent(ev);
}

void Window::MainWindow::Setting::onSaveClicked(bool) { save(); }

void Window::MainWindow::Setting::onPortEdited(QString port_) {
  port = port_.toInt();
}

void Window::MainWindow::Setting::onCodeEdited(QString code) {
  activation_code = code;

  validateCode(code);
}

void Window::MainWindow::Setting::onLogLevelChanged(int level) {
  assert(0 < level && level < spdlog::level::level_enum::n_levels);
  log_level = (spdlog::level::level_enum)level;
}

void Window::MainWindow::Setting::onLanguageFileEdited(QString file) {
  language_file = file;
}

bool Window::MainWindow::Setting::validateCode(QString code) {
  try {
    auto token = code.toStdString();
    auto decoded = jwt::decode(token);
    auto verifier =
        jwt::verify()
            .allow_algorithm(jwt::algorithm::ed448(pub_key, "", "", ""))
            .with_issuer("auth0");

    verifier.verify(decoded);

    auto request = fmt::format(
        "http://xkkongqwq.tpddns.cn:11459/online?token={}&version={}", token,
        MusubiVersion);
    auto response = cpr::Get(cpr::Url(request));

    if (response.error || response.status_code != 200) {
      spdlog::error("verification error, token expired or activation server "
                    "temporarily unavailable");

      ui->activation_status->setText(tr(
          R"(<html><head/><body><p><span style=" font-weight:700;">Activation status: activation code expired or activation server temporarily unavailable</span></p></body></html>)"));

      return false;
    }

    activation_level = (ActivationLevel)std::stoul(
        decoded.get_payload_claim("level").as_string());
    auto expire_days =
        std::chrono::duration_cast<std::chrono::days>(
            decoded.get_expires_at() - std::chrono::system_clock::now())
            .count();
    user =
        QString::fromStdString(decoded.get_payload_claim("user").as_string());
    auto level_name = std::string(magic_enum::enum_name(activation_level));

    ui->activation_status->setText(
        tr(R"(<html><head/><body><p><span style=" font-weight:700;">Activation status: User: %1, activation success, expires at %2 day(s), level: %3</span></p></body></html>)")
            .arg(user)
            .arg(expire_days)
            .arg(QString::fromStdString(level_name)));

    // if (is_automated == false) {
    //   QMessageBox::information(
    //       this, tr("Register Success"),
    //       tr("Congratulations, you are successfully registered, now restart "
    //          "the program and have fun, lol"));
    // }

    return true;
  } catch (const jwt::token_verification_exception &) {
    spdlog::error("verification error");
  } catch (const std::exception &) {
    spdlog::error("decode error");
  }

  ui->activation_status->setText(tr(
      R"(<html><head/><body><p><span style=" font-weight:700;">Activation status: activation code invalid</span></p></body></html>)"));

  return false;
}

void Window::MainWindow::Setting::onBrowseLanguageFile(bool) {
  QFileDialog dialog(this, tr("Select File"), ".\\translations");
  dialog.setOptions(QFileDialog::DontUseNativeDialog);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setNameFilter(tr("Translation files (*.qm)"));
  if (dialog.exec())
    language_file = dialog.selectedFiles()[0];
  else
    return;

  if (QFile::exists(language_file) == false)
    return;

  ui->language_file->setText(language_file);
}