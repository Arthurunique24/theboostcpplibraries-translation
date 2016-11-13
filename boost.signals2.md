# Chapter 67. Boost.Signals2

**Table of Contents**
* [Signals](#signals)
* [Connections](#connections)
* [Multithreading](#multithreading)

[Boost.Signals2](http://www.boost.org/libs/signals2) implements the signal/slot concept. One or multiple functions – called *slots* – are linked with an object that can emit a signal. Every time the signal is emitted, the linked functions are called.

The signal/slot concept can be useful when, for example, developing applications with graphical user interfaces. Buttons can be modelled so they emit a signal when a user clicks on them. They can support links to many functions to handle user input. That way it is possible to process events flexibly.

`std::function` can also be used for event handling. One crucial difference between `std::function` and Boost.Signals2, is that Boost.Signals2 can associate more than one event handler with a single event. Therefore, Boost.Signals2 is better for supporting event-driven development and should be the first choice whenever events need to be handled.

Boost.Signals2 succeeds the library Boost.Signals, which is deprecated and not discussed in this book.

<a id="signals"></a>
## Signals

Boost.Signals2 offers the class `boost::signals2::signal`, which can be used to create a signal. This class is defined in `boost/signals2/signal.hpp`. Alternatively, you can use the header file `boost/signals2.hpp`, which is a master header file that defines all of the classes and functions available in Boost.Signals2.

Boost.Signals2 defines `boost::signals2::signal` and other classes, as well as all functions in the namespace `boost::signals2`.

<a id="ex.signals2_01"></a>
### Example 67.1. “Hello, world!” with boost::signals2::signal

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

int main()
{
  signal<void()> s;
  s.connect([]{ std::cout << "Hello, world!\n"; });
  s();
}
```

`boost::signals2::signal` is a class template that expects as a template parameter the signature of the function that will be used as an event handler. In [Example 67.1](#ex.signals2_01), only functions with a signature of `void()` can be associated with the signal **s**.

A lambda function is associated with the signal **s** through `connect()`. Because the lambda function conforms to the required signature, `void()`, the association is successfully established. The lambda function is called whenever the signal **s** is triggered.

The signal is triggered by calling **s** like a regular function. The signature of this function matches the one passed as the template parameter. The brackets are empty because `void()` does not expect any parameters. Calling **s** results in a trigger that, in turn, executes the lambda function that was previously associated with `connect()`.

[Example 67.1](#ex.signals2_01) can also be implemented with `std::function`, as shown in [Example 67.2](#ex.signals2_02).

<a id="ex.signals2_02"></a>
### Example 67.2. “Hello, world!” with std::function

```cpp
#include <functional>
#include <iostream>

int main()
{
  std::function<void()> f;
  f = []{ std::cout << "Hello, world!\n"; };
  f();
}
```

In [Example 67.2](#ex.signals2_02), the lambda function is also executed when **f** is called. While `std::function` can only be used in a scenario like [Example 67.2](#ex.signals2_02), Boost.Signals2 provides far more variety. For example, it can associate multiple functions with a particular signal (see [Example 67.3](#ex.signals2_03)).

<a id="ex.signals2_03"></a>
### Example 67.3. Multiple event handlers with boost::signals2::signal

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

int main()
{
  signal<void()> s;
  s.connect([]{ std::cout << "Hello"; });
  s.connect([]{ std::cout << ", world!\n"; });
  s();
}
```

`boost::signals2::signal` allows you to assign multiple functions to a particular signal by calling `connect()` repeatedly. Whenever the signal is triggered, the functions are executed in the order in which they were associated with `connect()`.

The order can also be explicitly defined with the help of an overloaded version of `connect()`, which expects a value of type `int` as an additional parameter ([Example 67.4](#ex.signals2_04)).

<a id="ex.signals2_04"></a>
### Example 67.4. Event handlers with an explicit order

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

int main()
{
  signal<void()> s;
  s.connect(1, []{ std::cout << ", world!\n"; });
  s.connect(0, []{ std::cout << "Hello"; });
  s();
}
```

Like the previous example, [Example 67.4](#ex.signals2_04) displays `Hello, world!`.

To release an associated function from a signal, call `disconnect()`.

<a id="ex.signals2_05"></a>
### Example 67.5. Disconnecting event handlers from boost::signals2::signal

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

void hello() { std::cout << "Hello"; }
void world() { std::cout << ", world!\n"; }

int main()
{
  signal<void()> s;
  s.connect(hello);
  s.connect(world);
  s.disconnect(world);
  s();
}
```

[Example 67.5](#ex.signals2_05) only prints `Hello` because the association with `world()` was released before the signal was triggered.

Besides `connect()` and `disconnect()`, `boost::signals2::signal` provides a few more member functions (see [Example 67.6](#ex.signals2_06)).

<a id="ex.signals2_06"></a>
### Example 67.6. Additional member functions of boost::signals2::signal

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

int main()
{
  signal<void()> s;
  s.connect([]{ std::cout << "Hello"; });
  s.connect([]{ std::cout << ", world!"; });
  std::cout << s.num_slots() << '\n';
  if (!s.empty())
    s();
  s.disconnect_all_slots();
}
```

`num_slots()` returns the number of associated functions. If no function is associated, `num_slots()` returns 0. `empty()` tells you whether event handlers are connected or not. And `disconnect_all_slots()` does exactly what its name says: it releases all existing associations.

<a id="ex.signals2_07"></a>
### Example 67.7. Processing return values from event handlers

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

int main()
{
  signal<int()> s;
  s.connect([]{ return 1; });
  s.connect([]{ return 2; });
  std::cout << *s() << '\n';
}
```

In [Example 67.7](#ex.signals2_07) two lambda functions are associated with the signal **s**. The first lambda function returns 1, the second returns 2.

[Example 67.7](#ex.signals2_07) writes `2` to standard output. Both return values were correctly accepted by **s**, but all except the last one were ignored. By default, only the last return value of all associated functions is returned.

Please note that `s()` does not directly return the result of the last function called. An object of type `boost::optional` is returned, which when de-referenced returns the number 2. Triggering a signal that is not associated with any function does not yield any return value. Thus, in this case, `boost::optional` allows Boost.Signals2 to return an empty object. `boost::optional` is introduced in [Chapter 21](boost.optional.md).

It is possible to customize a signal so that the individual return values are processed accordingly. To do this, a *combiner* must be passed to `boost::signals2::signal` as a second template parameter.

<a id="ex.signals2_08"></a>
### Example 67.8. Finding the smallest return value with a user-defined combiner

```cpp
#include <boost/signals2.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace boost::signals2;

template <typename T>
struct min_element
{
  typedef T result_type;

  template <typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const
  {
    std::vector<T> v(first, last);
    return *std::min_element(v.begin(), v.end());
  }
};

int main()
{
  signal<int(), min_element<int>> s;
  s.connect([]{ return 1; });
  s.connect([]{ return 2; });
  std::cout << s() << '\n';
}
```

A combiner is a class with an overloaded `operator()`. This operator is automatically called with two iterators, which are used to access the functions associated with the particular signal. When the iterators are de-referenced, the functions are called and their return values become available in the combiner. A common algorithm from the standard library, such as `std::min_element()`, can then be used to calculate and return the smallest value (see [Example 67.8](#ex.signals2_08)).

`boost::signals2::signal` uses `boost::signals2::optional_last_value` as the default combiner. This combiner returns objects of type `boost::optional`. A user can define a combiner with a return value of any type. For instance, the combiner `min_element` in [Example 67.8](#ex.signals2_08) returns the type passed as a template parameter to `min_element`.

It is not possible to pass an algorithm such as `std::min_element()` as a template parameter directly to `boost::signals2::signal`. `boost::signals2::signal` expects that the combiner defines a type called `result_type`, which denotes the type of the value returned by `operator()`. Since this type is not defined by standard algorithms, the compiler will report an error.

Please note that it is not possible to pass the iterators **first** and **last** directly to `std::min_element()` because this algorithm expects forward iterators, while combiners work with input iterators. That’s why a vector is used to store all return values before determining the smallest value with `std::min_element()`.

[Example 67.9](#ex.signals2_09) modifies the combiner to store all return values in a container, rather than evaluating them. It stores all the return values in a vector that is then returned by `s()`.

<a id="connections"></a>
## Connections

Functions can be managed with the aid of the `connect()` and `disconnect()` member functions provided by `boost::signals2::signal`. Because `connect()` returns an object of type `boost::signals2::connection`, associations can also be managed differently (see [Example 67.10](#ex.signals2_10)).

<a id="ex.signals2_10"></a>
### Example 67.10. Managing connections with boost::signals2::connection

```cpp
#include <boost/signals2.hpp>
#include <iostream>

int main()
{
  boost::signals2::signal<void()> s;
  boost::signals2::connection c = s.connect(
    []{ std::cout << "Hello, world!\n"; });
  s();
  c.disconnect();
}
```

The `disconnect()` member function of `boost::signals2::signal` requires a function pointer to be passed in. Avoid this by calling `disconnect()` on the `boost::signals2::connection` object.

To block a function for a short time without removing the association from the signal, `boost::signals2::shared_connection_block` can be used.

<a id="ex.signals2_11"></a>
### Example 67.11. Blocking connections with shared_connection_block

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

int main()
{
  signal<void()> s;
  connection c = s.connect([]{ std::cout << "Hello, world!\n"; });
  s();
  shared_connection_block b{c};
  s();
  b.unblock();
  s();
}
```

[Example 67.11](#ex.signals2_11) executes the lambda function twice. The signal **s** is triggered three times, but the lambda function is not called the second time because an object of type `boost::signals2::shared_connection_block` was created to block the call. Once the object goes out of scope, the block is automatically removed. A block can also be removed explicitly by calling `unblock()`. Because it is called before the last trigger, the final call to the lambda function is executed again.

In addition to `unblock()`, `boost::signals2::shared_connection_block` provides the member functions `block()` and `blocking()`. The former is used to block a connection after a call to `unblock()`, while the latter makes it possible to check whether or not a connection is currently blocked.

Please note that `boost::signals2::shared_connection_block` carries the word “shared” for a reason: multiple objects of type `boost::signals2::shared_connection_block` can be initialized with the same connection.

<a id="ex.signals2_12"></a>
### Example 67.12. Blocking a connection multiple times

```cpp
#include <boost/signals2.hpp>
#include <iostream>

using namespace boost::signals2;

int main()
{
  signal<void()> s;
  connection c = s.connect([]{ std::cout << "Hello, world!\n"; });
  shared_connection_block b1{c, false};
  {
    shared_connection_block b2{c};
    std::cout << std::boolalpha << b1.blocking() << '\n';
    s();
  }
  s();
}
```

[Example 67.12](#ex.signals2_12) accesses **s** twice, but the lambda function is only called the second time. The program writes `Hello, world!` to the standard output stream only once.

Because `false` is passed to the constructor as the second parameter, the first object of type `boost::signals2::shared_connection_block` does not block the connection to the signal **s**. Hence, calling `blocking()` on the object **b1** returns `false`.

Nevertheless, the lambda function is not executed when **s** is first accessed because the access happens only after a second object of type `boost::signals2::shared_connection_block` has been instantiated. By not passing a second parameter to the constructor, the connection is blocked by the object. When **s** is accessed for the second time, the lambda function is executed because the block was automatically removed once **b2** went out of scope.

Boost.Signals2 can release a connection once the object whose member function is associated with a signal is destroyed.

<a id="ex.signals2_13"></a>
### Example 67.13. Member functions as event handlers

```cpp
#include <boost/signals2.hpp>
#include <memory>
#include <functional>
#include <iostream>

using namespace boost::signals2;

struct world
{
  void hello() const
  {
    std::cout << "Hello, world!\n";
  }
};

int main()
{
  signal<void()> s;
  {
    std::unique_ptr<world> w(new world());
    s.connect(std::bind(&world::hello, w.get()));
  }
  std::cout << s.num_slots() << '\n';
  s();
}
```

[Example 67.13](#ex.signals2_13) associates the member function of an object with a signal, with the help of `std::bind()`. The object is destroyed before the signal is triggered, which is a problem because, instead of passing an object of type `world`, only an address was passed to `std::bind()`. By the time `s()` is called, the object referenced no longer exists.

It is possible to modify the program so that the connection is automatically released once the object is destroyed. [Example 67.14](#ex.signals2_14) does this.

<a id="ex.signals2_14"></a>
### Example 67.14. Releasing associated member functions automatically

```cpp
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace boost::signals2;

struct world
{
  void hello() const
  {
    std::cout << "Hello, world!\n";
  }
};

int main()
{
  signal<void()> s;
  {
    boost::shared_ptr<world> w(new world());
    s.connect(signal<void()>::slot_type(&world::hello, w.get()).track(w));
  }
  std::cout << s.num_slots() << '\n';
  s();
}
```

Now `num_slots()` returns `0`. [Example 67.14](#ex.signals2_14) does not try to call a member function on an object that doesn’t exist when the signal is triggered. The change was to tie the object of type `world` to a smart pointer of type `boost::shared_ptr`, which is passed to `track()`. This member function is called on the slot that was passed to `connect()` to request tracking on the corresponding object.

A function or member function associated with a signal is called a *slot*. The type to specify a slot was not used in the previous examples because passing a pointer to a function or member function to `connect()` was sufficient. The corresponding slot was created and associated with the signal automatically.

In [Example 67.14](#ex.signals2_14), however, the smart pointer is associated with the slot by calling `track()`. Because the type of the slot depends on the signal, `boost::signals2::signal` provides a type `slot_type` to access the required type. `slot_type` behaves just like `std::bind`, making it possible to pass both parameters to describe the slot directly. `track()` can then be called to associate the slot with a smart pointer of type `boost::shared_ptr`. The object is then tracked, which causes the slot to be automatically removed once the tracked object is destroyed.

To manage objects with different smart pointers, slots provide a member function called `track_foreign()`. While `track()` expects a smart pointer of type `boost::shared_ptr`, `track_foreign()` allows you to, for example, use a smart pointer of type `std::shared_ptr`. Smart pointers other than `std::shared_ptr` and `std::weak_ptr` must be introduced to Boost.Signals2 before they can be passed to `track_foreign()`.

The consumer of a particular event can access an object of type `boost::signals2::signal` to create new associations or release existing ones.

<a id="ex.signals2_15"></a>
### Example 67.15. Creating new connections in an event handler

```cpp
#include <boost/signals2.hpp>
#include <iostream>

boost::signals2::signal<void()> s;

void connect()
{
  s.connect([]{ std::cout << "Hello, world!\n"; });
}

int main()
{
  s.connect(connect);
  s();
}
```

[Example 67.15](#ex.signals2_15) accesses **s** inside the `connect()` function to associate a lambda function with the signal. Since `connect()` is called when the signal is triggered, the question is whether the lambda function will also be called.

The program does not output anything, which means the lambda function is never called. While Boost.Signals2 supports associating functions to signals when a signal is triggered, the new associations will only be used when the signal is triggered again.

<a id="ex.signals2_16"></a>
### Example 67.16. Releasing connections in an event handler

```cpp
#include <boost/signals2.hpp>
#include <iostream>

boost::signals2::signal<void()> s;

void hello()
{
  std::cout << "Hello, world!\n";
}

void disconnect()
{
  s.disconnect(hello);
}

int main()
{
  s.connect(disconnect);
  s.connect(hello);
  s();
}
```

[Example 67.16](#ex.signals2_16) does not create a new association, instead it releases an existing one. As in [Example 67.15](#ex.signals2_15), this example writes nothing to the standard output stream.

This behavior can be explained quite simply. Imagine that a temporary copy of all slots is created whenever a signal is triggered. Newly created associations are not added to the temporary copy and therefore can only be called the next time the signal is triggered. Released associations, on the other hand, are still part of the temporary copy, but will be checked by the combiner when de-referenced to avoid calling a member function on an object that has already been destroyed.

<a id="ex.signals2_09"></a>
### Example 67.9. Receiving all return values with a user-defined combiner

```cpp
#include <boost/signals2.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace boost::signals2;

template <typename T>
struct return_all
{
  typedef T result_type;

  template <typename InputIterator>
  T operator()(InputIterator first, InputIterator last) const
  {
    return T(first, last);
  }
};

int main()
{
  signal<int(), return_all<std::vector<int>>> s;
  s.connect([]{ return 1; });
  s.connect([]{ return 2; });
  std::vector<int> v = s();
  std::cout << *std::min_element(v.begin(), v.end()) << '\n';
}
```

<a id="multithreading"></a>
## Multithreading

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
