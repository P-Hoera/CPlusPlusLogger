#pragma once

#include <qabstractitemmodel.h>

#include "QtLogEntry.h"

class LogEntryTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	LogEntryTableModel(QObject* parent = nullptr);

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	const std::string& getCompiledLogEntry(const int row) const;

	const QtLogEntry& getLogEntry(const int row) const;

	void appendRows(const std::vector<std::shared_ptr<QtLogEntry>>& logEntries);

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	void setFilter(const QHash<QString, bool>& logLevelFilter, const QHash<QString, bool>& moduleNameFilter, const QHash<QString, bool>& threadIDFilter);

	void clearFilter();

	int rowCountUnfiltered() const;

	const std::string& getCompiledLogEntryUnfiltered(const int row) const;

	const QtLogEntry& getLogEntryUnfiltered(const int row) const;

Q_SIGNALS:
	void rowsAppendedUnfiltered(const int first, const int last);

private:
	void increaseContainerSize(const int newSize);

	void partiallyFilter(const int first, const int last);

	static const int m_lineNumberColumn = 0;
	static const int m_timeStampColumn = 1;
	static const int m_logLevelColumn = 2;
	static const int m_moduleNameColumn = 3;
	static const int m_threadIDColumn = 4;
	static const int m_messageColumn = 5;
	static const int m_totalColumns = 6;
	std::vector<std::shared_ptr<QtLogEntry>> m_rawModelData;
	std::vector<int> m_filteredModelData;
	bool m_noFilter = true;
	QHash<QString, bool> m_logLevelFilter;
	QHash<QString, bool> m_moduleNameFilter;
	QHash<QString, bool> m_threadIDFilter;
};