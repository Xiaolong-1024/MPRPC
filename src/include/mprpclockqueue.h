//
// Created by Y2000 on 2024/12/3.
//

#ifndef MPRPCLOCKQUEUE_H
#define MPRPCLOCKQUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

// 异步线程安全队列
template<typename T>
class MPRpcLockQueue
{
public:
    // 入队
    void push(const T &value)
    {
        // 加锁, 保证线程安全, 超出生命周期自动释放
        std::unique_lock<std::mutex> lock(m_mutex);

        // 入队
        m_queue.push(value);

        // 唤醒所有等待的线程
        m_cond.notify_one();
    }

    // 出队
    T pop()
    {
        // 加锁, 保证线程安全, 超出生命周期自动释放
        std::unique_lock<std::mutex> lock(m_mutex);

        // 如果队列为空, 则等待
        while (m_queue.empty())
        {
            // 自动释放传入的锁, 阻塞当前线程, 直到被唤醒
            m_cond.wait(lock);
        }

        // 出队
        T value = m_queue.front();
        m_queue.pop();
        return value;
    }

private:
    // 日志队列
    std::queue<T> m_queue;

    // 互斥锁
    std::mutex m_mutex;

    // 条件变量
    std::condition_variable m_cond;
};

#endif //MPRPCLOCKQUEUE_H
