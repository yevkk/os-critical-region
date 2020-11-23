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

    template<typename P, typename V>
    class Lockable {
    public:
        class Proxy {
        public:
            explicit Proxy (P& primitive, V& value) :
                    _lock{primitive},
                    _value{value}
            { }

            [[nodiscard]]
            auto value () -> V&
            {
                return _value;
            }

        private:
            std::lock_guard<P> _lock;
            V _value;
        };

        template<typename... Args>
        explicit Lockable(Args&&...args) :
                _value{std::forward<Args...>(args...)}
        { }

        [[nodiscard]]
        auto lock() -> Proxy
        {
            return Proxy{_primitive, _value};
        }

    private:
        V _value;
        P _primitive;
    };

}

int main()
{


    return 0;
}