#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindowUI.h"

#include "SourceType.h"
#include "UIConnectionInterface.h"
#include "SourceProcessor.h"
#include "LogFileLoader.h"

class MainWindowUI : public QMainWindow, public UIConnectionInterface
{
	struct SourceData
	{
		int sourceID;
		std::shared_ptr<std::string> sourceName;
		SourceType sourceType;
	};

	struct EntriesToAdd
	{
		std::vector<std::shared_ptr<QtLogEntry>> entriesVector;
		bool addEntriesSignalSend = false;
	};

	Q_OBJECT

public:
	MainWindowUI(QWidget *parent = Q_NULLPTR);

	~MainWindowUI();

	virtual void openSource(const int sourceID, const std::shared_ptr<std::string>& sourceName, const SourceType sourceType) override;

	virtual void closeSource(const int sourceID) override;

	virtual void addLogEntry(const int sourceID, const std::shared_ptr<QtLogEntry>& logEntry) override;

Q_SIGNALS:
	void openSourceSignal();

	void addLogEntriesSignal(const int sourceID);

	void closeSourceSignal(const int sourceID);

private Q_SLOT:
	void openLogFile();

	void about();

	void openSourceSlot();

	void addLogEntriesSlot(const int sourceID);

	void closeSourceSlot(const int sourceID);

	void closeTab(int index);

protected:
	//virtual void closeEvent(QCloseEvent* event) override;

private:
	void setUpTabWidget();

	void setUpMenuBar();

	Ui::MainWindowUI m_ui;
	QTabWidget* m_tabWidget;

	std::mutex m_sourcesToCreateMutex;
	std::vector<SourceData> m_sourcesToCreateVector;
	bool m_openSourceSignalSend = false;

	std::mutex m_entriesToAddMutex;
	std::map<int, EntriesToAdd> m_entriesToAddMap;

	std::shared_ptr<SourceProcessor> m_sourceProcessor;
	std::shared_ptr<LogFileLoader> m_logFileLoader;
};
