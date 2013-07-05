#include "kcwnotifier.h"

KcwNotifier::KcwNotifier() {
}

KcwNotifier::KcwNotifier(const std::wstring& strName, bool create) {
}

KcwNotifier::~KcwNotifier() {
}

int KcwNotifier::open(const std::wstring& strName) {
    return 0;
}

void KcwNotifier::close() {
}

void KcwNotifier::notify() {
}

HANDLE KcwNotifier::handle() {
    return m_event;
}
