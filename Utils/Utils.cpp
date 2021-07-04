//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2018 NVIDIA Corporation. All rights reserved.

#include "Utils.h"

#include "foundation/PxSimpleTypes.h"

#include "CmPhysXCommon.h"

#include "PsAtomic.h"
#include "PsString.h"
#include "PsSync.h"
#include "PsThread.h"
#include "PsTime.h"
#include "PsMutex.h"
#include "PsAllocator.h"
#include "extensions/PxDefaultAllocator.h"
#include<filesystem>
namespace fs = std::experimental::filesystem;


BOOL SetConsoleColor(WORD wAttributes)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
		return FALSE;

	return SetConsoleTextAttribute(hConsole, wAttributes);
}


void Logger::notice(std::string str) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_BLUE); //blue
	std::cout << "[NOTICE]\t" << str << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void Logger::debug(std::string str) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN); //green
	std::cout << "[DEBUG]\t" << str << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void Logger::debug(physx::PxVec3& v) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN); //green
	std::cout << "[DEBUG]\tx:" << v.x << "\ty:" << v.y << "\tz:" << v.z << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void Logger::debug(glm::vec3& v) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN); //green
	std::cout << "[DEBUG]\tx:" << v.x << "\ty:" << v.y << "\tz:" << v.z << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void  Logger::debug(std::string str, physx::PxVec3& v) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN); //green
	std::cout << "[DEBUG]\t" << str << "\tx:" << v.x << "\ty:" << v.y << "\tz:" << v.z << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void  Logger::debug(std::string str, glm::vec3& v) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN); //green
	std::cout << "[DEBUG]\t" << str << "\tx:" << v.x << "\ty:" << v.y << "\tz:" << v.z << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void Logger::info(std::string str) {
	std::cout << "[INFO]\t" << str << "\n";
}
void Logger::warn(std::string str) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // yellow
	std::cout << "[WARN]\t" << str << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void Logger::error(std::string str) {
	SetConsoleColor(FOREGROUND_INTENSITY | FOREGROUND_RED); // red
	std::cout << "[ERROR]\t" << str << "\n";
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


int PointerUtils::getPtrIntValue(void* ptr) {
	char str[13];
	sprintf_s(str, 13, "%d", ptr);
	return fabs(atoi(str));
}
//计算文件的个数
int FileUtils::getFilesCount(std::string path)
{
	int rt = 0;
	for (const auto & entry : fs::directory_iterator(path))
		rt++;
	return rt;

}




/*得到文件夹内的文件*/
void FileUtils::getFiles(std::string path, std::vector<std::string>& files)
{
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				files.push_back(fileinfo.name);
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


/*
删除文件夹里面的文件
*/
void FileUtils::removeFileInDir(std::string path) {
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				remove(fileinfo.name);
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}


bool FileUtils::isFileExist(std::string fileName) {
	std::ifstream inFile;
	inFile.open(fileName, std::ios::in);
	if (inFile) {  //条件成立，则说明文件打开成功
		inFile.close();
		return true;
	}
	return false;
}


void StringUtils::split(std::string& s, std::string c, std::vector<std::string>& v) {
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

void StringUtils::replaceChar(std::string& str, char oldChar, char newChar) {
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == oldChar)
			str[i] = newChar;
	}
}


std::string StringUtils::stringToUTF8(const std::string & str) {
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

// compiler文件夹所在的目录
std::string PathUtils::getRoot() {
	return "../../";
}
// getRoot()目录下的resource文件夹目录
std::string PathUtils::getResourceRoot() {
	return PathUtils::getRoot() + "resource/";
}
//getResourceRoot()目录下的model目录
std::string PathUtils::getModelRoot() {
	return PathUtils::getResourceRoot() + "model/";
}

std::string PathUtils::getModelPath(std::string dir) {
	if (dir.size() > 2) {
		if (dir[1] == ':')//是绝对路径，直接返回
			return dir;
	}
	return PathUtils::getModelRoot() + dir;
}

namespace physx
{

	namespace
	{
		PxDefaultAllocator gUtilAllocator;

		struct UtilAllocator // since we're allocating internal classes here, make sure we align properly
		{
			void* allocate(size_t size, const char* file, PxU32 line) { return gUtilAllocator.allocate(size, NULL, file, int(line)); }
			void deallocate(void* ptr) { gUtilAllocator.deallocate(ptr); }
		};
	}


	namespace SnippetUtils
	{

		PxI32 atomicIncrement(volatile PxI32* val)
		{
			return Ps::atomicIncrement(val);
		}

		PxI32 atomicDecrement(volatile PxI32* val)
		{
			return Ps::atomicDecrement(val);
		}

		//******************************************************************************//

		PxU32 getNbPhysicalCores()
		{
			return Ps::Thread::getNbPhysicalCores();
		}

		//******************************************************************************//

		PxU32 getThreadId()
		{
			return static_cast<PxU32>(Ps::Thread::getId());
		}

		//******************************************************************************//

		PxU64 getCurrentTimeCounterValue()
		{
			return Ps::Time::getCurrentCounterValue();
		}

		PxReal getElapsedTimeInMilliseconds(const PxU64 elapsedTime)
		{
			return Ps::Time::getCounterFrequency().toTensOfNanos(elapsedTime) / (100.0f * 1000.0f);
		}

		PxReal getElapsedTimeInMicroSeconds(const PxU64 elapsedTime)
		{
			return Ps::Time::getCounterFrequency().toTensOfNanos(elapsedTime) / (100.0f);
		}

		//******************************************************************************//

		struct Sync : public Ps::SyncT<UtilAllocator> {};

		Sync* syncCreate()
		{
			return new(gUtilAllocator.allocate(sizeof(Sync), 0, 0, 0)) Sync();
		}

		void syncWait(Sync* sync)
		{
			sync->wait();
		}

		void syncSet(Sync* sync)
		{
			sync->set();
		}

		void syncReset(Sync* sync)
		{
			sync->reset();
		}

		void syncRelease(Sync* sync)
		{
			sync->~Sync();
			gUtilAllocator.deallocate(sync);
		}

		//******************************************************************************//

		struct Thread : public Ps::ThreadT<UtilAllocator>
		{
			Thread(ThreadEntryPoint entryPoint, void* data) :
				Ps::ThreadT<UtilAllocator>(),
				mEntryPoint(entryPoint),
				mData(data)
			{
			}

			virtual void execute(void)
			{
				mEntryPoint(mData);
			}

			ThreadEntryPoint mEntryPoint;
			void* mData;
		};

		Thread* threadCreate(ThreadEntryPoint entryPoint, void* data)
		{
			Thread* createThread = static_cast<Thread*>(gUtilAllocator.allocate(sizeof(Thread), 0, 0, 0));
			PX_PLACEMENT_NEW(createThread, Thread(entryPoint, data));
			createThread->start();
			return createThread;
		}

		void threadQuit(Thread* thread)
		{
			thread->quit();
		}

		void threadSignalQuit(Thread* thread)
		{
			thread->signalQuit();
		}

		bool threadWaitForQuit(Thread* thread)
		{
			return thread->waitForQuit();
		}

		bool threadQuitIsSignalled(Thread* thread)
		{
			return thread->quitIsSignalled();
		}

		void threadRelease(Thread* thread)
		{
			thread->~Thread();
			gUtilAllocator.deallocate(thread);
		}

		//******************************************************************************//

		struct Mutex : public Ps::MutexT<UtilAllocator> {};

		Mutex* mutexCreate()
		{
			return new(gUtilAllocator.allocate(sizeof(Mutex), 0, 0, 0)) Mutex();
		}

		void mutexLock(Mutex* mutex)
		{
			mutex->lock();
		}

		void mutexUnlock(Mutex* mutex)
		{
			mutex->unlock();
		}

		void mutexRelease(Mutex* mutex)
		{
			mutex->~Mutex();
			gUtilAllocator.deallocate(mutex);
		}


	} // namespace physXUtils
} // namespace physx
