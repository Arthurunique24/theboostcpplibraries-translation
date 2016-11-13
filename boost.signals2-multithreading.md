# Multithreading

Almost all classes provided by Boost.Signals2 are thread safe and can be used in multithreaded applications. For example, objects of type `boost::signals2::signal` and `boost::signals2::connection` can be accessed from different threads.

On the other hand, `boost::signals2::shared_connection_block` is not thread safe. This limitation is not important because multiple objects of type `boost::signals2::shared_connection_block` can be created in different threads and can use the same connection object.

<a id="ex.signals2_17"></a>
### Example 67.17. boost::signals2::signal is thread safe

```cpp
#include <boost/signals2.hpp>
#include <thread>
#include <mutex>
#include <iostream>

boost::signals2::signal<void(int)> s;
std::mutex m;

void loop()
{
  for (int i = 0; i < 100; ++i)
    s(i);
}

int main()
{
  s.connect([](int i){
    std::lock_guard<std::mutex> lock{m};
    std::cout << i << '\n';
  });
  std::thread t1{loop};
  std::thread t2{loop};
  t1.join();
  t2.join();
}
```

[Example 67.17](#ex.signals2_17) creates two threads that execute the `loop()` function, which accesses **s** one hundred times to call the associated lambda function. Boost.Signals2 explicitly supports simultaneous access from different threads to objects of type `boost::signals2::signal`.

[Example 67.17](#ex.signals2_17) displays numbers from 0 to 99. Because **i** is incremented in two threads and written to the standard output stream in the lambda function, the numbers will not only be displayed twice, they will also overlap. However, because `boost::signals2::signal` can be accessed from different threads, the program will not crash.

However, [Example 67.17](#ex.signals2_17) still requires synchronization. Because two threads access **s**, the associated lambda function runs in parallel in two threads. To avoid having the two threads interrupt each other while writing to standard output, a mutex is used to synchronize access to **std::cout**.

For single-threaded applications, support for multithreading can be disabled in Boost.Signals2.

<a id="ex.signals2_18"></a>
### Example 67.18. boost::signals2::signal without thread safety

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

signal<void()> s;

int main()
{
  typedef keywords::mutex_type<dummy_mutex> dummy_mutex;
  signal_type<void(), dummy_mutex>::type s;
  s.connect([]{ std::cout << "Hello, world!\n"; });
  s();
}
```

Out of the many template parameters supported by `boost::signals2::signal`, the last one defines the type of mutex used for synchronization. Fortunately, Boost.Signals2 offers a simpler way to disable synchronization than passing the complete list of parameters.

The `boost::signals2::keywords` namespace contains classes that make it possible to pass template parameters by name. `boost::signals2::keywords::mutex_type` can be used to pass the mutex type as the second template parameter to `boost::signals2::signal_type`. Please note that `boost::signals2::signal_type`, not `boost::signals2::signal`, must be used in this case. The type equivalent to `boost::signals2::signal`, which is required to define the signal **s**, is retrieved via `boost::signals2::signal_type::type`.

Boost.Signals2 provides an empty mutex implementation called `boost::signals2::dummy_mutex`. If a signal is defined with this class, it will no longer support multithreading (see [Example 67.18](#ex.signals2_18)).

[Prev](boost.signals2-connections.md) | [Next](boost.msm.md)
--- | ---
