#include "kcwnotifier.h"

KcwNotifier::KcwNotifier()
: m_event(NULL) {
}

KcwNotifier::KcwNotifier(const std::wstring& strName)
: m_event(NULL) {
}

KcwNotifier::~KcwNotifier() {
}

int KcwNotifier::open() {
    return -1;
}

int KcwNotifier::open(const std::wstring& strName) {
    return -1;
}

bool KcwNotifier::opened() const {
    return m_event != NULL;
}

void KcwNotifier::close() {
}

void KcwNotifier::notify() {
}

HANDLE KcwNotifier::handle() {
    return m_event;
}
