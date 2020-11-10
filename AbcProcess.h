#ifndef ABCPROCESS_H
#define ABCPROCESS_H

#include <QProcess>

class AbcProcess : public QProcess
{
    Q_OBJECT
public:
    enum ProcessType {ProcessUnknown, ProcessCompiler, ProcessViewer, ProcessPlayer, ProcessSynth};
    explicit AbcProcess(ProcessType which, QObject *parent);
    ProcessType which();
    QByteArray* log();

signals:
    void finished(int exitCode, QProcess::ExitStatus exitStatus, ProcessType which);

protected slots:
    void onOutput();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    ProcessType type;
    QByteArray output;
};

#endif // ABCPROCESS_H
