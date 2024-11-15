#ifndef TRANSLATION_PROGRESS_H
#define TRANSLATION_PROGRESS_H

#include <QString>
#include <QSettings>

class TranslationProgress {
public:
    static TranslationProgress& getInstance() {
        static TranslationProgress instance;
        return instance;
    }

    // 保存翻译进度
    void saveProgress(const QString& inputPath, const QString& outputPath, const QString& lastFile);
    
    // 检查是否有未完成的翻译任务
    bool hasUnfinishedTask(const QString& inputPath, const QString& outputPath, QString& lastFile);
    
    // 清除进度记录
    void clearProgress();

private:
    TranslationProgress() {}
    QString getProgressFilePath();
};

#endif // TRANSLATION_PROGRESS_H 