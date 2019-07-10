#pragma once

#include <qstring.h>
#include <vector>
#include <qjsonobject.h>

class FilterFile
{
public:
	FilterFile();

	~FilterFile();

	void load();

	void load(const QString& fileName);

	void save();

	void save(const QString& fileName);

	void addLogLevelFilter(const QString& entryName, const bool isActive);

	void addModuleNameFilter(const QString& entryName, const bool isActive);

	const std::vector<std::pair<QString, bool>>& getLogLevelFilterData();

	const std::vector<std::pair<QString, bool>>& getModuleNameFilterData();

	bool containsFilterData();

	bool containsLogLevelData();

	bool containsModuleNameData();

	void clear();

private:
	void loadFileVersion1(QJsonObject& rootObject);

	void addFilter(std::vector<std::pair<QString, bool>>& filterContainer, const QString& entryName, const bool isActive);

	std::vector<std::pair<QString, bool>> m_logLevelFilterData;
	std::vector<std::pair<QString, bool>> m_moduleNameFilterData;
};
