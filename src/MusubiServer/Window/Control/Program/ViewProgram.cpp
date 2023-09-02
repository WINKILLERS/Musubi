#include "ViewProgram.h"
#include "AApch.h"
#include "Network/ProgramChannel.h"
#include "qheaderview.h"
#include "qmessagebox.h"

Window::Control::ViewProgram::ViewProgram(Network::ProgramChannel *channel,
                                          QWidget *parent)
    : QDialog(parent), channel(channel), ui(new Ui::ViewProgram()) {
  spdlog::debug("initializing view program");

  ui->setupUi(this);

  view = new QTableView(this);
  model = new QStandardItemModel(view);
  selection = new QItemSelectionModel(model);

  view->setModel(model);
  view->setSelectionModel(selection);
  view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  view->setSelectionBehavior(QAbstractItemView::SelectRows);
  view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  ui->program_layout->addWidget(view);

  connect(view, &QTableView::doubleClicked, this,
          &Window::Control::ViewProgram::openPath);
  connect(ui->refresh, &QPushButton::clicked, this,
          &Window::Control::ViewProgram::refresh);
  connect(ui->uninstall, &QPushButton::clicked, this,
          &Window::Control::ViewProgram::uninstall);

  connect(channel, &Network::ProgramChannel::updateCompleted, this,
          &Window::Control::ViewProgram::update);

  if (channel->refresh()) {
    view->setEnabled(false);
  }
}

Window::Control::ViewProgram::~ViewProgram() {
  spdlog::debug("shutting down view program");

  view->deleteLater();
  model->deleteLater();
  selection->deleteLater();

  delete ui;
  ui = nullptr;

  channel = nullptr;
}

void Window::Control::ViewProgram::update() {
  list = channel->getPrograms();

  uint32_t row = 0;
  model->clear();
  model->setHorizontalHeaderLabels({tr("Program Name"), tr("Publisher"),
                                    tr("Version"), tr("Install Date"),
                                    tr("Install Path")});
  for (auto i = 0; i < list.size(); i++) {
    QStandardItem *name =
        new QStandardItem(QString::fromStdString(list[i].name));
    QStandardItem *publisher =
        new QStandardItem(QString::fromStdString(list[i].publisher));
    QStandardItem *version =
        new QStandardItem(QString::fromStdString(list[i].version));
    QStandardItem *install_date =
        new QStandardItem(QString::fromStdString(list[i].install_date));
    QStandardItem *install_path =
        new QStandardItem(QString::fromStdString(list[i].install_path));
    model->setItem(row, 0, name);
    model->setItem(row, 1, publisher);
    model->setItem(row, 2, version);
    model->setItem(row, 3, install_date);
    model->setItem(row, 4, install_path);
    row++;
  }

  view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  auto total_program = QString(tr("Total: %1 program(s)")).arg(list.size());
  ui->total_program->setText(total_program);

  auto current_user_ret = channel->getCurrentUser();
  QString current_user;

  if (current_user_ret.has_value() == false) {
    current_user = tr("Current user: Unknown");
  } else {
    current_user = tr("Current user: %1")
                       .arg(QString::fromStdString(current_user_ret.value()));
  }

  ui->current_user->setText(current_user);

  view->setEnabled(true);
}

void Window::Control::ViewProgram::refresh() {
  if (channel->refresh()) {
    view->setEnabled(false);
  }
}

void Window::Control::ViewProgram::uninstall() {
  spdlog::debug("requested uninstall program");

  QMessageBox::warning(this, tr("Under Development"),
                       tr("This function is not implemented yet"));
}

void Window::Control::ViewProgram::openPath(const QModelIndex &index) {
  const auto &path = model->index(index.row(), 4).data().toString();
  if (path.isEmpty()) {
    QMessageBox::warning(this, tr("Invalid Input"),
                         tr("This program has no path to open"));
    return;
  }

  if (channel->openInViewFile(path.toStdWString()) == false) {
    QMessageBox::critical(this, tr("Invalid Input"),
                          tr("Can not open file channel"));
    return;
  }

  return;
}