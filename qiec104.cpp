/*
 * This software implements an IEC 60870-5-104 protocol tester.
 * Copyright © 2010-2024 Ricardo L. Olsen
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

#include "qiec104.h"
#include <QFile>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslKey>
#include <QtWidgets/QApplication>

QIec104::QIec104(QObject *parent) : QObject(parent) {
  mEnding = false;
  mAllowConnect = true;
  SendCommands = 0;
  ForcePrimary = 0;
  mLog.activateLog();
  mLog.doLogTime();

  tcps = new QSslSocket(this);
  tmKeepAlive = new QTimer();

  connect(tmKeepAlive, SIGNAL(timeout()), this, SLOT(slot_keep_alive()));
  connect(tcps, SIGNAL(readyRead()), this, SLOT(slot_tcpreadytoread()));
  connect(tcps, SIGNAL(connected()), this, SLOT(slot_tcpconnect()));
  connect(tcps, SIGNAL(modeChanged(QSslSocket::SslMode)), this,
          SLOT(slot_modeChanged(QSslSocket::SslMode)));
  connect(tcps, SIGNAL(disconnected()), this, SLOT(slot_tcpdisconnect()));
  connect(tcps, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this,
          SLOT(slot_tcperror(QAbstractSocket::SocketError)),
          Qt::DirectConnection);

  if (mUseTls) {
      connect(tcps, &QSslSocket::sslErrors, this, &QIec104::slot_sslErrors);
      connect(tcps, &QSslSocket::errorOccurred, this, &QIec104::slot_socketError);
      connect(tcps, &QSslSocket::encrypted, this, &QIec104::slot_socketEncrypted);
      connect(tcps, &QSslSocket::handshakeInterruptedOnError, this,
              &QIec104::slot_handshakeInterruptedOnError);
  }

  // tcps->moveToThread(&tcpThread);
  // tcpThread.start(QThread::TimeCriticalPriority);
}

QIec104::~QIec104() {
  delete tmKeepAlive;
  delete tcps;
}

void QIec104::setTlsEnabled(bool enabled) {
  mUseTls = enabled;
  mLog.pushMsg(QString("TLS Enabled: %1")
                   .arg(mUseTls ? "Yes" : "No")
                   .toStdString()
                   .c_str());
}

void QIec104::setCaCertPath(const QString &path) {
  mCaCertPath = path;
  mLog.pushMsg(QString("CA Cert Path: %1").arg(path).toStdString().c_str());
}

void QIec104::setLocalCertPath(const QString &path) {
  mLocalCertPath = path;
  mLog.pushMsg(QString("Local Cert Path: %1").arg(path).toStdString().c_str());
}

void QIec104::setPrivateKeyPath(const QString &path) {
  mPrivateKeyPath = path;
  mLog.pushMsg(QString("Private Key Path: %1").arg(path).toStdString().c_str());
}

void QIec104::setPeerVerifyMode(QSslSocket::PeerVerifyMode mode) {
    mVerifyMode = mode;
}

void QIec104::waitBytes(int bytes, int msTout) {
  while (tcps->bytesAvailable() < bytes && msTout > 0) {
    tcps->waitForReadyRead(8);
    msTout -= 8;
  }
}

void QIec104::dataIndication(iec_obj *obj, unsigned numpoints) {
  emit signal_dataIndication(obj, numpoints);
}

void QIec104::connectTCP() {
  static int cnt = 0;
  char buf[100];

  tcps->abort();
  if (!mEnding && mAllowConnect) {

      if (mUseTls) {
          mLog.pushMsg("Attempting TLS connection...");

          QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
          sslConfig.setProtocol(QSsl::TlsV1_0OrLater);

          // Load CA certificates for peer verification
          if (!mCaCertPath.isEmpty()) {
              QList<QSslCertificate> caCerts = QSslCertificate::fromPath(mCaCertPath);
              if (caCerts.isEmpty()) {
                  mLog.pushMsg(
                      QString("Warning: Could not load CA certificate(s) from %1")
                          .arg(mCaCertPath)
                          .toStdString()
                          .c_str());
              } else {
                  sslConfig.setCaCertificates(caCerts);
                  mLog.pushMsg(QString("Loaded %1 CA certificate(s) from %2")
                                   .arg(caCerts.count())
                                   .arg(mCaCertPath)
                                   .toStdString()
                                   .c_str());
              }
          }

          // Load local certificate (optional for client, required if server needs
          // client auth)
          if (!mLocalCertPath.isEmpty()) {
              QList<QSslCertificate> localCerts =
                  QSslCertificate::fromPath(mLocalCertPath);
              if (!localCerts.isEmpty()) {
                  sslConfig.setLocalCertificate(localCerts.first());
                  mLog.pushMsg(QString("Loaded local certificate from %1")
                                   .arg(mLocalCertPath)
                                   .toStdString()
                                   .c_str());
              } else {
                  mLog.pushMsg(
                      QString("Warning: Could not load local certificate from %1")
                          .arg(mLocalCertPath)
                          .toStdString()
                          .c_str());
              }
          }

          // Load private key
          if (!mPrivateKeyPath.isEmpty()) {
              QFile keyFile(mPrivateKeyPath);
              if (keyFile.open(QIODevice::ReadOnly)) {
                  QSslKey privateKey(&keyFile, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
                  keyFile.close();
                  if (privateKey.isNull()) {
                      mLog.pushMsg(QString("Warning: Could not load private key from %1")
                                       .arg(mPrivateKeyPath)
                                       .toStdString()
                                       .c_str());
                  } else {
                      sslConfig.setPrivateKey(privateKey);
                      mLog.pushMsg(QString("Loaded private key from %1")
                                       .arg(mPrivateKeyPath)
                                       .toStdString()
                                       .c_str());
                  }
              } else {
                  mLog.pushMsg(QString("Warning: Could not open private key file %1")
                                   .arg(mPrivateKeyPath)
                                   .toStdString()
                                   .c_str());
              }
          }

          sslConfig.setSslOption(QSsl::SslOptionDisableServerNameIndication, true);
          QSslConfiguration::setDefaultConfiguration(sslConfig);
          tcps->setSslConfiguration(sslConfig);
          tcps->setPeerVerifyMode(mVerifyMode); // Set verification mode
          if (mVerifyMode > 1) {
              mLog.pushMsg("Verifying peer certificate: true");
          } else {
              mLog.pushMsg("Verifying peer certificate: false");
          }
          mLog.pushMsg(
              QSslSocket::sslLibraryBuildVersionString().toStdString().c_str());
          mLog.pushMsg(QSslSocket::sslLibraryVersionString().toStdString().c_str());
      }

    // alternate main and backup UTR IP address, if configured
    if ((++cnt) % 2 || strcmp(getSecondaryIP_backup(), "") == 0) {

      if (mUseTls) {
        tcps->connectToHostEncrypted(getSecondaryIP(), quint16(getPortTCP()));
      } else {
        tcps->connectToHost(getSecondaryIP(), quint16(getPortTCP()),
                              QIODevice::ReadWrite);
      }
      sprintf(buf, "Try to connect IP: %s", getSecondaryIP());
      mLog.pushMsg(const_cast<char *>(buf));

    } else {

      if (mUseTls) {
        tcps->connectToHostEncrypted(getSecondaryIP_backup(),
                                       quint16(getPortTCP()));
      } else {
        tcps->connectToHost(getSecondaryIP_backup(), quint16(getPortTCP()),
                              QIODevice::ReadWrite);
      }
      sprintf(buf, "Try to connect IP: %s", getSecondaryIP_backup());
      mLog.pushMsg(const_cast<char *>(buf));
    }
  }
}

void QIec104::disconnectTCP() { tcps->abort(); }

void QIec104::slot_tcperror(QAbstractSocket::SocketError socketError) {
  if (socketError != QAbstractSocket::SocketTimeoutError) {
    char buf[100];
    sprintf(buf, "SocketError: %d", socketError);
    mLog.pushMsg(const_cast<char *>(buf));
  }
}

int QIec104::readTCP(char *buf, int szmax) {
  int ret = int(tcps->read(buf, szmax));

  if (!mEnding && ret > 0)
    return ret;
  else
    return 0;
}

// send tcp data, user provided
void QIec104::sendTCP(char *data, int sz) {
  if (tcps->state() == QAbstractSocket::ConnectedState)
    if (!mEnding) {
      tcps->write(data, sz);
      tcps->flush();
      if (mLog.isLogging())
        LogFrame(data, sz, true);
    }
}

void QIec104::slot_tcpconnect() {
  tcps->setSocketOption(QAbstractSocket::LowDelayOption, 1);
  if (mUseTls) {
      // mLog.pushMsg("TLS Handshake Successful. Connection Encrypted.");
  } else {
      mLog.pushMsg("Plain TCP Connection Established.");
  }
  onConnectTCP();
  emit signal_tcp_connect();
}

void QIec104::slot_modeChanged(QSslSocket::SslMode mode) {
    if (mode == QSslSocket::SslClientMode) {
        mLog.pushMsg("TLS Handshake Successful. Connection Encrypted.");
    }
}

void QIec104::slot_tcpdisconnect() {
  onDisconnectTCP();
  emit signal_tcp_disconnect();
}

void QIec104::slot_keep_alive() {
  static unsigned int cnts = 1;

  if (!mEnding) {
    cnts++;

    if (!(cnts % 5))
      if (tcps->state() != QAbstractSocket::ConnectedState && mAllowConnect) {
        mLog.pushMsg("!!!!!TRY TO CONNECT!");
        connectTCP();
      }

    onTimerSecond();
  }
}

void QIec104::interrogationActConfIndication() {
  emit signal_interrogationActConfIndication();
}

void QIec104::interrogationActTermIndication() {
  emit signal_interrogationActTermIndication();
}

void QIec104::commandActRespIndication(iec_obj *obj) {
  emit signal_commandActRespIndication(obj);
}

void QIec104::terminate() {
  mEnding = true;
  tcps->abort();
  // tcpThread.quit();
  // tcpThread.wait(1000);
  // if (tcpThread.isRunning())
  //   tcpThread.terminate();
  // if (tcpThread.isRunning())
  //   tcpThread.wait(2000);
}

void QIec104::slot_tcpreadytoread() {
  if (tcps->bytesAvailable() < 6)
    tcps->waitForReadyRead(8);

  packetReadyTCP();
}

void QIec104::disable_connect() {
  mAllowConnect = false;
  if (tcps->state() == QAbstractSocket::ConnectedState)
    disconnectTCP();
}

void QIec104::enable_connect() { mAllowConnect = true; }

int QIec104::bytesAvailableTCP() { return int(tcps->bytesAvailable()); }

void QIec104::slot_sslErrors(const QList<QSslError> &errors) {
  QString errorStrings;
  for (const QSslError &error : errors) {
      errorStrings += error.errorString() + "; ";
      if (error.error() == QSslError::SelfSignedCertificate) {
          mLog.pushMsg("Ignoring self-signed certificate error (for testing).");
          tcps->ignoreSslErrors();
      }
  }
  mLog.pushMsg(QString("SSL Errors Encountered: %1")
                   .arg(errorStrings)
                   .toStdString()
                   .c_str());
  // tcps->ignoreSslErrors();
}

void QIec104::slot_socketError(QAbstractSocket::SocketError) {
    mLog.pushMsg(tcps->errorString().toStdString().c_str());
}

void QIec104::slot_socketEncrypted() { mLog.pushMsg("Encrypted slot!"); }

void QIec104::slot_handshakeInterruptedOnError(const QSslError &) {
    mLog.pushMsg(tcps->errorString().toStdString().c_str());
    mLog.pushMsg("Handshake interrupted, trying to continue...");
    tcps->continueInterruptedHandshake();
}
