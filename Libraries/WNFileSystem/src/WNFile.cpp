// Copyright (c) 2015, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "WNFileSystem/inc/WNFile.h"
#include "WNMemory/inc/WNStringUtility.h"
#include "WNMemory/inc/WNBasic.h"

#include <string>

#ifdef _WN_WINDOWS
#else
    #include <stdio.h>
#endif

#if defined _WN_LINUX || defined _WN_ANDROID
    #include <sys/file.h>
#endif

WNFileSystem::WNFile::WNFile() :
    mMaxBufferSize(0),
    mFileSize(0),
    mFileBuffer(wn_nullptr),
    #ifdef _WN_WINDOWS
        mFileHandle(INVALID_HANDLE_VALUE),
    #elif defined _WN_LINUX || defined _WN_ANDROID
        mFileHandle(NULL),
    #endif
    mIsFileOpen(wn_false),
    mIsValid(wn_false) {
}

WNFileSystem::WNFile::~WNFile() {
    if (mFileBuffer) {
        if (mFileMode & WNFileSystem::WNFile::eWNFMWrite) {
            CommitFileBuffer();
        }

        wn::memory::heap_free(mFileBuffer);
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

WNFileSystem::WNFile::WNFileError WNFileSystem::WNFile::OpenFile(const wn_char* _name, wn_uint32 _mode) {
    WN_DEBUG_ASSERT(!mIsFileOpen);

    mStream = (_mode & WNFileSystem::WNFile::eWNFMStream) == WNFileSystem::WNFile::eWNFMStream;

    if ((_mode & (WNFileSystem::WNFile::eWNFMCreate | WNFileSystem::WNFile::eWNFMClobber)) == WNFileSystem::WNFile::eWNFMClobber) {
        return(WNFileSystem::WNFile::eWNFEBadMode);
    }

    const wn_bool shouldCheckForFile = !(_mode & WNFileSystem::WNFile::eWNFMCreate);

    if (shouldCheckForFile && !DoesFileExist(_name)) {
        return(WNFileSystem::WNFile::does_not_exist);
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

        mFileHandle = CreateFile(_name, accessType, sharing, wn_nullptr, creationDisposition, 0, 0);

        if (mFileHandle == INVALID_HANDLE_VALUE) {
            return(WNFileSystem::WNFile::eWNFEBadMode);
        }

        if (!mStream) {
            LARGE_INTEGER filesize;

            if (!GetFileSizeEx(mFileHandle, &filesize)) {
                CloseHandle(mFileHandle);

                return(WNFileSystem::WNFile::eWNFEBadMode);
            }

            if (static_cast<LONGLONG>(static_cast<wn_size_t>(filesize.QuadPart)) != filesize.QuadPart) {
                CloseHandle(mFileHandle);

                return(WNFileSystem::WNFile::eWNFETooLarge);
            }

            mFileSize = static_cast<wn_size_t>(filesize.QuadPart);
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
            return(WNFileSystem::WNFile::error);
        }

        fseek(mFileHandle, 0L, SEEK_END);

        mFileSize = ftell(mFileHandle);

        fseek(mFileHandle, 0L, SEEK_SET);
    #endif

    mIsFileOpen = wn_true;
    mFileMode = _mode;
    mIsValid = wn_true;

    return(WNFileSystem::WNFile::ok);
}

wn_size_t WNFileSystem::WNFile::GetFileSize() const {
    return(mFileSize);
}

wn_char* WNFileSystem::WNFile::GetFolderName(const wn_char* _name) {
    std::string str(_name);
    wn_size_t loc = str.find_last_of("/\\");
    if(loc == std::string::npos) {
        return(wn::memory::strndup("", 2));
    }
    return(wn::memory::strndup(_name, loc + 1));
}

wn_char* WNFileSystem::WNFile::GetFileName(const wn_char* _name) {
    std::string str(_name);
    wn_size_t loc = str.find_last_of("/\\");
    if(loc == std::string::npos) {
      return(wn::memory::strndup(_name, 1024));
    }
    return(wn::memory::strndup(_name + loc, wn::memory::strlen(_name) - loc + 1));
}

wn_bool WNFileSystem::WNFile::DeleteFile(const wn_char* _name) {
#ifdef _WN_WINDOWS
    return(::DeleteFile(_name) == TRUE);
#else
    return(remove(_name) == 0);
#endif
}

wn_void WNFileSystem::WNFile::CollapseFolderStructure(wn_char* _name) {
    wn_char* lastSeperator = _name;
    wn_char* olderSeperator = wn_nullptr;
    wn_bool lastDot = false;
    wn_bool lastDoubleDot = false;
    wn_bool collapsed = false;
    for(wn_char* it = _name ; *it != '\0'; ++it) {
        if(*it == '\\') {
            *it = '/';
        }
        if(*it == '/') {
            if(lastDoubleDot) { //We have found a /something/../ ..destroy it
                if(olderSeperator) {
                  wn::memory::memmove(olderSeperator, it, wn::memory::strlen(it) + 1);
                    collapsed = true;
                    break;
                } else {
                    lastDoubleDot = false;
                    lastDot = false;
                }
            } else if (lastDot) {
                if(it - _name > 2) { //we have found a ./ destroy it
                  wn::memory::memmove(it - 2, it, wn::memory::strlen(it) + 1);
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


wn_bool WNFileSystem::WNFile::DoesFileExist(const wn_char* _name) {
    #ifdef _WN_WINDOWS
        const DWORD fileAttributes = GetFileAttributes(_name);

        if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
            return(wn_false);
        }

        if ((fileAttributes & FILE_ATTRIBUTE_NORMAL) == fileAttributes) {
            return(wn_true);
        }

        if ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
            (fileAttributes & FILE_ATTRIBUTE_DEVICE) ||
            (fileAttributes & FILE_ATTRIBUTE_OFFLINE) ||
            (fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
            (fileAttributes & FILE_ATTRIBUTE_SYSTEM)) {
                return(wn_false);
        }
    #elif defined _WN_LINUX || defined _WN_ANDROID
        FILE* file;

        if ((file = fopen(_name, "r")) == wn_nullptr) {
            return(wn_false);
        }

        fclose(file);
    #endif

    return(wn_true);
}

wn_char* WNFileSystem::WNFile::GetDataBuffer() {
    WN_DEBUG_ASSERT_DESC(!mStream, "You cannot get a data buffer for a streamed file");

    if (mFileBuffer != wn_nullptr || mFileSize == 0) {
        return(mFileBuffer);
    }

    mFileBuffer = wn::memory::heap_allocate<wn_char>(mFileSize);

    if (mFileBuffer == wn_nullptr) {
        return(wn_nullptr);
    }

    #ifdef _WN_WINDOWS
        wn_size_t readLeft = mFileSize;

        do {
            const DWORD maxRead = (static_cast<DWORD>(readLeft) == readLeft) ? static_cast<DWORD>(readLeft) : -1;
            DWORD amountRead = 0;

            if (!ReadFile(mFileHandle,
                          mFileBuffer + (mFileSize - readLeft),
                          maxRead,
                          &amountRead,
                          wn_nullptr)) {
                    wn::memory::heap_free(mFileBuffer);

                    return(wn_nullptr);
            }

            readLeft -= amountRead;
        } while (readLeft > 0);
    #elif defined _WN_LINUX || defined _WN_ANDROID
        if(!mFileHandle) {
            return(wn_nullptr);
        }
        if (fread(mFileBuffer, sizeof(wn_char), mFileSize, static_cast<FILE*>(mFileHandle)) != mFileSize) {
            wn::memory::heap_free(mFileBuffer);

            return(wn_nullptr);
        }
    #endif

    return(mFileBuffer);
}

wn_char* WNFileSystem::WNFile::ResizeDataBuffer(wn_size_t _size) {
    if (mFileBuffer) {
        mFileBuffer = wn::memory::heap_reallocate(mFileBuffer, _size);
    } else {
        mFileBuffer = wn::memory::heap_allocate<wn_char>(_size);
    }

    mFileSize = _size;

    if (mFileSize > 0) {
        WN_RELEASE_ASSERT(mFileBuffer != wn_nullptr);
    }

    return(mFileBuffer);
}

wn_bool WNFileSystem::WNFile::CommitFileBuffer() {
    WN_DEBUG_ASSERT(mFileBuffer != wn_nullptr);

    if (!(mFileMode & WNFileSystem::WNFile::eWNFMWrite)) {
        return(wn_false);
    }

    #ifdef _WN_WINDOWS
        wn_size_t writeLeft = mFileSize;

        do {
            const DWORD maxWrite = (static_cast<DWORD>(writeLeft) == writeLeft) ? static_cast<DWORD>(writeLeft) : -1;
            DWORD amountWrote = 0;

            if (!WriteFile(mFileHandle,
                           mFileBuffer + (mFileSize - writeLeft),
                           maxWrite,
                           &amountWrote,
                           wn_nullptr)) {
                    return(wn_false);
            }

            writeLeft -= amountWrote;
        } while (writeLeft > 0);
    #elif defined _WN_LINUX || defined _WN_ANDROID
        if (fwrite(mFileBuffer, sizeof(wn_char), mFileSize, mFileHandle) != mFileSize) {
            return(wn_false);
        }

        if (fflush(mFileHandle) == EOF) {
            return(wn_false);
        }
    #endif

    return(wn_true);
}

wn_size_t WNFileSystem::WNFile::ReadData(wn_char* _outBuffer, wn_size_t _amount) {
    WN_DEBUG_ASSERT(_outBuffer != wn_nullptr && _amount > 0);

    #ifdef _WN_WINDOWS
        wn_size_t totalRead = 0;
        wn_size_t readLeft = _amount;
        DWORD amountRead = 0;

        do {
            const DWORD maxRead = (static_cast<DWORD>(readLeft) == readLeft) ? static_cast<DWORD>(readLeft) : -1;

            if (!ReadFile(mFileHandle,
                          _outBuffer + (_amount - readLeft),
                          maxRead,
                          &amountRead,
                          wn_nullptr)) {
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
        const wn_size_t amountRead = fread(_outBuffer, sizeof(wn_char), _amount, mFileHandle);

        if (amountRead != _amount) {
            if (feof(mFileHandle)) {
                return(amountRead);
            }

            return(0);
        }

        return(_amount);
    #endif
}

wn_size_t WNFileSystem::WNFile::WriteData(const wn_char* _inBuffer, wn_size_t _amount) {
    WN_DEBUG_ASSERT(_inBuffer != wn_nullptr && _amount > 0);

    #ifdef _WN_WINDOWS
        wn_size_t totalWritten = 0;
        wn_size_t writeLeft = _amount;

        do {
            const DWORD maxWrite = (static_cast<DWORD>(writeLeft) == writeLeft) ? static_cast<DWORD>(writeLeft) : -1;
            DWORD amountWrote = 0;

            if (!WriteFile(mFileHandle,
                          _inBuffer + (_amount - writeLeft),
                           maxWrite,
                           &amountWrote,
                           wn_nullptr)) {
               return(0);
            }

            totalWritten += amountWrote;
            writeLeft -= amountWrote;
        } while (writeLeft > 0);

        return(totalWritten);
    #elif defined _WN_LINUX || defined _WN_ANDROID
        const wn_size_t amountWritten = fwrite(_inBuffer, sizeof(wn_char), _amount, mFileHandle);

        if (amountWritten != _amount) {
            return(0);
        }

        return(_amount);
    #endif
}

wn_bool WNFileSystem::WNFile::IsValid() {
    return(mIsValid);
}
