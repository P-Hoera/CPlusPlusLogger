#include "MainWindowUI.h"

#include "LogTabUI.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qthread.h>
#include <qevent.h>

MainWindowUI::MainWindowUI(QWidget *parent)
	: QMainWindow(parent)
{
	m_ui.setupUi(this);

	setUpTabWidget();
	setUpMenuBar();

	connect(this, &MainWindowUI::openSourceSignal, this, &MainWindowUI::openSourceSlot, Qt::QueuedConnection);
	connect(this, &MainWindowUI::addLogEntriesSignal, this, &MainWindowUI::addLogEntriesSlot, Qt::QueuedConnection);
	connect(this, &MainWindowUI::closeSourceSignal, this, &MainWindowUI::closeSourceSlot, Qt::QueuedConnection);

	m_sourceProcessor = std::shared_ptr<SourceProcessor>(new SourceProcessor(this));
	m_rpcServer = std::shared_ptr<LogViewerRPCServer>(new LogViewerRPCServer(m_sourceProcessor));
	m_logFileLoader = std::shared_ptr<LogFileLoader>(new LogFileLoader(m_sourceProcessor));

	m_rpcServer->enableLocalRPCConnection();
	m_rpcServer->enableNetworkRPCConnection();
}

void MainWindowUI::openSource(const int sourceID, const std::shared_ptr<std::string>& sourceName, const SourceType sourceType)
{
	std::unique_lock<std::mutex> lock(m_sourcesToCreateMutex);
	m_sourcesToCreateVector.push_back(SourceData{sourceID, sourceName, sourceType});
	if (!m_openSourceSignalSend)
	{
		emit openSourceSignal();
		m_openSourceSignalSend = true;
	}
}

void MainWindowUI::closeSource(const int sourceID)
{
	emit closeSourceSignal(sourceID);
}

void MainWindowUI::addLogEntry(const int sourceID, const std::shared_ptr<QtLogEntry>& logEntry)
{
	std::unique_lock<std::mutex> lock(m_entriesToAddMutex);
	EntriesToAdd& entriesToAdd = m_entriesToAddMap[sourceID];
	entriesToAdd.entriesVector.push_back(logEntry);
	if (!entriesToAdd.addEntriesSignalSend)
	{
		emit addLogEntriesSignal(sourceID);
		entriesToAdd.addEntriesSignalSend = true;
	}
}

void MainWindowUI::openLogFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Please choose Log File to load.");

	if (fileName != nullptr)
	{
		std::string fileNameString = QDir::toNativeSeparators(fileName).toStdString();
		try
		{
			m_logFileLoader->loadFile(fileNameString);
		}
		catch (const std::invalid_argument & exception)
		{
			QMessageBox::critical(this, "Failed to open File.", "Could not open the file.");
		}		
	}
}

void MainWindowUI::about()
{
	QMessageBox::about(this, "About", "LogViewer Version 1.0");
}

void MainWindowUI::openSourceSlot()
{
	std::unique_lock<std::mutex> lock(m_sourcesToCreateMutex);
	std::vector<SourceData> sourceData(std::move(m_sourcesToCreateVector));
	m_openSourceSignalSend = false;
	lock.unlock();

	for (const auto& entry : sourceData)
	{
		LogTabUI* logTab = new LogTabUI(m_tabWidget, entry.sourceType, *entry.sourceName, entry.sourceID);
		m_tabWidget->addTab(logTab, entry.sourceName->c_str());
	}
}

void MainWindowUI::addLogEntriesSlot(const int sourceID)
{
	std::unique_lock<std::mutex> lock(m_entriesToAddMutex);
	EntriesToAdd& entriesToAdd = m_entriesToAddMap[sourceID];
	std::vector<std::shared_ptr<QtLogEntry>> entriesToAddVector(std::move(entriesToAdd.entriesVector));
	entriesToAdd.addEntriesSignalSend = false;
	lock.unlock();

	for (int i = 0; i < m_tabWidget->count(); ++i)
	{
		LogTabUI* tab = dynamic_cast<LogTabUI*>(m_tabWidget->widget(i));

		if (tab->getSourceID() == sourceID)
		{
			tab->addLogEntries(entriesToAddVector);
			return;
		}
	}
}

void MainWindowUI::closeSourceSlot(const int sourceID)
{
	for (int i = 0; i < m_tabWidget->count(); ++i)
	{
		LogTabUI* tab = dynamic_cast<LogTabUI*>(m_tabWidget->widget(i));

		if (tab->getSourceID() == sourceID)
		{
			tab->closeSource();
			return;
		}
	}
}

void MainWindowUI::closeTab(int index)
{
	LogTabUI* tab = dynamic_cast<LogTabUI*>(m_tabWidget->widget(index));
	m_tabWidget->removeTab(index);
	delete tab;
}

/*
void MainWindowUI::closeEvent(QCloseEvent* event)
{
	m_rpcServer.reset();
	m_logFileLoader.reset();
	m_sourceProcessor.reset();

	event->accept();
}
*/

void MainWindowUI::setUpTabWidget()
{
	m_tabWidget = new QTabWidget(m_ui.centralWidget);
	m_ui.centralWidget->layout()->addWidget(m_tabWidget);
	m_tabWidget->setTabsClosable(true);
	connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindowUI::closeTab, Qt::QueuedConnection);
}

void MainWindowUI::setUpMenuBar()
{
	connect(m_ui.actionOpenLogFile, &QAction::triggered, this, &MainWindowUI::openLogFile);
	connect(m_ui.actionAbout, &QAction::triggered, this, &MainWindowUI::about);
}
