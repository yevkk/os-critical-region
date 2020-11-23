#include <atomic>
#include <mutex>


namespace lab::utils
{
    template<typename T>
    class Incrementable {
    public:
        explicit Incrementable(T value) noexcept :
            _value{std::move(value)}
        {}

        void increment() noexcept
        {
            ++_value;
        }

    private:
        T _value;
    };

    template<typename T>
    class Incrementable<std::atomic<T>> {
    public:
        explicit Incrementable(T value) :
                _value{std::move(value)}
        { }

        void increment()
        {
            _value.fetch_add(1, std::memory_order::memory_order_relaxed);
        }

    private:
        std::atomic<T> _value;
    };

   

}

int main()
{

    return 0;
}