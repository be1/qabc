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
    void outputText(const QByteArray& text);
#if 0
    void errorText(const QByteArray& text);
#endif

protected slots:
    void onOutput();
#if 0
    void onStdout();
    void onStderr();
#endif
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    ProcessType type;
    QByteArray output;
};

#endif // ABCPROCESS_H
