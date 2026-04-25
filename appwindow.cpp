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

#include "appwindow.h"
#include "mainwindow.h"

#include <QCoreApplication>
#include <QFile>
#include <QRegularExpression>
#include <QSettings>
#include <QTabWidget>
#include <algorithm>

namespace {
constexpr auto kCurrentDirIniFile = "/qtester104.ini";
constexpr auto kConfDirIniFile = "../conf/qtester104.ini";
}

AppWindow::AppWindow(QWidget* parent)
    : QMainWindow(parent), mTabs(new QTabWidget(this)), mIniPath(resolveIniPath()) {
  setCentralWidget(mTabs);
  setWindowTitle(QStringLiteral("QTester104 IEC60870-5-104"));

  QSettings settings(mIniPath, QSettings::IniFormat);
  const QStringList rtuSections = discoverRtuSections(settings);

  for (int index = 0; index < rtuSections.size(); ++index) {
    const QString& section = rtuSections.at(index);
    auto* client = new MainWindow(mIniPath, section, index == 0, mTabs);
    mTabs->addTab(client, tabTitleForSection(settings, section));
  }
}

AppWindow::~AppWindow() = default;

QString AppWindow::resolveIniPath() const {
  QString iniPath = QCoreApplication::applicationDirPath() + kCurrentDirIniFile;
  if (!QFile(iniPath).exists()) {
    iniPath = kConfDirIniFile;
  }

  if (QCoreApplication::arguments().count() > 1) {
    iniPath = QCoreApplication::arguments().at(1);
  }

  return iniPath;
}

QStringList AppWindow::discoverRtuSections(QSettings& settings) const {
  QStringList rtuSections;
  const QRegularExpression sectionPattern(QStringLiteral("^RTU(\\d+)$"));

  for (const QString& group : settings.childGroups()) {
    if (sectionPattern.match(group).hasMatch()) {
      rtuSections.append(group);
    }
  }

  if (rtuSections.isEmpty()) {
    rtuSections.append(QStringLiteral("RTU1"));
  }

  std::sort(rtuSections.begin(), rtuSections.end(),
            [&sectionPattern](const QString& left, const QString& right) {
              const auto leftMatch = sectionPattern.match(left);
              const auto rightMatch = sectionPattern.match(right);
              return leftMatch.captured(1).toInt() < rightMatch.captured(1).toInt();
            });

  return rtuSections;
}

QString AppWindow::tabTitleForSection(QSettings& settings,
                                      const QString& section) const {
  const QString ip = settings.value(section + "/IP_ADDRESS", "").toString().trimmed();
  if (ip.isEmpty()) {
    return section;
  }
  return section + " - " + ip;
}
