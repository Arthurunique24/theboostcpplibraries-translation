# Глава 33. Boost.Interprocess #

Содержание:

[Совместно-используемая память](#Совместно-используемая-память)  
[Управление совместно-используемой памятью](#Управляемая-совместная-память)  
[Синхронизация](#Синхронизация)

Interprocess описывает механизмы для обмена данными между программами, работающими на одном и том же компьютере. Не 
включает в себя сетевую связь. Для обмена данными между программами в одной сети, смотрите [Главу 32](https://theboostcpplibraries.com/boost.asio "Глава 32. Boost.Asio"), которая описывает Boost.Asio.

В этой главе представлена библиотека [Boost.Interprocess](http://www.boost.org/doc/libs/1_62_0/doc/html/interprocess.html), которая содержит многочисленные классы, которые абстрагируют специальные интерфейсы операционной системы для межпроцессного взаимодействия. Несмотря на то, что концепции межпроцессного взаимодействия одинаковы среди разных операционных систем, интерфейсы могут сильно различаться. Boost.Interprocess обеспечивает не зависимый от платформы доступ.

Boost.Asio может использоваться для обмена данными между процессами, проходящими на одном компьютере, однако Boost.Interprocess 
обеспечивает более высокую производительность. Boost.Interprocess вызывает функции операционной системы, оптимизированные для обмена данными между процессами, проходящими на одном компьютере. Таким образом, Boost.Interprocess - лучший выбор для обмена данными без использования сети.

# Совместно используемая память #

Общая память, как правило, самая быстрая форма межпроцессного взаимодействия. Под этим понятием подразумевается область памяти, которая разделяется между процессами. Один процесс может записывать данные в этой области, а другой процесс может читать его.

В Boost.Interprocess класс `boost::interprocess::shared_memory_object` используется для представления общей памяти. Чтобы использовать этот класс необходимо подключить заголовочный файл `boost/interprocess/shared_memory_object.hpp`.

#### Пример 33.1. Создание общей памяти
```c++
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object shdmem{open_or_create, "Boost", read_write};
  shdmem.truncate(1024);
  std::cout << shdmem.get_name() << '\n';
  offset_t size;
  if (shdmem.get_size(size))
    std::cout << size << '\n';
}
```


Конструктор `boost::interprocess::shared_memory_object` принимает три параметра. Первый параметр определяет открытие или создание общей памяти. В [Примере 33.1](#Пример-331-Создание-общей-памяти) 
работают оба варианта: `boost::interprocess::open_or_create` откроет общую память, если она уже существует или создаст ее в ином  случае. 

Открытие существующей совместной памяти предполагает, что она была создана ранее. Для того, чтобы однозначно определить совместно-используемую  память, ей присваивается имя, которое передается вторым параметром в конструктор `boost::interprocess::shared_memory_object`.

Третий параметр определяет, каким образом процесс может получить доступ к памяти. В [Примере 33.1](#Пример-331-Создание-общей-памяти) 
`boost::interprocess::read_write` показывает, что процесс имеет доступ к чтению и записи.

После создания объекта типа `boost::interprocess::shared_memory_object` соответствующий общий блок памяти будет существовать внутри операционной системы. Изначально размер этой памяти 0. Чтобы использовать эту область, вызовите `truncate()`, передав размер памяти в байтах. В [Примере 33.1](#Пример-331-Создание-общей-памяти) общая память обеспечивает область, размером 1024 байта. `truncate()` может быть вызвана только в том случае, если совместная память была открыта с использованием `boost::interprocess::read_write`. В ином случае бросается исключение с типом `boost::interprocess::interprocess_exception`. `truncate()` можно вызывать для регулирования размера выделенной памяти.

После создания общей памяти, могут быть вызваны методы, такие как `get_name()` и `get_size()`, для получения имени или размера этой памяти.

Поскольку совместная память используется для обмена информацией между разными процессами, каждый из них должен отобразить? общую память в свое адресное пространство. Для этого используется класс `boost::interprocess::mapped_region`. Неожиданностью может оказаться то, что для доступа к совместной памяти понадобятся два класса (`boost::interprocess::shared_memory_object` и `boost::interprocess::mapped_region`). Это сделано для того, чтобы класс `boost::interprocess::mapped_region` мог также быть использован для отображения других объектов в адресное пространство процесса.

#### Пример 33.2. Отображение общей памяти в адресное пространство процесса
```c++
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object shdmem{open_or_create, "Boost", read_write};
  shdmem.truncate(1024);
  mapped_region region{shdmem, read_write};
  std::cout << std::hex << region.get_address() << '\n';
  std::cout << std::dec << region.get_size() << '\n';
  mapped_region region2{shdmem, read_only};
  std::cout << std::hex << region2.get_address() << '\n';
  std::cout << std::dec << region2.get_size() << '\n';
}
```


Для использования класса `boost::interprocess::mapped_region` необъходимо подключить заголовочный файл `boost/interprocess/mapped_region.hpp`. В качестве первого параметра конструктора `boost::interprocess::mapped_region` должен быть передан объект типа  `boost::interprocess::shared_memory_object` Второй параметр определяет степень доступа к памяти (read-only или read-write).

В [Примере 33.2](#Пример-332-Отображение-общей-памяти-в-адресное-пространство-процесса) создается два объекта типа `boost::interprocess::mapped_region`. Общая память с именем Boost отображается дважды в адресное пространство процесса. Адрес и размер отображенной области памяти записываются в стандартный поток вывода с помощью методов `get_address()` и `get_size()`. `get_size()` возвращает 1024 в обоих случаях, но возвращаемое значение `get_address()` отличается для каждого объекта.

####Примечание

[Пример 33.2](#Пример-332-Отображение-общей-памяти-в-адресное-пространство-процесса) и еще несколько примеров, приведенных ниже вызовут ошибку при компиляции в  Visual C++ 2013 и Boost 1.55.0(исправлена в Boost 1.56.0). Этот баг описан в [тикете 9332](https://svn.boost.org/trac/boost/ticket/9332).

#### Пример 33.3. Запись и чтение числа в общей памяти
```c++
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object shdmem{open_or_create, "Boost", read_write};
  shdmem.truncate(1024);
  mapped_region region{shdmem, read_write};
  int *i1 = static_cast<int*>(region.get_address());
  *i1 = 99;
  mapped_region region2{shdmem, read_only};
  int *i2 = static_cast<int*>(region2.get_address());
  std::cout << *i2 << '\n';
}
```
В [Примере 33.3](#Пример-333-Запись-и-чтение-числа-в-общей-памяти) используется отображенная область памяти для записи и чтения числа.  ***region*** записывает число `99` в начало области. ***region2*** читает соответствующее место в этой области и выводит число в стандартный поток вывода. Несмотря на то, что ***region*** и ***region2*** представляют различные области памяти в процессе, как видно в возвращаемом значении `get_address()` в предыдущем примере, программа напечатает `99`, поскольку обе области памяти имеют доступ к одной  совместной памяти.

#### Пример 33.4. Удаление совместно-используемой памяти
```c++
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  bool removed = shared_memory_object::remove("Boost");
  std::cout << std::boolalpha << removed << '\n';
}
```
Для удаления общей памяти, в `boost::interprocess::shared_memory_object` есть функция-член `remove()`, принимающая имя этой памяти в качестве параметра (см. [Пример 33.4](#Пример-334-Удаление-совместно-используемой-памяти)).

Boost.Interprocess частично поддерживает идиому RAII через класс `boost::interprocess::remove_shared_memory_on_destroy`. В конструкторе принимается имя существующей памяти, и, если объект этого класса был разрушен, совместная память удаляется (в деструкторе этого объекта).

Конструктор `boost::interprocess::remove_shared_memory_on_destroy` не создает и не открывает общую память. Таким образом он не является типичным представителем RAII.

Если не вызывается `remove()`, совместная память не удаляется, даже при завершении программы. Удаляется память автоматически или нет 
зависит от операционной системы. Windows и большинство Unix операционных систем автоматически удаляю память при перезагрузке.

Windows обеспечивает особый вид совместной памяти, которая автоматически удаляется при завершении последнего процесса, работающего с ней. Чтобы ее использовать, нужно работать с классом `boost::interprocess::windows_shared_memory`, описанным в `boost/interprocess/windows_shared_memory.hpp`(см. [Пример 33.5](#Пример-335-Использование-специальной-совместной-памяти-windows)). 

#### Пример 33.5. Использование специальной совместной памяти Windows
```c++
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  windows_shared_memory shdmem{open_or_create, "Boost", read_write, 1024};
  mapped_region region{shdmem, read_write};
  int *i1 = static_cast<int*>(region.get_address());
  *i1 = 99;
  mapped_region region2{shdmem, read_only};
  int *i2 = static_cast<int*>(region2.get_address());
  std::cout << *i2 << '\n';
}
```
В `boost::interprocess::windows_shared_memory` нет функции `truncate()`, вместо этого размер памяти передается в конструктор 4-м параметром.

Несмотря на то, что этот класс может использоваться только на Windows, он полезен, когда данные нужно разделить между процессами, использующими такой же специальный тип памяти.

# Управляемая совместная память

В предыдущем разделе был рассмотрен класс `boost::interprocess::shared_memory_object`, который используют для создания и управления общей памятью. На деле этот класс используется редко, так как для него необходимы программы, читающие и записывающие отдельные байты из и в совместную память. Стиль C++ способствует созданию объектов классов, скрывая специфику, где и как данные хранятся в памяти.

Boost.Interprocess предоставляет `boost::interprocess::managed_shared_memory` - класс, описанный в `boost/interprocess/managed_shared_memory.hpp`. Он используется для поддержки управляемой совместной памяти. Этот класс позволяет создавать объекты, хранящиеся в общей памяти, что делает эти объекты доступными для любого процесса, имеющего доступ к ней.

#### Пример 33.6. Использование управляемой общей памяти
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object::remove("Boost");
  managed_shared_memory managed_shm{open_or_create, "Boost", 1024};
  int *i = managed_shm.construct<int>("Integer")(99);
  std::cout << *i << '\n';
  std::pair<int*, std::size_t> p = managed_shm.find<int>("Integer");
  if (p.first)
    std::cout << *p.first << '\n';
}
```
[Пример 33.6](#Пример-336-Использование-управляемой-общей-памяти) открывает совместно-используемую память с именем Boost размером 1024 байта. Если ее не существовало, она создастся автоматически.

В обычной общей памяти отдельные байты доступны непосредственно для чтения или записи. Управляемая совместная память использует методы, такие, как `construct()`, которые принимают шаблонные параметры (в [Примере 33.6](#Пример-336-Использование-управляемой-общей-памяти) - `int`). Метод принимает имя объекта, созданного в общей памяти. [Пример 33.6](#Пример-336-Использование-управляемой-общей-памяти) использует имя Integer.

Поскольку `construct()` возвращает прокси объект, параметры могут передаваться для инициализации созданного объекта. Синтаксис выглядит, как вызов конструктора. Это гарантирует, что объекты могут быть созданы и инициализированы в управляемой общей памяти.

Чтобы получить доступ к конкретному объекту в управляемой совместной памяти используется метод `find()`. После передачи имени объекта, который требуется найти, в качестве параметра, `find()` возвращает указатель на этот объект или 0, если он не был найден.

Как видно в [Примере 33.6](#Пример-336-Использование-управляемой-общей-памяти), `find()` возвращает объект типа `std::pair`. указатель на объект лежит в **first**. [Пример 33.7](#Пример-337-Создание-массивов-в-совместной-памяти) показывает, что лежит в **second**.

#### Пример 33.7. Создание массивов в совместной памяти
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object::remove("Boost");
  managed_shared_memory managed_shm{open_or_create, "Boost", 1024};
  int *i = managed_shm.construct<int>("Integer")[10](99);
  std::cout << *i << '\n';
  std::pair<int*, std::size_t> p = managed_shm.find<int>("Integer");
  if (p.first)
  {
    std::cout << *p.first << '\n';
    std::cout << p.second << '\n';
  }
}
```
В [Примере 33.7](#Пример-337-Создание-массивов-в-совместной-памяти) массив с десятью элементами типа `int` создается, путем записи в квадратные скобки числа `10` после вызова `construct()`. Это же число выведется в стандартный поток вывода, путем использования переменной **second**. Благодаря этой переменной вы можете узнать, найденные `find()` объекты являются единичными (тогда в **second** будет записана цифра 1) или представляют собой массивы. 

Обратите внимание, что все десять элементов в массиве инициализируются значением 99. Чтобы инициализировать переменные с разными значениями, нужно передать итератор. 

`construct()` не сработает, если объект с данным именем уже существует в управляемой общей памяти. В этом случае `construct()` вернет 0, и инициализации не произойдет. Чтобы использовать существующий объект, используйте метод `find_or_construct()`, который возвращает указатель на существующий объект или создает новый.

#### Пример 33.8. Исключение типа `boost::interprocess::bad_alloc`
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  try
  {
    shared_memory_object::remove("Boost");
    managed_shared_memory managed_shm{open_or_create, "Boost", 1024};
    int *i = managed_shm.construct<int>("Integer")[4096](99);
  }
  catch (boost::interprocess::bad_alloc &ex)
  {
    std::cerr << ex.what() << '\n';
  }
}
```
Есть и другие случаи, при которых `construct()` не сработает. [Пример 33.8](#Пример-338-Исключение-типа-boostinterprocessbad_alloc) пытыется создать массив типа `int` с 4096-ю элементами. Управляемая совместная память при этом вмещает только 1024 байта. Это вызывает исключение типа ` boost::interprocess::bad_alloc`.

После того, как объекты были созданы в управляемой общей памяти, они могут быть разрушены с помощью метода `destroy()`.


#### Пример 33.9. Удаление объектов в совместно используемой памяти
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object::remove("Boost");
  managed_shared_memory managed_shm{open_or_create, "Boost", 1024};
  int *i = managed_shm.find_or_construct<int>("Integer")(99);
  std::cout << *i << '\n';
  managed_shm.destroy<int>("Integer");
  std::pair<int*, std::size_t> p = managed_shm.find<int>("Integer");
  std::cout << p.first << '\n';
}
```

В [Примере 33.9](#Пример-339-Удаление-объектов-в-совместно-используемой-памяти), имя объекта, который нужно удалить, передается в качестве единственного параметра в метод `destroy()`. По возвращаемому значению типа `bool` можно проверить, был ли найден данный объект и успешно ли удален. Поскольку объект всегда будет удален, если он найден, возвращаемое значение `false` указывает на то, что ни один объект с указанным именем не был найден.

Метод `destroy_ptr()` может быть использован для передачи указателя на объект в управляемой совместно-используемой памятью. Он также может быть использован для удаления массивов.

Поскольку управляемая совместно-используемая память позволяет достаточно легко обмениваться объектами между процессами, представляется естественным так же использовать и контейнеры из стандартной библиотеки. Тем не менее, эти контейнеры выделяют памят с использованием оператора `new()`. Для того, чтобы использовать эти контейнеры в управляемом совместно используемой памяти, им должно быть указано, что выделение пямяти должно происходить в расшаренной(!) памяти.

Многие реализации стандартной библиотеки не являются достаточно гибкими, чтобы использовать контейнеры, такие как `std::string` или `std::list` с **Boost.Interprocess**. Речь идет о таких реализациях, как в Visual C ++ 2013, GCC и Clang.

Для того, чтобы позволить разработчикам использовать контейнеры из стандартной библиотеки, Boost.Interprocess обеспечивает более гибкую реализацию в пространстве имен `boost::interprocess`. Например, `boost::interprocess::string` действует точно так же, как и его аналог `std::string`, за исключением того, что строки могут безопасно храниться в управляемой совместно-используемой памяти [Пример 33.10](#Пример-3310-Размещение-строки-в-общую-память).


#### Пример 33.10. Размещение строки в общую память
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  shared_memory_object::remove("Boost");
  managed_shared_memory managed_shm{open_or_create, "Boost", 1024};
  
  typedef allocator<char,
    managed_shared_memory::segment_manager> CharAllocator;
  typedef basic_string<char, std::char_traits<char>, CharAllocator> string_t;
  
  string_t *s = managed_shm.find_or_construct<string_t>("String")("Hello!",
    managed_shm.get_segment_manager());
    
  s->insert(5, ", world");
  std::cout << *s << '\n';
}
```

Чтобы создать строку, которая будет выделять память в той же управляемой совместно используемой памяти, в которой она находится, должен быть определен соответствующий тип. Новый тип-строка должен использовать аллокатор, предоставляемый Boost.Interprocess, вместо аллокатора по умолчанию, предоставляемый стандартом.

Для этой цели Boost.Interprocess обеспечивает класс `boost::interprocess::allocator`, который определен в `boost/interprocess/allocators/allocator.hpp`. С помощью этого класса может быть создан распределитель памяти, который внутри использует менеджер сегментов управляемой совместно используемой памяти. Менеджер сегментов отвечает за управление памятью в пределах разделенной памяти блока. С помощью нового созданного аллокатора можно определить соответствующий тип строки. Но опять же, как было указано выше, используйте `boost::interprocess::basic_string` вместо `std::basic_string`. Новый тип (string_t) в [примере 33.10](#Пример-3310-Размещение-строки-в-общую-память) - основан на `boost::interprocess::basic_string` и обращается к менеджеру сегментов через свой распределитель. Чтобы позволить конкретному экземпляру, созданного вызовом `find_or_construct()`, знать, к какому сегмент-менеджеру он должен получить доступ, нужно передать указатель на соответствующий менеджер сегмента в качестве второго параметра конструктора.

Boost.Interprocess предоставляет реализации для многих других контейнеров из стандартной библиотеки. Например, `boost::interprocess::vector` и `boost::interprocess::map` определены в `boost/interprocess/containers/vector.hpp`  и `boost/interprocess/containers/map.hpp` соответственно.

Пожалуйста, обратите внимание, что контейнеры из **Boost.Container** поддерживают **Boost.Interprocess** и могут быть помещены в общую память. Они могут быть использованы вместо контейнеров от boost::interprocess. **Boost.Container** описан в [Главе 20](https://theboostcpplibraries.com/boost.container "Boost.Container").

Всякий раз, когда управляемая совместно используемая память доступна из различных процессов, такие операции, как создание, поиск и уничтожение объектов, автоматически синхронизируются. Если две программы пытаются создавать объекты с разными именами в управляемой совместно используемой памяти, доступ сериализуется соответствующим образом. Для одновременного выполнения нескольких операций, не прерываясь операциями из другого процесса, используйте метод `atomic_func()` ([пример 33.11](#Пример-3311-Атомарныйный-доступ-на-управляемой-совместно-используемой-памяти)).


#### Пример 33.11. Атомарныйный доступ на управляемой совместно используемой памяти
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <functional>
#include <iostream>

using namespace boost::interprocess;

void construct_objects(managed_shared_memory &managed_shm)
{
  managed_shm.construct<int>("Integer")(99);
  managed_shm.construct<float>("Float")(3.14);
}

int main()
{
  shared_memory_object::remove("Boost");
  managed_shared_memory managed_shm{open_or_create, "Boost", 1024};
  auto atomic_construct = std::bind(construct_objects,
    std::ref(managed_shm));
  managed_shm.atomic_func(atomic_construct);
  std::cout << *managed_shm.find<int>("Integer").first << '\n';
  std::cout << *managed_shm.find<float>("Float").first << '\n';
}
```
`atomic_func()` принимает в качестве единственного параметра функцию, которая не принимает никаких параметров и не имеет возвращаемого значения. Передаваемая функция будет вызываться таким образом, что обеспечивается исключительный доступ к управляемой совместно используемой памяти. Тем не менее, исключительный доступ обеспечивается только тогда, когда все другие процессы, которые имеют доступ к управляемой совместно используемой памяти, также используют `atomic_func()`. Если другой процесс имеет указатель на объект в управляемой совместно используемой памяти, он может получить доступ к объекту и изменять его, используя свой указатель.

**Boost.Interprocess** также может быть использован для синхронизации доступа к объектам. Так как **Boost.Interprocess** не знает, кто может получить доступ к отдельным объектам в конкретный момент времени, синхронизация должна быть явно обработана. В следующем разделе представлены классы, предусмотренные для синхронизации.



# Синхронизация

**Boost.Interprocess** позволяет нескольким процессам использовать общую памяти одновременно. Поскольку общая память, по определению, общая именно между процессами, **Boost.Interprocess** должен поддерживать какую-либо синхронизацию.

Думая о синхронизации, на ум сразу приходят классы C++11 стандартной библиотеки или **Boost.Thread**. Но эти классы могут использоваться только для синхронизации потоков в одном процессе - они не поддерживают синхронизацию различных процессов. Но поскольку проблема в обоих случаях одинакова, концепции решений так же похожи.

Во время синхронизации объекты, такие как мьютексы (`mutex`) и условные переменные (`condition_variable`), находятся в одном адресном пространстве в многопоточных приложениях и поэтому доступны для всех потоков, проблема с общей памятью как раз заключается в том, что независимые процессы должны совместно использовать эти объекты. Например если один процесс создает мьютекс, он каким-то образом должен быть доступен из другого процесса.

**Boost.Interprocess** предоставляет два вида объектов синхронизации: 
    1) анонимные объекты непосредственно хранятся в общей памяти, что делает их автоматически доступными для всех процессов.
    2) именованные объекты управляются операционной системой, не хранятся в общей памяти и могут предоставить ссылку программам по имени.
    
#### Пример 33.12. Использование именного мьютекса с помощью `boost::interprocess::named_mutex`
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  managed_shared_memory managed_shm{open_or_create, "shm", 1024};
  int *i = managed_shm.find_or_construct<int>("Integer")();
  named_mutex named_mtx{open_or_create, "mtx"};
  named_mtx.lock();
  ++(*i);
  std::cout << *i << '\n';
  named_mtx.unlock();
}    
```

В [примере 33.12](#Пример-3312-Использование-именного-мьютекса-с-помощью-boostinterprocessnamed_mutex) создается и используется именованный мьютекс с помощью класса `boost::interprocess::named_mutex`, который определяется в `boost/interprocess/sync/named_mutex.hpp.`

Конструктор `boost::interprocess::named_mutex` принимает параметр, указывающий, следует ли создать или открыть мьютекс, и имя мьютекса. Каждый процесс, который знает это имя, может открыть этот же мьютекс. Для доступа к данным в общей памяти, программа должна заблокировать mutex путем вызова функции метода `lock()` . Потому что мьютексы могут быть во владении только одного процесса, пока другой будет ждать его освобождения (методом `unlock()`). После того, как процесс получает владение мьютексом, он имеет монопольный доступ к мьютекс-ресурсу. В [примере 33.12](#Пример-3312-Использование-именного-мьютекса-с-помощью-boostinterprocessnamed_mutex) ресурс является переменной типа `int`, который увеличивается и записывается в стандартный поток вывода.

Если образец программы запустить несколько раз, каждый экземпляр будет печатать значение, увеличенное на 1, по сравнению с предыдущим значением. Благодаря mutex, доступ к общей памяти и к самой переменной синхронизированы между различными процессами.

#### Пример 33.13. Использование анонимного мьютекса с помощью  `interprocess_mutex`
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  managed_shared_memory managed_shm{open_or_create, "shm", 1024};
  int *i = managed_shm.find_or_construct<int>("Integer")();
  interprocess_mutex *mtx =
    managed_shm.find_or_construct<interprocess_mutex>("mtx")();
  mtx->lock();
  ++(*i);
  std::cout << *i << '\n';
  mtx->unlock();
}
```

В [Примере 33.13](#Пример-3313-Использование-анонимного-мьютекса-с-помощью--interprocess_mutex
) используется анонимный мьютекс типа `boost::interprocess::interprocess_mutex`, который определяется в `boost/interprocess/sync/interprocess_mutex.hpp`. Для того, чтобы мьютекс был доступным для всех процессов, он хранится в общей памяти.

[Пример 33.13](#Пример-3313-Использование-анонимного-мьютекса-с-помощью--interprocess_mutex
) ведет себя точно также, как и предыдущий. Единственным отличием является mutex, который теперь хранятся в общей памяти. Это можно сделать методами `construct()` или `find_or_construct()` из класса `boost::interprocess::managed_shared_memory`.

В дополнение к `lock()` классы  `boost::interprocess::named_mutex` и `boost::interprocess::interprocess_mutex` предоставляют методы `try_lock()` и `timed_lock()`. Они ведут себя так же, как их коллеги в стандартной библиотеке и **Boost.Thread**. Если требуются рекурсивные мьютексы, **Boost.Interprocess** предоставляет два класса: `boost::interprocess::named_recursive_mutex` и `boost::interprocess::interprocess_recursive_mutex`.

В то время как мьютексы гарантируют эксклюзивный доступ к общему ресурсу, условные переменые контролируют, кто имеет эксклюзивный доступ и в какое время. В целом условные переменые, предоставляемые **Boost.Interprocess** работают, так же, как и аналоги в C++11 **std** и **Boost.Thread**. Они имеют схожий интерфейс, так что пользователи этих библиотек сразу почувствуют себя в родной среде при использовании этих переменных в **Boost.Interprocess**.

#### Пример 33.14. Использование именованного условия с помощью `boost::interprocess::named_condition`
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  managed_shared_memory managed_shm{open_or_create, "shm", 1024};
  int *i = managed_shm.find_or_construct<int>("Integer")(0);
  named_mutex named_mtx{open_or_create, "mtx"};
  named_condition named_cnd{open_or_create, "cnd"};
  scoped_lock<named_mutex> lock{named_mtx};
  while (*i < 10)
  {
    if (*i % 2 == 0)
    {
      ++(*i);
      named_cnd.notify_all();
      named_cnd.wait(lock);
    }
    else
    {
      std::cout << *i << std::endl;
      ++(*i);
      named_cnd.notify_all();
      named_cnd.wait(lock);
    }
  }
  named_cnd.notify_all();
  shared_memory_object::remove("shm");
  named_mutex::remove("mtx");
  named_condition::remove("cnd");
}
```

В [Примере 33.14](#Пример-3314-Использование-именованного-условия-с-помощью-boostinterprocessnamed_condition) используется условная переменная типа `boost::interprocess::named_condition`, который определяется в `boost/interprocess/sync/named_condition.hpp`. Эта переменная не обязана хранится в общей памяти, так как она именованная.

Приложение использует while-цикл для увеличения значения переменной типа `int`, который хранится в общей памяти. Хотя переменная увеличивается с каждой итерацией цикла, в поток вывода будет записано только каждое нечетное число.

Каждый раз, когда переменная увеличивается на 1, вызывается метод `wait()` условной переменной *named_cnd*. Объект *lock* ( в [примере 33.14](#Пример-3314-Использование-именованного-условия-с-помощью-boostinterprocessnamed_condition)), передается в этот метод. Это следует идиоме RAII - владеть мьютексом внутри конструктора и освободить его деструктором.

 Объект *lock* создается перед while-циклом и принимает владение мьютексом для полного выполнения программы. Однако если его передать методу `wait()` в качестве параметра, блокировка автоматически снимается.
 
Условные переменнные используются для ожидания сигнала, указывающего, что ожидание окончено. Синхронизация находится под контролем методов `wait()` и `notify_all()`. Когда программа вызывает `wait()`, соответствующий мьютекс освобождается. Затем программа ожидает, пока `notify_all()` не будет вызвана на этой же переменной состояния.

 После увеличения переменной *i* на единицу с помощью while-цикла, программа ожидает сигнала путем вызова `wait()`. Для того, чтобы сигнал "выстрелил", второй экземпляр программы должен быть запущен.

Второй экземпляр пытается завладеть мьютексом перед входом в while-цикл. Это выполняется успешно, поскольку первый экземпляр освобождает мьютекс путем вызова `wait()`. Поскольку переменная *i* увеличена один раз, второй экземпляр выполняет else-ветвь if-выражения и записывает текущее значение в стандартный поток вывода. Затем значение увеличивается на 1.

Теперь второй экземпляр также вызывает `wait()`. Однако перед этим, он вызывает `notify_all()`, который гарантирует, что два экземпляра синхронизированы. Первый экземпляр получает уведомление и пытается получить власть над мьютексом, который по-прежнему принадлежит второму процессу. Однако поскольку второй экземпляр вызывает `wait()` сразу же после вызова метода `notify_all()`, который автоматически освобождает владение, первый экземпляр возьмет на себя ответственность в этот момент.

Оба экземпляра чередуются, инкрементируя значения переменной в общей памяти. Однако только один экземпляр записывает значение в стандартный поток вывода. Как только переменная достигает значения 10, while-цикл закончен. Для того, чтобы избежать вечного ожидания сигнала другим экземпляром, `notify_all()` вызывается еще раз после цикла. Перед завершением, общая память, мьютекс и условная переменная будут уничтожены.

Итак, есть два типа мьютекса-анонимный тип, который должен храниться в общей памяти, и именованный тип, а также два типа условных переменных. Пример [33.15](#Пример-3315-Использование-анонимной-условной-переменной-с-помощью-interprocess_condition) - то же самое, что и предыдущий пример, но с использованием анонимной условной переменной.

#### Пример 33.15. Использование анонимной условной переменной с помощью `interprocess_condition`
```c++
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <iostream>

using namespace boost::interprocess;

int main()
{
  managed_shared_memory managed_shm{open_or_create, "shm", 1024};
  int *i = managed_shm.find_or_construct<int>("Integer")(0);
  interprocess_mutex *mtx =
    managed_shm.find_or_construct<interprocess_mutex>("mtx")();
  interprocess_condition *cnd =
    managed_shm.find_or_construct<interprocess_condition>("cnd")();
  scoped_lock<interprocess_mutex> lock{*mtx};
  while (*i < 10)
  {
    if (*i % 2 == 0)
    {
      ++(*i);
      cnd->notify_all();
      cnd->wait(lock);
    }
    else
    {
      std::cout << *i << std::endl;
      ++(*i);
      cnd->notify_all();
      cnd->wait(lock);
    }
  }
  cnd->notify_all();
  shared_memory_object::remove("shm");
}
```

[Пример 33.15](#Пример-3315-Использование-анонимной-условной-переменной-с-помощью-interprocess_condition) работает точно как предыдущий и также должен быть запущен дважды, чтобы увеличить значение переменной `int` в на десять.

Кроме мьютексов и условных переменных, **Boost.Interprocess** также поддерживает семафоры и блокировки файлов. Семафоры похожи на условные переменные, за исключением того, что они не различаются состояниями -  вместо этого они основаны на счетчике. Блокировки файлов ведут себя как мьютексы, за исключением того, что они используются с файлами на жестком диске, а не с объектами в памяти.

Так же,как Стандартная библиотека С++11 и **Boost.Thread** предоставляют разные типы мьютексов и блокировок, **Boost.Interprocess** предоставляет несколько мьютексов и блокировок. Например, мьютексы могут находиться во владении взаимоисключающим образом или невзаимоисключающим. Это полезно, если несколько процессов одновременно считывают данные, так как взаимоисключающий мьютекс требуется только для записи данных. Для применения идиомы RAII к индивидуальным мьютексам доступны различные классы для блокировок.

Имена должны быть уникальными, если не используются объекты анонимной синхронизации. Несмотря на то, что мьютексы и условные переменные являются объектами, основанные на различных классах, это не обязательно справедливо для зависимых интерфейсов операционной системы, которые использует **Boost.Interprocess**. В Windows для мьютексов и условных переменных используются одни и те же функции операционной системы. Если же одинаковые имена используются для двух объектов разных типов, программа будет  **некорректно** вести себя в в ОС Windows.