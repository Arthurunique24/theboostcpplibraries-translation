# Глава 32. Boost.Asio

**Содержание**

<hr>

+ [Способы и объекты ввода-вывода](#ioservices)  
+ [Масштабируемость и Многопоточность](#threads)  
+ [Сетевое программирование](#network)  
+ [Сопрограммы](#coroutines)  
+ [Платформо-зависимые объекты ввода-вывода](#iospecific)  

<hr>

В этой главе рассматривается библиотека [Boost.Asio](http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio.html). Asio используется для асинхронного ввода-вывода. Эта библиотека позволяет обрабатывать данные в асинхронном режиме. Асинхронный режим означает, что при инициализации иных операций, главному потоку не нужно ждать их завершения. Вместо этого Boost.Asio уведомляет программу, когда операция закончилась. Преимуществом является то, что другие операции могут выполнятся параллельно.  

Boost.Thread еще одна библиотека, которая позволяет выполнять операции одновременно. Разница между Boost.Thread и Boost.Asio заключается в том, что с Boost.Thread вы получаете доступ к ресурсам внутри программы, а с Boost.Asio вы получаете доступ к ресурсам за пределами программы. Например, если вы разрабатываете функцию, которую требуется запустить для трудоемких вычислений, можете вызвать эту функцию в потоке, а его запустить на другом ядре процессора. Потоки позволяют вам получить доступ к процессору и использовать его ядра. С точки зрения вашей программы, ядра процессора являются внутренним ресурсом. Если же вы хотите получить доступ к внешним ресурсам, использйте Boost.Asio.

Сетевые соединения являются примером внешних ресурсов. Если данные должны быть отправлены или получены, сетевая карта выполняет определнные операции. Для операции передачи, сетевая карта получает указатель на буфер с данными для отправки. Для операции получения, сетевая карта получает указатель на буфер, который заполняется поступившими данными. Поскольку сетевая карта является внешним ресурсом для вашей программы, она может выполнять операции независимо. Для этого необходимо только время, которое вы могли бы использовать для выполнения иных задач. Boost.Asio позволяет более эффективно использовать доступные устройства благодаря параллельному выполнению.  

Передача и прием данных по сети реализована в виде асинхронной операции в Boost.Asio. Считайте асинхронную операцию функцией, которая сразу завершается, но без какого-либо результата. Результат передается позже.  

На первом этапе асинхронная операция запускается. На втором этапе программа получает уведомление, когда асинхронная операция закончилась. Такое разделение между началом и завершением позволяет получить доступ к внешним ресурсам без блокировки вызовов функций. 

<a name="ioservices"></a>
# Объекты ввода - вывода

Программы, которые используют Boost.Asio для асинхронной обработки данных, основаны на **сервисе** и **объектах ввода-вывода**. I/O сервис - это абстрактный интерфейс операционной системы, который обрабатывает данные в асинхронном режиме. Объекты ввода-вывода инициируют асинхронные операции. Эти два понятия необходимы для разделения задач: сервисы ввода-вывода использую API операционной системы, а I/O объекты общаются с разработчиком.

Как пользователь Boost.Asio вы не подключаетесь напрямую к I/O сервисам, а управляете ими с помощью объектов. Объект сервиса ввода-вывода можно представить как список, в котором перечислены сервисы ввода-вывода. Каждый объект знает свой сервис и получает к нему доступ через объект сервиса ввода-вывода.

Boost.Asio содержит ```boost::asio::io_service``` - класс для объекта сервиса ввода-вывода. Каждая программа, основанная на Boost.Asio, использует объект типа ```boost::asio::io_service```. Этот объект может объявляться глобально.

Для объекта сервиса существует единственный класс, а объектов ввода-вывода может быть несколько. Применение I/O объекта зависит только от постановки задачи. Например, если взаимодействие будет происходить по TCP/IP протоколам, то нужно использовать объект типа ```boost::asio::ip::tcp::socket```. Если данные должны быть переданы асинхронно по порту, то используйте ```boost::asio::serial_port```. Для соединения по таймеру используется ```boost::asio::steady_timer```.  
  
```boost::asio::steady_timer``` можно ассоциировать с будильником. Функция завершится по окончанию времени, установленного в таймере, и сообщит об этом. ```boost::asio::steady_timer``` просто ждет завершения установленного времении и в этот момент внешние ресурсы не доступны. Однако они являются частью операционной системы, и она может уведомить основной поток, что выполнение параллельной операции завершилось, и ресурсы освободились. Вышеописанное помогает избежать создания нового потока. ```boost::asio::steady_timer``` - очень простой для использования объект. 

*Заметка*:  
Из-за ошибки в Boost.Asio, не представляется возможным собрать следующий пример при компиляции на Clang. Описание ошибки можно найти здесь [ticket 8835](https://svn.boost.org/trac/boost/ticket/8835). В качестве решения замените ```std::chrono``` на ```boost::chrono```. Это позволит скомпилировать примеры на Clang.

<a name="example321"></a>
###Пример 32.1. Используется ```boost::asio::steady_timer```

```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>
    
using namespace boost::asio;

int main()
{
  io_service ioservice;
  
  steady_timer timer{ioservice, std::chrono::seconds{3}};
  timer.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  ioservice.run();
}
```

В [Примере 32.1](#example321) создается объект сервиса ввода-вывода - **ioservice**, и используется для инициализации **таймера** (одного из типов I/O объектов). Объекты ```boost::asio::steady_timer``` ожидают объект сервиса ввода-вывода в качестве первого параметра в конструкторе. Поскольку **таймер** представляет собой будильник, он может быть передан вторым параметром в конструктор, который определяет конкретное время или период времени, когда будильник должен "зазвонить". В [примере 32.1](#example321), будильник установлен на 3 секунды. Отсчет времени начинается с определения **таймера**.

Вместо вызова блокирующей функции, которая завершится, когда время закончится, Boost.Asio позволяет начать асинхронную операцию. Чтобы это сделать, вызовите функцию ```async_wait()```, которая ожидает обработчик в качестве единственного параметра. Обработчик является функцией или функциональным объектом, который вызывается, когда асинхронная операция завершается. В [примере 32.1](#example321) в качестве параметра передается лямбда-функция.

Лямбда-функция вызовется только через 3 секунды. Когда ```async_wait()``` возвращает управление, программа может сделать что-то еще.

```async_wait()``` является неблокирующей функцией. Объекты ввода-вывода, как правило, могут обеспечивать блокирование функций. Например, вы можете вызвать функцию блокировки ```wait()``` в ```boost::asio::steady_timer```. Поскольку эта функция ничего не делает и просто ждет, когда истечет время, она не принимает никакого обработчика. ```wait()``` завершается либо в определенное время, либо после какого - то периода.

В конце функции ```main()``` в [примере 32.1](#example321) вызывается метод ```run()``` объекта сервиса ввода-вывода. Этот вызов необходим, поскольку функционирующие системные функции должны взять на себя контроль. Следует помнить, что сервис ввода-вывода в объекте, который реализует асинхронные операции, основан на использовании функций операционной системы.

В то время как ```async_wait()``` запускает асинхронную операцию и завершается в момент завершения выполнения, ```run()``` блокирует сервис до окончания выполнения всех остальных операций с ним. Многие операционные системы поддерживают асинхронные операции только через функцию блокировки. Следующий пример показывает, почему это, как правило, не является проблемой.

<a name="example322"></a>
###Пример 32.2. Две асинхронные операции с ```boost::asio::steady_timer```

```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <iostream>

using namespace boost::asio;

int main()
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::seconds{3}};
  timer1.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  steady_timer timer2{ioservice, std::chrono::seconds{4}};
  timer2.async_wait([](const boost::system::error_code &ec)
    { std::cout << "4 sec\n"; });

  ioservice.run();
}
```

В [примере 32.2](#example322) используются два объекта типа ```boost::asio::steady_timer```. Первым объектом ввода-вывода является таймер, который ждет три секунды. Второй ждет в течение четырех секунд. После того, как период времени истекает, вызывается лямбда - функция, которая была передана в ```async_wait()```.

```run()``` вызывается только для объекта сервиса ввода-вывода в этом примере. Этот вызов передает управление функциям операционной системы, которые выполняют асинхронные операции. С их помощью, первая лямбда - функция вызывается через три секунды, а вторая в течение четырех.

Может казаться неожиданностью, что асинхронные операции требуют вызова функции блокировки. Тем не менее, это не является проблемой, так как программа должна быть защищена от выхода. Если ```run()``` не будет блокирован, функция ```main()``` завершит свое выполнение, и программа завершится. Если вы не хотите ждать завершения ```run()```, просто запустите его в новом потоке.

После того, как все таймеры завершили ожидание, никаких асинхронных операций, которые программа должна ожидать, больше нет.  

<a name="threads"></a>
# Масштабируемость и Многопоточность  

Разработка программы, основанной на библиотеке как Boost.Asio отличается от обычного стиля C ++. Функции, на которые может понадобиться достаточно много времени, нет смысла вызывать в последовательном порядке. Вместо вызова блокирущих функций, Boost.Asio запускает асинхронные операции. Функции, которые должны быть вызваны после завершения операции теперь вызываются без соответствующего обработчика. Недостатком данного подхода является разделение последовательно выполняемых функций, что может сделать код трудным для понимания.

Такие библиотеки как Boost.Asio обычно используются для достижения большей эффективности. При этом нет необходимости ждать завершения функции, программа может выполнять в этот момент другие задачи. Таким образом, можно запустить несколько асинхронных операций, которые выполняются одновременно. Но помните, что асинхронные операции, как правило, используются для доступа к ресурсам за пределами процесса. Поскольку ресурсы могут быть различными устройствами, они могут работать независимо друг от друга и выполнять операции параллельно.

Масштабируемость характеризует способность программы эффективно использовать дополнительные ресурсы. Если используются потоки,  то несколько функций выполняются одновременно на доступных ядрах процессора. Boost.Asio с потоками улучшает масштабируемость, поскольку программа может взаимодействовать с внутренними и внешними ресурсами, которые могут выполняться самостоятельно или в сотрудничестве друг с другом.

Если функция ```run()``` вызывается для объекта типа ***boost::asio::io_service***, связанные с ним обработчики вызываются в том же потоке. При использовании нескольких потоков, программа может вызвать ```run()``` несколько раз. После того, как асинхронная операция завершена, объект сервиса ввода-вывода будет запускать обработчик в одном из этих потоков. Если вторая операция завершается вскоре после первой, объект может запустить обработчик в любом из доступных. Однако, не только функции могут выполняться параллельно, но и обработчики также могут быть выполнены одновременно.    
 
<a name="example323"></a>
###Пример 32.3. Два потока объектов сервиса ввода - вывода для параллельного выполнения обработчиков

```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <thread>
#include <iostream>

using namespace boost::asio;

int main()
{
  io_service ioservice;

  steady_timer timer1{ioservice, std::chrono::seconds{3}};
  timer1.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  steady_timer timer2{ioservice, std::chrono::seconds{3}};
  timer2.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  std::thread thread1{[&ioservice](){ ioservice.run(); }};
  std::thread thread2{[&ioservice](){ ioservice.run(); }};
  thread1.join();
  thread2.join();
}
```

Предыдущий [пример 32.2](#example322) был преобразован в многопоточную программу ([пример 32.3](#example323)). С помощью ```std::thread``` были созданы два потока в функции ```main()```. ```run()``` вызывается для каждого объекта ввода-вывода в обоих потоках. Когда асинхронные операции завершаются, запускаются два потока, в которых обработчики выполняются параллельно.

В [примере 32.3](#example323), оба таймера должны завершиться через три секунды. Поскольку два потока доступны, обе лямбда-функции будут выполняться одновременно. Если второй таймер завершится в то время как таймер первого выполняется, второй обработчик будет запущен в доступном, то есть во втором, потоке. Если к этому моменту первый таймер тоже завершится, то обработчик может выбирать любой поток.

Конечно, не всегда имеет смысл использовать потоки. В [пример 32.3](#example323) показано, что нельзя одновременно выводить данные в стандартный поток. Вывод может быть перепутан. Оба обработчика, которые выполняются в двух потоках одновременно, используют один глобальный ресурс **std::cout**. Чтобы избежать коллизий, доступ к **std::cout** должен быть синхронизированным. Таким образом, преимущество потоков теряется, если обработчики не могут выполняться одновременно.

<a name="example324"></a>
###Пример 32.4. Один поток для каждого объекта сервиса ввода - вывода для параллельного выполнения обработчиков  

```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <thread>
#include <iostream>

using namespace boost::asio;

int main()
{
  io_service ioservice1;
  io_service ioservice2;

  steady_timer timer1{ioservice1, std::chrono::seconds{3}};
  timer1.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  steady_timer timer2{ioservice2, std::chrono::seconds{3}};
  timer2.async_wait([](const boost::system::error_code &ec)
    { std::cout << "3 sec\n"; });

  std::thread thread1{[&ioservice1](){ ioservice1.run(); }};
  std::thread thread2{[&ioservice2](){ ioservice2.run(); }};
  thread1.join();
  thread2.join();
}
```

Рекомендуется несколько раз вызывать метод ```run()``` для одного объекта, поскольку так можно сделать программу более масштабируемой. Вместо создания нескольких потоков для одного объекта сервиса ввода - вывода, можно также создавать несколько объектов.

В [примере 32.4](#example324) два объекта используют два таймера типа ```boost::asio::steady_timer```. Программа основана на двух потоках, каждый из которых связан с объектом ввода-вывода. Два I/O объекта - **timer1** и **timer2** не привязаны к одному объекту сервиса. Они связаны с различными объектами.

[Пример 32.4](#example324) работает так же, как и раньше. Нельзя сказать когда какой метод лучше использовать. Поскольку ```boost::asio::io_service``` представляет из себя интерфейс операционной системы, а любое решение зависит от конкретного интерфейса.

В операционной системе Windows ```boost::asio::io_service``` обычно основывается на IOCP, в Linux, основан на ```epoll()```. Наличие нескольких объектов ввода-вывода означает, что будет использовано несколько портов или произойдут вызовы ```epoll()```. Только от поставленной задачи зависит использовать один I/O порт или один вызов ```epoll()```.

<a name="network"></a>
# Сетевое программирование
Даже если Boost.Asio может обрабатывать любые виды данных асинхронно, в основном он используется для сетевого программирования. Это потому, что Boost.Asio поддерживал сетевые функции задолго до того, как были добавлены объекты ввода-вывода. Сетевые функции - идеальное использование для асинхронных операций, потому что передача данных через интернет может занять много времени. Это означает, что информация об успешном завершении передачи или об ошибке могут не быть доступными сразу же, как выполнятся функции, которые отправляют и получают данные.

Boost.Asio предоставляет много объектов ввода-вывода, чтобы разрабатывать сетевые программы. [Пример 32.5](#example325) использует класс `boost::asio::ip::tcp::socket`, чтобы установить соединение с другим компьютером. Этот пример посылает HTTP-запрос к веб-серверу, чтобы загрузить страницу.

<a name="example325"></a>
### Пример 32.5. Веб-клиент с boost::asio::ip::tcp::socket
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <array>
#include <string>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::resolver resolv{ioservice};
tcp::socket tcp_socket{ioservice};
std::array<char, 4096> bytes;

void read_handler(const boost::system::error_code &ec,
  std::size_t bytes_transferred)
{
  if (!ec)
  {
    std::cout.write(bytes.data(), bytes_transferred);
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void connect_handler(const boost::system::error_code &ec)
{
  if (!ec)
  {
    std::string r =
      "GET / HTTP/1.1\r\nHost: theboostcpplibraries.com\r\n\r\n";
    write(tcp_socket, buffer(r));
    tcp_socket.async_read_some(buffer(bytes), read_handler);
  }
}

void resolve_handler(const boost::system::error_code &ec,
  tcp::resolver::iterator it)
{
  if (!ec)
    tcp_socket.async_connect(*it, connect_handler);
}

int main()
{
  tcp::resolver::query q{"theboostcpplibraries.com", "80"};
  resolv.async_resolve(q, resolve_handler);
  ioservice.run();
}
```

[Пример 32.5](#example325) использует три обработчика (handlers): `connect_handler()` и `read_handler()` вызываются, когда соединение установлено и данные получены. `resolve_handler()` используется для разрешения имён.

Поскольку данные могут быть получены только после того, как соединение было установлено, а так же связь может быть установлена только после того, как имя было разрешено (resolved), различные асинхронные операции запускаются в обработчиках. В `resolve_handler()` итератор **it**, который указывает на конечную точку (endpoint), разрешенную из имени, используется с **tcp_socket** для установления соединения. В `connect_handler()`идёт обращение к **tcp_socket**, чтобы отправить HTTP-запрос и начать прием данных. Так как все операции являются асинхронными, обработчики передаются в соответствующие функции. В зависимости от операций, может понадобиться передать дополнительные параметры. Например, итератор **it** ссылается на конечную точку, разрешенную из имени. Массив **bytes** используется для хранения полученных данных.

В `main()` `boost::asio::ip::tcp::resolver::query` инстанцируется для создания объекта **q**. **q** представляет собой запрос для разрешителя (resolver) имён - объект ввода-вывода, имеющий тип `boost::asio::ip::tcp::resolver`. Передавая **q** в `async_resolve()`, асинхронная операция запускается для разрешения имени. [Пример 32.5](#example325) разрешает имя `theboostcpplibraries.com`. После запуска асинхронной операции, `run()` вызывается на объекте сервиса ввода-вывода для передачи управления операционной системе.

Когда имя разрешается, вызывается `resolve_handler()`. Обработчик сначала проверяет, было ли разрешение имени успешным. В этом случае **ес** равен 0. Только после этого идёт обращение к сокету, чтобы установить соединение. Адрес сервера для подключения обеспечивается вторым параметром, чей тип `boost::asio::ip::tcp::resolver::iterator`. Этот параметр - результат разрешения имен.

Вызов `async_connect()` сопровождается вызовом обработчика `connect_handler()`. Снова сначала проверяется **ес**, чтобы выяснить, может ли быть установлено соединение. Если так, то `async_read_some()` вызывается на сокете. С помощью этого вызова начинается чтение данных. Полученные данные хранятся в массиве **bytes**, который передается в качестве первого параметра в `async_read_some()`.

`read_handler()` вызывается, когда один или несколько байтов были получены и скопированы в **bytes**. Параметр **bytes_transferred** типа std::size_t содержит число байтов, которые были получены. Как обычно, обработчик должен сначала проверить, **ес** завершена ли асинхронная операция успешно. Только в этом случае данные записываются в стандартный вывод.

Обратите внимание, что `read_handler()` снова вызывает `async_read_some()` после того, как данные были записаны в **std::cout**. Это необходимо, потому что вы не можете быть уверены, что вся домашняя страница была загружена и скопирована в **bytes** одной асинхронной операцией. Повторящиеся вызовы `async_read_some()`, сопровождаемые повторяющтмися вызовами `read_handler()` заканчиваются только тогда, когда соединение закрывается, что происходит, когда веб-сервер отправил всю домашнюю страницу. Затем `read_handler()` сообщает об ошибке в **ес**. С этого момента никакие дополнительных данных не записывается в **std::cout** и `async_read()` не вызывается на сокете. Поскольку нет никаких незавершенных асинхронных операций, программа завершает свою работу.

<a name="example326"></a>
### Пример 32.6. Сервер времени (time server) с boost::asio::ip::tcp::acceptor
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <ctime>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::endpoint tcp_endpoint{tcp::v4(), 2014};
tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
tcp::socket tcp_socket{ioservice};
std::string data;

void write_handler(const boost::system::error_code &ec,
  std::size_t bytes_transferred)
{
  if (!ec)
    tcp_socket.shutdown(tcp::socket::shutdown_send);
}

void accept_handler(const boost::system::error_code &ec)
{
  if (!ec)
  {
    std::time_t now = std::time(nullptr);
    data = std::ctime(&now);
    async_write(tcp_socket, buffer(data), write_handler);
  }
}

int main()
{
  tcp_acceptor.listen();
  tcp_acceptor.async_accept(tcp_socket, accept_handler);
  ioservice.run();
}
```

[Пример 32.6](#example326) является сервером времени. Вы можете связаться с telnet-клиентом, чтобы получить текущее время. После этого сервер выключается.

Сервер времени использует объект ввода-вывода `boost::asio::ip::tcp::acceptor`, чтобы принять входящее соединение из другой программы. Вы должны инициализировать объект, поэтому он знает, какой протокол и порт использовать. В этом примере, переменная **tcp_endpoint** типа `boost::asio::ip::tcp::endpoint` используется, чтобы сообщить **tcp_acceptor** принимать входящие соединения 4-й версии протокола Интернет на порту 2014.

После инициализации акцептора (acceptor) вызывается `listen()`, чтобы акцептор начал прослушивание. Затем вызывается `async_accept()`, чтобы принять первую попытку подключения. Разъем должен быть передан первым параметром в `async_accept()`. Он будет использоваться для отправки и приема данных на новом соединении.

Как только другая программа устанавливает соединение, вызывается `accept_handler()`. Если соединение было установлено успешно, текущее время отправляется с `boost::asio::async_write()`. Эта функция записывает все данные в **data** для сокета. `boost::asio::ip::tcp::socket` также предоставляет функцию-член `async_write_some()`. Эта функция вызывает обработчик, когда по крайней мере один байт был послан. Затем обработчик должен проверить, сколько байт было отправлено и сколько еще нужно отправить. Затем он снова должен вызвать `async_write_some()`. Многократное вычисление числа байтов, которых нужно отправить, и вызова `async_write_some()` можно избежать, если использовать `boost::asio::async_write()`. Асинхронная операция, которая началась с этой функцией завершается только тогда, когда все байты в **data** были отправлены.

После отправки данных вызывается `write_handler()`. Эта функция вызывает `shutdown()` с параметром  **boost::asio::ip::tcp::socket::shutdown_send**, который сообщает, что программа закончила отправку данных через сокет. Поскольку нет никаких отложенных асинхронных операций, [Пример 32.6](#example326) завершается. Обратите внимание, что, хотя переменная **data** используется только в `accept_handler()`, она не может быть локальной переменной. **data** передается по ссылке из `boost::asio::buffer()` в `boost::asio::async_write()`. Когда `boost::asio::async_write()` и `accept_handler()` завершаются, асинхронная операция началась, но еще не завершена. **data** должна существовать, пока асинхронная операция не завершена. Если **data** является глобальной переменной, то это гарантировано.

<a name="coroutines"></a>
# Сопрограммы (coroutines)

Начиная с версии 1.54.0, Boost.Asio поддерживает сопрограммы. Вы можете использовать Boost.Coroutine напрямую, но явная поддержка сопрограммам в Boost.Asio облегчает их использование.

Сопрограммы позволяют создать структуру, которая отражает реальную логику программы. Асинхронные операции не делят функции, так как нет никаких обработчиков, чтобы определить, что должно произойти, когда асинхронная операция завершается. Вместо вызовов обработчиками друг друга, программа может использовать последовательную структуру.

<a name="example327"></a>
### Пример 32.7. Сопрограммы с Boost.Asio
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <list>
#include <string>
#include <ctime>

using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::endpoint tcp_endpoint{tcp::v4(), 2014};
tcp::acceptor tcp_acceptor{ioservice, tcp_endpoint};
std::list<tcp::socket> tcp_sockets;

void do_write(tcp::socket &tcp_socket, yield_context yield)
{
  std::time_t now = std::time(nullptr);
  std::string data = std::ctime(&now);
  async_write(tcp_socket, buffer(data), yield);
  tcp_socket.shutdown(tcp::socket::shutdown_send);
}

void do_accept(yield_context yield)
{
  for (int i = 0; i < 2; ++i)
  {
    tcp_sockets.emplace_back(ioservice);
    tcp_acceptor.async_accept(tcp_sockets.back(), yield);
    spawn(ioservice, [](yield_context yield)
      { do_write(tcp_sockets.back(), yield); });
  }
}

int main()
{
  tcp_acceptor.listen();
  spawn(ioservice, do_accept);
  ioservice.run();
}
```

`boost::asio::spawn()` - функция, которую нужно вызывать, для использования сопрограмм с Boost.Asio. Первый передаваемый параметр должен быть объектом сервиса ввода-вывода. Второй параметр - сопрограммная функция. Эта функция должна принимать в качестве единственного параметра объект типа `boost::asio::yield_context`. Она не должен иметь возвращаемого значения. [Пример 32.7](#example327) использует `do_accept()` и `do_write()` как сопрограммы. Если сигнатура функции отличается, как в случае для `do_write()`, вы должны использовать адаптер, например, `std::bind`, или лямбда-функции.

Вместо обработчика вы можете передать объект типа `boost::asio::yield_context` в асинхронную функцию. `do_accept()` передает параметр **yield** в `async_accept()`. В `do_write()` **yield** передается в  `async_write()`. Эти функциональные вызовы по-прежнему начинают асинхронные операции, но не один из обработчиков не будет вызван, когда операция завершится. Вместо этого контекст, в котором были начаты асинхронные операции, восстанавливается. Когда эти асинхронные операции завершатся, программа продолжает с того места, где она была прервана.

`do_accept()` содержит цикл `for`. Новый сокет передается `async_accept()` каждый раз, когда функция вызывается. Когда клиент устанавливает соединение, вызывается `do_write()` как сопрограмма с `boost::asio::spawn()`, чтобы отправить текущее время клиенту.

Цикл `for` позволяет легко видеть, что программа может обслужить двух клиентов перед выходом. Так как пример основан на сопрограммах, повторное выполнение асинхронной операции может быть реализован в цикле `for`. Это улучшает читаемость программы, так как вы не должны отслеживать потенциальные вызовы обработчиков, чтобы выяснить, когда последняя асинхронная операция будет завершена. Если сервер времени должен поддерживать более двух клиентов, нужно модифицировать только цикл `for`.

<a name="iospecific"></a>
# Платформо-зависимые объекты ввода-вывода

До сих пор все примеры, приведенные в этой главе, были независимыми от платформы. Объекты ввода-вывода, такие как `boost::asio::steady_timer` и `oost::asio::ip::tcp::socket`, поддерживаются на всех платформах. Тем не менее Boost.Asio также предоставляет платформо-зависимые объекты ввода-вывода, поскольку некоторые асинхронные операции доступны только на определенных платформах, например, Windows или Linux.

<a name="example328"></a>
### Пример 32.8. Использование boost::asio::windows::object_handle
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/windows/object_handle.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <Windows.h>

using namespace boost::asio;
using namespace boost::system;

int main()
{
  io_service ioservice;

  HANDLE file_handle = CreateFileA(".", FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  char buffer[1024];
  DWORD transferred;
  OVERLAPPED overlapped;
  ZeroMemory(&overlapped, sizeof(overlapped));
  overlapped.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  ReadDirectoryChangesW(file_handle, buffer, sizeof(buffer), FALSE,
    FILE_NOTIFY_CHANGE_FILE_NAME, &transferred, &overlapped, NULL);

  windows::object_handle obj_handle{ioservice, overlapped.hEvent};
  obj_handle.async_wait([&buffer, &overlapped](const error_code &ec) {
    if (!ec)
    {
      DWORD transferred;
      GetOverlappedResult(overlapped.hEvent, &overlapped, &transferred,
        FALSE);
      auto notification = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
      std::wcout << notification->Action << '\n';
      std::streamsize size = notification->FileNameLength / sizeof(wchar_t);
      std::wcout.write(notification->FileName, size);
    }
  });

  ioservice.run();
}
```

[Пример 32.8](#example328) использует объект ввода-вывода `boost::asio::windows::object_handle`, который доступен только на Windows. Объект `boost::asio::windows::object_handle`, который основан на Windows-функции `RegisterWaitForSingleObject()`, позволяет запускать асинхронные операции для ручек (handle) объекта. Все ручки, принятые `RegisterWaitForSingleObject(),` могут использоваться с `boost::asio::windows::object_handle`. С `async_wait()` возможно асинхронно ожидать, пока не изменится ручка объекта.

[Пример 32.8](#example328) инициализирует объект **obj_handle** типа `boost::asio::windows::object_handle` с ручкой, созданной с помощью Windows-функции `CreateEvent()`. Ручка является частью структуры `OVERLAPPED`, чей адрес передается Windows-функции `ReadDirectoryChangesW()`. Окна использует структуры `OVERLAPPED` для запуска асинхронных операций.

Функция `ReadDirectoryChangesW()` может быть использована для слежения за каталогом и ожидания изменений в нём. Для асинхронного вызова функции структура `OVERLAPPED` должна быть передана `ReadDirectoryChangesW()`. Чтобы сообщить о завершении асинхронной операции через Boost.Asio, обработчик событий сохраняется в структуре `OVERLAPPED` до того, как он передается `ReadDirectoryChangesW()`. Впоследствии этот обработчик события передается **obj_handle**. Когда `async_wait()` вызывается на **obj_handle**, обработчик выполняется при обнаружении изменения в наблюдаемой директории.

При запуске [Пример 32.8](#example328), создайте новый файл в каталоге, из которого будет запущен пример. Программа обнаружит новый файл и напишет сообщение в стандартный поток вывода.

<a name="example329"></a>
### Пример 32.9. Использование boost::asio::windows::overlapped_ptr
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/windows/overlapped_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <Windows.h>

using namespace boost::asio;
using namespace boost::system;

int main()
{
  io_service ioservice;

  HANDLE file_handle = CreateFileA(".", FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

  error_code ec;
  auto &io_service_impl = use_service<detail::io_service_impl>(ioservice);
  io_service_impl.register_handle(file_handle, ec);

  char buffer[1024];
  auto handler = [&buffer](const error_code &ec, std::size_t) {
    if (!ec)
    {
      auto notification =
        reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
      std::wcout << notification->Action << '\n';
      std::streamsize size = notification->FileNameLength / sizeof(wchar_t);
      std::wcout.write(notification->FileName, size);
    }
  };
  windows::overlapped_ptr overlapped{ioservice, handler};
  DWORD transferred;
  BOOL ok = ReadDirectoryChangesW(file_handle, buffer, sizeof(buffer),
    FALSE, FILE_NOTIFY_CHANGE_FILE_NAME, &transferred, overlapped.get(),
    NULL);
  int last_error = GetLastError();
  if (!ok && last_error != ERROR_IO_PENDING)
  {
    error_code ec{last_error, error::get_system_category()};
    overlapped.complete(ec, 0);
  }
  else
  {
    overlapped.release();
  }

  ioservice.run();
}
```

[Пример 32.9](#example329), как и предыдущий, использует `ReadDirectoryChangesW()`, чтобы следить за каталогом. На этот раз, асинхронный вызов `ReadDirectoryChangesW()` не связан с обработчиком событий. В примере используется класс `boost::asio::windows::overlapped_ptr`, который в своей реализации использует структуру `OVERLAPPED`. `get()` возвращает указатель на внутреннюю структуру `OVERLAPPED`. В примере указатель затем передается в `ReadDirectoryChangesW()`.

`boost::asio::windows::overlapped_ptr` это объект ввода-вывода, который не имеет функции-члена, чтобы начать асинхронную операцию. Асинхронная операция запускается, когда передаётся указатель на внутреннюю переменную `OVERLAPPED` в Windows-функцию. В дополнение к объекту сервиса ввода-вывода, конструктор `boost::asio::windows::overlapped_ptr` ожидает обработчик, который будет вызываться, когда асинхронная операция завершается.

[Пример 32.9](#example329) использует `boost::asio::use_service()`, чтобы получить ссылку на службу в объекте службы ввода-вывода **ioservice**. `boost::asio::use_service()` представляет собой шаблон функции. Тип службы ввода-вывода, который вы хотите извлечь, должен быть передан в качестве параметра шаблона. В примере повышение передается `boost::asio::detail::io_service_impl`. Этот тип сервиса ввода-вывода самый близкий к операционной системе. В операционной системе Windows `boost::asio::detail::io_service_impl` использует IOCP, а на Linux использует `epoll()`. `boost::asio::detail::io_service_impl` - это определение типа, который устанавливается для `boost::asio::detail::win_iocp_io_service` на Windows и `boost::asio::detail::task_io_service` на Linux.

`boost::asio::detail::win_iocp_io_service` предоставляет функцию-член `register_handle()`, чтобы связать ручку с IOCP-ручкой. `register_handle()` вызывает Windows-функцию `CreateIoCompletionPort()`. Этот вызов необходим для примера, чтобы jy работал правильно. Ручка, возвращаемая `CreateFileA()`, может быть передана через **overlapped** в `ReadDirectoryChangesW()` только после того, как он будет связан с IOCP-ручкой.

[Пример 32.9](#example329) проверяет, удачно ли завершилась `ReadDirectoryChangesW()`. Если `ReadDirectoryChangesW()` завершилась с ошибкой, вызывается `complete()` на **overlapped**, чтобы завершенить асинхронную операцию для Boost.Asio. Параметры, передаваемые для `complete()` перенаправляются к обработчику.

Если `ReadDirectoryChangesW()` завершается успешно, вызывается `release()` . Далее асинхронная операция находится в ожидании и завершается только после завершения операции, которая была инициирована с помощью Windows-функции `ReadDirectoryChangesW()`.

<a name="example3210"></a>
### Пример 32.10. Использование boost::asio::posix::stream_descriptor
```c++
#include <boost/asio/io_service.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>
#include <unistd.h>

using namespace boost::asio;

int main()
{
  io_service ioservice;

  posix::stream_descriptor stream{ioservice, STDOUT_FILENO};
  auto handler = [](const boost::system::error_code&, std::size_t) {
    std::cout << ", world!\n";
  };
  async_write(stream, buffer("Hello"), handler);

  ioservice.run();
}
```

[Пример 32.10](#example3210) представляет объект ввода-вывода для POSIX платформ.

`boost::asio::posix::stream_descriptor` может быть инициализирован с дескриптора файла, чтобы начать асинхронную операцию для данного файлового дескриптора. В примере, **stream** связан с дескриптор файла `STDOUT_FILENO`, чтобы записать строку асинхронно в стандартный поток вывода.