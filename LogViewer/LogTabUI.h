#pragma once

#include <QtWidgets/QWidget>
#include<qstandarditemmodel.h>
#include "ui_LogTabUI.h"

#include "SaveMode.h"
#include "SourceType.h"
#include "LogEntryTableModel.h"

#include "QtLogEntry.h"

#include <mutex>

class LogTabUI : public QWidget
{
	Q_OBJECT

public:
	LogTabUI(QWidget* parent, const SourceType sourceType, const std::string& sourceName, const int sourceID);

	void saveToFile(const std::string fileName, const SaveMode saveMode);

	void addLogEntries(const std::vector<std::shared_ptr<QtLogEntry>>& logEntries);

	void closeSource();

	int getSourceID();

private Q_SLOT:
	void clearFilter();

	void updateFilterComboBoxesAndFilterEntriesTable(const QModelIndex& parent, int first, int last);

	void filterEntriesTable();

private:
	void setUpTableViewModel();

	void setUpSourceTypeAndStatusLabel();

	void setSaveToFileElementsVisible(const bool visible);

	void setUpFilterComboBoxes();

	void updateFilterComboBoxes(const int first, const int last);

	void partiallyFilterEntriesTable(const int first, const int last);

	Ui::LogTabUI m_ui;
	SourceType m_sourceType;
	std::string m_sourceName;
	LogEntryTableModel* m_logEntryTableModel;
	QStandardItemModel* m_logLevelComboBoxModel;
	QStandardItemModel* m_threadIDComboBoxModel;
	QStandardItemModel* m_moduleNameComboBoxModel;
	int m_sourceID;
	bool m_staticDataFilterActive = false;
	bool m_dynamicDataFilterActive = false;
};