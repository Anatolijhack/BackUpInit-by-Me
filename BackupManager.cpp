#include "BackupManagger.h"
#include "TimeUntils.h"
#include "FileHash.h"
#include <filesystem>


namespace fs = std::filesystem;

BackupManager::BackupManager(const Config& cfg)
    : config(cfg), pool(cfg.threads)
{
    finalDest = config.destiantion /
        (config.sources.filename().string() + "_" + getTimerstamp());
}

void BackupManager::run() {
    logger.log(LogLevel::INFO, "Backup started");

    fs::create_directories(finalDest);
    pool.waitFinished();
    scanFiles();
    if (config.sync)
    {
        removeExtraFiles();
    }

    logger.log(LogLevel::INFO, "Backup finished");
}
void BackupManager::scanFiles() {
    for (auto& entry : fs::recursive_directory_iterator(config.sources)) {
        if (fs::is_regular_file(entry)) {
            pool.enqueue([this, path = entry.path()]() {
                processFile(path);
                });
        }
    }
}

void BackupManager::processFile(const fs::path& file) {
    fs::path relative = fs::relative(file, config.sources);
    fs::path target = finalDest / relative;

    fs::create_directories(target.parent_path());

    if (!config.incremental || isModified(file, target))
    {
        fs::copy_file(file, target, fs::copy_options::overwrite_existing);
        logger.log(LogLevel::INFO, "Copied: " + file.string());
    }
}
void BackupManager::removeExtraFiles()
{
    for (auto& entry : fs::recursive_directory_iterator(finalDest))
    {
        if (!fs::is_regular_file(entry))
        {
            continue;
        }
        fs::path relative = fs::relative(entry.path(), finalDest);
        fs::path srcFile = config.sources / relative;

        if (!fs::exists(srcFile))
        {
            try
            {
                if (config.dryRun)
                {
                    
                    logger.log(LogLevel::INFO, "[DRY RUN] Would delete:" + entry.path().string());
                }
                else
                {
                    
                    fs::remove(entry.path());
                    logger.log(LogLevel::WARM, "Deleted:" + entry.path().string());
                }
            }  
            catch (...)
            {
                
                logger.log(LogLevel::ERROR, "Failed to open file:" + entry.path().string());
            }
        }
    }
}