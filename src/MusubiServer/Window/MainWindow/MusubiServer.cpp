#include "MusubiServer.h"
#include "AApch.h"
#include "Network/FileChannel.h"
#include "Network/Tcp.h"
#include "Window/Control/File/ViewFile.h"
#include "Window/Control/Program/ViewProgram.h"
#include "qheaderview.h"
#include "qmessagebox.h"


Window::MainWindow::MusubiServer::MusubiServer(Setting *setting,
                                               QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MusubiServerClass()), setting(setting) {
  spdlog::info("server starting, version: {}, build: {}", MusubiVersion,
               MusubiBuildTime);

  ui->setupUi(this);

  if (setting->getActivationLevel() == ActivationLevel::NotActivated) {
    spdlog::error("not registered");

    QMessageBox::critical(this, tr("Registration Required"),
                          tr("You are not registered, or token expired, please "
                             "input new one or contact your administrator"));

    setting->show();
    return;
  }

  spdlog::info("setting up handler");
  handler = new Network::TcpHandler(setting->getPort(), this);
  if (handler->listen() == false) {
    spdlog::error("listen address error, resetting");

    QMessageBox::critical(this, tr("Listen Error"),
                          tr("Handler listen failed, check your port setting "
                             "or restart computer"));
    return;
  }

  spdlog::info("setting up client view");
  client_view = new View::Client::ClientView(this);
  client_model = new Model::Client::ClientModel(client_view);
  selection = new QItemSelectionModel(client_model);
  connect(client_view, &QTableView::doubleClicked, client_model,
          &Model::Client::ClientModel::onDoubleClicked);
  connect(&handler->notifier, &Network::IConnectionNotify::clientConnected,
          client_model, &Model::Client::ClientModel::addClient);
  connect(&handler->notifier, &Network::IConnectionNotify::clientDisconnected,
          client_model, &Model::Client::ClientModel::deleteClient);
  client_view->setModel(client_model);
  client_view->setSelectionModel(selection);
  client_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  client_view->setSelectionMode(QAbstractItemView::SingleSelection);
  client_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  client_view->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  ui->client_layout->addWidget(client_view);

  spdlog::info("setting up status bar");
  label_address = new QLabel(ui->statusBar);
  label_address->setText(tr("Version: %1, Build: %2, Listening on port: %3")
                             .arg(QString::fromStdString(MusubiVersion))
                             .arg(QString::fromStdString(MusubiBuildTime))
                             .arg(setting->getPort()));
  ui->statusBar->addPermanentWidget(label_address);
  spdlog::info("listening on port: {}", setting->getPort());

  spdlog::info("setting up main window");
  label_count = new QLabel(ui->statusBar);
  label_count->setText(tr("Connected 0 client(s)"));
  connect(&handler->notifier, &Network::IConnectionNotify::clientConnected,
          this, &MusubiServer::updateCount);
  connect(&handler->notifier, &Network::IConnectionNotify::clientDisconnected,
          this, &MusubiServer::updateCount);
  ui->statusBar->addWidget(label_count);

  spdlog::info("connecting slots");
  connect(ui->actionSettings, &QAction::triggered, this,
          &MusubiServer::actShowSetting);
  connect(ui->actionLicense, &QAction::triggered, this,
          &MusubiServer::actShowLicense);
  connect(ui->actionView_File, &QAction::triggered, this,
          &MusubiServer::actShowViewFile);
  connect(ui->actionView_Program, &QAction::triggered, this,
          &MusubiServer::actShowProgram);
  connect(ui->actionRsNormal, &QAction::triggered, this,
          &MusubiServer::actShowNormalScreen);

  if (setting->getActivationLevel() < ActivationLevel::Tester) {
  }

  show();
}

Window::MainWindow::MusubiServer::~MusubiServer() {
  spdlog::info("server stopped");

  delete ui;
}

void Window::MainWindow::MusubiServer::actShowSetting() { setting->show(); }

void Window::MainWindow::MusubiServer::actShowLicense() {
  if (license == nullptr) {
    license = new License(
        setting->getRegisteredUser(),
        setting->getActivationLevel() != ActivationLevel::NotActivated, this);
  }

  license->show();
}

void Window::MainWindow::MusubiServer::actShowViewFile() {
  openSubchannel(Packet::Handshake::Role::file);
}

void Window::MainWindow::MusubiServer::actShowProgram() {
  openSubchannel(Packet::Handshake::Role::program);
}

void Window::MainWindow::MusubiServer::actShowNormalScreen() {
  openSubchannel(Packet::Handshake::Role::remote_screen);
}

bool Window::MainWindow::MusubiServer::openSubchannel(
    Packet::Handshake::Role role) {
  auto selection = getSelection();
  if (selection == nullptr) {
    return false;
  }

  auto request_ret = selection->requestOpenChannel(role);
  if (request_ret.has_value() == false) {
    return false;
  }

  auto request = std::move(request_ret.value());
  request.then([this](Network::AbstractChannel *sub_channel) {
    sub_channel->showWindow();
  });

  return true;
}

void Window::MainWindow::MusubiServer::updateCount() {
  auto text =
      QString(tr("Connected %1 client(s)")).arg(handler->getClientCount());
  label_count->setText(text);
}

Network::Controller *Window::MainWindow::MusubiServer::getSelection() {
  auto indexes = client_view->selectionModel()->selectedIndexes();
  if (indexes.isEmpty()) {
    QMessageBox::warning(this, tr("Invalid Input"),
                         tr("No client selected, please select one"));
    return nullptr;
  }

  auto row = indexes[0].row();

  return client_model->getController(row);
}