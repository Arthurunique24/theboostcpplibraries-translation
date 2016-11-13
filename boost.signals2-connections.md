# Connections

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

[Prev](boost.signals2-signals.md) | [Next](boost.signals2-multithreading.md)
--- | ---
