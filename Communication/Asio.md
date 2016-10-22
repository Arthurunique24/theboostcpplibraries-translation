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

<a name="#example327"></a>
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

Вместо обработчика вы можете передать объект типа `boost::asio::yield_context` в асинхронную функцию. `do_accept()` передает параметр **yield** в `async_accept()`. В `do_write()`, **yield** передается в  `async_write()`. Эти функциональные вызовы по-прежнему начинают асинхронные операции, но не один из обработчиков не будет вызван, когда операция завершится. Вместо этого контекст, в котором были начаты асинхронные операции, восстанавливается. Когда эти асинхронные операции завершатся, программа продолжает с того места, где она была прервана.

`do_accept()` содержит цикл `for`. Новый сокет передается `async_accept()` каждый раз, когда функция вызывается. Когда клиент устанавливает соединение, вызывается `do_write()` как сопрограмма с `boost::asio::spawn()`, чтобы отправить текущее время клиенту.

Цикл `for` позволяет легко видеть, что программа может обслужить двух клиентов перед выходом. Так как пример основан на сопрограммах, повторное выполнение асинхронной операции может быть реализован в цикле `for`. Это улучшает читаемость программы, так как вы не должны отслеживать потенциальные вызовы обработчиков, чтобы выяснить, когда последняя асинхронная операция будет завершена. Если сервер времени должен поддерживать более двух клиентов, нужно модифицировать только цикл `for`.
