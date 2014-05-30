// Observer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "stdafx.h"
#pragma warning (disable:4996)
#include <vector>
#include <string>
#include <iostream>
#include <set>
#include <stdexcept>
#include <functional>
#include <map>

#include <boost/signals2.hpp>

using namespace std;

/*
 Слушатель процесса копирования
*/
class IFileCopierListener
{
public:
	virtual void OnFileCopied(
		size_t copiedFiles,
		size_t totalFiles,
		string const& lastCopiedFileName
		) = 0;
	virtual ~IFileCopierListener(){}
};

/*
 Копировщик файлов, уведомляющий своего слушателя о прогрессе выполнения
*/
class CFileCopier
{
public:
	void CopyFiles(vector<string> const& fileNames, IFileCopierListener * listener = nullptr)
	{
		size_t counter = 0;
		for (auto fileName : fileNames)
		{
			CopyFile(fileName);
			++counter;

			if (listener)
			{
				listener->OnFileCopied(counter, fileNames.size(), fileName);
			}

		}
	}
private:
	void CopyFile(string const& fileName)
	{
		// do nothing
	}
};

/*
 Реализация слушателя, выводящая прогресс копирования в stdout
*/
class CDefaultFileCopierListener : public IFileCopierListener
{
public:
	virtual void OnFileCopied(size_t copiedFiles, size_t totalFiles, string const& lastCopiedFileName) override
	{
		double percent = (double)copiedFiles / totalFiles;
		cout << lastCopiedFileName << " copied (" << percent * 100 << "%)\n";
	}
};

/*
Альтернативная реализация слушателя
*/
class CSuperFileCopierListener : public IFileCopierListener
{
public:
	virtual void OnFileCopied(size_t copiedFiles, size_t totalFiles, string const& lastCopiedFileName) override
	{
		double percent = (double)copiedFiles / totalFiles;
		cout << "[" << lastCopiedFileName << " copied (" << percent * 100 << "%)]\n";
	}
};

/*
Слушатель-мельтиплексер, уведомляющий пару слушателей об изменении прогресса
*/
class CMultiplexingFileCopierListener : public IFileCopierListener
{
public:
	virtual void OnFileCopied(size_t copiedFiles, size_t totalFiles, string const& lastCopiedFileName) override
	{
		m_listener1.OnFileCopied(copiedFiles, totalFiles, lastCopiedFileName);
		m_listener2.OnFileCopied(copiedFiles, totalFiles, lastCopiedFileName);
	}
private:
	CSuperFileCopierListener m_listener1;
	CDefaultFileCopierListener m_listener2;
};

void MultiplexingFileCopierListenerExample()
{
	CFileCopier copier;
	CMultiplexingFileCopierListener listener;
	copier.CopyFiles({ "1.txt", "database.txt" }, &listener);
}




/*
Копировщик файлов, позволяющий добавлять и удалять произвольное количество слушателей
*/
class CFileCopier2
{
public:
	void AddListener(IFileCopierListener & listener)
	{
		m_listeners.insert(&listener);
	}

	void RemoveListener(IFileCopierListener & listener)
	{
		m_listeners.erase(&listener);
	}

	void CopyFiles(vector<string> const& fileNames)
	{
		size_t counter = 0;
		for (auto fileName : fileNames)
		{
			CopyFile(fileName);
			++counter;

			// Создаем копию коллекции наблюдателей, чтобы обеспечить корректность работы при добавлении / удалении
			// наблюдателей
			auto listeners(m_listeners);
			for (auto listener : m_listeners)
			{
				listener->OnFileCopied(counter, fileNames.size(), fileName);
			}
		}
	}
private:
	void CopyFile(string const& fileName)
	{
		// do nothing
	}

	set<IFileCopierListener *> m_listeners;
};

void MultipleObserversExample()
{
	CFileCopier2 copier;
	CDefaultFileCopierListener listener1;
	copier.AddListener(listener1);
	CSuperFileCopierListener listener2;
	copier.AddListener(listener2);

	copier.CopyFiles({ "1.txt", "database.txt" });

	copier.RemoveListener(listener1);

	cout << "--------------\n";

	copier.CopyFiles({ "3.txt", "file1.txt", "tararam.jpg" });
}


/*
Копировщик файлов, наблюдатели которого - функциональные объекты
*/
class CFileCopier3
{
public:
	typedef function<void(size_t copiedFiles, size_t totalFiles, string const& lastCopiedFileName)> FileCopiedHandler;
	CFileCopier3()
		:m_listenerId(0)
	{
	}

	size_t AddListener(FileCopiedHandler const& listener)
	{
		m_listeners.insert(make_pair(m_listenerId, listener));
		// TODO: обеспечить уникальность m_listenerId при переполнении
		return m_listenerId++;
	}

	void RemoveListener(size_t listenerId)
	{
		m_listeners.erase(listenerId);
	}

	void CopyFiles(vector<string> const& fileNames)
	{
		size_t counter = 0;
		for (auto fileName : fileNames)
		{
			CopyFile(fileName);
			++counter;

			for (auto listenerInfo : m_listeners)
			{
				listenerInfo.second(counter, fileNames.size(), fileName);
			}
		}
	}
private:
	void CopyFile(string const& fileName)
	{
		// do nothing
	}

	map<size_t, FileCopiedHandler> m_listeners;
	size_t m_listenerId;
};

void FunctionalObjectObserverExample()
{
	CFileCopier3 copier;
	size_t listenerId = copier.AddListener([](size_t copiedFiles, size_t totalFiles, string const& lastCopiedFileName){
		cout << "Copied " << copiedFiles << " of " << totalFiles << "\n";
	});
	CDefaultFileCopierListener listener;

	copier.AddListener(bind(&CDefaultFileCopierListener::OnFileCopied, &listener, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	copier.CopyFiles({ "3.txt", "file1.txt", "tararam.jpg" });

	copier.RemoveListener(listenerId);

	cout << "--------------\n";

	copier.CopyFiles({ "aaaaa.txt", "bbbb.txt", "ccc.jpg" });
}

/*
Копировщик файлов, наблюдатели которого - функциональные объекты.
Для уведомления наблюдателей используется библиотека Boost.Signals2
*/
class CFileCopier4
{
public:

	typedef boost::signals2::signal<void(size_t copiedFiles, size_t totalFiles, string const& lastCopiedFileName)> OnFileCopied;
	typedef OnFileCopied::slot_type OnFileCopiedSlotType;
	typedef boost::signals2::connection Connection;

	Connection DoOnFileCopied(OnFileCopiedSlotType const& handler)
	{
		return m_onFileCopied.connect(handler);
	}

	void CopyFiles(vector<string> const& fileNames)
	{
		size_t counter = 0;
		for (auto fileName : fileNames)
		{
			CopyFile(fileName);
			++counter;
			m_onFileCopied(counter, fileNames.size(), fileName);
		}
	}
private:
	void CopyFile(string const& fileName)
	{
		// do nothing
	}
	OnFileCopied m_onFileCopied;
};

void ObservingWithBoostSignals()
{
	CFileCopier4 copier;
	CDefaultFileCopierListener listener;
	copier.DoOnFileCopied(bind(&CDefaultFileCopierListener::OnFileCopied, &listener, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	{
		// Scoped connection при своем разрушении автоматически отсоединится от сигнала
		boost::signals2::scoped_connection conn = copier.DoOnFileCopied([](size_t copiedFiles, size_t totalFiles, string const& lastCopiedFileName){
			cout << copiedFiles << ". File " << lastCopiedFileName << " copied\n";
		});
		copier.CopyFiles({ "vasya.txt", "petya.txt", "seryozha.jpg", "oleg.png" });
	}

	cout << "--------------\n";

	copier.CopyFiles({ "vasya2.txt", "petya2.txt", "seryozha2.jpg", "oleg2.png" });
}

int _tmain(int argc, _TCHAR* argv[])
{
	MultiplexingFileCopierListenerExample();

	cout << "=============\n";

	MultipleObserversExample();

	cout << "=============\n";

	FunctionalObjectObserverExample();

	cout << "=============\n";

	ObservingWithBoostSignals();

	return 0;
}