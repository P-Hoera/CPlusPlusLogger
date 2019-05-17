#pragma once

#include <qabstractitemmodel.h>

#include "QtLogEntry.h"

class LogEntryTableModel : public QAbstractTableModel
{
public:
	LogEntryTableModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	void clear();

	const std::string& getCompiledLogEntry(const int row) const;

	const QtLogEntry& getLogEntry(const int row) const;

	void appendRows(const std::vector<std::shared_ptr<QtLogEntry>>& logEntries);

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
	void increaseContainerSize(const int newSize);

	static const int m_timeStampColumn = 0;
	static const int m_logLevelColumn = 1;
	static const int m_moduleNameColumn = 2;
	static const int m_threadIDColumn = 3;
	static const int m_messageColumn = 4;
	static const int m_totalColumns = 5;
	std::vector<std::shared_ptr<QtLogEntry>> m_rawModelData;
};