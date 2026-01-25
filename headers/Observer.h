#ifndef OOP_OBSERVER_H
#define OOP_OBSERVER_H


class Observer {
public:
    virtual ~Observer() = default;
    virtual void update() = 0;
};


#endif //OOP_OBSERVER_H