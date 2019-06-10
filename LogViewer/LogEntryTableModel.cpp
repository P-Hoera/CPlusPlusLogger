#include "LogEntryTableModel.h"

LogEntryTableModel::LogEntryTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	increaseContainerSize(150);
}

int LogEntryTableModel::rowCount(const QModelIndex& parent) const
{
	return m_filteredModelData.size();
}

int LogEntryTableModel::columnCount(const QModelIndex& parent) const
{
	return m_totalColumns;
}

QVariant LogEntryTableModel::data(const QModelIndex& index, int role) const
{
	int column = index.column();
	int row = index.row();
	int size = m_rawModelData.size();

	auto& entry = m_rawModelData[m_filteredModelData[row]];

	if (role == Qt::DisplayRole)
	{
		switch (column)
		{
		case m_lineNumberColumn:
			return QString::number(m_filteredModelData[row] + 1);
		case m_timeStampColumn:
			return entry->getTimeStampQString();
		case m_logLevelColumn:
			return entry->getLogLevelQString();
		case m_moduleNameColumn:
			return entry->getModuleNameQString();
		case m_threadIDColumn:
			return entry->getThreadIDQString();
		case m_messageColumn:
			return entry->getMessageQString();
		}
	}
	else if (role == Qt::BackgroundRole)
	{
		return entry->getBackgroundQBrush();
	}
	else if (role == Qt::TextAlignmentRole && column == m_lineNumberColumn)
	{
		return (Qt::AlignRight | Qt::AlignVCenter);
	}

	return QVariant();
}

const std::string& LogEntryTableModel::getCompiledLogEntry(const int row) const
{
	return m_rawModelData[m_filteredModelData[row]]->getCompiledLogEntry();
}

const QtLogEntry& LogEntryTableModel::getLogEntry(const int row) const
{
	return *m_rawModelData[m_filteredModelData[row]];
}

void LogEntryTableModel::appendRows(const std::vector<std::shared_ptr<QtLogEntry>>& logEntries)
{
	int rows = m_rawModelData.size();
	int maxRows = m_rawModelData.capacity();

	if ((rows + logEntries.size()) > maxRows)
	{
		increaseContainerSize(rows + logEntries.size() + 150);
	}

	for (const auto& entry : logEntries)
	{
		m_rawModelData.push_back(entry);
	}

	partiallyFilter(rows, rows + logEntries.size() - 1);

	emit rowsAppendedUnfiltered(rows, rows + logEntries.size() - 1);
}

QVariant LogEntryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case m_lineNumberColumn:
			return QString("Line");
		case m_timeStampColumn:
			return QString("Timestamp");
		case m_logLevelColumn:
			return QString("Log Level");
		case m_moduleNameColumn:
			return QString("Module Name");
		case m_threadIDColumn:
			return QString("Thread ID");
		case m_messageColumn:
			return QString("Message");
		}
	}

	if (role == Qt::DisplayRole && orientation == Qt::Vertical)
	{
		return QString::number(section);
	}

	return QVariant();
}

void LogEntryTableModel::setFilter(const QHash<QString, bool>& logLevelFilter, const QHash<QString, bool>& moduleNameFilter, const QHash<QString, bool>& threadIDFilter)
{
	m_logLevelFilter = logLevelFilter;
	m_moduleNameFilter = moduleNameFilter;
	m_threadIDFilter = threadIDFilter;

	if (m_logLevelFilter.empty() && m_moduleNameFilter.empty() && m_threadIDFilter.empty())
	{
		m_noFilter = true;
	}
	else
	{
		m_noFilter = false;
	}

	m_filteredModelData.clear();
	partiallyFilter(0, m_rawModelData.size() - 1);
}

void LogEntryTableModel::clearFilter()
{
	setFilter({}, {}, {});
}

int LogEntryTableModel::rowCountUnfiltered() const
{
	return m_rawModelData.size();
}

const std::string& LogEntryTableModel::getCompiledLogEntryUnfiltered(const int row) const
{
	return m_rawModelData[row]->getCompiledLogEntry();
}

const QtLogEntry& LogEntryTableModel::getLogEntryUnfiltered(const int row) const
{
	return *m_rawModelData[row];
}

void LogEntryTableModel::increaseContainerSize(const int newSize)
{
	m_rawModelData.reserve(newSize);
	m_filteredModelData.reserve(newSize);
}

void LogEntryTableModel::partiallyFilter(const int first, const int last)
{
	beginResetModel();

	if (m_noFilter)
	{
		for (int i = first; i <= last; ++i)
		{
			m_filteredModelData.push_back(i);
		}
	}
	else
	{
		for (int i = first; i <= last; ++i)
		{
			const auto& entry = *m_rawModelData[i];

			if (m_logLevelFilter.contains(entry.getLogLevelQString()) && m_moduleNameFilter.contains(entry.getModuleNameQString()) && m_threadIDFilter.contains(entry.getThreadIDQString()))
			{
				m_filteredModelData.push_back(i);
			}
		}
	}

	endResetModel();
}