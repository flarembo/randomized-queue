#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

struct random_struct
{
    random_struct()
        : m_rand_engine(std::random_device{}()) // construct random device and call to get seed for random engine construction
    {
    }
    std::size_t get_rand(std::size_t from, std::size_t to) const
    {
        std::uniform_int_distribution distribution(from, to); // distribution construction is quite cheap
        return distribution(m_rand_engine);
    }

private:
    mutable std::mt19937 m_rand_engine;
};

template <class T>
class randomized_queue
{
private:
    std::vector<T> m_queue;
    random_struct random = random_struct();

public:
    randomized_queue();

    bool empty() const;

    std::size_t size() const;

    void enqueue(T add);

    const T & sample() const;

    T dequeue();

    template <bool isConst>
    struct Iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using vector_type = std::conditional_t<isConst, const std::vector<T>, std::vector<T>>;
        using value_type = std::conditional_t<isConst, const T, T>;
        using pointer = value_type *;
        using reference = value_type &;

        Iterator() = default;

        friend bool operator==(const Iterator & lhs, const Iterator & rhs)
        {
            return (lhs.queue == nullptr && rhs.queue == nullptr) ||
                    (lhs.take == rhs.take && lhs.queue != nullptr && rhs.queue != nullptr && lhs.queue->begin() == rhs.queue->begin());
        }

        friend bool operator!=(const Iterator & lhs, const Iterator & rhs)
        {
            return !(lhs == rhs);
        }

        reference operator*() const
        {
            return (*queue)[shuffle[take]];
        }

        pointer operator->() const
        {
            return &operator*();
        }

        Iterator & operator++()
        {
            take++;
            return *this;
        }

        Iterator operator++(int)
        {
            auto tmp = *this;
            operator++();
            return tmp;
        }

        Iterator & operator--()
        {
            take--;
            return *this;
        }

        Iterator operator--(int)
        {
            auto tmp = *this;
            operator--();
            return tmp;
        }

    private:
        friend class randomized_queue<T>;

        std::size_t take{0};
        vector_type * queue{nullptr};
        std::vector<std::size_t> shuffle;
        random_struct random;

        void generate_shuffle();

        Iterator(vector_type * queue, std::size_t take);
    };

public:
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
};

template <class T>
template <bool isConst>
randomized_queue<T>::Iterator<isConst>::Iterator(randomized_queue<T>::Iterator<isConst>::vector_type * queue, std::size_t take)
    : take(take)
    , queue(queue)
{
    generate_shuffle();
}

template <class T>
template <bool isConst>
void randomized_queue<T>::Iterator<isConst>::generate_shuffle()
{
    shuffle.resize(queue->size());
    for (std::size_t i = 0; i < queue->size(); i++) {
        shuffle[i] = i;
    }

    for (std::size_t i = queue->size(); i > 0; i--) {
        std::size_t j = random.get_rand(0, i - 1);
        std::swap(shuffle[i - 1], shuffle[j]);
    }
}

template <class T>
T randomized_queue<T>::dequeue()
{
    auto take = random.get_rand(0, m_queue.size() - 1);
    std::swap(m_queue[take], m_queue[m_queue.size() - 1]);
    T ans = std::move(m_queue[m_queue.size() - 1]);
    m_queue.pop_back();
    return ans;
}

template <class T>
const T & randomized_queue<T>::sample() const
{
    return m_queue[random.get_rand(0, m_queue.size() - 1)];
}

template <class T>
void randomized_queue<T>::enqueue(T add)
{
    m_queue.push_back(std::forward<T>(add));
}

template <class T>
std::size_t randomized_queue<T>::size() const
{
    return m_queue.size();
}

template <class T>
bool randomized_queue<T>::empty() const
{
    return m_queue.empty();
}

template <class T>
randomized_queue<T>::randomized_queue()
{
}

template <class T>
typename randomized_queue<T>::const_iterator randomized_queue<T>::cbegin() const { return {&m_queue, 0}; }

template <class T>
typename randomized_queue<T>::const_iterator randomized_queue<T>::cend() const { return {&m_queue, m_queue.size()}; }

template <class T>
typename randomized_queue<T>::const_iterator randomized_queue<T>::end() const
{
    return cend();
}

template <class T>
typename randomized_queue<T>::const_iterator randomized_queue<T>::begin() const
{
    return cbegin();
}

template <class T>
typename randomized_queue<T>::iterator randomized_queue<T>::end() { return {&m_queue, m_queue.size()}; }

template <class T>
typename randomized_queue<T>::iterator randomized_queue<T>::begin() { return {&m_queue, 0}; }
