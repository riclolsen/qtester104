// Protocol behaviour tests for QTester104.
//
// Drives iec104_class with an in-memory "socket" (no network) and checks how
// received frames, sent commands and timers are handled; QIec104 is checked
// for its data hand-off and connection control. Exit code = number of failures.

#include "iec104_class.h"
#include "qiec104.h"

#include <QApplication>
#include <cstdio>
#include <deque>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(bool ok, const char* what) {
  std::printf("%s %s\n", ok ? "PASS" : "FAIL", what);
  if (!ok)
    failures++;
}

using Bytes = std::vector<unsigned char>;

// iec104_class over an in-memory byte stream
struct FakeLink : iec104_class {
  std::deque<unsigned char> in;
  std::vector<Bytes> out;
  std::vector<iec_obj> responses;
  std::vector<iec_obj> data;
  int disconnects = 0;
  bool acceptData = true;

  FakeLink() {
    setSecondaryAddress(1);
    onConnectTCP();
    out.clear(); // drop the STARTDTACT sent on connect
  }
  void waitBytes(int, int) override {}
  void connectTCP() override {}
  void disconnectTCP() override { disconnects++; }
  int readTCP(char* b, int n) override {
    int k = 0;
    while (k < n && !in.empty()) {
      b[k++] = char(in.front());
      in.pop_front();
    }
    return k;
  }
  void sendTCP(char* d, int n) override { out.emplace_back(d, d + n); }
  int bytesAvailableTCP() override { return int(in.size()); }
  void commandActRespIndication(iec_obj* o) override { responses.push_back(*o); }
  void dataIndication(iec_obj* o, unsigned n) override { data.insert(data.end(), o, o + n); }
  bool canAcceptData() override { return acceptData; }

  void feed(const Bytes& f) {
    in.insert(in.end(), f.begin(), f.end());
    packetReadyTCP();
  }
  void startDT() { feed({0x68, 4, 0x0b, 0, 0, 0}); } // STARTDTCON
  void ticks(int n) {
    for (int i = 0; i < n; i++)
      onTimerSecond();
  }
};

Bytes iframe(unsigned short ns, const Bytes& asdu) {
  Bytes f{0x68, static_cast<unsigned char>(4 + asdu.size()),
          static_cast<unsigned char>(ns & 0xff), static_cast<unsigned char>(ns >> 8), 0, 0};
  f.insert(f.end(), asdu.begin(), asdu.end());
  return f;
}

Bytes sframe(unsigned short nr) {
  return {0x68, 4, 0x01, 0, static_cast<unsigned char>(nr & 0xff), static_cast<unsigned char>(nr >> 8)};
}

// C_SC_NA_1 activation confirmation for object 100, select
const Bytes scConfirm{45, 1, 7, 0, 1, 0, 100, 0, 0, 0x81};

iec_obj singleCommand(unsigned address) {
  iec_obj c{};
  c.type = iec104_class::C_SC_NA_1;
  c.address = address;
  c.scs = 1;
  return c;
}

void commandTests() {
  FakeLink p;
  iec_obj c = singleCommand(100);
  check(!p.sendCommand(&c) && p.out.empty(), "command refused before STARTDTCON");
  p.startDT();
  p.setCommandsEnabled(false);
  check(!p.sendCommand(&c) && p.out.empty(), "command refused when commands are disabled");
  p.setCommandsEnabled(true);
  check(p.sendCommand(&c) && p.out.size() == 1, "command sent when enabled and ready");

  iec_obj big = singleCommand(0x1000001); // would wrap to address 1
  p.out.clear();
  check(!p.sendCommand(&big) && p.out.empty(), "address above 24 bits refused, not truncated");
  iec_obj maxAddr = singleCommand(0xFFFFFF);
  check(p.sendCommand(&maxAddr) && p.out.size() == 1, "address 16777215 accepted");

  iec_obj bits{};
  bits.type = iec104_class::C_BO_TA_1;
  bits.address = 200;
  bits.value = 0xA5A5A5A5u;
  p.out.clear();
  check(p.sendCommand(&bits) && p.out.size() == 1 && p.out[0].size() == 6 + 6 + 3 + 4 + 7,
        "C_BO_TA_1 sent with the right length");

  FakeLink g;
  g.solicitGI();
  g.solicitInterrogation(21);
  check(g.out.empty(), "GI and group interrogation refused before STARTDTCON");
  g.startDT();
  g.solicitGI();
  check(g.out.size() == 1, "GI sent after STARTDTCON");
}

void receiveTests() {
  FakeLink p;
  p.startDT();
  p.feed(iframe(0, scConfirm));
  check(p.responses.size() == 1, "valid command confirmation delivered");
  p.feed(iframe(2, {45, 1, 7, 0, 1, 0})); // header only, no object
  check(p.responses.size() == 1, "truncated command frame not replayed from stale bytes");

  // C_SC_TA_1 confirmation, time 2026-08-07 06:05:01.234
  p.feed(iframe(4, {58, 1, 7, 0, 1, 0, 101, 0, 0, 0x81, 0xd2, 0x04, 5, 6, 7, 8, 26}));
  check(p.responses.size() == 2 && p.responses[1].timetag.year == 26 &&
            p.responses[1].timetag.msec == 1234 && p.responses[1].timetag.mday == 7,
        "time tag of a timed command confirmation kept");

  p.feed(iframe(6, {51, 1, 7, 0, 1, 0, 200, 0, 0, 0xa5, 0xa5, 0xa5, 0xa5}));
  check(p.responses.size() == 3 && uint32_t(p.responses[2].value) == 0xA5A5A5A5u,
        "C_BO_NA_1 confirmation parsed");

  p.feed(iframe(8, {45, 0, 7, 0, 1, 0, 100, 0, 0, 0x81})); // zero objects
  check(p.responses.size() == 3 && p.disconnects == 0, "ASDU with zero objects ignored");
  p.feed(iframe(10, {45, 2, 7, 0, 1, 0, 100, 0, 0, 0x81, 101, 0, 0, 0x81}));
  check(p.responses.size() == 3, "command ASDU with two objects ignored");

  // monitored data: unset fields are zero, step position is signed
  FakeLink d;
  d.startDT();
  d.feed(iframe(0, {1, 2, 20, 0, 1, 0, 10, 0, 0, 1, 11, 0, 0, 0}));
  check(d.data.size() == 2 && d.data[0].sp == 1 && d.data[1].sp == 0 &&
            d.data[0].timetag.year == 0 && d.data[0].timetag.msec == 0,
        "M_SP_NA_1 parsed with unset fields zeroed");
  d.data.clear();
  d.feed(iframe(2, {5, 2, 20, 0, 1, 0, 20, 0, 0, 0x7F, 0, 21, 0, 0, 0x3F, 0}));
  check(d.data.size() == 2 && d.data[0].value == -1 && d.data[1].value == 63,
        "M_ST_NA_1 step position decoded as signed (-1, 63)");
  d.data.clear();
  d.feed(iframe(4, {32, 1, 20, 0, 1, 0, 22, 0, 0, 0x40, 0, 0, 0, 0, 0, 1, 1, 26}));
  check(d.data.size() == 1 && d.data[0].value == -64, "M_ST_TB_1 step position -64");
}

void framingTests() {
  FakeLink s;
  s.startDT();
  s.feed(iframe(2, scConfirm)); // first frame starting at N(S)=1
  check(s.disconnects == 0 && s.responses.size() == 1, "first I-frame at N(S)=1 accepted");
  s.feed(iframe(2, scConfirm));
  check(s.disconnects == 1 && s.responses.size() == 1, "repeated N(S)=1 is a sequence error");

  FakeLink f;
  f.startDT();
  Bytes bad = iframe(0, scConfirm);
  bad[2] = 0x01; // S-format control field, with an ASDU
  f.feed(bad);
  check(f.responses.empty() && f.disconnects == 1, "S-format frame carrying data rejected");
  FakeLink u;
  u.startDT();
  u.feed({0x68, 4, 0x00, 0, 0, 0}); // I-format without ASDU
  check(u.disconnects == 1, "I-format frame without ASDU rejected");

  FakeLink r;
  r.startDT();
  const Bytes frame = iframe(0, scConfirm);
  r.feed(Bytes(frame.begin(), frame.begin() + 8));
  check(r.responses.empty(), "partial frame waits for the rest");
  r.feed(Bytes(frame.begin() + 8, frame.end()));
  check(r.responses.size() == 1, "partial frame resumed on the next read");

  FakeLink b;
  b.startDT();
  b.acceptData = false;
  b.feed(iframe(0, scConfirm));
  check(b.responses.empty() && !b.in.empty(), "reading pauses while the consumer is full");
  b.acceptData = true;
  b.packetReadyTCP();
  check(b.responses.size() == 1 && b.in.empty(), "reading resumes with the bytes left");
}

void acknowledgeTests() {
  {
    FakeLink a;
    a.startDT();
    iec_obj c = singleCommand(5);
    a.sendCommand(&c);
    a.ticks(14);
    check(a.disconnects == 0, "no t1 timeout before 15 s");
    a.ticks(1);
    check(a.disconnects == 1, "unacknowledged I-frame closes the link after t1");
  }
  {
    FakeLink a;
    a.startDT();
    iec_obj c = singleCommand(5);
    a.sendCommand(&c);
    a.ticks(10);
    a.feed(sframe(2)); // acknowledges it
    a.ticks(10);       // would have timed out at 15
    check(a.disconnects == 0, "acknowledged I-frame keeps the link");
  }
  {
    FakeLink a;
    a.startDT();
    iec_obj c = singleCommand(5);
    a.sendCommand(&c);
    a.sendCommand(&c);
    a.feed(sframe(2)); // first acknowledged: t1 restarts for the second
    for (int i = 0; i < 12; i++) {
      a.onTimerSecond();
      a.feed(sframe(2)); // same N(R) again: no progress
    }
    a.ticks(3);
    check(a.disconnects == 1, "repeated ack without progress doesn't stop t1");
  }
  {
    FakeLink a;
    a.startDT();
    a.feed(sframe(4)); // nothing was sent
    check(a.disconnects == 1, "N(R) acknowledging unsent frames closes the link");
  }
}

void logTests() {
  TLogMsg log;
  log.activateLog();
  log.setMaxMsg(3);
  for (int i = 0; i < 10; i++)
    log.pushMsg("x");
  for (int i = 0; i < 3; i++)
    log.pullMsg();
  log.pushMsg("after");
  const std::string notice = log.pullMsg();
  check(notice.find("7 MESSAGES DROPPED") != std::string::npos && log.pullMsg() == "after",
        "log overflow is reported");
}

// QIec104 with access to its protected data hand-off
struct TestWorker : QIec104 {
  using QIec104::canAcceptData;
  using QIec104::dataIndicationBatch;
};

void workerTests() {
  TestWorker w;
  int notifications = 0;
  QObject::connect(&w, &QIec104::signal_dataAvailable, [&] { notifications++; });

  QVector<iec_obj> batch(10);
  w.dataIndicationBatch(batch);
  QVector<iec_obj> batch2(5);
  w.dataIndicationBatch(batch2);
  check(notifications == 1, "one notification until the data is taken");
  const auto taken = w.takePendingData();
  check(taken.size() == 2 && taken[0].size() == 10 && taken[1].size() == 5,
        "batches taken in order");

  QVector<iec_obj> flood(QIec104::kMaxPendingObjects);
  w.dataIndicationBatch(flood);
  check(!w.canAcceptData(), "reading pauses at the backlog limit");
  w.takePendingData();
  check(w.canAcceptData(), "reading allowed again once taken");

  // a connection attempt in progress is cancelled when connecting is disabled
  char ip[] = "10.255.255.1"; // not routable: stays in ConnectingState
  w.setSecondaryIP(ip);
  w.enable_connect();
  w.tcps->connectToHost(QString::fromLatin1(ip), 2404);
  w.disable_connect();
  check(w.tcps->state() == QAbstractSocket::UnconnectedState,
        "disable_connect cancels a connection in progress");
}

} // namespace

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  commandTests();
  receiveTests();
  framingTests();
  acknowledgeTests();
  logTests();
  workerTests();
  std::printf("%d failure(s)\n", failures);
  return failures;
}
