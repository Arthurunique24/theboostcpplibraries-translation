# Глава 5. Boost.StringAlgorithms
[Boost.StringAlgorithms](http://www.boost.org/libs/algorithm/string) - библиотека, предоставляющая множество автономных функций для обработки строк. Строки могут быть типа `std::string`, `std::wstring` или любого другого экземпляра шаблонного класса `std::basic_string`. Это включает в себя строковые классы `std::u16string` и `std::u32string` введеные с C++11.

Функции классифицированы в разных заголовочных файлах. Например, функции преобразования из верхнего регистра в нижний регистр определены в `boost/algorithm/string/case_conv.hpp`. Поскольку Boost.StringAlgorithms состоит больше чем из 20 различных категорий и стольких же заголовочных файлов, `boost/algorithm/string.hpp` выступает в качестве общего заголовка, включая все другие заголовочные файлы для удобства.

#### Пример 5.1. Преобразование строк к верхнему регистру

```    
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  std::cout << to_upper_copy(s) << '\n';
}
```

Функция `boost::algorithm::to_upper_copy()` преобразовывает строку в верхний регистр и `boost::algorithm::to_lower_copy()` преобразовывает строку в нижний регистр. Обе функции возвращают копию строки ввода, преобразованной в указанном случае. Чтобы преобразовать строку на месте, используйте функции `boost::algorithm::to_upper()` или `boost::algorithm::to_lower()`.

[Пример 5.1]() преобразовывает строку “Boost C++ Libraries” в верхний регистр с помощью `boost::algorithm::to_upper_copy()`. Пример выписывает "BOOST C++ LIBRARIES" в стандартный вывод.

Функции от Boost.StringAlgorithms рассматривают наборы параметров. Функции как `boost::algorithm::to_upper_copy()` используют глобальный набор параметров, если в качестве параметра явно не передан никакой другой набор параметров.

#### Пример 5.2. Преобразование строки к верхнему регистру с набором параметров

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <locale>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ k\xfct\xfcphaneleri";
  std::string upper_case1 = to_upper_copy(s);
  std::string upper_case2 = to_upper_copy(s, std::locale{"Turkish"});
  std::locale::global(std::locale{"Turkish"});
  std::cout << upper_case1 << '\n';
  std::cout << upper_case2 << '\n';
}
```

[Пример 5.2]() вызывает `boost::algorithm::to_upper_copy()` дважды, чтобы преобразовать турецкую строку “Boost C++ kütüphaneleri” к верхнему регистру. Первый вызов `boost::algorithm::to_upper_copy()` использует глобальный набор параметров, которая в этом случае является набором параметров C. В наборе параметров C нет никакого прописного отображения для символов с перегласовкой, таким образом, вывод будет "BOOST C++ KüTüPHANELERI".

Турецкий набор параметров передаётся во втором вызове `boost::algorithm::to_upper_copy()`. Поскольку этот набор параметров имеет прописные эквиваленты для перегласовки, вся строка может быть преобразована в верхний регистр. Поэтому второй вызов правильно преобразовывает строку, которая выглядит как: "BOOST C++ KÜTÜPHANELERI".

```
Если вы хотите выполнить пример в операционной системе POSIX, замените “Turkish” на “tr_TR” и будьте уверены, что турецкий набор параметров установлен.
```

#### Пример 5.3. Алгоритмы для удаления символов из строки

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  std::cout << erase_first_copy(s, "s") << '\n';
  std::cout << erase_nth_copy(s, "s", 0) << '\n';
  std::cout << erase_last_copy(s, "s") << '\n';
  std::cout << erase_all_copy(s, "s") << '\n';
  std::cout << erase_head_copy(s, 5) << '\n';
  std::cout << erase_tail_copy(s, 9) << '\n';
}
```

Boost.StringAlgorithms обеспечивает некоторые функции, которые вы можете использовать для удаления отдельных символов из строки (см. [пример 5.3]()). Например, `boost::algorithm::erase_all_copy()` удалит все случаи возникновения определенного символа из строки. Чтобы удалить только первое возникновение символа, используйте `boost::algorithm::erase_first_copy()`. Чтобы сократить строку определенным количеством символов на любом конце, используйте функции `boost::algorithm::erase_head_copy()` и `boost::algorithm::erase_tail_copy()`.

#### Example 5.4. Поиск подстрок с `boost::algorithm::find_first()` 

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  boost::iterator_range<std::string::iterator> r = find_first(s, "C++");
  std::cout << r << '\n';
  r = find_first(s, "xyz");
  std::cout << r << '\n';
}
```

Функции, такие как `boost::algorithm::find_first()`, `boost::algorithm::find_last()`, `boost::algorithm::find_nth()`, `boost::algorithm::find_head()` и `boost::algorithm::find_tail()` доступны для нахождения строки в строках.

Все эти функции возвращают пару итераторов типа `boost::iterator_range`. Этот класс происходит от Boost.Range, который реализует диапазон, основанный на идее итератора. Поскольку оператор `operator<<` перегружен для `boost::iterator_range`, результат отдельного алгоритма поиска может быть записан непосредственно в стандартный вывод. [Пример 5.4]() вывод C++ для первого результата и пустая строка для второй.

#### Пример 5.5. Склеивание строк с `boost::algorithm::join()`

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::vector<std::string> v{"Boost", "C++", "Libraries"};
  std::cout << join(v, " ") << '\n';
}
```

Контейнер строк передан как первый параметр к функции `boost::algorithm::join()`, который связывает их через второй параметр. [Пример 5.5]() выведет "Boost C++ Libraries".

#### Пример 5.6. Алгоритмы для замены символов в строке

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  std::cout << replace_first_copy(s, "+", "-") << '\n';
  std::cout << replace_nth_copy(s, "+", 0, "-") << '\n';
  std::cout << replace_last_copy(s, "+", "-") << '\n';
  std::cout << replace_all_copy(s, "+", "-") << '\n';
  std::cout << replace_head_copy(s, 5, "BOOST") << '\n';
  std::cout << replace_tail_copy(s, 9, "LIBRARIES") << '\n';
}
```

Как функции для поиска строк или удаления символов из строк, Boost.StringAlgorithms также обеспечивает функции для замены подстрок в строке. Они включают следующие функции: `boost::algorithm::replace_first_copy()`, `boost::algorithm::replace_nth_copy()`, `boost::algorithm::replace_last_copy()`, `boost::algorithm::replace_all_copy()`, `boost::algorithm::replace_head_copy()` и `boost::algorithm::replace_tail_copy()`.  Они могут быть применены таким же образом как функции для поиска и удаления, кроме они требуют дополнительного параметра – заменяющая строка (см. [пример 5.6]()).

#### Пример 5.7. Алгоритмы для подрезки строки

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "\t Boost C++ Libraries \t";
  std::cout << "_" << trim_left_copy(s) << "_\n";
  std::cout << "_" << trim_right_copy(s) << "_\n";
  std::cout << "_" << trim_copy(s) << "_\n";
}
```

Чтобы удалить пробелы на любом конце строки, используйте `boost::algorithm::trim_left_copy()`, `boost::algorithm::trim_right_copy()` и `boost::algorithm::trim_copy()` (см. [пример 5.7]()). Глобальный набор параметров определяет, какие символы считаются пробелами.

Boost.StringAlgorithms позволяет вам обеспечить предикат как дополнительный параметр для различных функций, чтобы определить, какие символы строки применяются к функции. Версии с предикатами: `boost::algorithm::trim_right_copy_if()`, `boost::algorithm::trim_left_copy_if()`, и `boost::algorithm::trim_copy_if()`.

#### Пример 5.8. Создание предикатов с `boost::algorithm::is_any_of()`

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "--Boost C++ Libraries--";
  std::cout << trim_left_copy_if(s, is_any_of("-")) << '\n';
  std::cout << trim_right_copy_if(s, is_any_of("-")) << '\n';
  std::cout << trim_copy_if(s, is_any_of("-")) << '\n';
}
```

[Пример 5.8]() использует другую функцию `boost::algorithm::is_any_of()`, которая является функцией-помощник для создания предиката, которая проверяет, передавался ли определенный символ – как параметр `is_any_of()` – в строке. С `boost::algorithm::is_any_of()`, символы для обрезки строки могут быть определены. [Пример 5.8]() использует символ дефиса.

Boost.StringAlgorithms обеспечивает много функций-помощников, которые возвращают используемые предикаты.

#### Пример 5.8. Создание предикатов с `boost::algorithm::is_digit()`

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "123456789Boost C++ Libraries123456789";
  std::cout << trim_left_copy_if(s, is_digit()) << '\n';
  std::cout << trim_right_copy_if(s, is_digit()) << '\n';
  std::cout << trim_copy_if(s, is_digit()) << '\n';
}
```

Предикат, возвращенный функцией `boost::algorithm::is_digit()`, тестирует, числовой ли символ. В [примере 5.9](), `boost::algorithm::is_digit()` используется для удаления цифры из строки s.

Boost.StringAlgorithms также обеспечивает функции-помощники для проверки прописной символ или строчный: `boost::algorithm::is_upper()` и `boost::algorithm::is_lower()`. Все эти функции используют глобальный набор параметров по умолчанию, если вы не передаёте другой набор в качестве параметра.

Помимо предикатов, которые проверяют отдельные символы строки, Boost.StringAlgorithms также предлагает функции для работы со строками (см. [пример 5.10]()).

#### Пример 5.10. Алгоритмы для сравнения строк с другими

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  std::cout.setf(std::ios::boolalpha);
  std::cout << starts_with(s, "Boost") << '\n';
  std::cout << ends_with(s, "Libraries") << '\n';
  std::cout << contains(s, "C++") << '\n';
  std::cout << lexicographical_compare(s, "Boost") << '\n';
}
```

`boost::algorithm::starts_with()`, `boost::algorithm::ends_with()`, `boost::algorithm::contains()`, и `boost::algorithm::lexicographical_compare()` функции сравнивающие две отдельные строки.

[Пример 5.11]() представляет функцию, которая разделяет строку на более мелкие части.

#### Пример 5.11. Разделение строк с `boost::algorithm::split()`

```
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  std::vector<std::string> v;
  split(v, s, is_space());
  std::cout << v.size() << '\n';
}
```

С `boost::algorithm::split()`, данная строка может быть разделена на основе разделителя. Подстроки сохранены в контейнере. Функция требует как третий параметр - предикат, который тестирует каждый символ и проверяет, должна ли строка быть разделена в данной позиции. [Пример 5.11]() использует функцию-помощник `boost::algorithm::is_space()` для создания предиката, который разделяет строку в каждом пробеле.

У многих функций, представленных в этой главе, есть версии, которые игнорируют регистр строки. У этих версий обычно похожие названия, за исключением ведущего “i”. Например, эквивалентная функция `boost::algorithm::erase_all_copy()` является `boost::algorithm::ierase_all_copy()`.

Наконец-то, большинство функций Boost.StringAlgorithms также поддерживает регулярные выражения. [Пример 5.12]() использует функцию `boost::algorithm::find_regex()` для поиска регулярного выражения.

#### Пример 5.12. Поиск строк с boost::algorithm::find_regex()

```
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <string>
#include <iostream>

using namespace boost::algorithm;

int main()
{
  std::string s = "Boost C++ Libraries";
  boost::iterator_range<std::string::iterator> r =
    find_regex(s, boost::regex{"\\w\\+\\+"});
  std::cout << r << '\n';
}
```

Чтобы использовать регулярное выражение, программа получает доступ к вызову класса `boost::regex`, который представлен в [Главе 8](https://theboostcpplibraries.com/boost.regex).

[Пример 5.12]() выписывает "C++" в стандартный вывод.
