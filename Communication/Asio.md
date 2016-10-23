# Сетевое программирование
Даже если Boost.Asio может обрабатывать любые виды данных асинхронно, в основном он используется для сетевого программирования. Это потому, что Boost.Asio поддерживал сетевые функции задолго до того, как были добавлены объекты ввода-вывода. Сетевые функции - идеальное использование для асинхронных операций, потому что передача данных через интернет может занять много времени. Это означает, что информация об успешном завершении передачи или об ошибке могут не быть доступными сразу же, как выполнятся функции, которые отправляют и получают данные.

Boost.Asio предоставляет много объектов ввода-вывода, чтобы разрабатывать сетевые программы. [Пример 32.5](#Пример-325-Веб-клиент-с-boostasioiptcpsocket) использует класс `boost::asio::ip::tcp::socket`, чтобы установить соединение с другим компьютером. Этот пример посылает HTTP-запрос к веб-серверу, чтобы загрузить страницу.

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
[Пример 32.5](#Пример-325-Веб-клиент-с-boostasioiptcpsocket) использует три обработчика (handlers): `connect_handler()` и `read_handler()` вызываются, когда соединение установлено и данные получены. `resolve_handler()` используется для разрешения имён.

Поскольку данные могут быть получены только после того, как соединение было установлено, а так же связь может быть установлена только после того, как имя было разрешено (resolved), различные асинхронные операции запускаются в обработчиках. В `resolve_handler()` итератор **it**, который указывает на конечную точку (endpoint), разрешенную из имени, используется с **tcp_socket** для установления соединения. В `connect_handler()`идёт обращение к **tcp_socket**, чтобы отправить HTTP-запрос и начать прием данных. Так как все операции являются асинхронными, обработчики передаются в соответствующие функции. В зависимости от операций, может понадобиться передать дополнительные параметры. Например, итератор **it** ссылается на конечную точку, разрешенную из имени. Массив **bytes** используется для хранения полученных данных.

В `main()` `boost::asio::ip::tcp::resolver::query` инстанцируется для создания объекта **q**. **q** представляет собой запрос для разрешителя (resolver) имён - объект ввода-вывода, имеющий тип `boost::asio::ip::tcp::resolver`. Передавая **q** в `async_resolve()`, асинхронная операция запускается для разрешения имени. [Пример 32.5](#Пример-325-Веб-клиент-с-boostasioiptcpsocket) разрешает имя `theboostcpplibraries.com`. После запуска асинхронной операции, `run()` вызывается на объекте сервиса ввода-вывода для передачи управления операционной системе.

Когда имя разрешается, вызывается `resolve_handler()`. Обработчик сначала проверяет, было ли разрешение имени успешным. В этом случае **ес** равен 0. Только после этого идёт обращение к сокету, чтобы установить соединение. Адрес сервера для подключения обеспечивается вторым параметром, чей тип `boost::asio::ip::tcp::resolver::iterator`. Этот параметр - результат разрешения имен.

Вызов `async_connect()` сопровождается вызовом обработчика `connect_handler()`. Снова сначала проверяется **ес**, чтобы выяснить, может ли быть установлено соединение. Если так, то `async_read_some()` вызывается на сокете. С помощью этого вызова начинается чтение данных. Полученные данные хранятся в массиве **bytes**, который передается в качестве первого параметра в `async_read_some()`.

`read_handler()` вызывается, когда один или несколько байтов были получены и скопированы в **bytes**. Параметр **bytes_transferred** типа std::size_t содержит число байтов, которые были получены. Как обычно, обработчик должен сначала проверить, **ес** завершена ли асинхронная операция успешно. Только в этом случае данные записываются в стандартный вывод.

Обратите внимание, что `read_handler()` снова вызывает `async_read_some()` после того, как данные были записаны в **std::cout**. Это необходимо, потому что вы не можете быть уверены, что вся домашняя страница была загружена и скопирована в **bytes** одной асинхронной операцией. Повторящиеся вызовы `async_read_some()`, сопровождаемые повторяющтмися вызовами `read_handler()` заканчиваются только тогда, когда соединение закрывается, что происходит, когда веб-сервер отправил всю домашнюю страницу. Затем `read_handler()` сообщает об ошибке в **ес**. С этого момента никакие дополнительных данных не записывается в **std::cout** и `async_read()` не вызывается на сокете. Поскольку нет никаких незавершенных асинхронных операций, программа завершает свою работу.

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
[Пример 32.6](#Пример-326-Сервер-времени-time-server-с-boostasioiptcpacceptor) является сервером времени. Вы можете связаться с telnet-клиентом, чтобы получить текущее время. После этого сервер выключается.

Сервер времени использует объект ввода-вывода `boost::asio::ip::tcp::acceptor`, чтобы принять входящее соединение из другой программы. Вы должны инициализировать объект, поэтому он знает, какой протокол и порт использовать. В этом примере, переменная **tcp_endpoint** типа `boost::asio::ip::tcp::endpoint` используется, чтобы сообщить **tcp_acceptor** принимать входящие соединения 4-й версии протокола Интернет на порту 2014.

После инициализации акцептора (acceptor) вызывается `listen()`, чтобы акцептор начал прослушивание. Затем вызывается `async_accept()`, чтобы принять первую попытку подключения. Разъем должен быть передан первым параметром в `async_accept()`. Он будет использоваться для отправки и приема данных на новом соединении.

Как только другая программа устанавливает соединение, вызывается `accept_handler()`. Если соединение было установлено успешно, текущее время отправляется с `boost::asio::async_write()`. Эта функция записывает все данные в **data** для сокета. `boost::asio::ip::tcp::socket` также предоставляет функцию-член `async_write_some()`. Эта функция вызывает обработчик, когда по крайней мере один байт был послан. Затем обработчик должен проверить, сколько байт было отправлено и сколько еще нужно отправить. Затем он снова должен вызвать `async_write_some()`. Многократное вычисление числа байтов, которых нужно отправить, и вызова `async_write_some()` можно избежать, если использовать `boost::asio::async_write()`. Асинхронная операция, которая началась с этой функцией завершается только тогда, когда все байты в **data** были отправлены.

После отправки данных вызывается `write_handler()`. Эта функция вызывает `shutdown()` с параметром  **boost::asio::ip::tcp::socket::shutdown_send**, который сообщает, что программа закончила отправку данных через сокет. Поскольку нет никаких отложенных асинхронных операций, [Пример 32.6](#Пример-326-Сервер-времени-time-server-с-boostasioiptcpacceptor) завершается. Обратите внимание, что, хотя переменная **data** используется только в `accept_handler()`, она не может быть локальной переменной. **data** передается по ссылке из `boost::asio::buffer()` в `boost::asio::async_write()`. Когда `boost::asio::async_write()` и `accept_handler()` завершаются, асинхронная операция началась, но еще не завершена. **data** должна существовать, пока асинхронная операция не завершена. Если **data** является глобальной переменной, то это гарантировано.

# Сопрограммы (coroutines)
Начиная с версии 1.54.0, Boost.Asio поддерживает сопрограммы. Вы можете использовать Boost.Coroutine напрямую, но явная поддержка сопрограммам в Boost.Asio облегчает их использование.

Сопрограммы позволяют создать структуру, которая отражает реальную логику программы. Асинхронные операции не делят функции, так как нет никаких обработчиков, чтобы определить, что должно произойти, когда асинхронная операция завершается. Вместо вызовов обработчиками друг друга, программа может использовать последовательную структуру.

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
`boost::asio::spawn()` - функция, которую нужно вызывать, для использования сопрограмм с Boost.Asio. Первый передаваемый параметр должен быть объектом сервиса ввода-вывода. Второй параметр - сопрограммная функция. Эта функция должна принимать в качестве единственного параметра объект типа `boost::asio::yield_context`. Она не должен иметь возвращаемого значения. [Пример 32.7](#Пример-327-Сопрограммы-с-boostasio) использует `do_accept()` и `do_write()` как сопрограммы. Если сигнатура функции отличается, как в случае для `do_write()`, вы должны использовать адаптер, например, `std::bind`, или лямбда-функции.

Вместо обработчика вы можете передать объект типа `boost::asio::yield_context` в асинхронную функцию. `do_accept()` передает параметр **yield** в `async_accept()`. В `do_write()` **yield** передается в  `async_write()`. Эти функциональные вызовы по-прежнему начинают асинхронные операции, но не один из обработчиков не будет вызван, когда операция завершится. Вместо этого контекст, в котором были начаты асинхронные операции, восстанавливается. Когда эти асинхронные операции завершатся, программа продолжает с того места, где она была прервана.

`do_accept()` содержит цикл `for`. Новый сокет передается `async_accept()` каждый раз, когда функция вызывается. Когда клиент устанавливает соединение, вызывается `do_write()` как сопрограмма с `boost::asio::spawn()`, чтобы отправить текущее время клиенту.

Цикл `for` позволяет легко видеть, что программа может обслужить двух клиентов перед выходом. Так как пример основан на сопрограммах, повторное выполнение асинхронной операции может быть реализован в цикле `for`. Это улучшает читаемость программы, так как вы не должны отслеживать потенциальные вызовы обработчиков, чтобы выяснить, когда последняя асинхронная операция будет завершена. Если сервер времени должен поддерживать более двух клиентов, нужно модифицировать только цикл `for`.

# Платформо-зависимые объекты ввода-вывода
До сих пор все примеры, приведенные в этой главе, были независимыми от платформы. Объекты ввода-вывода, такие как `boost::asio::steady_timer` и `oost::asio::ip::tcp::socket`, поддерживаются на всех платформах. Тем не менее Boost.Asio также предоставляет платформо-зависимые объекты ввода-вывода, поскольку некоторые асинхронные операции доступны только на определенных платформах, например, Windows или Linux.

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
[Пример 32.8](#Пример-328-Использование-boostasiowindowsobject_handle) использует объект ввода-вывода `boost::asio::windows::object_handle`, который доступен только на Windows. Объект `boost::asio::windows::object_handle`, который основан на Windows-функции `RegisterWaitForSingleObject()`, позволяет запускать асинхронные операции для ручек (handle) объекта. Все ручки, принятые `RegisterWaitForSingleObject(),` могут использоваться с `boost::asio::windows::object_handle`. С `async_wait()` возможно асинхронно ожидать, пока не изменится ручка объекта.

[Пример 32.8](#Пример-328-Использование-boostasiowindowsobject_handle) инициализирует объект **obj_handle** типа `boost::asio::windows::object_handle` с ручкой, созданной с помощью Windows-функции `CreateEvent()`. Ручка является частью структуры `OVERLAPPED`, чей адрес передается Windows-функции `ReadDirectoryChangesW()`. Окна использует структуры `OVERLAPPED` для запуска асинхронных операций.

Функция `ReadDirectoryChangesW()` может быть использована для слежения за каталогом и ожидания изменений в нём. Для асинхронного вызова функции структура `OVERLAPPED` должна быть передана `ReadDirectoryChangesW()`. Чтобы сообщить о завершении асинхронной операции через Boost.Asio, обработчик событий сохраняется в структуре `OVERLAPPED` до того, как он передается `ReadDirectoryChangesW()`. Впоследствии этот обработчик события передается **obj_handle**. Когда `async_wait()` вызывается на **obj_handle**, обработчик выполняется при обнаружении изменения в наблюдаемой директории.

При запуске [Пример 32.8](#Пример-328-Использование-boostasiowindowsobject_handle), создайте новый файл в каталоге, из которого будет запущен пример. Программа обнаружит новый файл и напишет сообщение в стандартный поток вывода.

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
[Пример 32.9](#Пример-329-Использование-boostasiowindowsoverlapped_ptr), как и предыдущий, использует `ReadDirectoryChangesW()`, чтобы следить за каталогом. На этот раз, асинхронный вызов `ReadDirectoryChangesW()` не связан с обработчиком событий. В примере используется класс `boost::asio::windows::overlapped_ptr`, который в своей реализации использует структуру `OVERLAPPED`. `get()` возвращает указатель на внутреннюю структуру `OVERLAPPED`. В примере указатель затем передается в `ReadDirectoryChangesW()`.

`boost::asio::windows::overlapped_ptr` это объект ввода-вывода, который не имеет функции-члена, чтобы начать асинхронную операцию. Асинхронная операция запускается, когда передаётся указатель на внутреннюю переменную `OVERLAPPED` в Windows-функцию. В дополнение к объекту сервиса ввода-вывода, конструктор `boost::asio::windows::overlapped_ptr` ожидает обработчик, который будет вызываться, когда асинхронная операция завершается.

[Пример 32.9](#Пример-329-Использование-boostasiowindowsoverlapped_ptr) использует `boost::asio::use_service()`, чтобы получить ссылку на службу в объекте службы ввода-вывода **ioservice**. `boost::asio::use_service()` представляет собой шаблон функции. Тип службы ввода-вывода, который вы хотите извлечь, должен быть передан в качестве параметра шаблона. В примере повышение передается `boost::asio::detail::io_service_impl`. Этот тип сервиса ввода-вывода самый близкий к операционной системе. В операционной системе Windows `boost::asio::detail::io_service_impl` использует IOCP, а на Linux использует `epoll()`. `boost::asio::detail::io_service_impl` - это определение типа, который устанавливается для `boost::asio::detail::win_iocp_io_service` на Windows и `boost::asio::detail::task_io_service` на Linux.

`boost::asio::detail::win_iocp_io_service` предоставляет функцию-член `register_handle()`, чтобы связать ручку с IOCP-ручкой. `register_handle()` вызывает Windows-функцию `CreateIoCompletionPort()`. Этот вызов необходим для примера, чтобы jy работал правильно. Ручка, возвращаемая `CreateFileA()`, может быть передана через **overlapped** в `ReadDirectoryChangesW()` только после того, как он будет связан с IOCP-ручкой.

[Пример 32.9](#Пример-329-Использование-boostasiowindowsoverlapped_ptr) проверяет, удачно ли завершилась `ReadDirectoryChangesW()`. Если `ReadDirectoryChangesW()` завершилась с ошибкой, вызывается `complete()` на **overlapped**, чтобы завершенить асинхронную операцию для Boost.Asio. Параметры, передаваемые для `complete()` перенаправляются к обработчику.

Если `ReadDirectoryChangesW()` завершается успешно, вызывается `release()` . Далее асинхронная операция находится в ожидании и завершается только после завершения операции, которая была инициирована с помощью Windows-функции `ReadDirectoryChangesW()`.

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
[Пример 32.10](#Пример-3210-Использование-boostasioposixstream_descriptor) представляет объект ввода-вывода для POSIX платформ.

`boost::asio::posix::stream_descriptor` может быть инициализирован с дескриптора файла, чтобы начать асинхронную операцию для данного файлового дескриптора. В примере, **stream** связан с дескриптор файла `STDOUT_FILENO`, чтобы записать строку асинхронно в стандартный поток вывода.
