#pragma once

#include <QObject>
#include <QProcess>
#include <QTimer>

class WidgetBackend final : public QObject {
    Q_OBJECT
    Q_PROPERTY(int cpu READ cpu NOTIFY metricsChanged)
    Q_PROPERTY(int ram READ ram NOTIFY metricsChanged)
    Q_PROPERTY(int gpu READ gpu NOTIFY metricsChanged)
    Q_PROPERTY(int vram READ vram NOTIFY metricsChanged)
    Q_PROPERTY(int downloadKbps READ downloadKbps NOTIFY metricsChanged)
    Q_PROPERTY(QString downloadText READ downloadText NOTIFY metricsChanged)
    Q_PROPERTY(int temperature READ temperature NOTIFY metricsChanged)
    Q_PROPERTY(int battery READ battery NOTIFY metricsChanged)
    Q_PROPERTY(bool batteryAvailable READ batteryAvailable NOTIFY metricsChanged)
    Q_PROPERTY(QString uptime READ uptime NOTIFY metricsChanged)
    Q_PROPERTY(QString specs READ specs NOTIFY specsChanged)
    Q_PROPERTY(QString journal READ journal NOTIFY journalChanged)
    Q_PROPERTY(int activeWorkspace READ activeWorkspace NOTIFY workspaceChanged)
public:
    explicit WidgetBackend(QObject *parent = nullptr);

    int cpu() const { return m_cpu; }
    int ram() const { return m_ram; }
    int gpu() const { return m_gpu; }
    int vram() const { return m_vram; }
    int downloadKbps() const { return m_downloadKbps; }
    QString downloadText() const { return m_downloadText; }
    int temperature() const { return m_temperature; }
    int battery() const { return m_battery; }
    bool batteryAvailable() const { return m_batteryAvailable; }
    QString uptime() const { return m_uptime; }
    QString specs() const { return m_specs; }
    QString journal() const { return m_journal; }
    int activeWorkspace() const { return m_activeWorkspace; }

    Q_INVOKABLE void refreshJournal();
    Q_INVOKABLE void switchWorkspace(int workspace);

signals:
    void metricsChanged();
    void specsChanged();
    void journalChanged();
    void workspaceChanged();

private:
    void refreshMetrics();
    void refreshSpecs();
    void setJournal(const QString &text);

    QProcess m_process;
    QProcess m_hyprctl;
    QProcess m_nvidiaSmi;
    QProcess m_freeMemory;
    QTimer m_timer;
    int m_cpu = 0;
    int m_ram = 0;
    int m_gpu = 0;
    int m_vram = 0;
    int m_downloadKbps = 0;
    QString m_downloadText = QStringLiteral("0 kbps");
    quint64 m_previousReceivedBytes = 0;
    double m_smoothedDownloadKbps = 0.0;
    QString m_downloadInterface;
    int m_temperature = 0;
    int m_battery = 0;
    bool m_batteryAvailable = false;
    QString m_uptime;
    QString m_specs;
    QString m_journal;
    int m_activeWorkspace = 1;
};
