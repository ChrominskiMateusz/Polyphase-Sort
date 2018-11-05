#pragma once
#include <cmath>
#include "FileIO.h"

template<typename T>
class Logic
{
public:
	void sort (void);
	void printFiles (void);
	Logic (const std::string& fileName);
	Logic (const std::string& fileName, int amount, const bool& doRandom);
	~Logic (void);
	FileIO<T> *physical;
	float fCount{};
	bool doPrint{};
private:
	void merge (void);
	void resetG (void);
	void setLines (void);
	void switchLine (void);
	void printBuffers (void);
	void firstStepSort (void);
	void clearBPointers (void);
	void saveSortedFile (void);
	void clearRecordsAmount (void);
	void afterMergeCleaning (void);
	void checkSave (const int& index);
	void clearFile (const int& index);
	void initiateVar (const int& sourceIndex);
	void distributeV2 (const int& sourceIndex);
	void placeS (const int& sourceIndex, int& count);
	void checkBonded (const int& sourceIndex, int& count);
	void moveS (const int& fromIndex, const int& toIndex);
	void checkGet (const int& souceIndex, bool& isEnd, T& prevR);
	void checkEnd (const int& souceIndex, bool& isEnd, T& prevR);
	void moveRecord (const int& fromIndex, const int& toIndex, bool& isEnd, T& prevR);
	int dummy[3]{};
	int seriesAmount[3]{};
	int bPointer[4]{};
	int lineNO;
	int firstLine;
	int secondLine;
	int thirdLine;
	T prev{};
	T prevS{};
};

template<typename T>
Logic<T>::Logic (const std::string& fileName, int amount, const bool& doRandom) :
	physical{ new FileIO<T> (fileName, amount, doRandom) }
{}

template<typename T>
Logic<T>::Logic (const std::string& fileName) :
	physical{ new FileIO<T> (fileName) }
{}

template<typename T>
Logic<T>::~Logic ()
{
	delete physical;
}

template<typename T>
void Logic<T>::distributeV2 (const int& sourceIndex)		// distribiute records on 2 lines
{
	int count = 1;
	const int length = physical->getLength (sourceIndex);
	initiateVar (sourceIndex);

	while (!(physical->files[sourceIndex].tellg () == length && bPointer[sourceIndex] == physical->recordsAmount[sourceIndex]))
		placeS (sourceIndex, count);
	
	switchLine ();
	if (count != seriesAmount[lineNO - 1])
	{
		switchLine ();
		dummy[lineNO - 1] = count;
	}
}

template<typename T>
void Logic<T>::merge (void)			// merging: 2S -> 1S
{
	bool isEnd = false;
	bool isEndS = false;
	prev = {};
	prevS = {};

	while (!isEnd && !isEndS)
	{
		checkGet (firstLine, isEnd, prev);
		checkGet (secondLine, isEndS, prevS);

		while (bPointer[firstLine] != -1 && bPointer[firstLine] < physical->recordsAmount[firstLine] && !isEnd
			&& bPointer[secondLine] != -1 && bPointer[secondLine] < physical->recordsAmount[secondLine] && !isEndS)
			if (!physical->buffers[firstLine][bPointer[firstLine]].compare (physical->buffers[secondLine][bPointer[secondLine]]))
				moveRecord (firstLine, thirdLine, isEnd, prev);
			else
				moveRecord (secondLine, thirdLine, isEndS, prevS);
		checkSave (thirdLine);
	}

	if (isEnd && !isEndS)
	{
		moveS (secondLine, thirdLine);
		isEndS = true;
	}
	else if (isEndS && !isEnd)
	{
		moveS (firstLine, thirdLine);
		isEnd = true;
	}
}

template<typename T>
void Logic<T>::sort (void)			// sort data
{
	firstStepSort ();
	while (seriesAmount[firstLine - 1] + seriesAmount[secondLine - 1] + seriesAmount[thirdLine - 1] != 1)
	{
		while (seriesAmount[secondLine - 1] != 0)
		{
			merge ();
			seriesAmount[firstLine - 1]--;
			seriesAmount[secondLine - 1]--;
			seriesAmount[thirdLine - 1]++;
		}
		afterMergeCleaning ();
	}
	saveSortedFile ();
}

template<typename T>
void Logic<T>::printFiles (void)
{
	std::cout << "\nFirst File:\n";
	physical->printData (1);
	
	std::cout << "\nSecond File:\n";
	physical->printData (2);

	std::cout << "\nThird File:\n";
	physical->printData (3);
}

template<typename T>
void Logic<T>::checkBonded (const int& sourceIndex, int& count)
{
	if (bPointer[sourceIndex] == -1)
		physical->getRecords (sourceIndex, bPointer[sourceIndex]);
	int tmp = physical->files[sourceIndex].tellg ();
	tmp -= (physical->recordsAmount[sourceIndex] - bPointer[sourceIndex]) * sizeof (T);

	if (seriesAmount[lineNO - 1] > 0 && tmp < physical->getLength (sourceIndex))
	{
		T nextRecord = physical->buffers[sourceIndex][bPointer[sourceIndex]];

		T lastRecord;
		if (physical->recordsAmount[lineNO] == 0)
		{
			physical->files[lineNO].clear ();
			physical->files[lineNO].seekg (physical->getLength (lineNO) - sizeof (T));
			physical->files[lineNO].read (reinterpret_cast<char *>(&lastRecord), sizeof (T));
			physical->files[lineNO].seekg (physical->getLength (lineNO));
		}
		else
			lastRecord = physical->buffers[lineNO][physical->recordsAmount[lineNO] - 1];

		if (nextRecord.compare (lastRecord))
		{
			count++;
			seriesAmount[lineNO - 1]--;
		}
	}
}

template<typename T>
void Logic<T>::placeS (const int& sourceIndex, int& count)		// place S on line
{
	moveS (sourceIndex, lineNO);
	seriesAmount[lineNO - 1]++;
	count--;
	if (!count)
	{
		count = seriesAmount[lineNO - 1];
		switchLine ();
		checkBonded (sourceIndex, count);
	}
}

template<typename T>
void Logic<T>::clearRecordsAmount (void)
{
	for (int i{}; i < physical->recordsAmount.size (); i++)
		physical->recordsAmount[i] = 0;
}

template<typename T>
void Logic<T>::afterMergeCleaning (void)
{
	if (physical->recordsAmount[thirdLine] > 0)
		physical->saveRecords (thirdLine);

	fCount++;
	bPointer[secondLine] = -1;
	bPointer[thirdLine] = -1;
	clearFile (secondLine);
	physical->files[thirdLine].clear ();
	physical->files[thirdLine].seekg (0);
	setLines ();
	physical->recordsAmount[thirdLine] = 0;
	if (doPrint)
		printFiles ();
}

template<typename T>
void Logic<T>::checkSave (const int& index)
{
	if (physical->recordsAmount[index] == FileIO<T>::BUFFER_CAPACITY)
		physical->saveRecords (index);
}

template<typename T>
void Logic<T>::switchLine (void)
{
	if (lineNO == firstLine)
		lineNO = secondLine;
	else
		lineNO = firstLine;
}

template<typename T>
void Logic<T>::printBuffers (void)
{
	for (int i{ 1 }; i < FileIO<T>::FILES_AMOUNT; i++)
		for (int j{}; j < physical->recordsAmount[i]; j++)
			physical->buffers[i][j].printRecord ();
}

template<typename T>
void Logic<T>::firstStepSort (void)			// distribute and clean dummies
{
	distributeV2 (FileIO<T>::dataFile);
	physical->saveRecords (FileIO<T>::firstBuffer);
	physical->saveRecords (FileIO<T>::secondBuffer);
	std::cout << "1.45 * log2(s) = " << 1.45 * log2 (seriesAmount[0] + seriesAmount[1]) << std::endl;

	resetG ();
	setLines ();
	clearBPointers ();

	for (int i{ dummy[lineNO - 1] }; i > 0; i--)
	{
		moveS (secondLine, thirdLine);
		seriesAmount[secondLine - 1]--;
		seriesAmount[thirdLine - 1]++;
	}
	dummy[lineNO - 1] = 0;
}

template<typename T>
void Logic<T>::clearBPointers (void)
{
	for (int i{}; i < FileIO<T>::FILES_AMOUNT; i++)
		bPointer[i] = -1;
}

template<typename T>
void Logic<T>::saveSortedFile (void)
{
	std::fstream f;
	f.open ("data/out.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
	physical->files[firstLine].clear ();
	physical->files[firstLine].seekg (0);
	
	T t;
	while (physical->files[firstLine].read (reinterpret_cast<char*>(&t), sizeof (T)))
		f.write (reinterpret_cast<const char*>(&t), sizeof (T));

	f.close ();
}

template<typename T>
void Logic<T>::initiateVar (const int& sourceIndex)
{
	clearBPointers ();
	firstLine = FileIO<T>::firstBuffer;
	secondLine = FileIO<T>::secondBuffer;
	thirdLine = FileIO<T>::thirdBuffer;
	lineNO = firstLine;
	physical->files[sourceIndex].seekg (0);
}

template<typename T>
void Logic<T>::setLines (void)
{
	int i{};
	while (seriesAmount[i] != 0)
		i++;
	thirdLine = i;

	if (seriesAmount[(i + 1) % 3] + dummy[(i + 1) % 3] > seriesAmount[(i + 2) % 3] + dummy[(i + 2) % 3])
	{
		firstLine = (i + 1) % 3;
		secondLine = (i + 2) % 3;
	}
	else
	{
		firstLine = (i + 2) % 3;
		secondLine = (i + 1) % 3;
	}

	firstLine++;
	secondLine++;
	thirdLine++;
}

template<typename T>
void Logic<T>::clearFile (const int& index)
{
	std::string path = "data/";
	switch (index)
	{
	case FileIO<T>::firstBuffer:
		path += "first";
		break;

	case FileIO<T>::secondBuffer:
		path += "second";
		break;

	case FileIO<T>::thirdBuffer:
		path += "third";
		break;
	}
	path += "BufferFile.dat";

	physical->files[index].close ();
	physical->files[index].open (path, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
}

template<typename T>
void Logic<T>::checkEnd (const int& sourceIndex, bool& isEnd, T& prevR)
{
	if (bPointer[sourceIndex] == FileIO<T>::BUFFER_CAPACITY)
	{
		prevR = physical->buffers[sourceIndex][bPointer[sourceIndex] - 1];
		bPointer[sourceIndex] = -1;
	}
	else if (bPointer[sourceIndex] == physical->recordsAmount[sourceIndex] ||
		physical->buffers[sourceIndex][bPointer[sourceIndex] - 1].compare (physical->buffers[sourceIndex][bPointer[sourceIndex]]))
		isEnd = true;
}

template<typename T>
void Logic<T>::moveRecord (const int& fromIndex, const int& toIndex, bool& isEnd, T& prevR)
{
	physical->buffers[toIndex][physical->recordsAmount[toIndex]] = physical->buffers[fromIndex][bPointer[fromIndex]];
	bPointer[fromIndex]++;
	physical->recordsAmount[toIndex]++;
	checkSave (toIndex);
	checkEnd (fromIndex, isEnd, prevR);
}

template<typename T>
void Logic<T>::resetG (void)
{
	for (int i{}; i < FileIO<T>::FILES_AMOUNT; i++)
	{
		physical->files[i].clear ();
		physical->files[i].seekg (0);
	}
}

template<typename T>
void Logic<T>::moveS (const int& fromIndex, const int& toIndex)		// transport 1S 
{
	bool isEnd = false;
	prev = {};
	while (!isEnd)
	{
		checkGet (fromIndex, isEnd, prev);

		while (bPointer[fromIndex] != -1 && bPointer[fromIndex] < physical->recordsAmount[fromIndex] && !isEnd)
			moveRecord (fromIndex, toIndex, isEnd, prev);

		checkSave (toIndex);
		if (physical->recordsAmount[fromIndex] != FileIO<T>::BUFFER_CAPACITY)
			isEnd = true;
	}
}

template<typename T>
void Logic<T>::checkGet (const int& souceIndex, bool& isEnd, T& prevR)
{
	if (bPointer[souceIndex] == -1)
	{
		physical->getRecords (souceIndex, bPointer[souceIndex]);
		if (prevR.compare (physical->buffers[souceIndex][0]))
			isEnd = true;
	}
}