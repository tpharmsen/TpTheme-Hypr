#include "WidgetBackend.h"

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QSysInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStorageInfo>

namespace {
QString firstLine(const QString &path)
{
    QFile file(path);
    return file.open(QIODevice::ReadOnly | QIODevice::Text)
        ? QString::fromUtf8(file.readLine()).trimmed()
        : QString();
}

QString uptimeText()
{
    const QString uptime = firstLine(QStringLiteral("/proc/uptime"));
    const qlonglong totalMinutes = qlonglong(uptime.section(' ', 0, 0).toDouble()) / 60;
    const qlonglong days = totalMinutes / 1440;
    const qlonglong hours = (totalMinutes % 1440) / 60;
    const qlonglong minutes = totalMinutes % 60;
    if (days > 0)
        return QStringLiteral("%1d %2h %3m").arg(days).arg(hours).arg(minutes);
    if (hours > 0)
        return QStringLiteral("%1h %2m").arg(hours).arg(minutes);
    return QStringLiteral("%1m").arg(minutes);
}

}

WidgetBackend::WidgetBackend(QObject *parent) : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &WidgetBackend::refreshMetrics);
    m_timer.start(1000);
    connect(&m_hyprctl, &QProcess::finished, this, [this] {
        const QJsonObject data = QJsonDocument::fromJson(
            m_hyprctl.readAllStandardOutput()).object();
        const int workspace = data.value(QStringLiteral("id")).toInt();
        if (workspace > 0 && workspace != m_activeWorkspace) {
            m_activeWorkspace = workspace;
            emit workspaceChanged();
        }
    });
    connect(&m_nvidiaSmi, &QProcess::finished, this, [this](int exitCode) {
        if (exitCode == 0) {
            const QByteArray output = m_nvidiaSmi.readAllStandardOutput().trimmed();
            bool ok = false;
            const int utilization = output.split('\n').constFirst().trimmed().toInt(&ok);
            if (ok)
                m_gpu = qBound(0, utilization, 100);
        } else {
            const QString gpuBusy = firstLine(
                QStringLiteral("/sys/class/drm/card0/device/gpu_busy_percent"));
            if (!gpuBusy.isEmpty())
                m_gpu = qBound(0, gpuBusy.toInt(), 100);
        }
        emit metricsChanged();
    });
    connect(&m_freeMemory, &QProcess::finished, this, [this](int exitCode) {
        if (exitCode != 0)
            return;
        const QStringList lines = QString::fromUtf8(
            m_freeMemory.readAllStandardOutput()).split('\n');
        for (const QString &line : lines) {
            const QString trimmed = line.trimmed();
            if (!trimmed.startsWith(QLatin1String("Mem:")))
                continue;
            const QStringList fields = trimmed.split(
                QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            if (fields.size() < 3)
                break;
            const qlonglong total = fields.at(1).toLongLong();
            const qlonglong used = fields.at(2).toLongLong();
            if (total > 0) {
                m_ram = qBound(0, int(used * 100 / total), 100);
                emit metricsChanged();
            }
            break;
        }
    });
    refreshMetrics();
    refreshSpecs();
    refreshJournal();
}

void WidgetBackend::refreshMetrics()
{
    if (m_hyprctl.state() == QProcess::NotRunning)
        m_hyprctl.start(QStringLiteral("hyprctl"), {QStringLiteral("activeworkspace"), QStringLiteral("-j")});
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
            const QString line = QString::fromUtf8(memFile.readLine()).trimmed();
            const QStringList fields = line.split(
                QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            if (fields.size() >= 2) {
                if (fields.constFirst() == QLatin1String("MemTotal:"))
                    totalMem = fields.at(1).toLongLong();
                else if (fields.constFirst() == QLatin1String("MemAvailable:"))
                    availableMem = fields.at(1).toLongLong();
            }
        }
    }
    if (totalMem > 0)
        m_ram = qBound(0, int((totalMem - availableMem) * 100 / totalMem), 100);
    if (m_freeMemory.state() == QProcess::NotRunning)
        m_freeMemory.start(QStringLiteral("free"), {QStringLiteral("-b")});

    if (m_nvidiaSmi.state() == QProcess::NotRunning)
        m_nvidiaSmi.start(
            QStringLiteral("nvidia-smi"),
            {QStringLiteral("--query-gpu=utilization.gpu"),
             QStringLiteral("--format=csv,noheader,nounits")});
    const QString vramUsed = firstLine(
        QStringLiteral("/sys/class/drm/card0/device/mem_info_vram_used"));
    const QString vramTotal = firstLine(
        QStringLiteral("/sys/class/drm/card0/device/mem_info_vram_total"));
    if (!vramUsed.isEmpty() && !vramTotal.isEmpty() && vramTotal.toLongLong() > 0)
        m_vram = qBound(0, int(vramUsed.toLongLong() * 100 / vramTotal.toLongLong()), 100);

    QString downloadInterface;
    QFile routeFile(QStringLiteral("/proc/net/route"));
    if (routeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        routeFile.readLine();
        while (!routeFile.atEnd()) {
            const QString line = QString::fromUtf8(routeFile.readLine()).trimmed();
            const QStringList fields = line.split(
                QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            if (fields.size() >= 4
                && fields.at(1) == QLatin1String("00000000")) {
                bool flagsOk = false;
                const int flags = fields.at(3).toInt(&flagsOk, 16);
                if (flagsOk && (flags & 1) != 0) {
                    downloadInterface = fields.constFirst();
                    break;
                }
            }
        }
    }
    if (downloadInterface != m_downloadInterface) {
        m_downloadInterface = downloadInterface;
        m_previousReceivedBytes = 0;
        m_smoothedDownloadKbps = 0.0;
    }

    quint64 receivedBytes = 0;
    if (!downloadInterface.isEmpty()) {
        QFile netFile(QStringLiteral("/proc/net/dev"));
        if (netFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            while (!netFile.atEnd()) {
                const QString line = QString::fromUtf8(netFile.readLine()).trimmed();
                if (!line.contains(':'))
                    continue;
                const QString interfaceName = line.section(':', 0, 0).trimmed();
                if (interfaceName != downloadInterface)
                    continue;
                const QStringList fields = line.section(':', 1).split(
                    QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
                if (!fields.isEmpty())
                    receivedBytes = fields.constFirst().toULongLong();
                break;
            }
        }
    }
    if (m_previousReceivedBytes > 0 && receivedBytes >= m_previousReceivedBytes) {
        const quint64 bytesPerSecond = receivedBytes - m_previousReceivedBytes;
        const quint64 kilobitsPerSecond = bytesPerSecond * 8 / 1000;
        const int instantKbps =
            int(qMin<quint64>(kilobitsPerSecond, 2147483647ULL));
        m_smoothedDownloadKbps = m_smoothedDownloadKbps > 0.0
            ? (m_smoothedDownloadKbps * 0.7) + (instantKbps * 0.3)
            : instantKbps;
        m_downloadKbps = int(m_smoothedDownloadKbps);
        m_downloadText = m_downloadKbps >= 1000
            ? QStringLiteral("%1 Mbps").arg(m_downloadKbps / 1000.0, 0, 'f', 1)
            : QStringLiteral("%1 kbps").arg(m_downloadKbps);
    } else {
        m_downloadKbps = 0;
        m_downloadText = QStringLiteral("0 kbps");
        m_smoothedDownloadKbps = 0.0;
    }
    m_previousReceivedBytes = receivedBytes;

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

void WidgetBackend::switchWorkspace(int workspace)
{
    if (workspace < 1 || workspace > 10)
        return;
    QProcess::startDetached(QStringLiteral("hyprctl"),
                            {QStringLiteral("dispatch"), QStringLiteral("workspace"),
                             QString::number(workspace)});
}

void WidgetBackend::refreshSpecs()
{
    QProcess fastfetch;
    fastfetch.start(QStringLiteral("fastfetch"),
                    {QStringLiteral("--pipe"), QStringLiteral("--logo"), QStringLiteral("none")});
    if (!fastfetch.waitForStarted(1000) || !fastfetch.waitForFinished(3000)) {
        m_specs = QStringLiteral("fastfetch unavailable");
        emit specsChanged();
        return;
    }

    QStringList fields;
    for (const QString &line :
         QString::fromUtf8(fastfetch.readAllStandardOutput()).split('\n')) {
        const QString field = line.trimmed();
        if (field.isEmpty() || field.contains(QRegularExpression(QStringLiteral("^[-]+$"))))
            continue;
        fields.append(field);
    }

    m_specs = fields.join(QStringLiteral("     "));
    if (m_specs.isEmpty())
        m_specs = QStringLiteral("No fastfetch system information available");
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
