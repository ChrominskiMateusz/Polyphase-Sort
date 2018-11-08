#pragma once
#include "Record.h"
#include <string>
#include <vector>
#include <fstream>

template<typename T>
class FileIO
{
public:
	FileIO (const std::string& fileName);
	FileIO (const std::string& fileName, int amount, const bool& doRandom);
	void printData (const int& fileIndex);
	void init (const std::string& fileName);
	void saveRecords (const int& bufferIndex);		// save buffer
	const int getLength (const int& sourceIndex);
	void getRecords (const int& bufferIndex, int& bPointer);	// fill buffer w/ records
	void createData (const std::string& fileName, int amount, const bool& doRandom);
	~FileIO (void);
	int operations;
	std::vector<T*> buffers;
	std::vector<int> recordsAmount;
	std::vector<std::fstream> files;
	static const int BUFFER_CAPACITY;
	static const int FILES_AMOUNT;
	enum
	{
		dataFile, firstBuffer, secondBuffer, thirdBuffer
	};
};

template<typename T>
FileIO<T>::FileIO (const std::string& fileName, int amount, const bool& doRandom)
{
	createData (fileName, amount, doRandom);
	init (fileName);
}

template<typename T>
FileIO<T>::FileIO (const std::string& fileName)
{
	init (fileName);
}

template<typename T>
void FileIO<T>::getRecords (const int& bufferIndex, int& bPointer)
{
	bPointer = 0;
	int length = getLength (bufferIndex);

	if ((length - files[bufferIndex].tellg ()) / sizeof (T) <= BUFFER_CAPACITY)
		recordsAmount[bufferIndex] = (length - files[bufferIndex].tellg ()) / sizeof (T);
	else
		recordsAmount[bufferIndex] = BUFFER_CAPACITY;

	operations++;
	files[bufferIndex].read (reinterpret_cast<char *>(buffers[bufferIndex]), recordsAmount[bufferIndex] * sizeof (T));
}

template<typename T>
void FileIO<T>::saveRecords (const int& bufferIndex)
{
	operations++;
	files[bufferIndex].write (reinterpret_cast<const char*>(buffers[bufferIndex]), recordsAmount[bufferIndex] * sizeof (T));
	recordsAmount[bufferIndex] = 0;
}

template<typename T>
FileIO<T>::~FileIO (void)
{
	for (int i{ 0 }; i < FILES_AMOUNT; i++)
	{
		files[i].close ();
		delete[] buffers[i];
	}
}

template<typename T>
void FileIO<T>::createData (const std::string& fileName, int amount, const bool& doRandom)
{
	std::srand (std::time (nullptr));
	std::string path = "data/";
	path += fileName;
	path += ".dat";

	std::fstream dataFile;
	dataFile.open (path, std::ios::binary | std::ios::out);

	T t;
	while (amount--)
	{
		if (doRandom)
			t.randomValues ();
		else
			t.typeValues ();
		dataFile.write (reinterpret_cast<const char*>(&t), sizeof (T));
	}

	dataFile.close ();
}

template<typename T>
void FileIO<T>::printData (const int& fileIndex)
{
	int tmp1 = files[fileIndex].tellg ();
	files[fileIndex].clear ();
	files[fileIndex].seekg (0);

	T t;
	while (files[fileIndex].read (reinterpret_cast<char*>(&t), sizeof (T)))
		t.printRecord ();

	files[fileIndex].clear ();
	files[fileIndex].seekg (tmp1, files[fileIndex].beg);
}

template<typename T>
void FileIO<T>::init (const std::string& fileName)
{
	operations = 0;
	files.resize (FILES_AMOUNT);						// reservation of vectors 'n arrays	
	buffers.resize (FILES_AMOUNT);
	recordsAmount.resize (FILES_AMOUNT);
	for (int i{ 0 }; i < FILES_AMOUNT; i++)
		buffers[i] = new T[BUFFER_CAPACITY];

	const std::string path = "data/" + fileName + ".dat";

	files[dataFile].open (path, std::ios::binary | std::ios::in | std::ios::out | std::ios::app);
	files[firstBuffer].open ("data/firstBufferFile.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
	files[secondBuffer].open ("data/secondBufferFile.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
	files[thirdBuffer].open ("data/thirdBufferFile.dat", std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
}

template<typename T>
const int FileIO<T>::getLength (const int& sourceIndex)			// of file
{
	int tmp = files[sourceIndex].tellg ();
	files[sourceIndex].clear ();
	files[sourceIndex].seekg (0, files[sourceIndex].end);
	int length = files[sourceIndex].tellg ();
	files[sourceIndex].clear ();
	files[sourceIndex].seekg (tmp);

	return length;
}

template<typename T>
const int FileIO<T>::FILES_AMOUNT = 4;			// 2 + 1 + dataFileBuffer

template<typename T>
const int FileIO<T>::BUFFER_CAPACITY = 6;