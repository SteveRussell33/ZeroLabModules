/*
 * This is a very simple circular buffer meant specifically
 * as a delay, hence the simple interface. It has no notion of
 * how full it is and provides no protections against invalid 
 * addressing. 
 */

/*
 When writing this and testing it I had to continually remind
 myself of the invariant:
 - except during push() and clear(), _head *always* points to the D[0]
 */

#ifndef DELAY_BUFFER
#define DELAY_BUFFER
#include <cstring>

template<typename T>
class DelayBuffer {

    T* _buf;
    size_t _head;
    const size_t MAX_SIZE;

    DelayBuffer() { ; }

public:
    DelayBuffer(size_t max_size) :
        _buf(new T[max_size]),
        _head(max_size-1),
        MAX_SIZE(max_size)
    {}

    void push(T t) {
        // increment head, wrap if necessary
        if(++_head >= MAX_SIZE)
            _head = 0;

        _buf[_head] = t;
    }

    T read(int delay=-1) {
        if(delay == -1)
            delay = MAX_SIZE;

        return _buf[mask(delay)];
    }

    // this is the old usage and is deprecated
    // clean it up and switch to the new write() below
    // void write(time_t delay, T t, float new_mix=0) {
    //     int idx = mask(delay);
    //     if(new_mix <= 0) 
    //         _buf[idx] += t;
    //     else
    //         _buf[idx] = (1-new_mix)*_buf[idx] + new_mix*t;
    // }

    // overwrite a delay-line entry
    void write(time_t delay, T t) {
        _buf[mask(delay)] = t;
    }

    // add to a delay-line entry
    void add(time_t delay, T t) {
        _buf[mask(delay)] += t;
    }
    
    // mix new value into a delay-line entry
    void mix(time_t delay, T t, float mix=0.5f) {
        int idx = mask(delay);
        _buf[idx] = (mix * t) + ((1-mix) * _buf[idx]);    
    }

    void clear() {
        memset(_buf, 0, MAX_SIZE*sizeof(T));
        _head = MAX_SIZE-1; // this isn't *necessary* but does make it "just like brand new"
    }

    size_t mask(size_t t) {
        int i = _head - t;
        if(i < 0)
            i+= MAX_SIZE;
        return i;
    }
    // purely for debugging
    void dump() {
        std::cout << "CB2 Dump" << std::endl;
        std::cout << "\tbuffer size: " << MAX_SIZE << std::endl <<
            "\thead: " << _head << std::endl << "\telements: [";
        for(size_t i=0; i<MAX_SIZE; i++) {
            std::cout << _buf[i] << ",";
        }
        std::cout << "]" << std::endl;
    }
    

};


#endif