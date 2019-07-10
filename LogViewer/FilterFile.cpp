#include "FilterFile.h"

#include <qfiledialog.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

FilterFile::FilterFile()
{}

FilterFile::~FilterFile()
{}

void FilterFile::load()
{
	QString fileName = QFileDialog::getOpenFileName(nullptr, "Please choose filter File to load.", QString(), "JSON file (*.json)");

	if(fileName.isEmpty())
	{
		return;
	}

	load(fileName);
}

void FilterFile::load(const QString& fileName)
{
	m_logLevelFilterData.clear();
	m_moduleNameFilterData.clear();

	QFile file(fileName);

	file.open(QIODevice::ReadOnly);

	if (!file.isOpen())
	{
		throw std::invalid_argument("Could not open file for load operation.");
	}

	QByteArray data = file.readAll();

	file.close();

	QJsonDocument jsonDocument = QJsonDocument::fromJson(data);

	QJsonObject rootObject = jsonDocument.object();

	int fileVersion;

	if (rootObject.contains("Version"))
	{
		fileVersion = rootObject["Version"].toInt(-1);
	}
	else
	{
		throw std::invalid_argument("Invalid Filter File Format.");
	}

	switch (fileVersion)
	{
		case 1:
		{
			loadFileVersion1(rootObject);
			break;
		}
		default:
		{
			throw std::invalid_argument("Unsupported Filter File Version.");
		}
	}
}

void FilterFile::save()
{
	QString fileName = QFileDialog::getSaveFileName(nullptr, "Please choose File to save to.", QString(), "JSON file (*.json)");

	if (fileName.isEmpty())
	{
		return;
	}

	save(fileName);
}

void FilterFile::save(const QString& fileName)
{
	QFile file(fileName);

	file.open(QIODevice::WriteOnly);

	if(!file.isOpen())
	{
		throw std::invalid_argument("Could not open file for save operation.");
	}

	QJsonObject rootObject;

	rootObject["Version"] = 1;

	QJsonObject logLevelObject;

	for (const auto& entry : m_logLevelFilterData)
	{
		logLevelObject[entry.first] = entry.second;
	}

	QJsonObject moduleNameObject;

	for (const auto& entry : m_moduleNameFilterData)
	{
		moduleNameObject[entry.first] = entry.second;
	}

	rootObject["LogLevel"] = logLevelObject;
	rootObject["ModuleName"] = moduleNameObject;

	QJsonDocument jsonDocument(rootObject);

	file.write(jsonDocument.toJson());
}

void FilterFile::addLogLevelFilter(const QString& entryName, const bool isActive)
{
	addFilter(m_logLevelFilterData, entryName, isActive);
}

void FilterFile::addModuleNameFilter(const QString& entryName, const bool isActive)
{
	addFilter(m_moduleNameFilterData, entryName, isActive);
}

const std::vector<std::pair<QString, bool>>& FilterFile::getLogLevelFilterData()
{
	return m_logLevelFilterData;
}

const std::vector<std::pair<QString, bool>>& FilterFile::getModuleNameFilterData()
{
	return m_moduleNameFilterData;
}

bool FilterFile::containsFilterData()
{
	return !m_logLevelFilterData.empty() || !m_moduleNameFilterData.empty();
}

bool FilterFile::containsLogLevelData()
{
	return !m_logLevelFilterData.empty();
}

bool FilterFile::containsModuleNameData()
{
	return !m_moduleNameFilterData.empty();
}

void FilterFile::clear()
{
	m_logLevelFilterData.clear();
	m_moduleNameFilterData.clear();
}

void FilterFile::loadFileVersion1(QJsonObject& rootObject)
{
	QJsonObject logLevelObject;

	if (rootObject.contains("LogLevel") && rootObject["LogLevel"].isObject())
	{
		logLevelObject = rootObject["LogLevel"].toObject();
	}

	for (const auto& entry : logLevelObject.keys())
	{
		if (logLevelObject[entry].isBool())
		{
			bool visibility = logLevelObject[entry].toBool();
			m_logLevelFilterData.push_back({ entry, visibility });
		}
	}

	QJsonObject moduleNameObject;

	if (rootObject.contains("ModuleName") && rootObject["ModuleName"].isObject())
	{
		moduleNameObject = rootObject["ModuleName"].toObject();
	}

	for (const auto& entry : moduleNameObject.keys())
	{
		if (moduleNameObject[entry].isBool())
		{
			bool visibility = moduleNameObject[entry].toBool();
			m_moduleNameFilterData.push_back({ entry, visibility });
		}
	}
}

void FilterFile::addFilter(std::vector<std::pair<QString, bool>>& filterContainer, const QString& entryName, const bool isActive)
{
	for (auto& entry : filterContainer)
	{
		if (entry.first == entryName)
		{
			entry.second = isActive;
			return;
		}
	}

	filterContainer.push_back({ entryName, isActive });
}