[chapter21]: https://theboostcpplibraries.com/boost.optional
# Глава 67. Boost.Signals2

**Содержание**
* [Сигналы](#signals)
* [Соединения](#connections)
* [Многопоточность](#multithreading)

[Boost.Signals2](http://www.boost.org/libs/signals2) реализует концепцию "сигнал/слот". Одна или несколько функций под названием *слоты* связаны с объектом, который может "посылать" сигнал. Каждый раз, когда посылается сигнал, связанные с ним функции будут вызваны.

Концепция "сигнал/слот" может быть полезна, например, при разработке приложений с графическим интерфейсом. Кнопки могут быть сделаны так, чтобы посылать сингнал, когда пользователь нажимает на них. Они могут поддерживать связи сразу с несколькими функциями для обработки пользовательского ввода. Таким образом становится возможно гибко обрабатывать события.

Шаблонный класс `std::function` так же может быть использован для обработки событий. Важное различие между `std::function` и Boost.Signals2 состоит в том, что Boost.Signals2 позволяет связать больше одного обработчика с одним событием. Поэтому, библиотека Boost.Signals2 лучше для событийно-ориентированной разработки и должна быть первой, в случае, если необходимо обрабатывать события.

Boost.Signals2 сменяет библиотеку Boost.Signals, которая устарела и не рассматривается в данной книге.

<a id="signals"></a>
## Сигналы

Boost.Signals2 предостовляет класс `boost::signals2::signal`, который используется для создания сигналов. Этот класс определен в файле  `boost/signals2/signal.hpp`. Также, вы можете использовать главный заголовочный файл `boost/signals2.hpp`, в котором определены все классы и функции библиотеки Boost.Signals2.

Boost.Signals2 определяет `boost::signals2::signal` и другие классы, так же как и все функции, в пространстве имен `boost::signals2`.

<a id="ex.signals2_01"></a>
### Пример 67.1. “Hello, world!” с использованием boost::signals2::signal

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

Шаблонный класс `boost::signals2::signal` в качестве параметра принимает сигнатуру функции, используемой как обработчик событий. В [Примере 67.1](#ex.signals2_01), только функции с сигнатурой `void()` могут быть связаны с сигналом **s**.

Лямбда-функция связывается с сигналом **s** с помощью функции `connect()`. Так как лямбда-функция удовлетворяет требуемой сигнатуре `void()`, связывание успешно произведено. Лямбда-функция будет вызвана всякий раз, когда посылается сигнал **s**.

Сигнал посылается с помощью вызова **s** как обычной функции. Сигнатура этой функции совпадает с той, которая была передана в качестве параметра шаблона. Скобки пусты потому, что `void()` не ожидает никаких параметров. Вызов сигнала **s** приводит к его срабатыванию, что в свою очередь запускает лямбда-функцию, которая была ранее связана с помошью `connect()`.

[Пример 67.1](#ex.signals2_01) так же может быть реализован с помощью `std::function`, так как показано в [Примере 67.2](#ex.signals2_02).

<a id="ex.signals2_02"></a>
### Пример 67.2. “Hello, world!” с использованием std::function

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

В [Примере 67.2](#ex.signals2_02) лямбда-функция также вызывается, когда **f** будет вызвана. В тов время как `std::function` может использоваться только в как [Примере 67.2](#ex.signals2_02), Библиотека Boost.Signals2 предоставляет гораздо больше разнообразия. Например, она может связать несколько функций с определенным сигналом (см. [Пример 67.3](#ex.signals2_03)).

<a id="ex.signals2_03"></a>
### Пример 67.3. Несколько обработчиков событий с использованием boost::signals2::signal

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

`boost::signals2::signal` позволяет вам связать несколько функций с определенным сигналом с помощью многократных вызовов `connect()`. Всякий раз, когда сигнал срабатывает, функции будут вызваны в порядке, в котором они были связвны с помощью `connect()`.

Так же, порядок может быть определен явно с помощью перегруженной `connect()`, которое ожидает значение типа `int` в качестве дополнительного параметра ([Пример 67.4](#ex.signals2_04)).

<a id="ex.signals2_04"></a>
### Пример 67.4. Обработчики событий с явным порядком

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

Как и предыдущий пример, [Пример 67.4](#ex.signals2_04) выведет на экран `Hello, world!`.

Чтобы отвязать функцию от сигнала, вызовете `disconnect()`.

<a id="ex.signals2_05"></a>
### Пример 67.5. Отсоединение обработчиков событий от boost::signals2::signal

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

[Пример 67.5](#ex.signals2_05) выведетна экран только `Hello` потому что связь с `world()` была разорвана до того как сигнал был послан.

Помимо `connect()` и `disconnect()`, `boost::signals2::signal` предлагает ещё несколько методов (см. [Пример 67.6](#ex.signals2_06)).

<a id="ex.signals2_06"></a>
### Пример 67.6. Дополнительные методы boost::signals2::signal

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

`num_slots()` возвращает число связанных функций. Если связанных функуый нет, то `num_slots()` возвращает 0. `empty()` сообщит вам, связаны ли с ней обработчики событий или нет . А `disconnect_all_slots()` делает именно то, что говорит его имя: он разрывает все существующие связи.

<a id="ex.signals2_07"></a>
### Пример 67.7. Обработка возвращаемых значений из обработчиков событий

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

В [Примере 67.7](#ex.signals2_07) две лямбда-функции связаны с сигналом **s**. Первая лямбда-функция возвращает 1, а вторая - 2.

[Пример 67.7](#ex.signals2_07) выведет `2` в стандартный поток вывода. Оба возвращаемых значения были правильно приняты сигналом **s**, но все, кроме последнего, были проигнорированы. По умолчанию, возвращается только последнее возвращаемое значение всех связанных функций.

Обратите внимание, что `s()` не возвращает результат последней вызванной функции напрямую. Возвращается объект типа `boost::optional`, который при разименовывании возвращает число 2. Посланый сигнал, который не связан с какой-либо функцией, не возвращает никакого значения. Таким образом, в данном случае, `boost::optional` позволяет Boost.Signals2 вернуть пустой объект. С классом `boost::optional` мы познакомились в [Главе 21][chapter21].

Можно настроить сигнал таким образом, что каждое возвращаемое значение будет соответствующим образом обработано. Чтобы сделать это, необходимо передать *комбайнер* в `boost::signals2::signal` в качестве второго шаблонного параметра.

<a id="ex.signals2_08"></a>
### Пример 67.8. Поиск наименьшего возвращаемого значения с использованием пользовательского комбайнера

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

Комбайнер - это класс с перегруженным оператором `operator()`. Этот оператор будет автоматически вызван с двумя итераторами, которые используются для доступа к функциям, связанным с определенным сигналом. При разименовывании итераторов, происходит вызов соответствующих функций, и их возвращаемые значения становятся доступны в комбайнере. В дальнейщем, обычный алгоритм из стандартной библиотеки, такой как `std::min_element()` может быть использован для расчета и возврата наименьшего значения (см. [Пример 67.8](#ex.signals2_08)).

`boost::signals2::signal` использует `boost::signals2::optional_last_value` как комбайнер поумолчанию. Этот комбайнер возвращает объект типа `boost::optional`. Пользователь может определить комбайнер с возвращаемым значением любого типа. Например, комбайнер `min_element` в [Примере 67.8](#ex.signals2_08) возвращает значение такого же типа, который был передан в качестве шаблоного параметра в `min_element`.

Невозможно передать такой алгоритм, как `std::min_element()` в качестве шаблонного параметра напямую в `boost::signals2::signal`. `boost::signals2::signal` ожидакт, чтов комбайнер обявлет тип `result_type`, который указывает тип значения, возвращаемого оператором `operator()`. Так как этот тип не определен в стандартных алгоритмах, компилятор выдаст сообщение об ошибке.

Обратите внимание, что не представляется возможным передать итераторы **first** и **last** в `std::min_element()` напрямую, потому что этот алгоритм ожидает итераторы прямого доступа (forward iterators), в то время как комбайнер работает с итераторами ввода (input iterators). Поэтому, перед тем как определить наименьшее значение с помощью `std::min_element()`, для хранения всех возвращаемых значений используется вектор.

В [Примере 67.9](#ex.signals2_09) комбайнер изменен так, что бы хранить все возвращаемые значения в контейнере, вместо того чтобы сравнивать их. Он размещает все возвращаемые значения в векторе, который будет возвращен при вызове`s()`.

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

<a id="connections"></a>
## Соединения

Функциями можно управлять с помощью методов `connect()` и `disconnect()`, предоставляемых классом `boost::signals2::signal`. Поскольку `connect()` возвращает объект типа `boost::signals2::connection`, связи также могут управляться по-разному (см. [Пример 67.10](#ex.signals2_10)).

<a id="ex.signals2_10"></a>
### Пример 67.10. Управление соединениями с помощью boost::signals2::connection

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

Функция `disconnect()` класса `boost::signals2::signal` требует передачи указателя на функцию в качестве аргумента. Избежать этого можно путем вызова `disconnect()` с объектом `boost::signals2::connection`.

Для того, чтобы заблокировать функцию на короткий промежуток времени не разрывая связи с сигналом, может быть использован `boost::signals2::shared_connection_block`.

<a id="ex.signals2_11"></a>
### Пример 67.11. Блокировка соединений с помощью shared_connection_block

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

В [Примере 67.11](#ex.signals2_11) лямбда-функция вызывается дважды. Сигнал **s** посылается три раза, но лямбда-функция не будет вызвана во второй раз потому что был создан объект типа `boost::signals2::shared_connection_block` для того чтобы заблокировать вызов. Как только объект выходит из области видимости, блок автоматически удаляется. Блок также может быть удален в явном виде с помощью вызова функции `unblock()`. Так как она вызывается до последней отправки сигнала, последний вызов лямбда-функции будет выполнен.

В дополнение к `unblock()`, `boost::signals2::shared_connection_block` предоставляет функции `block()` и `blocking()`. Первая используется для блокирования соединения после вызова `unblock()`, в то время как последняя дает возможность проверить заблокировано ли соединение в данный момент или нет.

Обратите внимание, что `boost::signals2::shared_connection_block` содержит слово “shared” по причине того, что несколько объектов типа `boost::signals2::shared_connection_block` могут быть иничиализированы с помощью одного и тогоже соединения.

<a id="ex.signals2_12"></a>
### Пример 67.12. Многократная блокировка соединений

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

В [Примере 67.12](#ex.signals2_12) сигнал **s** посылается дважды, но лямбда-функция быдет вызвана только во второй раз. Программа выведет `Hello, world!` в стандартный поток вывода всего один раз.

Так как `false` передан в конструктор в качестве второго аргумента, первый объект типа `boost::signals2::shared_connection_block` не блокирует соединение с сигналом **s**. Следовательно, вызов `blocking()` объекта **b1** вернет `false`.

Тем не менее, лямбда-функция не выполняется при первом вызове **s**, так как доступ происходит только после того как второй объект типа `boost::signals2::shared_connection_block` будет инстанцирован. Но, не передавая второй параметр конструктору, соединение блокируется объектом. Когда **s** вызывается во второй раз, лямбда-функция выполнится, так как блокировка была автоматически удалена, когда **b2** вышел из области видимости.

Boost.Signals2 может разорвать соединение как только объект, чьим членом является функция, связанная с сигналом, будет уничтожен.

<a id="ex.signals2_13"></a>
### Пример 67.13. Фунция-член в качестве обработчика событий

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

В [Примере 67.13](#ex.signals2_13) функция-член объекта связывается с сигналом с помощью `std::bind()`. Объект удален до того как сигнал будет послан, что является проблемой, так как вместо того чтобы передать объект типа `world` в `std::bind()` будет передан только адрес. К тому времени как `s()` будут вызван, объект, на который мы ссылались, не будет существовать.

Возможно изменить программу так, что бы соедининие автоматически разрывалось как только объект был уничтожен. [Пример 67.14](#ex.signals2_14) делается именно это.

<a id="ex.signals2_14"></a>
### Пример 67.14. Автоматический разрыв соединения с функцией-членом

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

Теперь `num_slots()` вернет `0`. В [Примере 67.14](#ex.signals2_14) при отправки сигнала, не происходит попытки вызвать функцию-член, которая несуществует. Измениние состоит в том, что мы обернули объект типа `world` умным указателем `boost::shared_ptr`, который передаётся в `track()`. Эта функция-член вызыается на слот, который был передан в `connect()` для запроса отслеживания соответствующего объекта.

Функция или функция-член связанная с сигналом называется *слот*. Тип для задания слота не был использован в предыдущих примерах, потому что передать указатель на функцию или функцию-член в `connect()` было достаточно. The corresponding slot was created and associated with the signal automatically. Соответствующий слот был создан и связан с сигналом автоматически.

В [Примере 67.14](#ex.signals2_14), тем не менее, умный указатель связывается со слотом путем вызова `track()`. Так как тип слота зависит от сигнала, `boost::signals2::signal` предоставляет тип `slot_type` для доступа к требующемуся типу. `slot_type` ведет себя так же, как `std::bind`, что позволяет передавать оба параметра для описания слота напрямую. `track()` может связать слот с умным указателем типа `boost::shared_ptr`. Затем объект отслеживается, что приводит к тому, что слот будет автоматически удален, как только отслеживаемый объект разрушается.

Для того, чтобы управлять объектами с другими умными указателями, слоты предоставляют функцию-член под названием `track_foreign()`. В то время как `track()` ожидает, что умный указатель типа `boost::shared_ptr`, `track_foreign()` позволяет, например, использовать умный указатель типа `std::shared_ptr`. Умные указатели, кроме `std::shared_ptr` и `std::weak_ptr`, должны быть добавлены в Boost.Signals2 , прежде чем они могут быть переданы в `track_foreign()`.

Подписчик конкретного события может получить доступ к объекту типа `boost::signals2::signal` для создания новых связей или разрыва связи с существующими.

<a id="ex.signals2_15"></a>
### Пример 67.15. Создание новых соединений в обработчике событий

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

В [Примере 67.15](#ex.signals2_15) мы получаем доступ к **s** внутри функции `connect()` для связывания лямбда-функции с сигналом. Так как `connect()` вызывается, когда сигнал уже послан, возникает вопрос, будет ли вызвана лямбда-функция.

Программа не выводит ничего, что означает, что лямбда-функция никогда не вызывается. В то время как Boost.Signals2 поддерживает связывание функций с сигналами, когда сигнал уже послан, новые связи будут использованны только при следующем вызове сигнала.

<a id="ex.signals2_16"></a>
### Пример 67.16. Разрыв соединения в обработчике событий

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

В [Примере 67.16](#ex.signals2_16) не создаются новые связи, вместо этого разрываются существующие. Как и в [Примере 67.15](#ex.signals2_15), ничего не будет выведено в стандартный поток вывода.

Такое поведение можно объяснить довольно просто. Представьте себе, что временная копия всех слотов создается всякий раз, когда срабатывает сигнал. Вновь созданные связи не будут добавлены во временную копию и, следовательно, могут быть вызваны только при следующим срабатывании сигнала. Разорванные связи, с другой стороны, по-прежнему часть временной копии, но будут проверяться комбайнером при разыменовывании ссылки, чтобы избежать вызова функции-члена объекта, который уже был разрушен.

<a id="multithreading"></a>
## Многопоточность

Almost all classes provided by Boost.Signals2 are thread safe and can be used in multithreaded applications. For example, objects of type `boost::signals2::signal` and `boost::signals2::connection` can be accessed from different threads.

On the other hand, `boost::signals2::shared_connection_block` is not thread safe. This limitation is not important because multiple objects of type `boost::signals2::shared_connection_block` can be created in different threads and can use the same connection object.

<a id="ex.signals2_17"></a>
### Пример 67.17. boost::signals2::signal is thread safe

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

[Пример 67.17](#ex.signals2_17) creates two threads that execute the `loop()` function, which accesses **s** one hundred times to call the associated lambda function. Boost.Signals2 explicitly supports simultaneous access from different threads to objects of type `boost::signals2::signal`.

[Пример 67.17](#ex.signals2_17) displays numbers from 0 to 99. Because **i** is incremented in two threads and written to the standard output stream in the lambda function, the numbers will not only be displayed twice, they will also overlap. However, because `boost::signals2::signal` can be accessed from different threads, the program will not crash.

However, [Пример 67.17](#ex.signals2_17) still requires synchronization. Because two threads access **s**, the associated lambda function runs in parallel in two threads. To avoid having the two threads interrupt each other while writing to standard output, a mutex is used to synchronize access to **std::cout**.

For single-threaded applications, support for multithreading can be disabled in Boost.Signals2.

<a id="ex.signals2_18"></a>
### Пример 67.18. boost::signals2::signal without thread safety

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

Boost.Signals2 provides an empty mutex implementation called `boost::signals2::dummy_mutex`. If a signal is defined with this class, it will no longer support multithreading (see [Пример 67.18](#ex.signals2_18)).
