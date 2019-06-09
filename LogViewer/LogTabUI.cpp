#include "LogTabUI.h"

#include "LogLevelQColor.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qthread.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qhash.h>
#include <fstream>

LogTabUI::LogTabUI(QWidget* parent, const SourceType sourceType, const std::string& sourceName, const int sourceID)
	: QWidget(parent), m_sourceType(sourceType), m_sourceName(sourceName), m_sourceID(sourceID)
{
	m_ui.setupUi(this);
	setUpTableViewModel();
	setUpSourceTypeAndStatusLabel();
	setSaveToFileElementsVisible(false);
	setUpFilterComboBoxes();

	connect(m_ui.clearFilterPushButton, &QPushButton::clicked, this, &LogTabUI::clearFilter);
	connect(m_ui.loadFilterButton, &QPushButton::clicked, this, &LogTabUI::loadFilter);
	connect(m_ui.saveFilterButton, &QPushButton::clicked, this, &LogTabUI::saveFilter);
	connect(m_logEntryTableModel, &LogEntryTableModel::rowsInserted, this, &LogTabUI::updateFilterComboBoxesAndFilterEntriesTable);
	connect(m_ui.saveAllEntriesToFile, &QPushButton::clicked, this, &LogTabUI::saveAllEntriesToFile);
	connect(m_ui.saveVisibleEntriesToFile, &QPushButton::clicked, this, &LogTabUI::saveVisibleEntriesToFile);
}

void LogTabUI::addLogEntries(const std::vector<std::shared_ptr<QtLogEntry>>& logEntries)
{
	m_logEntryTableModel->appendRows(logEntries);
}

void LogTabUI::closeSource()
{
	QString sourceStatus;

	switch (m_sourceType)
	{
	case SourceType::File:
	{
		sourceStatus = "loading complete";
		break;
	}
	case SourceType::RPCConnection:
	{
		sourceStatus = "disconnected";
		break;
	}
	}

	m_ui.labelSourceStatusText->setText(sourceStatus);
}

int LogTabUI::getSourceID()
{
	return m_sourceID;
}

void LogTabUI::clearFilter()
{
	if (m_logLevelFilerActive == false && m_moduleNameFilerActive == false && m_threadIDFilterActive == false)
	{
		return;
	}

	for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
	{
		m_moduleNameComboBoxModel->item(i)->setData(Qt::Checked, Qt::CheckStateRole);
	}

	for (int i = 1; i < m_threadIDComboBoxModel->rowCount(); ++i)
	{
		m_threadIDComboBoxModel->item(i)->setData(Qt::Checked, Qt::CheckStateRole);
	}

	for (int i = 1; i < m_logLevelComboBoxModel->rowCount(); ++i)
	{
		m_logLevelComboBoxModel->item(i)->setData(Qt::Checked, Qt::CheckStateRole);
	}

	for (int i = 0; i < m_logEntryTableModel->rowCount(); ++i)
	{
		m_ui.entriesTableView->setRowHidden(i, false);
	}

	m_logLevelFilerActive = false;
	m_moduleNameFilerActive = false;
	m_threadIDFilterActive = false;
}

void LogTabUI::loadFilter()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Please choose filter File to load.", QString(), "JSON file (*.json)");

	QFile file(fileName);

	file.open(QIODevice::ReadOnly);

	QByteArray data = file.readAll();

	QJsonDocument jsonDocument = QJsonDocument::fromJson(data);

	QJsonObject rootObject = jsonDocument.object();

	int fileVersion;

	if (rootObject.contains("Version"))
	{
		fileVersion = rootObject["Version"].toInt(-1);
	}

	QJsonObject logLevelObject;

	if (rootObject.contains("LogLevel") && rootObject["LogLevel"].isObject())
	{
		logLevelObject = rootObject["LogLevel"].toObject();
	}

	std::vector<std::pair<QString, bool>> logLevelData;

	for (const auto& entry : logLevelObject.keys())
	{
		if (logLevelObject[entry].isBool())
		{
			bool visibility = logLevelObject[entry].toBool();
			logLevelData.push_back({ entry, visibility });
		}
	}

	QJsonObject moduleNameObject;

	if (rootObject.contains("ModuleName") && rootObject["ModuleName"].isObject())
	{
		moduleNameObject = rootObject["ModuleName"].toObject();
	}

	std::vector<std::pair<QString, bool>> moduleNameData;

	for (const auto& entry : moduleNameObject.keys())
	{
		if (moduleNameObject[entry].isBool())
		{
			bool visibility = moduleNameObject[entry].toBool();
			moduleNameData.push_back({ entry, visibility });
		}
	}

	for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
	{
		m_moduleNameComboBoxModel->item(i)->setData(Qt::Unchecked, Qt::CheckStateRole);
	}

	for (int i = 1; i < m_logLevelComboBoxModel->rowCount(); ++i)
	{
		m_logLevelComboBoxModel->item(i)->setData(Qt::Unchecked, Qt::CheckStateRole);
	}

	for (const auto& entry : logLevelData)
	{
		for (int i = 1; i < m_logLevelComboBoxModel->rowCount(); ++i)
		{
			if (entry.first == m_logLevelComboBoxModel->item(i)->text())
			{
				if (entry.second)
				{
					m_logLevelComboBoxModel->item(i)->setData(Qt::Checked, Qt::CheckStateRole);
				}
			}
		}
	}

	for (const auto& entry : moduleNameData)
	{
		bool found = false;

		for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
		{
			if (entry.first == m_moduleNameComboBoxModel->item(i)->text())
			{
				if (entry.second)
				{
					m_moduleNameComboBoxModel->item(i)->setData(Qt::Checked, Qt::CheckStateRole);
				}

				found = true;
				break;
			}
		}

		if (found)
		{
			continue;
		}

		QStandardItem* item = new QStandardItem(entry.first);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		if (entry.second)
		{
			item->setData(Qt::Checked, Qt::CheckStateRole);
		}
		else
		{
			item->setData(Qt::Unchecked, Qt::CheckStateRole);
		}
		m_moduleNameComboBoxModel->appendRow(item);

		int newModuleNameWidth = m_ui.moduleNameComboBox->view()->fontMetrics().width(entry.first);
		if (newModuleNameWidth > m_moduleNameWidth)
		{
			m_moduleNameWidth = newModuleNameWidth;
		}
	}

	m_ui.moduleNameComboBox->view()->setMinimumWidth(m_moduleNameWidth + 45);

	filterEntriesTable();
}

void LogTabUI::saveFilter()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Please choose File to save to.", QString(), "JSON file (*.json)");

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);

	QJsonObject rootObject;

	rootObject["Version"] = 1;

	QJsonObject logLevelObject;

	for (int i = 1; i < m_logLevelComboBoxModel->rowCount(); ++i)
	{
		bool isChecked;
		if (m_logLevelComboBoxModel->item(i)->data(Qt::CheckStateRole) == Qt::Checked)
		{
			isChecked = true;
		}
		else
		{
			isChecked = false;
		}

		logLevelObject[m_logLevelComboBoxModel->item(i)->text()] = isChecked;
	}

	QJsonObject moduleNameObject;

	for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
	{
		bool isChecked;
		if (m_moduleNameComboBoxModel->item(i)->data(Qt::CheckStateRole) == Qt::Checked)
		{
			isChecked = true;
		}
		else
		{
			isChecked = false;
		}

		moduleNameObject[m_moduleNameComboBoxModel->item(i)->text()] = isChecked;
	}

	rootObject["LogLevel"] = logLevelObject;
	rootObject["ModuleName"] = moduleNameObject;

	QJsonDocument jsonDocument(rootObject);

	file.write(jsonDocument.toJson());
}

void LogTabUI::updateFilterComboBoxesAndFilterEntriesTable(const QModelIndex& parent, int first, int last)
{
	updateFilterComboBoxes(first, last);
	partiallyFilterEntriesTable(first, last);
}

void LogTabUI::filterEntriesTable()
{
	partiallyFilterEntriesTable(0, m_logEntryTableModel->rowCount() - 1);
}

void LogTabUI::saveAllEntriesToFile()
{
	saveToFile(SaveMode::SaveAllEntries);
}

void LogTabUI::saveVisibleEntriesToFile()
{
	saveToFile(SaveMode::SaveVisibleEntries);
}

void LogTabUI::saveToFile(const SaveMode saveMode)
{
	QString fileName = QFileDialog::getSaveFileName(this, "Please choose File to save to.");

	if (fileName == nullptr)
	{
		return;
	}

	std::ofstream file(QDir::toNativeSeparators(fileName).toStdString());

	if (!file.is_open())
	{
		QMessageBox::critical(this, "Failed to open File.", "Could not open the file.");
		return;
	}

	int entryRows = m_logEntryTableModel->rowCount();

	m_ui.progressBarSavingToFile->setRange(0, entryRows - 1);
	m_ui.progressBarSavingToFile->setValue(0);

	setSaveToFileElementsVisible(true);

	switch (saveMode)
	{
	case SaveMode::SaveAllEntries:
	{
		for (int i = 0; i < entryRows; ++i)
		{
			file << m_logEntryTableModel->getCompiledLogEntry(i);
			m_ui.progressBarSavingToFile->setValue(i);
		}
		break;
	}
	case SaveMode::SaveVisibleEntries:
	{
		for (int i = 0; i < entryRows; ++i)
		{
			if (!m_ui.entriesTableView->isRowHidden(i))
			{
				file << m_logEntryTableModel->getCompiledLogEntry(i);
			}
			m_ui.progressBarSavingToFile->setValue(i);
		}
		break;
	}
	}

	setSaveToFileElementsVisible(false);
}

void LogTabUI::setUpTableViewModel()
{
	m_logEntryTableModel = new LogEntryTableModel(this);
	m_ui.entriesTableView->setModel(m_logEntryTableModel);
	m_ui.entriesTableView->verticalHeader()->hide();
	m_ui.entriesTableView->resizeColumnsToContents();
	int columnWidth = m_ui.entriesTableView->columnWidth(1);
	columnWidth = 2.2f * columnWidth;
	m_ui.entriesTableView->setColumnWidth(1, columnWidth);
	m_ui.entriesTableView->setColumnWidth(0, 1.5f * m_ui.entriesTableView->columnWidth(0));
	m_ui.entriesTableView->horizontalHeader()->setStretchLastSection(true);
	m_ui.entriesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void LogTabUI::setUpSourceTypeAndStatusLabel()
{
	QString sourceType;
	QString sourceStatus;

	switch (m_sourceType)
	{
	case SourceType::File:
	{
		sourceType = "File : ";
		sourceType += m_sourceName.c_str();
		sourceStatus = "loading";
		break;
	}
	case SourceType::RPCConnection:
	{
		sourceType = "RPC connection : ";
		sourceType += m_sourceName.c_str();
		sourceStatus = "connected";
		break;
	}
	}

	m_ui.labelSourceTypeText->setText(sourceType);
	m_ui.labelSourceStatusText->setText(sourceStatus);
}

void LogTabUI::setSaveToFileElementsVisible(const bool visible)
{
	m_ui.labelSavingToFile->setVisible(visible);
	m_ui.progressBarSavingToFile->setVisible(visible);
}

void LogTabUI::setUpFilterComboBoxes()
{
	m_logLevelComboBoxModel = new QStandardItemModel(this);

	QStandardItem* item = new QStandardItem(QString("Log Level"));
	m_logLevelComboBoxModel->setItem(0, 0, item);

	std::vector<Log::LogLevel> logLevelVector({ Log::LogLevel::Fatal, Log::LogLevel::Error, Log::LogLevel::Warning, Log::LogLevel::Info, Log::LogLevel::Debug, Log::LogLevel::Trace });

	for (const auto& entry : logLevelVector)
	{
		item = new QStandardItem(QString::fromStdString(Log::logLevelToString(entry)));
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		item->setData(Qt::Checked, Qt::CheckStateRole);
		item->setData(getLogLevelQColor(entry), Qt::BackgroundRole);
		//item->setData(QBrush(QColor(255, 255, 255)), Qt::ForegroundRole);
		m_logLevelComboBoxModel->appendRow(item);
	}

	m_ui.logLevelComboBox->setModel(m_logLevelComboBoxModel);
	m_ui.logLevelComboBox->view()->setTextElideMode(Qt::ElideNone);
	connect(m_logLevelComboBoxModel, &QStandardItemModel::dataChanged, this, &LogTabUI::filterEntriesTable);

	m_moduleNameComboBoxModel = new QStandardItemModel(this);

	item = new QStandardItem(QString("Module Name"));
	m_moduleNameComboBoxModel->setItem(0, 0, item);

	m_ui.moduleNameComboBox->setModel(m_moduleNameComboBoxModel);
	m_ui.moduleNameComboBox->view()->setTextElideMode(Qt::ElideNone);
	connect(m_moduleNameComboBoxModel, &QStandardItemModel::dataChanged, this, &LogTabUI::filterEntriesTable);

	m_threadIDComboBoxModel = new QStandardItemModel(this);

	item = new QStandardItem(QString("Thread ID"));
	m_threadIDComboBoxModel->setItem(0, 0, item);

	m_ui.threadIDComboBox->setModel(m_threadIDComboBoxModel);
	m_ui.threadIDComboBox->view()->setTextElideMode(Qt::ElideNone);
	connect(m_threadIDComboBoxModel, &QStandardItemModel::dataChanged, this, &LogTabUI::filterEntriesTable);
}

void LogTabUI::updateFilterComboBoxes(const int first, const int last)
{
	QHash<QString, bool> moduleNames;
	QHash<QString, bool> threadIDs;

	for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
	{
		moduleNames.insert(m_moduleNameComboBoxModel->item(i)->text(), true);
	}

	for (int i = 1; i < m_threadIDComboBoxModel->rowCount(); ++i)
	{
		threadIDs.insert(m_threadIDComboBoxModel->item(i)->text(), true);
	}

	for (int i = first; i <= last; ++i)
	{
		const auto& entry = m_logEntryTableModel->getLogEntry(i);
		const QString& moduleName = entry.getModuleNameQString();
		const QString& threadID = entry.getThreadIDQString();

		if (!moduleNames.contains(moduleName))
		{
			moduleNames.insert(moduleName, true);

			QStandardItem* item = new QStandardItem(moduleName);
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			if (m_moduleNameFilerActive)
			{
				item->setData(Qt::Unchecked, Qt::CheckStateRole);
			}
			else
			{
				item->setData(Qt::Checked, Qt::CheckStateRole);
			}
			m_moduleNameComboBoxModel->appendRow(item);

			int newModuleNameWidth = m_ui.moduleNameComboBox->view()->fontMetrics().width(moduleName);
			if (newModuleNameWidth > m_moduleNameWidth)
			{
				m_moduleNameWidth = newModuleNameWidth;
			}
		}

		if (!threadIDs.contains(threadID))
		{
			threadIDs.insert(threadID, true);

			QStandardItem* item = new QStandardItem(threadID);
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			if (m_threadIDFilterActive)
			{
				item->setData(Qt::Unchecked, Qt::CheckStateRole);
			}
			else
			{
				item->setData(Qt::Checked, Qt::CheckStateRole);
			}
			m_threadIDComboBoxModel->appendRow(item);

			int newThreadIDWidth = m_ui.threadIDComboBox->view()->fontMetrics().width(threadID);
			if (newThreadIDWidth > m_threadIDWidth)
			{
				m_threadIDWidth = newThreadIDWidth;
			}
		}
	}

	m_ui.moduleNameComboBox->view()->setMinimumWidth(m_moduleNameWidth + 45);
	m_ui.threadIDComboBox->view()->setMinimumWidth(m_threadIDWidth + 45);
}

void LogTabUI::partiallyFilterEntriesTable(const int first, const int last)
{
	QHash<QString, bool> moduleNameFilter;
	QHash<QString, bool> threadIDFilter;
	QHash<QString, bool> logLevelFilter;

	m_logLevelFilerActive = false;
	m_moduleNameFilerActive = false;
	m_threadIDFilterActive = false;

	for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
	{
		if (m_moduleNameComboBoxModel->item(i)->checkState() == Qt::Checked)
		{
			moduleNameFilter.insert(m_moduleNameComboBoxModel->item(i)->text(), true);
		}
		else
		{
			m_moduleNameFilerActive = true;
		}
	}

	for (int i = 1; i < m_threadIDComboBoxModel->rowCount(); ++i)
	{
		if (m_threadIDComboBoxModel->item(i)->checkState() == Qt::Checked)
		{
			threadIDFilter.insert(m_threadIDComboBoxModel->item(i)->text(), true);
		}
		else
		{
			m_threadIDFilterActive = true;
		}
	}

	for (int i = 1; i < m_logLevelComboBoxModel->rowCount(); ++i)
	{
		if (m_logLevelComboBoxModel->item(i)->checkState() == Qt::Checked)
		{
			logLevelFilter.insert(m_logLevelComboBoxModel->item(i)->text(), true);
		}
		else
		{
			m_logLevelFilerActive = true;
		}
	}

	for (int i = first; i <= last; ++i)
	{
		const auto& entry = m_logEntryTableModel->getLogEntry(i);

		if (logLevelFilter.contains(entry.getLogLevelQString()) && moduleNameFilter.contains(entry.getModuleNameQString()) && threadIDFilter.contains(entry.getThreadIDQString()))
		{
			m_ui.entriesTableView->setRowHidden(i, false);
		}
		else
		{
			m_ui.entriesTableView->setRowHidden(i, true);
		}
	}
}