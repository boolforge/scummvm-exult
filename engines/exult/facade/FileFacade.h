#ifndef EXULT_FILEFACADE_H
#define EXULT_FILEFACADE_H

#include "IFileSource.h"
// Assuming Exult's file handling logic is in extern/files
// #include "extern/files/ExultFileManager.h"

struct FileFacade {
  IFileSource* _fileSource;

  FileFacade(IFileSource* fileSource) : _fileSource(fileSource) {}

  size_t readFile(void* dst, size_t sz) {
    return _fileSource->read(dst, sz);
  }

  bool seekFile(size_t pos) {
    return _fileSource->seek(pos);
  }

  size_t getFileSize() const {
    return _fileSource->size();
  }
};

#endif // EXULT_FILEFACADE_H

