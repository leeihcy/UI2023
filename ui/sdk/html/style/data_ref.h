#ifndef _HTML_STYLE_DATA_REF_H_
#define _HTML_STYLE_DATA_REF_H_

#include <memory>

/*
这个 Access 函数是 Chromium 中实现写时复制（Copy-on-Write, COW） 的关键方法。
这个函数用于安全地获取数据的可变访问权限，同时确保数据共享的安全性。

工作原理
检查引用计数：

if (!data_->HasOneRef())
HasOneRef() 检查当前数据是否只有一个引用
如果只有当前对象引用数据，说明是安全的直接修改
如果有多个引用，则不能直接修改，否则会影响其他对象

*/

namespace html {


template <typename T>
class DataRef {
  // USING_FAST_MALLOC(DataRef);

 public:
  const T* Get() const { return data_.get(); }

  const T& operator*() const { return *Get(); }
  const T* operator->() const { return Get(); }

  T* Access() {
    // if (!data_->HasOneRef())
    if (data_.use_count() > 1)
      data_ = data_->Copy();
    return data_.get();
  }

  void Init() {
    // DCHECK(!data_);
    data_ = T::Create();
  }

  bool operator==(const DataRef<T>& o) const {
    // DCHECK(data_);
    // DCHECK(o.data_);
    return data_ == o.data_ || *data_ == *o.data_;
  }

  bool operator!=(const DataRef<T>& o) const {
    // DCHECK(data_);
    // DCHECK(o.data_);
    return data_ != o.data_ && *data_ != *o.data_;
  }

  void operator=(std::nullptr_t) { data_ = nullptr; }

 private:
  std::shared_ptr<T> data_;
};

}

#endif