#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QStringList>

// 递归遍历资源目录，收集所有文件路径
void listAllQrcResources(const QString& resourcePath, QStringList& allResources) {
    QDir resourceDir(resourcePath);
    if (!resourceDir.exists()) {
        qWarning() << "资源目录不存在：" << resourcePath;
        return;
    }

    // 遍历当前目录下的所有文件和子目录
    QFileInfoList fileInfos = resourceDir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,  // 显示文件、目录，排除 . 和 ..
        QDir::Name  // 按名称排序
    );

    for (const QFileInfo& fileInfo : fileInfos) {
        QString filePath = fileInfo.filePath();  // 资源路径（如 :/UI/testform.ui.qml）
        if (fileInfo.isFile()) {
            // 若是文件，添加到列表
            allResources.append(filePath);
        } else if (fileInfo.isDir()) {
            // 若是目录，递归遍历子目录
            listAllQrcResources(filePath, allResources);
        }
    }
}

int test(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // 存储所有资源路径
    QStringList allQrcResources;
    // 从资源根路径 :/ 开始遍历（对应 QRC 中的 PREFIX /）
    listAllQrcResources(":/", allQrcResources);

    // 打印所有资源路径
    qDebug() << "===== 所有 QRC 资源路径 =====";
    for (const QString& path : allQrcResources) {
        qDebug() << path;
    }
    qDebug() << "===========================";
    qDebug() << "资源总数：" << allQrcResources.size();

    return app.exec();
}