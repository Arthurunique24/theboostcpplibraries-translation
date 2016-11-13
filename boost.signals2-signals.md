# Signals

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

[Prev](boost.signals2.md) | [Next](boost.signals2-connections.md)
--- | ---
