#ifndef OOP_OBSERVER_H
#define OOP_OBSERVER_H


/**
 * @class Observer
 * @brief Interface for the Observer pattern.
 */
class Observer {
public:
    virtual ~Observer() = default;

    /**
     * @brief Called when the observed subject changes.
     */
    virtual void update() = 0;
};


#endif //OOP_OBSERVER_H