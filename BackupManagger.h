#pragma once
#include "Config.h"
#include "looger.h"
#include "ThreadPool.h"

class BackupManager
{
public:
	BackupManager(const Config& cfg);
	void run();
	
private:
	void scanFiles();
	void processFile(const std::filesystem::path& file);
	void removeExtraFiles();

	Config config;
	ThreadPool pool;
	Logger logger;

	std::filesystem::path finalDest;
};