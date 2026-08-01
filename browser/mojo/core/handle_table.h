#ifndef MOJO_CORE_HANDLE_TABLE_H_
#define MOJO_CORE_HANDLE_TABLE_H_

#include "mojo/core/dispatcher.h"
#include <map>
#include <memory>


namespace mojo {
namespace core {

class HandleTable {
public:
  MojoHandle AddDispatcher(std::shared_ptr<Dispatcher> dispatcher);
  MojoResult GetAndRemoveDispatcher(MojoHandle,
                                    std::shared_ptr<Dispatcher>* dispatcher);

  struct Entry {
    explicit Entry(std::shared_ptr<Dispatcher> dispatcher);
    ~Entry();
    Entry(const Entry &entry);
    Entry &operator=(const Entry &) = delete;

    const std::shared_ptr<Dispatcher> dispatcher;
    bool busy = false;
  };
  class EntriesAccessor {
  public:
    EntriesAccessor();
    ~EntriesAccessor();

    // Returns whether an Entry was inserted.
    bool Add(MojoHandle handle, Entry entry);

    enum RemovalCondition { kRemoveOnlyIfBusy, kRemoveOnlyIfNotBusy };
    MojoResult Remove(MojoHandle handle,
                      RemovalCondition removal_condition,
                      std::shared_ptr<Dispatcher>* dispatcher);

  private:
    std::map<MojoHandle, Entry> handles_;
    std::shared_ptr<Dispatcher> last_read_dispatcher_;
    MojoHandle last_read_handle_ = MOJO_HANDLE_INVALID;
  };

private:
  EntriesAccessor entries_;
  uintptr_t next_available_handle_ = 1;
};

} // namespace core
} // namespace mojo

#endif