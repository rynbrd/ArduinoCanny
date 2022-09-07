#ifndef _CANNY_QUEUE_H_
#define _CANNY_QUEUE_H_

namespace Canny {

// A fixed-size queue for buffering frames.
template <typename Frame>
class Queue {
    public:
        // Default constructor. Creates a queue with no capacity. All calls
        // to the queue fail.
        Queue();

        // Construct a queue with the given capacity. If frame_reserve_capacity
        // is >0 then the frame allocated for the queue have data capacity
        // reserved for that size.
        Queue(size_t capacity, size_t frame_reserve_capacity = 0);

        // Destroy the queue.
        ~Queue();

        // Return a pointer to the storage location of the next slot in the
        // queue. This should be set to a value and then enqueued via the
        // regular enqueue function. This allow the queue's buffer to be
        // written to directly, thus avoiding allocating an extra frame where
        // not necessary. Return nullptr if the queue is full. The queue
        // retains ownership of the pointer. The pointer is invalid after any
        // additional method call.
        Frame* alloc();

        // Enqueue a copy of a frame. Return true on success or false if the
        // queue is full.
        bool enqueue(const Frame& frame);

        // Dequeue a frame and return a pointer ot it. The queue retains
        // ownership of the pointer. The pointer is valid until a call to
        // enqueue, dequeue, or the destructor is made. Return nullptr if the
        // queue is empty.
        Frame* dequeue();

        // Return a pointer to the next frame in the queue. The queue retains
        // ownership of the pointer. The pointer is valid until a call to
        // enqueue, dequeue, or the destructor is made. Return nullptr if the
        // queue is empty.
        Frame* peek() const;

        // Return the number of items in the queue.
        size_t size() const { return size_; }

        // Return the capacity of the queue.
        size_t capacity() const { return capacity_; }

        // Return true if the queue is empty;
        bool empty() const { return size_ == 0; }

        // Return true if the queue is full.
        bool full() const { return size_ >= capacity_; }

    private:
        Frame* buffer_;
        size_t capacity_;
        size_t size_;
        size_t front_;
};

}  // namespace Canny

#include "Queue.tpp"
#endif  // _CANNY_QUEUE_H_
