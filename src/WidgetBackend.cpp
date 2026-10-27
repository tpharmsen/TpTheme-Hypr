#include "WidgetBackend.h"

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QSysInfo>

namespace {
QString firstLine(const QString &path)
{
    QFile file(path);
    return file.open(QIODevice::ReadOnly | QIODevice::Text)
        ? QString::fromUtf8(file.readLine()).trimmed()
        : QString();
}
}

WidgetBackend::WidgetBackend(QObject *parent) : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &WidgetBackend::refreshMetrics);
    m_timer.start(1000);
    refreshMetrics();
    refreshSpecs();
    refreshJournal();
}

void WidgetBackend::refreshMetrics()
{
    const QString stat = firstLine(QStringLiteral("/proc/stat"));
    static qlonglong previousIdle = 0;
    static qlonglong previousTotal = 0;
    const QStringList cpu = stat.split(QRegularExpression(QStringLiteral("\\s+")),
                                       Qt::SkipEmptyParts);
    if (cpu.size() >= 5) {
        const qlonglong user = cpu[1].toLongLong();
        const qlonglong nice = cpu[2].toLongLong();
        const qlonglong system = cpu[3].toLongLong();
        const qlonglong idle = cpu[4].toLongLong();
        const qlonglong total = user + nice + system + idle
            + (cpu.size() > 5 ? cpu[5].toLongLong() : 0)
            + (cpu.size() > 6 ? cpu[6].toLongLong() : 0);
        if (previousTotal > 0 && total > previousTotal)
            m_cpu = qBound(0, 100 - int((idle - previousIdle) * 100 / (total - previousTotal)), 100);
        previousIdle = idle;
        previousTotal = total;
    }

    QFile memFile(QStringLiteral("/proc/meminfo"));
    qlonglong totalMem = 0;
    qlonglong availableMem = 0;
    if (memFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!memFile.atEnd()) {
            const QString line = QString::fromUtf8(memFile.readLine());
            if (line.startsWith(QStringLiteral("MemTotal:")))
                totalMem = line.section(' ', -2, -2).toLongLong();
            else if (line.startsWith(QStringLiteral("MemAvailable:")))
                availableMem = line.section(' ', -2, -2).toLongLong();
        }
    }
    if (totalMem > 0)
        m_ram = qBound(0, int((totalMem - availableMem) * 100 / totalMem), 100);

    const QString gpuBusy = firstLine(
        QStringLiteral("/sys/class/drm/card0/device/gpu_busy_percent"));
    if (!gpuBusy.isEmpty())
        m_gpu = qBound(0, gpuBusy.toInt(), 100);
    const QString vramUsed = firstLine(
        QStringLiteral("/sys/class/drm/card0/device/mem_info_vram_used"));
    const QString vramTotal = firstLine(
        QStringLiteral("/sys/class/drm/card0/device/mem_info_vram_total"));
    if (!vramUsed.isEmpty() && !vramTotal.isEmpty() && vramTotal.toLongLong() > 0)
        m_vram = qBound(0, int(vramUsed.toLongLong() * 100 / vramTotal.toLongLong()), 100);

    for (const QString &zone : QDir(QStringLiteral("/sys/class/thermal"))
             .entryList({QStringLiteral("thermal_zone*")}, QDir::Dirs | QDir::NoDotAndDotDot)) {
        const int milliCelsius = firstLine(
            QStringLiteral("/sys/class/thermal/%1/temp").arg(zone)).toInt();
        if (milliCelsius > 0) {
            m_temperature = milliCelsius / 1000;
            break;
        }
    }

    QFile uptimeFile(QStringLiteral("/proc/uptime"));
    if (uptimeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const int seconds = QString::fromUtf8(uptimeFile.readLine())
                                .section(' ', 0, 0).toDouble();
        m_uptime = QStringLiteral("%1h %2m").arg(seconds / 3600).arg((seconds % 3600) / 60);
    }

    m_batteryAvailable = false;
    for (const QString &name : QDir(QStringLiteral("/sys/class/power_supply"))
             .entryList({QStringLiteral("BAT*")}, QDir::Dirs | QDir::NoDotAndDotDot)) {
        QFile battery(QStringLiteral("/sys/class/power_supply/%1/capacity").arg(name));
        if (battery.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_battery = qBound(0, battery.readLine().trimmed().toInt(), 100);
            m_batteryAvailable = true;
            break;
        }
    }
    emit metricsChanged();
}

void WidgetBackend::refreshSpecs()
{
    QString os = QStringLiteral("Linux");
    QFile release(QStringLiteral("/etc/os-release"));
    if (release.open(QIODevice::ReadOnly | QIODevice::Text))
        while (!release.atEnd()) {
            const QString line = QString::fromUtf8(release.readLine()).trimmed();
            if (line.startsWith(QStringLiteral("PRETTY_NAME="))) {
                os = line.mid(12).remove('"');
                break;
            }
        }
    const QString cpu = firstLine(QStringLiteral("/proc/cpuinfo")).section(':', 1).trimmed();
    m_specs = QStringLiteral("OS: %1     HOST: %2     KERNEL: %3     WM: Hyprland     "
                             "THEME: TpTheme     CPU: %4     WAYLAND: native")
        .arg(os, QSysInfo::machineHostName(), QSysInfo::kernelVersion(),
             cpu.isEmpty() ? QStringLiteral("Unknown") : cpu);
    emit specsChanged();
}

void WidgetBackend::refreshJournal()
{
    if (m_process.state() != QProcess::NotRunning)
        return;
    connect(&m_process, &QProcess::finished, this, [this](int, QProcess::ExitStatus) {
        const QString output = QString::fromUtf8(m_process.readAllStandardOutput()).trimmed();
        setJournal(output.isEmpty() ? QStringLiteral("No journal output available.") : output);
    }, Qt::SingleShotConnection);
    m_process.start(QStringLiteral("journalctl"),
                    {QStringLiteral("-n"), QStringLiteral("50"),
                     QStringLiteral("--no-pager"), QStringLiteral("-o"), QStringLiteral("short")});
}

void WidgetBackend::setJournal(const QString &text)
{
    if (m_journal == text)
        return;
    m_journal = text;
    emit journalChanged();
}
