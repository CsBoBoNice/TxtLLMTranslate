#include "TranslationProgress.h"
#include <QCoreApplication>
#include <QDir>

QString TranslationProgress::getProgressFilePath() {
    return QCoreApplication::applicationDirPath() + "/translation_progress.ini";
}

void TranslationProgress::saveProgress(const QString& inputPath, const QString& outputPath, const QString& lastFile) {
    QSettings settings(getProgressFilePath(), QSettings::IniFormat);
    settings.setValue("Progress/InputPath", inputPath);
    settings.setValue("Progress/OutputPath", outputPath);
    settings.setValue("Progress/LastFile", lastFile);
}

bool TranslationProgress::hasUnfinishedTask(const QString& inputPath, const QString& outputPath, QString& lastFile) {
    QSettings settings(getProgressFilePath(), QSettings::IniFormat);
    
    QString savedInputPath = settings.value("Progress/InputPath").toString();
    QString savedOutputPath = settings.value("Progress/OutputPath").toString();
    
    if (savedInputPath == inputPath && savedOutputPath == outputPath) {
        lastFile = settings.value("Progress/LastFile").toString();
        return !lastFile.isEmpty();
    }
    
    return false;
}

void TranslationProgress::clearProgress() {
    QSettings settings(getProgressFilePath(), QSettings::IniFormat);
    settings.clear();
} 