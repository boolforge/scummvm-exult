#ifndef EXULT_SCUMMVMFILEADAPTER_H
#define EXULT_SCUMMVMFILEADAPTER_H

#include "IFileSource.h"
#include "common/streams.h"
#include "ExultFileAdapter.h" // Assuming this exists for opening files

struct ScummVMFileAdapter : IFileSource {
  ScummVM::Common::SeekableReadStream* _stream;

  ScummVMFileAdapter(ScummVM::Common::SeekableReadStream* stream) : _stream(stream) {}

  size_t read(void* dst, size_t sz) override {
    return _stream->read(dst, sz);
  }

  bool seek(size_t pos) override {
    return _stream->seek(pos);
  }

  size_t size() const override {
    return _stream->size();
  }
};

#endif // EXULT_SCUMMVMFILEADAPTER_H

