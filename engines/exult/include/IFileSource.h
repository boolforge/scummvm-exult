#ifndef EXULT_IFILESOURCE_H
#define EXULT_IFILESOURCE_H
#include <cstddef>
struct IFileSource {
  virtual ~IFileSource() = default;
  virtual size_t read(void* dst, size_t sz)=0;
  virtual bool seek(size_t pos)=0;
  virtual size_t size() const=0;
};
#endif

