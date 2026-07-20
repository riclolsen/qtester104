/*
 * This software implements an IEC 60870-5-104 protocol tester.
 * Copyright © 2010-present Ricardo L. Olsen
 *
 * Disclaimer
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QCloseEvent>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QItemSelectionModel>
#include <QRegularExpression>
#include <QMetaObject>
#include <QScrollBar>
#include <QSslSocket>
#include <QPalette>
#include <QStyleFactory>
#include <QSettings>
#include <QTextCursor>
#include <string>

using namespace std;

static unsigned int parseIoa(const QString& str) {
    if (str.contains('-') || str.contains('.')) {
        QStringList parts = str.split(QRegularExpression("[-.]"));
        if (parts.size() >= 3) {
            unsigned int low = parts[0].toUInt();
            unsigned int mid = parts[1].toUInt();
            unsigned int high = parts[2].toUInt();
            return low + (mid << 8) + (high << 16);
        }
    }
    return str.toUInt();
}

void MainWindow::queueProtocolCall(const std::function<void(QIec104*)>& fn) {
  QMetaObject::invokeMethod(
      i104,
      [worker = i104, fn]() { fn(worker); },
      Qt::QueuedConnection);
}

void MainWindow::queueProtocolCommand(const iec_obj& obj) {
  QMetaObject::invokeMethod(
      i104,
      [worker = i104, command = obj]() mutable { worker->sendCommand(&command); },
      Qt::QueuedConnection);
}

void MainWindow::shutdownProtocolThread() {
  if (ProtocolShutdown) {
    return;
  }

  ProtocolShutdown = true;

  if (i104 == nullptr) {
    return;
  }

  QMetaObject::invokeMethod(i104, [worker = i104]() { worker->terminate(); },
                            Qt::BlockingQueuedConnection);
  protocolThread.quit();
  protocolThread.wait();
  i104->moveToThread(QApplication::instance()->thread());
  delete i104;
  i104 = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------

MainWindow::MainWindow(const QString& iniPath,
                       const QString& rtuSection,
                       bool enableI104M,
                       QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      tmLogMsg(nullptr),
      tmUiDataPump(nullptr),
      i104(new QIec104()),
      pendingDataPointCount(0),
      pointTableSortPending(false),
      pointTableResizePending(false),
      logTickCount(0),
      LastCommandAddress(0),
      SendCommands(0),
      Hide(0),
      PrimaryAddress(1),
      SecondaryAddress(1),
      ForcePrimary(0),
      ProtocolKeepAliveActive(false),
      ProtocolShutdown(false),
      IniPath(iniPath),
      RtuSection(rtuSection),
      ProtocolPort(2404),
      EnableI104M(enableI104M) {
  I104M_Logar = 1;
  i104->mLog.deactivateLog();

  // look for the ini file in the application dir, if not found use the conf dir
  QString ininame = IniPath;
  if (ininame.isEmpty()) {
    ininame = QCoreApplication::applicationDirPath() + CURDIRINIFILENAME;
  }
  if (!QFile(ininame).exists()) {
    ininame = CONFDIRINIFILENAME;
  }

  if (QCoreApplication::arguments().count() > 1) {
    ininame = QCoreApplication::arguments().at(1);
  }

  // busca configuracoes no arquivo ini
  QSettings settings(ininame, QSettings::IniFormat);
  const QString rtuPrefix = RtuSection + "/";

  QString useTls = settings.value(rtuPrefix + "USE_TLS", "0").toString().trimmed();
  QString caCertPath = settings.value(rtuPrefix + "CA_CERT_PATH", "").toString();
  QString localCertPath = settings.value(rtuPrefix + "LOCAL_CERT_PATH", "").toString();
  QString privateKeyPath = settings.value(rtuPrefix + "PRIVATE_KEY_PATH", "").toString();
  QString verifyPeer = settings.value(rtuPrefix + "VERIFY_PEER", "0").toString().trimmed();

  PrimaryAddress = settings.value("IEC104/PRIMARY_ADDRESS", 1).toInt();
  ForcePrimary = settings.value("I104M/FORCE_PRIMARY", 0).toInt();
  SecondaryAddress = settings.value(rtuPrefix + "SECONDARY_ADDRESS", 1).toInt();
  SendCommands = settings.value(rtuPrefix + "ALLOW_COMMANDS", 0).toInt();

  QString IPEscravo;
  QString IPEscravoBackup = settings.value(rtuPrefix + "IP_ADDRESS_BACKUP", "").toString();
  IPEscravo = settings.value(rtuPrefix + "IP_ADDRESS", "").toString();
  SecondaryIp = IPEscravo;
  ProtocolPort = settings.value(rtuPrefix + "TCP_PORT", ProtocolPort).toUInt();
  const unsigned giPeriod = settings.value(rtuPrefix + "GI_PERIOD", 330).toUInt();
  const QSslSocket::PeerVerifyMode verifyMode =
      verifyPeer != "0" ? QSslSocket::AutoVerifyPeer : QSslSocket::QueryPeer;

  // this is for using with the OSHMI HMI in a dual architecture
  QSettings settings_oshmi("../conf/hmi.ini", QSettings::IniFormat);
  I104M_host_dual.setAddress(
      settings_oshmi.value("REDUNDANCY/OTHER_HMI_IP", "0.0.0.0").toString());
  I104M_host.setAddress("127.0.0.1");
  I104M_CntDnToBePrimary = I104M_CntToBePrimary;

  ui->setupUi(this);
  if (menuBar()) {
    menuBar()->hide();
  }
  if (statusBar()) {
    statusBar()->hide();
  }
  on_cb888Mode_stateChanged(ui->cb888Mode->isChecked());

  // TLS checkbox visibility logic
  if (ui->cbEnableTls) {
      ui->cbEnableTls->setVisible(useTls != "0");
      ui->cbEnableTls->setChecked(useTls != "0");
  }

  if (ui->cbLog->isChecked()) {
      QDate dt = QDate::currentDate();
      QString str = dt.toString() + QString(" - ") + QString(QTESTER_VERSION);
      i104->mLog.activateLog();
      i104->mLog.pushMsg(str.toStdString().c_str());
  } else
      i104->mLog.deactivateLog();

  // this is for hiding the window when runnig
  Hide = settings_oshmi.value("RUN/HIDE", "").toInt();

  this->setWindowTitle(tr("QTester104 IEC60870-5-104"));

  QIntValidator *validator = new QIntValidator(0, 65535, this);
  ui->leLinkAddress->setValidator(validator);
  QIntValidator *validator1 = new QIntValidator(0, 65535, this);
  ui->lePort->setValidator(validator1);
  QIntValidator *validator2 = new QIntValidator(0, 255, this);
  ui->leMasterAddress->setValidator(validator2);

  QRegularExpression rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}("
                        "?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
  QValidator *valip = new QRegularExpressionValidator(rx, this);
  ui->leIPRemoto->setValidator(valip);

  if (EnableI104M) {
    udps = new QUdpSocket(this);
    udps->bind(I104M_porta_escuta);
    udps->open(QIODevice::ReadWrite);
  }

  QString qs;
  QTextStream(&qs) << ProtocolPort;
  ui->lePort->setText(qs);
  ui->leIPRemoto->setText(IPEscravo);
  qs = "";
  QTextStream(&qs) << SecondaryAddress;
  ui->leLinkAddress->setText(qs);
  qs = "";
  QTextStream(&qs) << PrimaryAddress;
  ui->leMasterAddress->setText(qs);

  ui->leIPRemoto->setText(IPEscravo);

  tmLogMsg = new QTimer();
  tmUiDataPump = new QTimer();
  tmI104M_kamsg = new QTimer();

  i104->moveToThread(&protocolThread);
  protocolThread.start();

  if (udps != nullptr) {
    connect(udps, SIGNAL(readyRead()), this, SLOT(slot_I104M_ready_to_read()));
  }
  connect(tmLogMsg, SIGNAL(timeout()), this, SLOT(slot_timer_logmsg()));
  connect(tmUiDataPump, SIGNAL(timeout()), this, SLOT(slot_processPendingUiData()));
  connect(tmI104M_kamsg, SIGNAL(timeout()), this,
          SLOT(slot_timer_I104M_kamsg()));
  connect(i104, &QIec104::signal_dataIndication, this,
          &MainWindow::slot_dataIndication);
  connect(i104, &QIec104::signal_interrogationActConfIndication, this,
          &MainWindow::slot_interrogationActConfIndication);
  connect(i104, &QIec104::signal_interrogationActTermIndication, this,
          &MainWindow::slot_interrogationActTermIndication);
  connect(i104, &QIec104::signal_tcp_connect, this, &MainWindow::slot_tcpconnect);
  connect(i104, &QIec104::signal_tcp_disconnect, this,
          &MainWindow::slot_tcpdisconnect);
  connect(i104, &QIec104::signal_commandActRespIndication, this,
          &MainWindow::slot_commandActRespIndication);

  queueProtocolCall([=](QIec104* worker) {
    worker->setTlsEnabled(useTls != "0");
    worker->setCaCertPath(caCertPath);
    worker->setLocalCertPath(localCertPath);
    worker->setPrivateKeyPath(privateKeyPath);
    worker->setPeerVerifyMode(verifyMode);
    worker->setPrimaryAddress(PrimaryAddress);
    worker->ForcePrimary = ForcePrimary;
    worker->setSecondaryAddress(SecondaryAddress);
    worker->SendCommands = SendCommands;
    worker->setSecondaryIP_backup(
        const_cast<char*>(IPEscravoBackup.toStdString().c_str()));
    worker->setSecondaryIP(const_cast<char*>(SecondaryIp.toStdString().c_str()));
    worker->setPortTCP(ProtocolPort);
    worker->setGIPeriod(giPeriod);
  });

  ui->pbGI->setEnabled(false);
  ui->pbSendCommandsButton->setEnabled(false);

  ui->twPontos->clear();
  ui->twPontos->setSortingEnabled(false);
  ui->twPontos->setColumnCount(8);
  ui->twPontos->sortByColumn(0, Qt::AscendingOrder);

  if (IPEscravo != "")
    on_pbConnect_clicked();

  QStringList colunas;
  colunas << "Address"
          << "CA"
          << "Value"
          << "Type"
          << "Cause"
          << "Flags"
          << "Count"
          << "TimeTag";
  ui->twPontos->setHorizontalHeaderLabels(colunas);

  tmLogMsg->start(350);

  if (EnableI104M && I104M_HaveDualHost()) {
    tmI104M_kamsg->start(I104M_seconds_kamsg * 1000);
    isPrimary = false;
    queueProtocolCall([](QIec104* worker) { worker->disable_connect(); });
    ui->lbMode->setText("<font color='red'>Secondary</font>");
  } else {
    isPrimary = true;
    queueProtocolCall([](QIec104* worker) { worker->enable_connect(); });
    ui->lbMode->setText("<font color='green'>Primary</font>");
  }

  ui->lbCopyright->setText(QString(QTESTER_VERSION) + QString(" - ") +
                           QString(QTESTER_COPYRIGHT));

  QFont font = QFont("Consolas");
  font.setStyleHint(QFont::Monospace);
  font.setPointSize(8);
  font.setFixedPitch(true);
  ui->lwLog->setFont(font);

  // Set default theme to Light
  ui->cbTheme->setCurrentIndex(0);
  on_cbTheme_currentIndexChanged(0);
  connect(ui->cbTheme, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_cbTheme_currentIndexChanged);
}

MainWindow::~MainWindow() {
  shutdownProtocolThread();
  delete ui;
  delete tmLogMsg;
  delete tmUiDataPump;
  delete tmI104M_kamsg;
}

void MainWindow::on_pbGI_clicked() {
  queueProtocolCall([](QIec104* worker) { worker->solicitGI(); });
}

void MainWindow::on_pbConnect_clicked() {
  if (ProtocolKeepAliveActive) {
    ProtocolKeepAliveActive = false;
    queueProtocolCall([](QIec104* worker) {
      worker->tmKeepAlive->stop();
      worker->tcps->close();
      worker->slot_tcpdisconnect();
    });
  } else {
    SecondaryIp = ui->leIPRemoto->text();
    ProtocolPort = ui->lePort->text().toUInt();
    SecondaryAddress = ui->leLinkAddress->text().toInt();
    PrimaryAddress = ui->leMasterAddress->text().toInt();

    /* // --- Apply TLS Settings from UI ---
    // (Assuming UI elements like ui->cbUseTls exist)
    bool useTls = ui->cbEnableTls->isChecked();
    QString caCertPath = ui->leCaCertPath->text();
    QString localCertPath = ui->leLocalCertPath->text();
    QString privateKeyPath = ui->lePrivateKeyPath->text();
    bool verifyPeer = ui->cbVerifyPeer->isChecked();

    i104.setTlsEnabled(useTls);
    i104.setCaCertPath(caCertPath);
    i104.setLocalCertPath(localCertPath);
    i104.setPrivateKeyPath(privateKeyPath);
    i104.setPeerVerifyMode(verifyPeer ? QSslSocket::VerifyPeer : QSslSocket::VerifyNone); // Adjust if using QueryPeer etc.
    */

    const bool tlsEnabled = ui->cbEnableTls->isChecked();
    queueProtocolCall([=](QIec104* worker) {
      worker->setSecondaryIP(
          const_cast<char*>(SecondaryIp.toStdString().c_str()));
      worker->setPortTCP(ProtocolPort);
      worker->setSecondaryAddress(SecondaryAddress);
      worker->setPrimaryAddress(PrimaryAddress);
      worker->setTlsEnabled(tlsEnabled);
    });

    QString qs;
    ui->leIPRemoto->setText(SecondaryIp);
    QTextStream(&qs) << ui->leLinkAddress->text().toInt();
    ui->leLinkAddress->setText(qs);
    qs = "";
    QTextStream(&qs) << ui->leMasterAddress->text().toInt();
    ui->leMasterAddress->setText(qs);

    ui->lePort->setEnabled(false);
    ui->leIPRemoto->setEnabled(false);
    ui->leLinkAddress->setEnabled(false);
    ui->leMasterAddress->setEnabled(false);

    ui->pbConnect->setText("Give up...");
    ui->lbStatus->setText("<font color='green'>TRYING TO CONNECT!</font>");

    mapPtItem_ColAddress.clear();
    mapPtItem_ColCommonAddress.clear();
    mapPtItem_ColValue.clear();
    mapPtItem_ColType.clear();
    mapPtItem_ColCause.clear();
    mapPtItem_ColFlags.clear();
    mapPtItem_ColCount.clear();
    mapPtItem_ColTimeTag.clear();
    ui->twPontos->clearContents();
    ui->twPontos->setRowCount(0);
    // ui->lwLog->clear();
    ProtocolKeepAliveActive = true;
    queueProtocolCall([](QIec104* worker) { worker->tmKeepAlive->start(1000); });
  }
}

// receive data/commands from OSHMI
void MainWindow::slot_I104M_ready_to_read() {
  if (udps == nullptr) {
    return;
  }

  char buf[5000];

  unsigned char br[2000]; // buffer de recepcao
  int bytesrec;

  while (udps->hasPendingDatagrams()) {
    QHostAddress address;
    quint16 port;
    bytesrec = int(udps->readDatagram(reinterpret_cast<char *>(br), sizeof(br),
                                      &address, &port));

    if (bytesrec <= 0)
      return;

    // I104M message must be local or from redundant computer, consider ipv4 &
    // ipv6
    if (address.toString() != "127.0.0.1" &&
        address.toString() != "::ffff:127.0.0.1" &&
        address.toString() != I104M_host_dual.toString() &&
        address.toString() !=
            (QString("::ffff:") + I104M_host_dual.toString())) {
      I104M_Loga(QString("R--> I104M: Message from invalid origin ") +
                 address.toString());
      continue;
    }

    // I104M message
    t_msgcmd *pmsg = reinterpret_cast<t_msgcmd *>(br);

    const int dumplim = 100; // limit the hex dump so buf cannot overflow
    sprintf(buf, "%3d: I104M: ", bytesrec);
    for (int i = 0; i < bytesrec && i < dumplim; i++)
      sprintf(buf + strlen(buf), "%02x ", br[i]);
    if (bytesrec > dumplim)
      sprintf(buf + strlen(buf), "...");
    I104M_Loga(buf);

    if (bytesrec < int(sizeof(t_msgcmd)) || pmsg->signature != MSGCMD_SIG) {
      I104M_Loga("R--> I104M: Invalid Message!");
      continue;
    }

    iec_obj obj = {};
    obj.cause = iec104_class::ACTIVATION;
    obj.address = pmsg->endereco;
    obj.ca = static_cast<unsigned short>(pmsg->utr);
    obj.qu = static_cast<unsigned char>(pmsg->qu);
    obj.se = static_cast<unsigned char>(pmsg->sbo);
    obj.type = static_cast<unsigned char>(pmsg->tipo);

    switch (pmsg->tipo) {
    case I104M_ASDU_SPECIAL_CMD: // special command
      if (pmsg->endereco ==
          I104M_SPECIAL_CMD_ADDR_REQ_GI) { // request general interrogation
        I104M_Loga("R--> I104M: REQ GI");
        queueProtocolCall([](QIec104* worker) { worker->solicitGI(); });
      } else if (pmsg->endereco == I104M_SPECIAL_CMD_ADDR_KEEP_ALIVE &&
                 (address.toString() == I104M_host_dual.toString() ||
                  address.toString() ==
                      (QString("::ffff:") + I104M_host_dual.toString())) &&
                 ForcePrimary == 0) { // keep alive from the redundant computer
        I104M_Loga("R--> I104M: KEEP ALIVE FROM REDUNDANT COMPUTER");
        if (isPrimary) {
          I104M_Loga("     I104M: BECOMMING SECONDARY!");
          ui->lbMode->setText("<font color='red'>Secondary</font>");
        }
        isPrimary = false;
        queueProtocolCall([](QIec104* worker) { worker->disable_connect(); });
        I104M_CntDnToBePrimary =
            I104M_CntToBePrimary; // restart count to be primary
      }
      break;
    case iec104_class::C_SC_TA_1: // single command with time tag
    case iec104_class::C_SC_NA_1: // single command
      sprintf(buf, "R--> I104M: Single Command %s", pmsg->onoff ? "on" : "off");
      I104M_Loga(buf);
      obj.sp = static_cast<unsigned char>(pmsg->onoff);
      queueProtocolCommand(obj);
      LastCommandAddress = obj.address;
      break;
    case iec104_class::C_DC_TA_1: // double command with time tag
    case iec104_class::C_DC_NA_1: // double command
      sprintf(buf, "R--> I104M: Double Command %s", pmsg->onoff ? "on" : "off");
      I104M_Loga(buf);
      obj.dp = pmsg->onoff ? 2 : 1;
      queueProtocolCommand(obj);
      LastCommandAddress = obj.address;
      break;
    case iec104_class::C_SE_TA_1: // set-point normalised command with time tag
    case iec104_class::C_SE_NA_1: // set-point normalised command
      sprintf(buf, "R--> I104M: set-point normalised command %f",
              double(pmsg->setpoint));
      I104M_Loga(buf);
      obj.value = pmsg->setpoint;
      queueProtocolCommand(obj);
      LastCommandAddress = obj.address;
      break;
    case iec104_class::C_SE_TB_1: // set-point scaled command with time tag
    case iec104_class::C_SE_NB_1: // set-point scaled command
      sprintf(buf, "R--> I104M: set-point scaled command %f",
              double(pmsg->setpoint));
      I104M_Loga(buf);
      obj.value = pmsg->setpoint;
      queueProtocolCommand(obj);
      LastCommandAddress = obj.address;
      break;
    case iec104_class::C_SE_TC_1: // set-point short floating point command with
                                  // time tag
    case iec104_class::C_SE_NC_1: // set-point short floating point command
      sprintf(buf, "R--> I104M: set-point short floating command %f",
              double(pmsg->setpoint));
      I104M_Loga(buf);
      obj.value = pmsg->setpoint;
      queueProtocolCommand(obj);
      LastCommandAddress = obj.address;
      break;
    case iec104_class::C_RC_TA_1: // regulating step command with time tag
    case iec104_class::C_RC_NA_1: // regulating step command
      sprintf(buf, "R--> I104M: regulating step command %s", pmsg->setpoint==0?"LOWER":"RAISE");
      I104M_Loga(buf);
      obj.rcs = pmsg->setpoint==0?1:2;
      queueProtocolCommand(obj);
      LastCommandAddress = obj.address;
      break;
    case iec104_class::C_BO_TA_1: // bitstring command with time tag
    case iec104_class::C_BO_NA_1: // bitstring command
      sprintf(buf, "R--> I104M: bitstring command %u", pmsg->setpoint_i32);
      I104M_Loga(buf);
      obj.value = pmsg->setpoint_i32;
      queueProtocolCommand(obj);
      LastCommandAddress = obj.address;
      break;
    }
  }
}

// Envio de comando
void MainWindow::on_pbSendCommandsButton_clicked() {
  iec_obj obj = {};
  obj.type = static_cast<unsigned char>(
      ui->cbCmdAsdu->currentText()
          .left(ui->cbCmdAsdu->currentText().indexOf(':'))
          .toUInt());

  if (obj.type == iec104_class::C_RD_NA_1) {
    if (ui->cb888Mode->isChecked()) {
        if (ui->leCmdAddressLow->text().trimmed() == "" && ui->leCmdAddressMid->text().trimmed() == "" && ui->leCmdAddressHigh->text().trimmed() == "") return;
        obj.address = ui->leCmdAddressLow->text().toUInt() + (ui->leCmdAddressMid->text().toUInt() << 8) + (ui->leCmdAddressHigh->text().toUInt() << 16);
    } else {
        if (ui->leCmdAddress->text().trimmed() == "") return;
        obj.address = parseIoa(ui->leCmdAddress->text());
    }
    obj.value = 0;
  } else
    // reset process and interrogation must have value set (qrp) or (qoi)
    if (obj.type == iec104_class::C_RP_NA_1 ||
        obj.type == iec104_class::C_IC_NA_1 ||
        obj.type == iec104_class::C_CI_NA_1) {
      if (ui->leCmdValue->text().trimmed() == "")
        return;
      obj.address = 0;
      obj.value = ui->leCmdValue->text().toDouble();
    } else
      // test command parameters if not sync command or test command (that don't
      // have parameters)
      if (obj.type != iec104_class::C_CS_NA_1 &&
          obj.type != iec104_class::C_TS_TA_1) {
        if (ui->leCmdValue->text().trimmed() == "") return;
        unsigned int parsedAddr = 0;
        if (ui->cb888Mode->isChecked()) {
            if (ui->leCmdAddressLow->text().trimmed() == "" && ui->leCmdAddressMid->text().trimmed() == "" && ui->leCmdAddressHigh->text().trimmed() == "") return;
            parsedAddr = ui->leCmdAddressLow->text().toUInt() + (ui->leCmdAddressMid->text().toUInt() << 8) + (ui->leCmdAddressHigh->text().toUInt() << 16);
        } else {
            if (ui->leCmdAddress->text().trimmed() == "") return;
            parsedAddr = parseIoa(ui->leCmdAddress->text());
        }
        if (parsedAddr == 0) return;

        obj.address = parsedAddr;
        obj.value = ui->leCmdValue->text().toDouble();
      }

  obj.ca = ui->leASDUAddr->text().toUShort();
  queueProtocolCall([ca = obj.ca](QIec104* worker) {
    worker->setSecondaryASDUAddress(ca);
  });
  QDateTime current = QDateTime::currentDateTime();

  switch (obj.type) {
  case iec104_class::C_IC_NA_1: // Interrogation
    queueProtocolCall([group = ui->leCmdValue->text().toInt()](QIec104* worker) {
      worker->solicitInterrogation(static_cast<char>(group));
    });
    return;
  case iec104_class::C_SC_NA_1:
  case iec104_class::C_SC_TA_1:
    obj.scs = static_cast<unsigned char>(ui->leCmdValue->text().toUInt());
    break;
  case iec104_class::C_DC_NA_1:
  case iec104_class::C_DC_TA_1:
    obj.dcs = static_cast<unsigned char>(ui->leCmdValue->text().toUInt());
    break;
  case iec104_class::C_RC_NA_1:
  case iec104_class::C_RC_TA_1:
    obj.rcs = static_cast<unsigned char>(ui->leCmdValue->text().toUInt());
    break;
  case iec104_class::C_SE_NA_1:
  case iec104_class::C_SE_TA_1:
    obj.value = ui->leCmdValue->text().toInt();
    break;
  case iec104_class::C_SE_NB_1:
  case iec104_class::C_SE_TB_1:
    obj.value = ui->leCmdValue->text().toInt();
    break;
  case iec104_class::C_SE_NC_1:
  case iec104_class::C_SE_TC_1:
    obj.value = ui->leCmdValue->text().toDouble();
    break;
  case iec104_class::C_CS_NA_1:
    obj.timetag.year = static_cast<uint8_t>(current.date().year() % 100);
    obj.timetag.month = static_cast<uint8_t>(current.date().month());
    obj.timetag.mday = static_cast<uint8_t>(current.date().day());
    obj.timetag.hour = static_cast<uint8_t>(current.time().hour());
    obj.timetag.min = static_cast<uint8_t>(current.time().minute());
    obj.timetag.msec = static_cast<uint16_t>(
        current.time().second() * 1000 + current.time().msec());
    obj.timetag.iv = 0;
    obj.timetag.su = static_cast<uint8_t>(current.isDaylightTime());
    obj.timetag.wday = static_cast<uint8_t>(current.date().dayOfWeek());;
    obj.timetag.res1 = 0;
    obj.timetag.res2 = 0;
    obj.timetag.res3 = 0;
    obj.timetag.res4 = 0;
    break;
  case iec104_class::C_TS_TA_1:
    obj.timetag.year = static_cast<uint8_t>(current.date().year() % 100);
    obj.timetag.month = static_cast<uint8_t>(current.date().month());
    obj.timetag.mday = static_cast<uint8_t>(current.date().day());
    obj.timetag.hour = static_cast<uint8_t>(current.time().hour());
    obj.timetag.min = static_cast<uint8_t>(current.time().minute());
    obj.timetag.msec = static_cast<uint16_t>(
        current.time().second() * 1000 + current.time().msec());
    obj.timetag.iv = 0;
    obj.timetag.su = static_cast<uint8_t>(current.isDaylightTime());
    obj.timetag.wday = static_cast<uint8_t>(current.date().dayOfWeek());;
    obj.timetag.res1 = 0;
    obj.timetag.res2 = 0;
    obj.timetag.res3 = 0;
    obj.timetag.res4 = 0;
    break;
  case iec104_class::C_RD_NA_1:
    break;
  case iec104_class::C_RP_NA_1:
    break;
  case iec104_class::P_ME_NA_1:
    obj.value = ui->leCmdValue->text().toInt();
    obj.kpa = static_cast<unsigned char>(
        ui->cbCmdDuration->currentText().left(1).toUInt());
    obj.lpc = static_cast<unsigned char>(ui->cbSBO->isChecked());
    obj.pop = 0;
    break;
  case iec104_class::P_ME_NB_1:
    obj.value = ui->leCmdValue->text().toInt();
    obj.kpa = static_cast<unsigned char>(
        ui->cbCmdDuration->currentText().left(1).toUInt());
    obj.lpc = static_cast<unsigned char>(ui->cbSBO->isChecked());
    obj.pop = 0;
    break;
  case iec104_class::P_ME_NC_1:
    obj.value = ui->leCmdValue->text().toDouble();
    obj.kpa = static_cast<unsigned char>(
        ui->cbCmdDuration->currentText().left(1).toUInt());
    obj.lpc = static_cast<unsigned char>(ui->cbSBO->isChecked());
    obj.pop = 0;
    break;
  case iec104_class::P_AC_NA_1:
    obj.value = ui->leCmdValue->text().toInt();
    obj.qpa = ui->leCmdValue->text().toInt();
    break;
  case iec104_class::C_CI_NA_1:
    obj.value = ui->leCmdValue->text().toInt();
    obj.frz = static_cast<unsigned char>(
        ui->cbCmdDuration->currentText().left(1).toUInt());
    break;
  }
  obj.qu = static_cast<unsigned char>(
      ui->cbCmdDuration->currentText().left(1).toUInt());
  obj.se = static_cast<unsigned char>(ui->cbSBO->isChecked());

  queueProtocolCommand(obj);
  LastCommandAddress = obj.address;
}

void MainWindow::I104M_Loga(QString str, int id) {
  if (I104M_Logar && id == 0) {
    i104->mLog.pushMsg(const_cast<char *>(str.toStdString().c_str()), 0);
  }
}

void MainWindow::slot_dataIndication(const QVector<iec_obj>& objects) {
  if (objects.isEmpty()) {
    return;
  }

  pendingDataPointCount += objects.size();
  pendingDataIndications.enqueue(objects);

  if (!tmUiDataPump->isActive()) {
    tmUiDataPump->start(0);
  }
}

void MainWindow::slot_processPendingUiData() {
  if (pendingDataIndications.isEmpty()) {
    tmUiDataPump->stop();
    return;
  }

  const qsizetype maxPointsPerTick =
      pendingDataPointCount > 20000 ? 12000 : 4000;
  const qint64 maxMillisPerTick =
      pendingDataPointCount > 20000 ? 30 : 20;

  qsizetype pointsProcessed = 0;
  QElapsedTimer elapsed;
  elapsed.start();

  const bool updatePointMap = ui->cbPointMap->isChecked();
  if (updatePointMap) {
    ui->twPontos->setUpdatesEnabled(false);
  }

  while (!pendingDataIndications.isEmpty()) {
    const QVector<iec_obj> objects = pendingDataIndications.dequeue();
    pendingDataPointCount -= objects.size();
    processDataIndicationBatch(objects);
    pointsProcessed += objects.size();

    if (pointsProcessed >= maxPointsPerTick || elapsed.elapsed() >= maxMillisPerTick) {
      break;
    }
  }

  if (updatePointMap) {
    ui->twPontos->setUpdatesEnabled(true);
    ui->twPontos->viewport()->update();
  }

  if (pointTableSortPending && pendingDataIndications.isEmpty()) {
    ui->twPontos->sortItems(0);
    pointTableSortPending = false;
  }

  if (pendingDataIndications.isEmpty()) {
    tmUiDataPump->stop();
  } else {
    tmUiDataPump->start(0);
  }
}

void MainWindow::processDataIndicationBatch(const QVector<iec_obj>& objects) {
  char buf[1500];
  char buftt[1500];
  int rw = -1;
  bool inserted = false;
  QTableWidgetItem *pitem;
  static const char *dblmsg[] = {"tra ", "off ", "on ", "ind "};
  const iec_obj* obj = objects.constData();
  const unsigned numpoints = static_cast<unsigned>(objects.size());

  if (numpoints == 0) {
    return;
  }

  I104M_processPoints(obj, numpoints);

  if (ui->cbPointMap->isChecked()) {
    for (unsigned i = 0; i < numpoints; i++, obj++) {
      pitem = nullptr;
      pitem = mapPtItem_ColAddress[std::make_pair(obj->ca, obj->address)];
      if (pitem == nullptr) {
        if (ui->cb888Mode->isChecked()) {
            sprintf(buf, "%u-%u-%u", obj->address & 0xFF, (obj->address >> 8) & 0xFF, (obj->address >> 16) & 0xFF);
        } else {
            sprintf(buf, "%u", obj->address);
        }

        // insere
        rw = ui->twPontos->rowCount();
        ui->twPontos->insertRow(rw);
        QTableWidgetItem *newItem = new QTableWidgetItem(buf);
        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->twPontos->setItem(rw, 0, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColAddress[std::make_pair(obj->ca, obj->address)] = newItem;

        newItem = new QTableWidgetItem();
        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->twPontos->setItem(rw, 1, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColCommonAddress[std::make_pair(obj->ca, obj->address)] =
            newItem;

        newItem = new QTableWidgetItem();
        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->twPontos->setItem(rw, 2, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColValue[std::make_pair(obj->ca, obj->address)] = newItem;

        newItem = new QTableWidgetItem();
        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->twPontos->setItem(rw, 3, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColType[std::make_pair(obj->ca, obj->address)] = newItem;

        newItem = new QTableWidgetItem();
        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->twPontos->setItem(rw, 4, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColCause[std::make_pair(obj->ca, obj->address)] = newItem;

        newItem = new QTableWidgetItem();
        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->twPontos->setItem(rw, 5, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColFlags[std::make_pair(obj->ca, obj->address)] = newItem;

        newItem = new QTableWidgetItem("0");
        newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->twPontos->setItem(rw, 6, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColCount[std::make_pair(obj->ca, obj->address)] = newItem;

        newItem = new QTableWidgetItem();
        ui->twPontos->setItem(rw, 7, newItem);
        newItem->setFlags(Qt::ItemIsSelectable);
        mapPtItem_ColTimeTag[std::make_pair(obj->ca, obj->address)] = newItem;

        inserted = true;
      }

      sprintf(buf, "%9.3f", double(obj->value));
      mapPtItem_ColValue[std::make_pair(obj->ca, obj->address)]->setText(buf);
      sprintf(buf, "%u", obj->ca);
      mapPtItem_ColCommonAddress[std::make_pair(obj->ca, obj->address)]
          ->setText(buf);
      sprintf(buf, "%d:%s", obj->type, i104->asduTiStr(obj->type).c_str());
      mapPtItem_ColType[std::make_pair(obj->ca, obj->address)]->setText(buf);
      sprintf(buf, "%d:%s", obj->cause, i104->causeStr(obj->cause).c_str());
      mapPtItem_ColCause[std::make_pair(obj->ca, obj->address)]->setText(buf);
      sprintf(buf, "%d",
              1 + mapPtItem_ColCount[std::make_pair(obj->ca, obj->address)]
                      ->text()
                      .toInt());
      mapPtItem_ColCount[std::make_pair(obj->ca, obj->address)]->setText(buf);

      QDateTime current = QDateTime::currentDateTime();
      sprintf(
          buftt, "Local: %s",
          current.toString("yyyy/MM/dd hh:mm:ss.zzz").toStdString().c_str());

      switch (obj->type) {
      case iec104_class::M_EP_TD_1:
        fmtCP56Time(buftt, &obj->timetag);
        sprintf(buf, "%s%s%s%s%s%s %dms", dblmsg[obj->dp], obj->bl ? "bl " : "",
                obj->nt ? "nt " : "", obj->sb ? "sb " : "",
                obj->iv ? "iv " : "", obj->ei ? "ei " : "",
                obj->elapsed_time.milliseconds);
        break;
      case iec104_class::M_EP_TE_1:
        fmtCP56Time(buftt, &obj->timetag);
        sprintf(buf, "%s%s%s%s%s%s%s%s%s%s%s %dms", obj->bl ? "bl " : "",
                obj->nt ? "nt " : "", obj->sb ? "sb " : "",
                obj->iv ? "iv " : "", obj->ei ? "ei " : "",
                obj->spe.gs ? "gs " : "", obj->spe.sl1 ? "sl1 " : "",
                obj->spe.sl2 ? "sl2 " : "", obj->spe.sl3 ? "sl3 " : "",
                obj->spe.sie ? "sie " : "", obj->spe.srd ? "srd " : "",
                obj->elapsed_time.milliseconds);
        break;
      case iec104_class::M_EP_TF_1:
        fmtCP56Time(buftt, &obj->timetag);
        sprintf(buf, "%s%s%s%s%s%s%s%s%s %dms", obj->bl ? "bl " : "",
                obj->nt ? "nt " : "", obj->sb ? "sb " : "",
                obj->iv ? "iv " : "", obj->ei ? "ei " : "",
                obj->oci.gc ? "gc " : "", obj->oci.cl1 ? "cl1 " : "",
                obj->oci.cl2 ? "cl2 " : "", obj->oci.cl3 ? "cl3 " : "",
                obj->elapsed_time.milliseconds);
        break;

      case iec104_class::M_SP_TB_1: // 30
        fmtCP56Time(buftt, &obj->timetag);
        [[fallthrough]];
      case iec104_class::M_SP_NA_1: // 1
        sprintf(buf, "%s%s%s%s%s", obj->sp ? "on " : "off ",
                obj->iv ? "iv " : "", obj->bl ? "bl " : "",
                obj->sb ? "sb " : "", obj->nt ? "nt " : "");
        break;
      case iec104_class::M_DP_TB_1: // 31
        fmtCP56Time(buftt, &obj->timetag);
        [[fallthrough]];
      case iec104_class::M_DP_NA_1: // 3
        sprintf(buf, "%s%s%s%s%s", dblmsg[obj->dp], obj->iv ? "iv " : "",
                obj->bl ? "bl " : "", obj->sb ? "sb " : "",
                obj->nt ? "nt " : "");
        break;
      case iec104_class::M_ST_TB_1: // 32
        fmtCP56Time(buftt, &obj->timetag);
        [[fallthrough]];
      case iec104_class::M_ST_NA_1: // 5
        sprintf(buf, "%s%s%s%s%s%s", obj->ov ? "ov " : "", obj->iv ? "iv " : "",
                obj->bl ? "bl " : "", obj->sb ? "sb " : "",
                obj->nt ? "nt " : "", obj->t ? "t " : "");
        break;

      case iec104_class::M_IT_TB_1: // 37
        fmtCP56Time(buftt, &obj->timetag);
        [[fallthrough]];
      case iec104_class::M_IT_NA_1: // 15
        sprintf(buf, "%s%s%s%s%u", obj->iv ? "iv " : "", obj->cadj ? "ca " : "",
                obj->cy ? "cy " : "", "sq=", obj->sq);
        break;

      case iec104_class::M_PS_NA_1: // 38
        sprintf(buf,
                "%s%s%s%s%s ST %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d CH %d%d%d%d "
                "%d%d%d%d %d%d%d%d %d%d%d%d [1-16]",
                obj->ov ? "ov " : "", obj->bl ? "bl " : "",
                obj->nt ? "nt " : "", obj->sb ? "sb " : "",
                obj->iv ? "iv " : "", obj->stcd.st1, obj->stcd.st2,
                obj->stcd.st3, obj->stcd.st4, obj->stcd.st5, obj->stcd.st6,
                obj->stcd.st7, obj->stcd.st8, obj->stcd.st9, obj->stcd.st10,
                obj->stcd.st11, obj->stcd.st12, obj->stcd.st13, obj->stcd.st14,
                obj->stcd.st15, obj->stcd.st16, obj->stcd.cd1, obj->stcd.cd2,
                obj->stcd.cd3, obj->stcd.cd4, obj->stcd.cd5, obj->stcd.cd6,
                obj->stcd.cd7, obj->stcd.cd8, obj->stcd.cd9, obj->stcd.cd10,
                obj->stcd.cd11, obj->stcd.cd12, obj->stcd.cd13, obj->stcd.cd14,
                obj->stcd.cd15, obj->stcd.cd16);
        break;

      case iec104_class::M_BO_TB_1: // 33
        fmtCP56Time(buftt, &obj->timetag);
        [[fallthrough]];
      case iec104_class::M_BO_NA_1: // 7
        sprintf(buf,
                "%s%s%s%s%s ST %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d "
                "%d%d%d%d %d%d%d%d %d%d%d%d [1-32]",
                obj->ov ? "ov " : "", obj->bl ? "bl " : "",
                obj->nt ? "nt " : "", obj->sb ? "sb " : "",
                obj->iv ? "iv " : "", obj->bsi.st1, obj->bsi.st2, obj->bsi.st3,
                obj->bsi.st4, obj->bsi.st5, obj->bsi.st6, obj->bsi.st7,
                obj->bsi.st8, obj->bsi.st9, obj->bsi.st10, obj->bsi.st11,
                obj->bsi.st12, obj->bsi.st13, obj->bsi.st14, obj->bsi.st15,
                obj->bsi.st16, obj->bsi.st17, obj->bsi.st18, obj->bsi.st19,
                obj->bsi.st20, obj->bsi.st21, obj->bsi.st22, obj->bsi.st23,
                obj->bsi.st24, obj->bsi.st25, obj->bsi.st26, obj->bsi.st27,
                obj->bsi.st28, obj->bsi.st29, obj->bsi.st30, obj->bsi.st31, obj->bsi.st32);
        break;

      case iec104_class::M_ME_TD_1: // 34
      case iec104_class::M_ME_TE_1: // 35
      case iec104_class::M_ME_TF_1: // 36
        fmtCP56Time(buftt, &obj->timetag);
        [[fallthrough]];
      case iec104_class::M_ME_NA_1: // 9
      case iec104_class::M_ME_NB_1: // 11
      case iec104_class::M_ME_NC_1: // 13
      case iec104_class::M_ME_ND_1: // 21
        sprintf(buf, "%s%s%s%s%s", obj->ov ? "ov " : "", obj->iv ? "iv " : "",
                obj->bl ? "bl " : "", obj->sb ? "sb " : "",
                obj->nt ? "nt " : "");
        break;
      }

      mapPtItem_ColFlags[std::make_pair(obj->ca, obj->address)]->setText(buf);
      mapPtItem_ColTimeTag[std::make_pair(obj->ca, obj->address)]->setText(
          buftt);
    }

    if (inserted) {
      pointTableSortPending = true;
      pointTableResizePending = true;
    }
  }
}

void MainWindow::slot_timer_logmsg() {
  static const int maxLogMsgsPerTick = 250;

  if (Hide)
    if (window() && window()->isVisible())
      window()->setVisible(false);

  // adjust size of rows and columns
  if (!(++logTickCount % 50))
    if (pointTableResizePending && pendingDataIndications.isEmpty()) {
      ui->twPontos->resizeRowsToContents();
      ui->twPontos->resizeColumnsToContents();
      pointTableResizePending = false;
    }

  if (i104->mLog.haveMsg()) {
    // append the whole batch inside a single edit block: one layout/paint pass;
    // the document's maximumBlockCount (set in the .ui) discards the oldest
    // lines automatically once the log is full
    int logMsgsProcessed = 0;
    QTextCursor cur(ui->lwLog->document());
    cur.movePosition(QTextCursor::End);
    cur.beginEditBlock();
    while (i104->mLog.haveMsg() && logMsgsProcessed < maxLogMsgsPerTick) {
      const QString msg = QString::fromStdString(i104->mLog.pullMsg());

      QTextCharFormat fmt;
      if (msg.contains(QLatin1String("I104M"))) {
        fmt.setForeground(Qt::darkGray);
      } else if (msg.contains(QLatin1String("COMMAND"))) {
        fmt.setForeground(Qt::darkGray);
        fmt.setBackground(Qt::lightGray);
      } else if (msg.contains(QLatin1Char('['))) {
        fmt.setForeground(Qt::red);
      } else {
        fmt.setForeground(Qt::darkGray);
      }

      if (!cur.atStart())
        cur.insertBlock();
      cur.insertText(msg, fmt);
      logMsgsProcessed++;
    }
    cur.endEditBlock();

    if (ui->cbAutoScroll->isChecked()) {
      QScrollBar* vbar = ui->lwLog->verticalScrollBar();
      vbar->setValue(vbar->maximum());
    }
  }
}

void MainWindow::slot_interrogationActConfIndication() {}

void MainWindow::slot_interrogationActTermIndication() {}

void MainWindow::slot_tcpconnect(const QString& peerAddress) {
  ui->leIPRemoto->setText(peerAddress);
  ui->lbStatus->setText("<font color='green'> TCP CONNECTED!</font>");
  ui->pbGI->setEnabled(true);
  ui->pbSendCommandsButton->setEnabled(true);
  ui->pbConnect->setText("Disconnect");
}

void MainWindow::slot_tcpdisconnect() {
  if (EnableI104M && I104M_HaveDualHost() && isPrimary == true) {
    I104M_CntDnToBePrimary =
        I104M_CntToBePrimary + 1; // wait a little more time to be primary again
                                  // to allow for the secondary to assume
    isPrimary = false;
    queueProtocolCall([](QIec104* worker) { worker->disable_connect(); });
    I104M_Loga(" --- I104M: BECOMING SECONDARY BY DISCONNECTION");
    ui->lbMode->setText("<font color='red'>Secondary</font>");
  }

  ui->lbStatus->setText("<font color='red'> TCP DISCONNECTED!</font>");
  ui->pbGI->setEnabled(false);
  ui->pbSendCommandsButton->setEnabled(false);

  if (ProtocolKeepAliveActive) {
    ui->pbConnect->setText("Give up");
    ui->lePort->setEnabled(false);
    ui->leIPRemoto->setEnabled(false);
    ui->leLinkAddress->setEnabled(false);
    ui->leMasterAddress->setEnabled(false);
  } else {
    ui->pbConnect->setText("Connect");
    ui->lePort->setEnabled(true);
    ui->leIPRemoto->setEnabled(true);
    ui->leLinkAddress->setEnabled(true);
    ui->leMasterAddress->setEnabled(true);
  }
}

void MainWindow::slot_commandActRespIndication(const iec_obj& obj) {

  char buf[1000];
  char buftt[1000];
  int rw = -1;
  QTableWidgetItem *pitem;
  static const char *pnmsg[] = {"pos ", "neg "};
  static const char *selmsg[] = {"exe ", "sel "};
  static const char *sglmsg[] = {"off ", "on "};
  static const char *dblmsg[] = {"tra ", "off ", "on ", "ind "};
  static const char *qumsg[] = {"uns ", "shp ", "lop ", "per ", "res "};
  static const char *rcsmsg[] = {"na0 ", "dec ", "inc ", "na3 "};
  static const char *kpamsg[] = {"unu ", "thr ", "fil ",
                                 "lli ", "hli ", "res "};
  // static const char* qpamsg[] = { "unu ", "gen ", "obj ", "trm ", "res " };
  // clamp device provided indexes to the "reserved" entry of the tables above
  const unsigned qu_idx = obj.qu > 3 ? 4 : obj.qu;
  const unsigned kpa_idx = obj.kpa > 4 ? 5 : obj.kpa;
  const unsigned qpa_idx = obj.qpa > 4 ? 5 : obj.qpa;

  if (obj.address == 0)
    return;

  if (ui->cbPointMap->isChecked()) {
    pitem = nullptr;
    pitem = mapPtItem_ColAddress[std::make_pair(obj.ca, obj.address)];
    if (pitem == nullptr) {
      sprintf(buf, "%06u", obj.address);

      // insere
      rw = ui->twPontos->rowCount();
      ui->twPontos->insertRow(rw);
      QTableWidgetItem *newItem = new QTableWidgetItem(buf);
      newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      ui->twPontos->setItem(rw, 0, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColAddress[std::make_pair(obj.ca, obj.address)] = newItem;

      newItem = new QTableWidgetItem();
      newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      ui->twPontos->setItem(rw, 1, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColCommonAddress[std::make_pair(obj.ca, obj.address)] =
          newItem;

      newItem = new QTableWidgetItem();
      newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      ui->twPontos->setItem(rw, 2, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColValue[std::make_pair(obj.ca, obj.address)] = newItem;

      newItem = new QTableWidgetItem();
      newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      ui->twPontos->setItem(rw, 3, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColType[std::make_pair(obj.ca, obj.address)] = newItem;

      newItem = new QTableWidgetItem();
      newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      ui->twPontos->setItem(rw, 4, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColCause[std::make_pair(obj.ca, obj.address)] = newItem;

      newItem = new QTableWidgetItem();
      newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      ui->twPontos->setItem(rw, 5, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColFlags[std::make_pair(obj.ca, obj.address)] = newItem;

      newItem = new QTableWidgetItem("0");
      newItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
      ui->twPontos->setItem(rw, 6, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColCount[std::make_pair(obj.ca, obj.address)] = newItem;

      newItem = new QTableWidgetItem();
      ui->twPontos->setItem(rw, 7, newItem);
      newItem->setFlags(Qt::ItemIsSelectable);
      mapPtItem_ColTimeTag[std::make_pair(obj.ca, obj.address)] = newItem;
    }

    sprintf(buf, "%9.3f", double(obj.value));
    mapPtItem_ColValue[std::make_pair(obj.ca, obj.address)]->setText(buf);
    sprintf(buf, "%u", obj.ca);
    mapPtItem_ColCommonAddress[std::make_pair(obj.ca, obj.address)]->setText(
        buf);
    sprintf(buf, "%d", obj.type);
    mapPtItem_ColType[std::make_pair(obj.ca, obj.address)]->setText(buf);
    sprintf(buf, "%d", obj.cause);
    mapPtItem_ColCause[std::make_pair(obj.ca, obj.address)]->setText(buf);
    sprintf(buf, "%d",
            1 + mapPtItem_ColCount[std::make_pair(obj.ca, obj.address)]
                    ->text()
                    .toInt());
    mapPtItem_ColCount[std::make_pair(obj.ca, obj.address)]->setText(buf);

    QDateTime current = QDateTime::currentDateTime();
    sprintf(buftt, "Local: %s",
            current.toString("yyyy/MM/dd hh:mm:ss.zzz").toStdString().c_str());
    switch (obj.type) {
    case iec104_class::C_SC_TA_1:
      fmtCP56Time(buftt, &obj.timetag);
      [[fallthrough]];
    case iec104_class::C_SC_NA_1:
      sprintf(buf, "%d", int(obj.scs));
      mapPtItem_ColValue[std::make_pair(obj.ca, obj.address)]->setText(buf);
      sprintf(buf, "%s%s%s%s", pnmsg[obj.pn], sglmsg[obj.scs],
              selmsg[obj.se], qumsg[qu_idx]);
      break;
    case iec104_class::C_DC_TA_1:
      fmtCP56Time(buftt, &obj.timetag);
      [[fallthrough]];
    case iec104_class::C_DC_NA_1:
      sprintf(buf, "%d", int(obj.dcs));
      mapPtItem_ColValue[std::make_pair(obj.ca, obj.address)]->setText(buf);
      sprintf(buf, "%s%s%s%s", pnmsg[obj.pn], dblmsg[obj.dcs],
              selmsg[obj.se], qumsg[qu_idx]);
      break;
    case iec104_class::C_RC_TA_1:
      fmtCP56Time(buftt, &obj.timetag);
      [[fallthrough]];
    case iec104_class::C_RC_NA_1:
      sprintf(buf, "%d", int(obj.rcs));
      mapPtItem_ColValue[std::make_pair(obj.ca, obj.address)]->setText(buf);
      sprintf(buf, "%s%s%s", pnmsg[obj.pn], rcsmsg[obj.rcs], selmsg[obj.se]);
      break;
    case iec104_class::C_SE_TA_1:
      fmtCP56Time(buftt, &obj.timetag);
      [[fallthrough]];
    case iec104_class::C_SE_NA_1:
      sprintf(buf, "%s%s", pnmsg[obj.pn], selmsg[obj.se]);
      break;
    case iec104_class::C_SE_TB_1:
      fmtCP56Time(buftt, &obj.timetag);
      [[fallthrough]];
    case iec104_class::C_SE_NB_1:
      sprintf(buf, "%s%s", pnmsg[obj.pn], selmsg[obj.se]);
      break;
    case iec104_class::C_SE_TC_1:
      fmtCP56Time(buftt, &obj.timetag);
      [[fallthrough]];
    case iec104_class::C_SE_NC_1:
      sprintf(buf, "%s%s", pnmsg[obj.pn], selmsg[obj.se]);
      break;
    case iec104_class::C_BO_TA_1:
      fmtCP56Time(buftt, &obj.timetag);
      [[fallthrough]];
    case iec104_class::C_BO_NA_1:
      sprintf(buf, "%s", pnmsg[obj.pn]);
      break;
    case iec104_class::P_ME_NA_1:
      [[fallthrough]];
    case iec104_class::P_ME_NB_1:
      [[fallthrough]];
    case iec104_class::P_ME_NC_1:
      sprintf(buf, "%s%s%s%s", pnmsg[obj.pn], kpamsg[kpa_idx],
              obj.lpc ? "lpc " : "", obj.pop ? "pop " : "");
      break;
    case iec104_class::P_AC_NA_1:
      sprintf(buf, "%s%s", pnmsg[obj.pn], kpamsg[qpa_idx]);
      break;
    }

    mapPtItem_ColFlags[std::make_pair(obj.ca, obj.address)]->setText(buf);
    mapPtItem_ColTimeTag[std::make_pair(obj.ca, obj.address)]->setText(buftt);
  }

  bool is_select = false;

  if (obj.cause == iec104_class::REQUEST ||
      obj.cause == iec104_class::ACTIVATION ||
      obj.cause == iec104_class::ACTCONFIRM)
    if (LastCommandAddress == obj.address) {
      i104->mLog.pushMsg("     COMMAND CONF INDICATION");
      is_select = (obj.se == iec104_class::SELECT);

      // if confirmed select, execute
      if (is_select && obj.pn == iec104_class::POSITIVE) {
        // if defined ASDU address on UI, use it
        // else will set to zero and use slave address (send Command will
        // substitute zero to slave address)
        iec_obj executeObj = obj;
        executeObj.ca = ui->leASDUAddr->text().toUShort();
        executeObj.se = iec104_class::EXECUTE;
        queueProtocolCommand(executeObj);
      }

      // respond to I104M only if it's not a select or if its a negative
      // response
      if ((is_select == false || obj.pn == iec104_class::NEGATIVE) &&
          EnableI104M && udps != nullptr) {
        t_msgsup I104M_msg;
        I104M_msg.signature = MSGSUP_SIG;
        I104M_msg.tipo = obj.type;
        I104M_msg.endereco = obj.address;
        I104M_msg.sec = obj.ca;
        I104M_msg.prim = unsigned(PrimaryAddress);
        // mask cause, and p/n result to bit 6 1=NEG 0=POS
        I104M_msg.causa = unsigned(
            obj.cause | (((obj.pn == iec104_class::NEGATIVE) ? 1 : 0) << 6));

        switch (obj.type) {
        case iec104_class::C_SC_NA_1:
        case iec104_class::C_SC_TA_1:
          I104M_msg.taminfo = 1;
          I104M_msg.info[0] = obj.scs;
          break;
        case iec104_class::C_DC_NA_1:
        case iec104_class::C_DC_TA_1:
          I104M_msg.taminfo = 1;
          I104M_msg.info[0] = obj.dcs;
          break;
        case iec104_class::C_RC_NA_1:
        case iec104_class::C_RC_TA_1:
          I104M_msg.taminfo = 1;
          I104M_msg.info[0] = obj.rcs;
          break;
        case iec104_class::C_SE_NA_1:
        case iec104_class::C_SE_TA_1:
          I104M_msg.taminfo = 4;
          *(reinterpret_cast<float *>(&I104M_msg.info)) = obj.value;
          break;

        case iec104_class::C_SE_NB_1:
        case iec104_class::C_SE_TB_1:
          I104M_msg.taminfo = 4;
          *(reinterpret_cast<float *>(&I104M_msg.info)) = obj.value;
          break;

        case iec104_class::C_SE_NC_1:
        case iec104_class::C_SE_TC_1:
          I104M_msg.taminfo = 4;
          *(reinterpret_cast<float *>(&I104M_msg.info)) = obj.value;
          break;

        case iec104_class::C_BO_NA_1:
        case iec104_class::C_BO_TA_1:
          I104M_msg.taminfo = 4;
          *(reinterpret_cast<uint32_t *>(&I104M_msg.info)) =
              uint32_t(obj.value);
          break;
        }
        if (obj.pn == iec104_class::NEGATIVE) {
          I104M_Loga("T<-- I104M: COMMAND REJECTED BY IEC104 SLAVE");
        } else {
          I104M_Loga("T<-- I104M: COMMAND ACCEPTED BY IEC104 SLAVE");
        }
        udps->writeDatagram(reinterpret_cast<char *>(&I104M_msg),
                            sizeof(I104M_msg), I104M_host, I104M_porta);
        if (I104M_HaveDualHost())
          udps->writeDatagram(reinterpret_cast<char *>(&I104M_msg),
                              sizeof(I104M_msg), I104M_host_dual, I104M_porta);
      }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  shutdownProtocolThread();
  event->accept();
}

void MainWindow::slot_timer_I104M_kamsg() {
  if (!EnableI104M || udps == nullptr) {
    return;
  }

  if (!isPrimary) {
    if (I104M_CntDnToBePrimary <= 0) {
      isPrimary = true;
      queueProtocolCall([](QIec104* worker) {
        worker->enable_connect();
        worker->tmKeepAlive->start();
      });
      I104M_CntDnToBePrimary = I104M_CntToBePrimary;
      I104M_Loga(" --- I104M: BECOMING PRIMARY BY TIMEOUT");
      ui->lbMode->setText("<font color='green'>Primary</font>");
    } else
      I104M_CntDnToBePrimary--;
  }

  if (isPrimary) {
    // send keepalive message to the dual host
    t_msgcmd i104M_msg;
    i104M_msg.signature = MSGCMD_SIG;
    i104M_msg.tipo = I104M_ASDU_SPECIAL_CMD;
    i104M_msg.endereco = I104M_SPECIAL_CMD_ADDR_KEEP_ALIVE;
    i104M_msg.setpoint_i32 = 0;
    i104M_msg.sbo = 0;
    i104M_msg.qu = 0;
    i104M_msg.utr = 0;
    udps->writeDatagram(reinterpret_cast<char *>(&i104M_msg), sizeof(i104M_msg),
                        I104M_host_dual, I104M_porta_escuta);
  }
}

void MainWindow::on_cbLog_clicked() {
  if (ui->cbLog->isChecked()) {
    i104->mLog.activateLog();
    QDate dt = QDate::currentDate();
    QString str = dt.toString() + QString(" - ") + QString(QTESTER_VERSION);
    i104->mLog.pushMsg(str.toStdString().c_str());
  } else
    i104->mLog.deactivateLog();
}

void MainWindow::on_pbCopyClipb_clicked() {
  QApplication::clipboard()->setText(ui->lwLog->toPlainText());
}

void MainWindow::on_pbCopyVals_clicked() {
  QString text = "Address\tCA\tValue\tASDU\tCause\tFlags\tCount\tTimeTag\n";

  for (int i = 0; i < ui->twPontos->rowCount(); i++) {
    for (int j = 0; j < 8; j++) {
      text = text + ui->twPontos->item(i, j)->text() + "\t";
    }
    text = text + "\n";
  }

  QApplication::clipboard()->setText(text);
}

void MainWindow::I104M_processPoints(const iec_obj *obj, unsigned numpoints) {
  if (!EnableI104M || udps == nullptr || numpoints == 0) {
    return;
  }

  t_msgsupsq msg;

  switch (obj->type) {
  case iec104_class::M_DP_TB_1: { // double state with time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = obj->type;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(digital_w_time7_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(digital_w_time7_seq)));
      *paddr = obj->address;

      // value and qualifier
      digital_w_time7_seq *i104mobj =
          reinterpret_cast<digital_w_time7_seq *>(paddr + 1);
      i104mobj->iq =
          static_cast<unsigned char>(obj->dp | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
      i104mobj->ano = obj->timetag.year;
      i104mobj->mes = obj->timetag.month;
      i104mobj->dia = obj->timetag.mday;
      i104mobj->hora = obj->timetag.hour;
      i104mobj->min = obj->timetag.min;
      i104mobj->ms = obj->timetag.msec;
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints *
                      (sizeof(int32_t) + sizeof(digital_w_time7_seq)));
  } break;
  case iec104_class::M_SP_TB_1: { // single state with time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = obj->type;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(digital_w_time7_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(digital_w_time7_seq)));
      *paddr = obj->address;

      // value and qualifier
      digital_w_time7_seq *i104mobj =
          reinterpret_cast<digital_w_time7_seq *>(paddr + 1);
      i104mobj->iq =
          static_cast<unsigned char>(obj->sp | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
      i104mobj->ano = obj->timetag.year;
      i104mobj->mes = obj->timetag.month;
      i104mobj->dia = obj->timetag.mday;
      i104mobj->hora = obj->timetag.hour;
      i104mobj->min = obj->timetag.min;
      i104mobj->ms = obj->timetag.msec;
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints *
                      (sizeof(int32_t) + sizeof(digital_w_time7_seq)));
  } break;
  case iec104_class::M_DP_NA_1: { // double state without time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = obj->type;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(digital_notime_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(digital_notime_seq)));
      *paddr = obj->address;

      // value and qualifier
      digital_notime_seq *i104mobj =
          reinterpret_cast<digital_notime_seq *>(paddr + 1);
      i104mobj->iq =
          static_cast<unsigned char>(obj->dp | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints *
                      (sizeof(int32_t) + sizeof(digital_notime_seq)));
  } break;
  case iec104_class::M_SP_NA_1: { // single state without time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = obj->type;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(digital_notime_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int) + sizeof(digital_notime_seq)));
      *paddr = obj->address;

      // value and qualifier
      digital_notime_seq *i104mobj =
          reinterpret_cast<digital_notime_seq *>(paddr + 1);
      i104mobj->iq =
          static_cast<unsigned char>(obj->sp | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints *
                      (sizeof(int32_t) + sizeof(digital_notime_seq)));
  } break;

  case iec104_class::M_ST_TB_1:   // 32 = step with time tag (will ignore time)
  case iec104_class::M_ST_NA_1: { // 5 = step without time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = iec104_class::M_ST_NA_1;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(step_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(step_seq)));
      *paddr = obj->address;

      // value and qualifier
      step_seq *i104mobj = reinterpret_cast<step_seq *>(paddr + 1);
      i104mobj->qds =
          static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
      i104mobj->vti = static_cast<unsigned char>(obj->value) |
                      static_cast<unsigned char>(obj->t << 7);
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints * (sizeof(int32_t) + sizeof(step_seq)));
  } break;

  case iec104_class::M_ME_TD_1:   // 34 = normalized with time tag
  case iec104_class::M_ME_NA_1: { // 9 = normalized without time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = iec104_class::M_ME_NA_1;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(analogico_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(analogico_seq)));
      *paddr = obj->address;

      // value and qualifier
      analogico_seq *i104mobj = reinterpret_cast<analogico_seq *>(paddr + 1);
      i104mobj->qds =
          static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
      i104mobj->sva = static_cast<short>(obj->value);
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints * (sizeof(int32_t) + sizeof(analogico_seq)));
  } break;

  case iec104_class::M_ME_TE_1:   // 35 = scaled with time tag
  case iec104_class::M_ME_NB_1: { // 11 = scaled without time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = iec104_class::M_ME_NB_1;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(analogico_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(analogico_seq)));
      *paddr = obj->address;

      // value and qualifier
      analogico_seq *i104mobj = reinterpret_cast<analogico_seq *>(paddr + 1);
      i104mobj->qds =
          static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
      i104mobj->sva = static_cast<short>(obj->value);
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints * (sizeof(int32_t) + sizeof(analogico_seq)));
  } break;

  case iec104_class::M_ME_TF_1:   // 36 = float with time tag
  case iec104_class::M_ME_NC_1: { // 13 = float without time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = iec104_class::M_ME_NC_1;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(flutuante_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(flutuante_seq)));
      *paddr = obj->address;

      // value and qualifier
      flutuante_seq *i104mobj = reinterpret_cast<flutuante_seq *>(paddr + 1);
      i104mobj->qds =
          static_cast<unsigned char>(obj->ov | (obj->bl << 4) | (obj->sb << 5) |
                                     (obj->nt << 6) | (obj->iv << 7));
      i104mobj->fr = obj->value;
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints * (sizeof(int32_t) + sizeof(flutuante_seq)));
  } break;

  case iec104_class::M_IT_TB_1:   // 37 = integrated totals with time tag
  case iec104_class::M_IT_NA_1: { // 15 = integrated totals without time tag
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = iec104_class::M_IT_NA_1;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(integrated_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(integrated_seq)));
      *paddr = obj->address;

      // value and qualifier
      integrated_seq *i104mobj = reinterpret_cast<integrated_seq *>(paddr + 1);
      // map carry to overflow (bit 0), adjusted to nt (bit 6), invalid to bit 7
      i104mobj->qds = static_cast<unsigned char>(
          obj->cy | (obj->cadj << 6) | (obj->iv << 7));
      i104mobj->bcr = obj->bcr;
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints * (sizeof(int32_t) + sizeof(integrated_seq)));
  } break;
  case iec104_class::M_BO_TB_1: // 33 = bitstring of 32 bits with time tag (will
                                // send as counter)
  case iec104_class::M_BO_NA_1: { // 7 = bitstring of 32 bits (will send as
                                  // counter)
    // note: bitstring could also possibly be interpreted as 32 single binary
    // states in sequence I opt to use counter to avoid possible address
    // conflict for bitstrings in sequence of addresses
    msg.signature = MSGSUPSQ_SIG;
    msg.tipo = iec104_class::M_IT_NA_1;
    msg.prim = static_cast<uint32_t>(PrimaryAddress);
    msg.sec = obj->ca;
    msg.causa = obj->cause;
    msg.taminfo = sizeof(integrated_seq);
    msg.numpoints = static_cast<uint32_t>(numpoints);

    for (unsigned count = 0; count < numpoints; count++, obj++) {
      uint32_t *paddr = reinterpret_cast<uint32_t *>(
          msg.info + count * (sizeof(int32_t) + sizeof(integrated_seq)));
      *paddr = obj->address;

      // value as counter and no qualifier
      integrated_seq *i104mobj = reinterpret_cast<integrated_seq *>(paddr + 1);
      i104mobj->qds = 0;
      i104mobj->bcr = obj->bsi.bsi;
    }

    SendOSHMI(reinterpret_cast<char *>(&msg),
              sizeof(int32_t) * 7 +
                  msg.numpoints * (sizeof(int32_t) + sizeof(integrated_seq)));
  } break;
  default:
    i104->mLog.pushMsg(
        "R--> IEC104 UNSUPPORTED TYPE, NOT FORWARDED TO I104M/OSHMI");
    break;
  }
}

void MainWindow::SendOSHMI(char *msg, uint32_t packet_size) {
  if (udps == nullptr) {
    return;
  }

  udps->writeDatagram(reinterpret_cast<const char *>(msg), packet_size,
                      I104M_host, I104M_porta);
  if (I104M_HaveDualHost())
    udps->writeDatagram(reinterpret_cast<const char *>(msg), packet_size,
                        I104M_host_dual, I104M_porta);
}

void MainWindow::fmtCP56Time(char *buf, const cp56time2a *timetag) {
  if (timetag->month == 0 || timetag->mday == 0)
    return;
  sprintf(buf, "Field: %02d/%02d/%02d %02d:%02d:%02d.%03d %s %s", timetag->year,
          timetag->month, timetag->mday, timetag->hour, timetag->min,
          timetag->msec / 1000, timetag->msec % 1000,
          timetag->iv ? "iv" : "ok",
          timetag->su ? "su" : "");
}

void MainWindow::on_cbTheme_currentIndexChanged(int index) {
    if (index == 1) { // Dark
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53,53,53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25,25,25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53,53,53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        qApp->setPalette(darkPalette);
    } else { // Light
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette sp = QApplication::style()->standardPalette();
        sp.setColor(QPalette::Text, Qt::black);
        qApp->setPalette(sp);
    }
}

void MainWindow::on_cbEnableTls_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    queueProtocolCall([enabled = ui->cbEnableTls->isChecked()](QIec104* worker) {
        worker->setTlsEnabled(enabled);
    });
}

void MainWindow::on_cb888Mode_stateChanged(int arg1)
{
    ui->leCmdAddress->setVisible(!arg1);
    ui->leCmdAddressLow->setVisible(arg1);
    ui->leCmdAddressMid->setVisible(arg1);
    ui->leCmdAddressHigh->setVisible(arg1);

    for (auto const& pair : mapPtItem_ColAddress) {
        unsigned int address = pair.first.second;
        char buf[64];
        if (arg1) {
            sprintf(buf, "%u-%u-%u", address & 0xFF, (address >> 8) & 0xFF, (address >> 16) & 0xFF);
        } else {
            sprintf(buf, "%u", address);
        }
        pair.second->setText(buf);
    }
}
