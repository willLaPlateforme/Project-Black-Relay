#pragma once
#include <vector>
#include <string>
#include <algorithm>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onEvent(const std::string& event, int value) = 0;
};

class IObservable {
public:
    virtual ~IObservable() = default;

    void addObserver(IObserver* obs) {
        _observers.push_back(obs);
    }
    void removeObserver(IObserver* obs) {
        _observers.erase(
            std::remove(_observers.begin(), _observers.end(), obs),
            _observers.end());
    }

protected:
    void notify(const std::string& event, int value = 0) {
        for (auto* o : _observers) o->onEvent(event, value);
    }

private:
    std::vector<IObserver*> _observers;
};
