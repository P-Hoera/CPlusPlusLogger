#include "LogEntryTableModel.h"

LogEntryTableModel::LogEntryTableModel(QObject* parent)
	: QAbstractTableModel(parent)
{
	increaseContainerSize(150);
}

int LogEntryTableModel::rowCount(const QModelIndex& parent) const
{
	return m_rawModelData.size();
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

	auto& entry = m_rawModelData[row];

	if (role == Qt::DisplayRole)
	{
		switch (column)
		{
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

	return QVariant();
}

void LogEntryTableModel::clear()
{
	beginResetModel();
	m_rawModelData.clear();
	endResetModel();
}

const std::string& LogEntryTableModel::getCompiledLogEntry(const int row) const
{
	return m_rawModelData[row]->getCompiledLogEntry();
}

const QtLogEntry& LogEntryTableModel::getLogEntry(const int row) const
{
	return *m_rawModelData[row];
}

void LogEntryTableModel::appendRows(const std::vector<std::shared_ptr<QtLogEntry>>& logEntries)
{
	int rows = m_rawModelData.size();
	int maxRows = m_rawModelData.capacity();

	if ((rows + logEntries.size()) > maxRows)
	{
		increaseContainerSize(rows + logEntries.size() + 150);
	}

	beginInsertRows(QModelIndex(), rows, rows + logEntries.size() - 1);

	for (const auto& entry : logEntries)
	{
		m_rawModelData.push_back(entry);
	}

	endInsertRows();
}

QVariant LogEntryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
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

void LogEntryTableModel::increaseContainerSize(const int newSize)
{
	m_rawModelData.reserve(newSize);
}