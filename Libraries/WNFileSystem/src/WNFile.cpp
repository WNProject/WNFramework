////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//                                                         WNProject                                                          //
//                                                                                                                            //
//         This file is distributed under the BSD 2-Clause open source license. See Licenses/License.txt for details.         //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WNFileSystem/inc/WNFile.h"
#include "WNMemory/inc/WNMemory.h"
#include "WNStrings/inc/WNStrings.h"
#include <string>
#ifdef _WN_WINDOWS
    #include <windows.h>
#else
    #include <stdio.h>
#endif

#if defined _WN_LINUX || defined _WN_ANDROID
    #include <sys/file.h>
#endif

WNFileSystem::WNFile::WNFile() :
    mMaxBufferSize(0),
    mFileSize(0),
    mFileBuffer(WN_NULL),
    #ifdef _WN_WINDOWS
        mFileHandle(INVALID_HANDLE_VALUE),
    #elif defined _WN_LINUX || defined _WN_ANDROID
        mFileHandle(NULL),
    #endif
    mIsFileOpen(WN_FALSE),
    mIsValid(WN_FALSE) {
}

WNFileSystem::WNFile::~WNFile() {
    if (mFileBuffer) {
        if (mFileMode & WNFileSystem::WNFile::eWNFMWrite) {
            CommitFileBuffer();
        }

        WNMemory::WNFreeT<WN_CHAR>(mFileBuffer);
    }

    #ifdef _WN_WINDOWS
        if (mFileHandle) {
            CloseHandle(mFileHandle);
        }
    #elif defined _WN_LINUX || defined _WN_ANDROID
        if (mFileHandle) {
            fclose(mFileHandle);
        }
    #endif
}

WNFileSystem::WNFile::WNFileError WNFileSystem::WNFile::OpenFile(const WN_CHAR* _name, WN_UINT32 _mode) {
    WN_DEBUG_ASSERT(!mIsFileOpen);

    mStream = (_mode & WNFileSystem::WNFile::eWNFMStream) == WNFileSystem::WNFile::eWNFMStream;

    if ((_mode & (WNFileSystem::WNFile::eWNFMCreate | WNFileSystem::WNFile::eWNFMClobber)) == WNFileSystem::WNFile::eWNFMClobber) {
        return(WNFileSystem::WNFile::eWNFEBadMode);
    }

    const WN_BOOL shouldCheckForFile = !(_mode & WNFileSystem::WNFile::eWNFMCreate);

    if (shouldCheckForFile && !DoesFileExist(_name)) {
        return(WNFileSystem::WNFile::eWNDoesNotExist);
    }

    #ifdef _WN_WINDOWS
        DWORD accessType = 0;

        accessType |= ((_mode & WNFileSystem::WNFile::eWNFMRead) ? GENERIC_READ : 0);
        accessType |= ((_mode & WNFileSystem::WNFile::eWNFMWrite) ? GENERIC_WRITE : 0);

        const DWORD sharing = (_mode & WNFileSystem::WNFile::eWNFMExclusive) ? 0 : (FILE_SHARE_READ | FILE_SHARE_WRITE);
        DWORD creationDisposition = (_mode & WNFileSystem::WNFile::eWNFMCreate) ? OPEN_ALWAYS : OPEN_EXISTING;

        if (_mode & WNFileSystem::WNFile::eWNFMClobber) {
            creationDisposition = CREATE_ALWAYS;
        }

        mFileHandle = CreateFile(_name, accessType, sharing, WN_NULL, creationDisposition, 0, 0);

        if (mFileHandle == INVALID_HANDLE_VALUE) {
            return(WNFileSystem::WNFile::eWNFEBadMode);
        }

        if (!mStream) {
            LARGE_INTEGER filesize;

            if (!GetFileSizeEx(mFileHandle, &filesize)) {
                CloseHandle(mFileHandle);

                return(WNFileSystem::WNFile::eWNFEBadMode);
            }

            if (static_cast<LONGLONG>(static_cast<WN_SIZE_T>(filesize.QuadPart)) != filesize.QuadPart) {
                CloseHandle(mFileHandle);

                return(WNFileSystem::WNFile::eWNFETooLarge);
            }

            mFileSize = static_cast<WN_SIZE_T>(filesize.QuadPart);
        }
    #elif defined _WN_LINUX || defined _WN_ANDROID
        if (_mode & WNFileSystem::WNFile::eWNFMClobber) {
            if ((_mode & WNFileSystem::WNFile::eWNFMRead) != WNFileSystem::WNFile::eWNFMRead) {
                mFileHandle = fopen(_name, "w");
            } else {
                mFileHandle = fopen(_name, "r+");
            }
        } else if ((_mode & WNFileSystem::WNFile::eWNFMRead) && (_mode & WNFileSystem::WNFile::eWNFMWrite)) {
            mFileHandle = fopen(_name, "r+");
        } else if ((_mode & WNFileSystem::WNFile::eWNFMRead)) {
            mFileHandle = fopen(_name, "r");
        } else if ((_mode & WNFileSystem::WNFile::eWNFMWrite)) {
            mFileHandle = fopen(_name, "a");
        }

        if (!mFileHandle) {
            return(WNFileSystem::WNFile::eWNError);
        }

        fseek(mFileHandle, 0L, SEEK_END);

        mFileSize = ftell(mFileHandle);

        fseek(mFileHandle, 0L, SEEK_SET);
    #endif

    mIsFileOpen = WN_TRUE;
    mFileMode = _mode;
    mIsValid = WN_TRUE;

    return(WNFileSystem::WNFile::eWNOK);
}

WN_SIZE_T WNFileSystem::WNFile::GetFileSize() {
    return(mFileSize);
}

WN_CHAR* WNFileSystem::WNFile::GetFolderName(const WN_CHAR* _name) {
    std::string str(_name);
    WN_SIZE_T loc = str.find_last_of("/\\");
    if(loc == std::string::npos) {
        return(WNStrings::WNStrNDup("", 2));
    }
    return(WNStrings::WNStrNDup(_name, loc + 1));
}

WN_CHAR* WNFileSystem::WNFile::GetFileName(const WN_CHAR* _name) {
    std::string str(_name);
    WN_SIZE_T loc = str.find_last_of("/\\");
    if(loc == std::string::npos) {
        return(WNStrings::WNStrNDup(_name, 1024));
    }
    return(WNStrings::WNStrNDup(_name + loc, WNStrings::WNStrLen(_name) - loc + 1));
}

WN_BOOL WNFileSystem::WNFile::DeleteFile(const WN_CHAR* _name) {
#ifdef _WN_WINDOWS
    return(::DeleteFile(_name) == TRUE);
#else
    return(remove(_name) == 0);
#endif
}

WN_VOID WNFileSystem::WNFile::CollapseFolderStructure(WN_CHAR* _name) {
    WN_CHAR* lastSeperator = _name;
    WN_CHAR* olderSeperator = WN_NULL;
    WN_BOOL lastDot = false;
    WN_BOOL lastDoubleDot = false;
    WN_BOOL collapsed = false;
    for(WN_CHAR* it = _name ; *it != '\0'; ++it) {
        if(*it == '\\') {
            *it = '/';
        }
        if(*it == '/') {
            if(lastDoubleDot) { //We have found a /something/../ ..destroy it
                if(olderSeperator) {
                    WNMemory::WNMemMove(olderSeperator, it, WNStrings::WNStrLen(it));
                    collapsed = true;
                    break;
                } else {
                    lastDoubleDot = false;
                    lastDot = false;
                }
            } else if (lastDot) {
                if(it - _name > 2) { //we have found a ./ destroy it
                    WNMemory::WNMemMove(it - 2, it, WNStrings::WNStrLen(it));
                    collapsed = true;
                    break;
                }
            } else {
                olderSeperator = lastSeperator;
                lastSeperator = it;
                lastDot = false;
                lastDoubleDot = false;
            }
        } else if (*it == '.') {
            if(lastDot) {
                lastDoubleDot = true;
            } else {
                lastDot = true;
            }
        } else {
            lastDot = false;
            lastDoubleDot = false;
        }
    }
    if(collapsed) {
        CollapseFolderStructure(_name);
    }
}


WN_BOOL WNFileSystem::WNFile::DoesFileExist(const WN_CHAR* _name) {
    #ifdef _WN_WINDOWS
        const DWORD fileAttributes = GetFileAttributes(_name);

        if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
            return(WN_FALSE);
        }

        if ((fileAttributes & FILE_ATTRIBUTE_NORMAL) == fileAttributes) {
            return(WN_TRUE);
        }

        if ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
            (fileAttributes & FILE_ATTRIBUTE_DEVICE) ||
            (fileAttributes & FILE_ATTRIBUTE_OFFLINE) ||
            (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
            (fileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
                return(WN_FALSE);
        }
    #elif defined _WN_LINUX || defined _WN_ANDROID
        FILE* file;

        if ((file = fopen(_name, "r")) == WN_NULL) {
            return(WN_FALSE);
        }

        fclose(file);
    #endif

    return(WN_TRUE);
}

WN_CHAR* WNFileSystem::WNFile::GetDataBuffer() {
    WN_DEBUG_ASSERT_DESC(!mStream, "You cannot get a data buffer for a streamed file");

    if (mFileBuffer != WN_NULL || mFileSize == 0) {
        return(mFileBuffer);
    }

    mFileBuffer = WNMemory::WNMallocT<WN_CHAR>(mFileSize);

    if (mFileBuffer == WN_NULL) {
        return(WN_NULL);
    }

    #ifdef _WN_WINDOWS
        WN_SIZE_T readLeft = mFileSize;

        do {
            const DWORD maxRead = (static_cast<DWORD>(readLeft) == readLeft) ? static_cast<DWORD>(readLeft) : -1;
            DWORD amountRead = 0;

            if (!ReadFile(mFileHandle,
                          mFileBuffer + (mFileSize - readLeft),
                          maxRead,
                          &amountRead,
                          WN_NULL)) {
                    WNMemory::WNFreeT<WN_CHAR>(mFileBuffer);

                    return(WN_NULL);
            }

            readLeft -= amountRead;
        } while (readLeft > 0);
    #elif defined _WN_LINUX || defined _WN_ANDROID
        if(!mFileHandle) {
            return(WN_NULL);
        }
        if (fread(mFileBuffer, sizeof(WN_CHAR), mFileSize, static_cast<FILE*>(mFileHandle)) != mFileSize) {
            WNMemory::WNFreeT<WN_CHAR>(mFileBuffer);

            return(WN_NULL);
        }
    #endif

    return(mFileBuffer);
}

WN_CHAR* WNFileSystem::WNFile::ResizeDataBuffer(WN_SIZE_T _size) {
    if (mFileBuffer) {
        mFileBuffer = WNMemory::WNReallocT<WN_CHAR>(mFileBuffer, _size);
    } else {
        mFileBuffer = WNMemory::WNMallocT<WN_CHAR>(_size);
    }

    mFileSize = _size;

    if (mFileSize > 0) {
        WN_RELEASE_ASSERT(mFileBuffer != WN_NULL);
    }

    return(mFileBuffer);
}

WN_BOOL WNFileSystem::WNFile::CommitFileBuffer() {
    WN_DEBUG_ASSERT(mFileBuffer != WN_NULL);

    if (!(mFileMode & WNFileSystem::WNFile::eWNFMWrite)) {
        return(WN_FALSE);
    }

    #ifdef _WN_WINDOWS
        WN_SIZE_T writeLeft = mFileSize;

        do {
            const DWORD maxWrite = (static_cast<DWORD>(writeLeft) == writeLeft) ? static_cast<DWORD>(writeLeft) : -1;
            DWORD amountWrote = 0;

            if (!WriteFile(mFileHandle,
                           mFileBuffer + (mFileSize - writeLeft),
                           maxWrite,
                           &amountWrote,
                           WN_NULL)) {
                    return(WN_FALSE);
            }

            writeLeft -= amountWrote;
        } while (writeLeft > 0);
    #elif defined _WN_LINUX || defined _WN_ANDROID
        if (fwrite(mFileBuffer, sizeof(WN_CHAR), mFileSize, mFileHandle) != mFileSize) {
            return(WN_FALSE);
        }

        if (fflush(mFileHandle) == EOF) {
            return(WN_FALSE);
        }
    #endif

    return(WN_TRUE);
}

WN_SIZE_T WNFileSystem::WNFile::ReadData(WN_CHAR* _outBuffer, WN_SIZE_T _amount) {
    WN_DEBUG_ASSERT(_outBuffer != WN_NULL && _amount > 0);

    #ifdef _WN_WINDOWS
        WN_SIZE_T totalRead = 0;
        WN_SIZE_T readLeft = _amount;
        DWORD amountRead = 0;

        do {
            const DWORD maxRead = (static_cast<DWORD>(readLeft) == readLeft) ? static_cast<DWORD>(readLeft) : -1;

            if (!ReadFile(mFileHandle,
                          _outBuffer + (_amount - readLeft),
                          maxRead,
                          &amountRead,
                          WN_NULL)) {
                    if (GetLastError() == ERROR_HANDLE_EOF) {
                        totalRead += amountRead;

                        break;
                    }

                    return(0);
            }

            totalRead += amountRead;
            readLeft -= amountRead;

            if (amountRead == 0) {
                break;
            }
        } while (readLeft > 0);

        return(totalRead);
    #elif defined _WN_LINUX || defined _WN_ANDROID
        if(!mFileHandle) {
            return(0);
        }
        const WN_SIZE_T amountRead = fread(_outBuffer, sizeof(WN_CHAR), _amount, mFileHandle);

        if (amountRead != _amount) {
            if (feof(mFileHandle)) {
                return(amountRead);
            }

            return(0);
        }

        return(_amount);
    #endif
}

WN_SIZE_T WNFileSystem::WNFile::WriteData(WN_CHAR* _inBuffer, WN_SIZE_T _amount) {
    WN_DEBUG_ASSERT(_inBuffer != WN_NULL && _amount > 0);

    #ifdef _WN_WINDOWS
        WN_SIZE_T totalWritten = 0;
        WN_SIZE_T writeLeft = _amount;

        do {
            const DWORD maxWrite = (static_cast<DWORD>(writeLeft) == writeLeft) ? static_cast<DWORD>(writeLeft) : -1;
            DWORD amountWrote = 0;

            if (!WriteFile(mFileHandle,
                          _inBuffer + (_amount - writeLeft),
                           maxWrite,
                           &amountWrote,
                           WN_NULL)) {
               return(0);
            }

            totalWritten += amountWrote;
            writeLeft -= amountWrote;
        } while (writeLeft > 0);

        return(totalWritten);
    #elif defined _WN_LINUX || defined _WN_ANDROID
        const WN_SIZE_T amountWritten = fwrite(_inBuffer, sizeof(WN_CHAR), _amount, mFileHandle);

        if (amountWritten != _amount) {
            return(0);
        }

        return(_amount);
    #endif
}

WN_BOOL WNFileSystem::WNFile::IsValid() {
    return(mIsValid);
}
