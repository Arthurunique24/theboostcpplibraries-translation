# Глава 29. Boost.Algorithm

Boost.Algorithm обеспечивает алгоритмы, которые дополняют алгоритмы стандартной библиотеки. В отличие от Boost.Range, Boost.Algorithm не вводит новые понятия. Алгоритмы, определенные в Boost.Algorithm напоминают алгоритмы из стандартной библиотеки.

Обратите внимание, что другие библиотеки Boost содержат огромное число алгоритмов. Например, алгоритмы для обработки строк есть в Boost.StringAlgorithms. Алгоритмы, предоставляемые Boost.Algorithm не привязаны к конкретным классам, таким как `std::string`. Как и алгоритмы из стандартной библиотеки, они могут быть использованы с любым контейнером.

<a name="ex.graph_01"/>
Пример 29.1 Проверяем на одно вхождение с `boost::algorithm::one_of_equal()`
```c++
#include <boost/algorithm/cxx11/one_of.hpp>
#include <array>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::array<int, 6> a{{0, 5, 2, 1, 4, 3}};
  auto predicate = [](int i){ return i == 4; };
  std::cout.setf(std::ios::boolalpha);
  std::cout << one_of(a.begin(), a.end(), predicate) << '\n';
  std::cout << one_of_equal(a.begin(), a.end(), 4) << '\n';
}
```

`boost::algorithm::one_of_equal()` проверяет, выполнено ли условие ровно один раз. Условие на проверку передается как предикат. В [Примере 29.1](#ex.graph_01) вызов `boost::algorithm::one_of()` вернет `true`, так как число 4 сохраняется ровно один раз в **a**.

Для проверки элементов в контейнере на равенство, вызовите `boost::algorithm::one_of_equal()`. Вы не передаете предикат. Вместо этого передается значение для сравнения с `boost::algorithm::one_of_equal()`. В [Примере 29.1](#ex.graph_01) вызов `boost::algorithm::one_of_equal()` также вернет `true`.

`boost::algorithm::one_of()` дополняет `std::all_of()`, `std::any_of()`, и `std::none_of()`, которые были добавлены в стандартную библиотеку с C++11. Тем не менее, Boost.Algorithm предоставляет функции `boost::algorithm::all_of()`, `boost::algorithm::any_of()`, и `boost::algorithm::none_of()` разработчикам, чья среда разработки не поддерживает C++11. Можете найти эти алгоритмы в заголовочных файлах `boost/algorithm/cxx11/all_of.hpp`, `boost/algorithm/cxx11/any_of.hpp`, и `boost/algorithm/cxx11/none_of.hpp`.

Boost.Algorithm также определяет следующие функции: `boost::algorithm::all_of_equal()`, `boost::algorithm::any_of_equal()`, и `boost::algorithm::none_of_equal()`.

Boost.Algorithm обеспечивает больше алгоритмов из стандартной библиотеки C++11. Например, у вас есть доступ к `boost::algorithm::is_partitioned()`, `boost::algorithm::is_permutation()`, `boost::algorithm::copy_n()`, `boost::algorithm::find_if_not()` и `boost::algorithm::iota()`. Эти функции работают как также названные функции из стандартной библиотеки C++11 и предназначены для разработчиков, которые не используют C++11. Тем не менее, Boost.Algorithm предоставляет немного функций, которые могут быть полезны разработчикам C++11 тоже.

<a name="ex.graph_02"/>
Пример 29.2 Еще варианты алгоритмов C++11
```c++
#include <boost/algorithm/cxx11/iota.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <vector>
#include <iterator>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::vector<int> v;
  iota_n(std::back_inserter(v), 10, 5);
  std::cout.setf(std::ios::boolalpha);
  std::cout << is_increasing(v) << '\n';
  std::ostream_iterator<int> out{std::cout, ","};
  copy_until(v, out, [](int i){ return i > 12; });
}
```

Boost.Algorithm обеспечивает алгоритм C++11 `boost::algorithm::iota()` в заголовочном файле `boost/algorithm/cxx11/iota.hpp`. Эта функция генерирует последовательные возрастающие числа. Она принимает два итератора начала и конца контейнера. Элементы в контейнере затем записываются последовательно возрастающими числами.

Вместо `boost::algorithm::iota()`, [Пример 29.2](#ex.graph_02) использует `boost::algorithm::iota_n()`. Эта функция принимает один итератор, чтобы знать куда надо записать числа. Количество чисел для генерации передается в качестве третьего параметра `boost::algorithm::iota_n()`.

`boost::algorithm::is_increasing()` и `boost::algorithm::is_sorted()`  определены в заголовочном файле `boost/algorithm/cxx11/is_sorted.hpp`. `boost::algorithm::is_increasing()` имеет ту же функцию, что и `boost::algorithm::is_sorted()`, но имя этой функции куда более ясно показывает, что функция проверяет. Соответственно, мой скилл английского подсказывает мне, что наша функция проверяет, находятли ли числа в возрастающем порядке. В заголовочном файле также определена функция `boost::algorithm::is_decreasing()`.

В [Примере 29.2](#ex.graph_02), **v** передается непосредственно `boost::algorithm::is_increasing()`. У всех функций, которые обеспечивает Boost.Algorithm есть вариант, работающий на основе диапазонов (*ranges*). Контейнеры могут быть переданы непосредственно в эти функции.

`boost::algorithm::copy_until()` определяется в `boost/algorithm/cxx11/copy_if.hpp`. Это еще один вариант `std::copy()`. Boost.Algorithm также обеспечивает `boost::algorithm::copy_while()`.

[Пример 29.2](#ex.graph_02) выведет `true` как результат `boost::algorithm::is_increasing()`, и `boost::algorithm::copy_until()` выведет числа `10`, `11` и `12`.

<a name="ex.graph_03"/>
Пример 29.3 C++14 алгоритмы из Boost.Algorithm
```c++
#include <boost/algorithm/cxx14/equal.hpp>
#include <boost/algorithm/cxx14/mismatch.hpp>
#include <vector>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::vector<int> v{1, 2};
  std::vector<int> w{1, 2, 3};
  std::cout.setf(std::ios::boolalpha);
  std::cout << equal(v.begin(), v.end(), w.begin(), w.end()) << '\n';
  auto pair = mismatch(v.begin(), v.end(), w.begin(), w.end());
  if (pair.first != v.end())
    std::cout << *pair.first << '\n';
  if (pair.second != w.end())
    std::cout << *pair.second << '\n';
}
```
Помимо алгоритмов стандартной библиотеки C++11, Boost.Algorithm также определяет алгоритмы, которые, весьма вероятно, (100%) будут добавлены к стандартной библиотеке с C++14. [Пример 29.3](#ex.graph_03) использует новые варианты двух таких функций, `boost::algorithm::equal()` и `boost::algorithm::mismatch()`. В отличие от старых их версий, которые были частью стандартной библиотеки с C++98, в новые функции передается не три, а четыре итератора. Алгоритмы в [Примере 29.3](#ex.graph_03) не ожидают, что вторая последовательность содержит столько же элементов, что и первая последовательность.

В то время как `boost::algorithm::equal()` возвращает `bool`, `boost::algorithm::mismatch()` возвращает два аргумента в `std::pair`. **first** и **second** ссылаются на элементы первой и второй последовательности, которые являются первыми несовпадениями. Эти итераторы могут также ссылаться на конец последовательности.

[Пример 29.3](#ex.graph_03) выводит `false` и `3` в стандартный поток вывода. `false` является возвращаемым значением `boost::algorithm::equal()`, `3` - третий элемент в **w**. Поскольку первые два элемента в **v** и **w** равны, `boost::algorithm::mismatch()` возвращает в **first** итератор конца **v** и в **second** итератор третьего элемента **w**. Из-за того, что первый ссылается на конец **v**,  неразыменованный итератор, ничего не выведется.


<a name="ex.graph_04"/>
Пример 29.4. Использование `boost::algorithm::hex()` и `boost::algorithm::unhex()`
```c++
#include <boost/algorithm/hex.hpp>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::vector<char> v{'C', '+', '+'};
  hex(v, std::ostream_iterator<char>{std::cout, ""});
  std::cout << '\n';

  std::string s = "C++";
  std::cout << hex(s) << '\n';

  std::vector<char> w{'4', '3', '2', 'b', '2', 'b'};
  unhex(w, std::ostream_iterator<char>{std::cout, ""});
  std::cout << '\n';

  std::string t = "432b2b";
  std::cout << unhex(t) << '\n';
}
```
[Пример 29.4](#ex.graph_04) использует две функции `boost::algorithm::hex()` и `boost::algorithm::unhex()`. Эти функции разработаны после также названных функций из системы баз данных MySQL. Они преобразуют символы в шестнадцатеричные значения или шестнадцатеричные значения в символы.

[Пример 29.4](#ex.graph_04) подает вектор **v** с символами “С”, “ + ” и “+” в `boost::algorithm::hex()`. Эта функция принимает итератор в качестве второго параметра для записи шестнадцатеричных значений. Пример запишет `43` для “C” и `2B` (дважды) для двух “+” в стандартный поток вывода. Второй вызов `boost::algorithm::hex()` делает то же самое, за исключением того, что “с++” передается как строка и “432B2B” возвращается в виде строки.

`boost::algorithm::unhex()` является противоположностью `boost::algorithm::hex()`. Если массив **w** из [Примера 29.4](#ex.graph_04) передается с шестью шестнадцатеричными значениями, то каждая из трех пар значений интерпретируется как ASCII-код. То же самое происходит со вторым вызовом
`boost::algorithm::unhex()`, когда шесть шестнадцатеричных значений передаются в виде строки. В обоих случаях **C++** выводится в стандартный поток вывода.

Boost.Algorithm предоставляет еще больше алгоритмов. Например, существует несколько алгоритмов сопоставления строки, которыми эффективно осуществлять поиск текста. Документация содержит обзор всех доступных алгоритмов (смотри другие главы).
