#ifndef SYSTEM_MODULE_DETAIL_POSTED_EVENT_HPP
#define SYSTEM_MODULE_DETAIL_POSTED_EVENT_HPP

#include "system_module/event.hpp"
#include "system_module/object.hpp"
#include <memory>

namespace twf {
namespace detail {

class Posted_event {
   public:
    Posted_event(Object* obj, std::unique_ptr<Event> ev, int priority);
    Posted_event(const Posted_event&) = delete;
    Posted_event& operator=(const Posted_event&) = delete;
    Posted_event(Posted_event&&) noexcept = default;
    Posted_event& operator=(Posted_event&&) noexcept = default;
    ~Posted_event() = default;

    Object* reciever() { return reciever_; }
    const Object* reciever() const { return reciever_; }
    Event& event() { return *event_; }
    const Event& event() const { return *event_; }
    int priority() const { return priority_; }

   private:
    Object* reciever_;
    std::unique_ptr<Event> event_;
    int priority_;
};

bool operator<(const Posted_event& x, const Posted_event& y);
bool operator>(const Posted_event& x, const Posted_event& y);
bool operator<(int priority, const Posted_event& pe);
bool operator<(const Posted_event& pe, int priority);
bool operator>(int priority, const Posted_event& pe);
bool operator>(const Posted_event& pe, int priority);
bool operator==(const Posted_event& x, const Posted_event& y);

}  // namespace detail
}  // namespace twf
#endif  // SYSTEM_MODULE_DETAIL_POSTED_EVENT_HPP
