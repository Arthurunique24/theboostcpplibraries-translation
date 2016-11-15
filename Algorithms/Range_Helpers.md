# Вспомогательные классы и функции

Алгоритмы и адаптеры, предоставляемые Boost.Range, основаны на шаблонах. То есть не придется изменять контейнер для диапазона, чтобы передать его алгоритму или адаптеру. В общем-то, в Boost.Range есть несколько классов диапазонов, `boost::iterator_range` самый важный из них. `boost::iterator_range` необходим, т.к. адаптеры и некоторые алгоритмы возвращают диапазоны, у которых должен быть тип. К тому же, есть вспомогательные функции, которые создают диапазоны, чьи итераторы содержат все данные, необходимые для итерации. Итераторы из таких диапазонов не ссылаются на контейнер или другую структуру данных. И в таком случае можно использовать `boost::iterator_range`.

<a name="ex.range_07"/>
 Пример 30.7. Фильтруем диапазон с `boost::adaptors::filter()`
```c++
#include <boost/range/algorithm.hpp>
#include <boost/range/irange.hpp>
#include <iostream>

int main()
{
  boost::integer_range<int> ir = boost::irange(0, 3);
  boost::copy(ir, std::ostream_iterator<int>{std::cout, ","});
}
```

[Пример 30.7](#ex.range_07) использует функцию `boost::irange()`. Эта функция создает диапазон для целочисленных значений (тип int) без необходимости использовать контейнер или другую структуру данных. Только нужно будет передать верхнюю и нижнюю границу в `boost::irange()`, причем верхняя граница будет такой, что все элементы строго меньше ее.

`boost::irange()` возвращает диапазон типа `boost::integer_range`. Этот класс наследован от `boost::iterator_range`. `boost::iterator_range` - шаблонный класс, принимающий в качестве своего единственного шаблонного параметра тип итератора. Итератор, использованный `boost::irange()`, тесно связан с этим классом и является его специализированной реализацией. Итак, нам тип итератора не извествен, а знаит он не может быть передан в `boost::iterator_range`. Тем не менее `boost::integer_range` работает только с целочисленными диапазонами, причем нет необходимости передавать тип итератора, отчего он проще в использовании.

[Пример 30.7](#ex.range_07) выведет `0,1,2`.

<a name="ex.range_08"/>
Пример 30.8. Создаем диапазон для потока ввода с `boost::istream_range()`
```c++
#include <boost/range/algorithm.hpp>
#include <boost/range/istream_range.hpp>
#include <iterator>
#include <iostream>

int main()
{
  boost::iterator_range<std::istream_iterator<int>> ir =
    boost::istream_range<int>(std::cin);
  boost::copy(ir, std::ostream_iterator<int>{std::cout, "\n"});
}
```

[Пример 30.8](#ex.range_08) знакомит вас с `boost::istream_range()`, которая создает диапазон для потока ввода. Функция возвращает диапазон как `boost::iterator_range`. Это означает, что тип итератора должен быть передан как шаблонный параметр.

Когда вы запустите [Пример 30.8](#ex.range_08), введете число и нажмете **Enter**, число выведется на следующей строчке. Если введете другое число и нажмете **Enter**, выведется новое напечатанное число. Диапазон, возвращаемый `boost::istream_range()` позволяет `boost::copy()` перебирать все введеные числа и выводить их в **std::cout**.

Вы можете прекратить работу программы в любой момент, нажав **Ctrl+C**.

Кроме `boost::iterator_range`, Boost.Range обеспечивает класс `boost::sub_range`, который наследован от `boost::iterator_range`. `boost::sub_range` шаблон по типу `boost::iterator_range`. Однако `boost::sub_range` вместо типа итератора принимает тип диапазона как шаблоный параметр. Такой подход может упростить использование.

<a name="ex.range_09"/>
Пример 30.9. Еще проще создание диапазонов с `boost::sub_range()`
```c++
#include <boost/range/algorithm.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/sub_range.hpp>
#include <array>
#include <iterator>
#include <iostream>

int main()
{
  std::array<int, 6> a{{0, 1, 2, 3, 4, 5}};
  boost::iterator_range<std::array<int, 6>::iterator> r1 =
    boost::random_shuffle(a);
  boost::sub_range<std::array<int, 6>> r2 =
    boost::random_shuffle(r1);
  boost::copy(r2, std::ostream_iterator<int>{std::cout, ","});
}
```
Некоторые алгоритмы Boost.Range возвращают диапазон – например, `boost::random_shuffle()`. Этот алгоритм непосредственно изменяет диапазон, переданный ему по ссылке и возвращает измененный диапазон. В [Примере 30.9](#ex.range_09), `boost::random_shuffle()` вызывается дважды, так что массив случайно перемешан дважды.

В примере значение, возвращаемое функцией `boost::random_shuffle()` присваивается двум переменным разных типов. У первой переменной тип `boost::iterator_range`; у второй `boost::sub_range`. Использование любого из этих классов отличается только в шаблонных параметрах. `boost::sub_range` инстанциируется проще `boost::iterator_range`, а еще у него есть определение типа `const_iterator`.
