#include "LogTabUI.h"

#include "LogLevelQColor.h"

#include <qmessagebox.h>
#include <qthread.h>
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
	connect(m_logEntryTableModel, &LogEntryTableModel::rowsInserted, this, &LogTabUI::updateFilterComboBoxesAndFilterEntriesTable);
}

void LogTabUI::saveToFile(const std::string fileName, const SaveMode saveMode)
{
	std::ofstream file(fileName);

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
	case SaveMode::SaveOnlyVisibleEntries:
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
	if (m_staticDataFilterActive == false && m_dynamicDataFilterActive == false)
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

	m_staticDataFilterActive = false;
	m_dynamicDataFilterActive = false;
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

void LogTabUI::setUpTableViewModel()
{
	m_logEntryTableModel = new LogEntryTableModel(this);
	m_ui.entriesTableView->setModel(m_logEntryTableModel);
	m_ui.entriesTableView->resizeColumnsToContents();
	int columnWidth = m_ui.entriesTableView->columnWidth(0);
	columnWidth = 2.2f * columnWidth;
	m_ui.entriesTableView->setColumnWidth(0, columnWidth);
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
	m_ui.line_2->setVisible(visible);
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
	connect(m_logLevelComboBoxModel, &QStandardItemModel::dataChanged, this, &LogTabUI::filterEntriesTable);

	m_moduleNameComboBoxModel = new QStandardItemModel(this);

	item = new QStandardItem(QString("Module Name"));
	m_moduleNameComboBoxModel->setItem(0, 0, item);

	m_ui.moduleNameComboBox->setModel(m_moduleNameComboBoxModel);
	connect(m_moduleNameComboBoxModel, &QStandardItemModel::dataChanged, this, &LogTabUI::filterEntriesTable);

	m_threadIDComboBoxModel = new QStandardItemModel(this);

	item = new QStandardItem(QString("Thread ID"));
	m_threadIDComboBoxModel->setItem(0, 0, item);

	m_ui.threadIDComboBox->setModel(m_threadIDComboBoxModel);
	connect(m_threadIDComboBoxModel, &QStandardItemModel::dataChanged, this, &LogTabUI::filterEntriesTable);
}

void LogTabUI::updateFilterComboBoxes(const int first, const int last)
{
	QStringList newModuleNames;
	QStringList newThreadIDs;

	QStringList existingModuleNames;
	QStringList existingThreadIDs;

	for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
	{
		existingModuleNames.append(m_moduleNameComboBoxModel->item(i)->text());
	}

	for (int i = 1; i < m_threadIDComboBoxModel->rowCount(); ++i)
	{
		existingThreadIDs.append(m_threadIDComboBoxModel->item(i)->text());
	}

	for (int i = first; i <= last; ++i)
	{
		const auto& entry = m_logEntryTableModel->getLogEntry(i);
		const QString& moduleName = entry.getModuleNameQString();
		const QString& threadID = entry.getThreadIDQString();

		if (!existingModuleNames.contains(moduleName) && !newModuleNames.contains(moduleName))
		{
			newModuleNames.append(moduleName);
		}

		if (!existingThreadIDs.contains(threadID) && !newThreadIDs.contains(threadID))
		{
			newThreadIDs.append(threadID);
		}
	}

	for (const auto& entry : newModuleNames)
	{
		QStandardItem* item = new QStandardItem(entry);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		if (m_staticDataFilterActive)
		{
			item->setData(Qt::Unchecked, Qt::CheckStateRole);
		}
		else
		{
			item->setData(Qt::Checked, Qt::CheckStateRole);
		}
		m_moduleNameComboBoxModel->appendRow(item);
	}

	for (const auto& entry : newThreadIDs)
	{
		QStandardItem* item = new QStandardItem(entry);
		item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		if (m_dynamicDataFilterActive)
		{
			item->setData(Qt::Unchecked, Qt::CheckStateRole);
		}
		else
		{
			item->setData(Qt::Checked, Qt::CheckStateRole);
		}
		m_threadIDComboBoxModel->appendRow(item);
	}
}

void LogTabUI::partiallyFilterEntriesTable(const int first, const int last)
{
	QStringList moduleNameFilter;
	QStringList threadIDFilter;
	QStringList logLevelFilter;

	m_staticDataFilterActive = false;
	m_dynamicDataFilterActive = false;

	for (int i = 1; i < m_moduleNameComboBoxModel->rowCount(); ++i)
	{
		if (m_moduleNameComboBoxModel->item(i)->checkState() == Qt::Checked)
		{
			moduleNameFilter.append(m_moduleNameComboBoxModel->item(i)->text());
		}
		else
		{
			m_staticDataFilterActive = true;
		}
	}

	for (int i = 1; i < m_threadIDComboBoxModel->rowCount(); ++i)
	{
		if (m_threadIDComboBoxModel->item(i)->checkState() == Qt::Checked)
		{
			threadIDFilter.append(m_threadIDComboBoxModel->item(i)->text());
		}
		else
		{
			m_dynamicDataFilterActive = true;
		}
	}

	for (int i = 1; i < m_logLevelComboBoxModel->rowCount(); ++i)
	{
		if (m_logLevelComboBoxModel->item(i)->checkState() == Qt::Checked)
		{
			logLevelFilter.append(m_logLevelComboBoxModel->item(i)->text());
		}
		else
		{
			m_staticDataFilterActive = true;
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