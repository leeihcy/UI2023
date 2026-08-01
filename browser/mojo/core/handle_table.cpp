#include "mojo/core/handle_table.h"

namespace mojo {
namespace core {


MojoHandle HandleTable::AddDispatcher(std::shared_ptr<Dispatcher> dispatcher) {
  // Oops, we're out of handles.
  if (next_available_handle_ == MOJO_HANDLE_INVALID) {
    return MOJO_HANDLE_INVALID;
  }

  MojoHandle handle = next_available_handle_++;
  const bool inserted = entries_.Add(handle, Entry(std::move(dispatcher)));

  return handle;
}

std::shared_ptr<Dispatcher> HandleTable::GetDispatcher(MojoHandle handle) {
  const std::shared_ptr<Dispatcher>* dispatcher = entries_.GetDispatcher(handle);
  return dispatcher == nullptr ? nullptr : *dispatcher;
}

MojoResult HandleTable::GetAndRemoveDispatcher(
    MojoHandle handle,
    std::shared_ptr<Dispatcher>* dispatcher) {
  std::shared_ptr<Dispatcher> removed_dispatcher;
  const MojoResult remove_result = entries_.Remove(
      handle, EntriesAccessor::RemovalCondition::kRemoveOnlyIfNotBusy,
      &removed_dispatcher);
  if (remove_result == MOJO_RESULT_NOT_FOUND) {
    return MOJO_RESULT_INVALID_ARGUMENT;
  }
  if (remove_result == MOJO_RESULT_BUSY) {
    return MOJO_RESULT_BUSY;
  }
  *dispatcher = std::move(removed_dispatcher);
  return MOJO_RESULT_OK;
}


bool HandleTable::EntriesAccessor::Add(const MojoHandle handle, Entry entry) {
  return handles_.emplace(handle, std::move(entry)).second;
}

const std::shared_ptr<Dispatcher>* HandleTable::EntriesAccessor::GetDispatcher(
    const MojoHandle handle) {
  if (last_read_handle_ != MOJO_HANDLE_INVALID && last_read_handle_ == handle) {
    return &last_read_dispatcher_;
  }
  const auto iter = handles_.find(handle);
  if (iter == handles_.end()) {
    return nullptr;
  }
  last_read_handle_ = handle;
  last_read_dispatcher_ = iter->second.dispatcher;
  return &last_read_dispatcher_;
}

MojoResult HandleTable::EntriesAccessor::Remove(
    const MojoHandle handle,
    const HandleTable::EntriesAccessor::RemovalCondition removal_condition,
    std::shared_ptr<Dispatcher>* dispatcher) {
  auto iter = handles_.find(handle);
  if (iter == handles_.end()) {
    return MOJO_RESULT_NOT_FOUND;
  }
  const bool is_busy = iter->second.busy;
  const bool remove_only_if_busy =
      removal_condition == RemovalCondition::kRemoveOnlyIfBusy;
  if (remove_only_if_busy == is_busy) {
    if (dispatcher != nullptr) {
      *dispatcher = iter->second.dispatcher;
    }
    if (iter->first == last_read_handle_) {
      last_read_handle_ = MOJO_HANDLE_INVALID;
      last_read_dispatcher_.reset();
    }
    handles_.erase(iter);
  }
  return is_busy ? MOJO_RESULT_BUSY : MOJO_RESULT_OK;
}


HandleTable::EntriesAccessor::EntriesAccessor() = default;
HandleTable::EntriesAccessor::~EntriesAccessor() = default;

HandleTable::Entry::Entry(std::shared_ptr<Dispatcher> dispatcher)
    : dispatcher(std::move(dispatcher)) {}

HandleTable::Entry::~Entry() = default;
HandleTable::Entry::Entry(const Entry& entry) = default;


}
}