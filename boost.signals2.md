# Chapter 67. Boost.Signals2

**Table of Contents**
* [Signals](boost.signals2-signals.md)
* [Connections](boost.signals2-connections.md)
* [Multithreading](boost.signals2-multithreading.md)

[Boost.Signals2](http://www.boost.org/libs/signals2) implements the signal/slot concept. One or multiple functions – called *slots* – are linked with an object that can emit a signal. Every time the signal is emitted, the linked functions are called.

The signal/slot concept can be useful when, for example, developing applications with graphical user interfaces. Buttons can be modelled so they emit a signal when a user clicks on them. They can support links to many functions to handle user input. That way it is possible to process events flexibly.

`std::function` can also be used for event handling. One crucial difference between `std::function` and Boost.Signals2, is that Boost.Signals2 can associate more than one event handler with a single event. Therefore, Boost.Signals2 is better for supporting event-driven development and should be the first choice whenever events need to be handled.

Boost.Signals2 succeeds the library Boost.Signals, which is deprecated and not discussed in this book.

[Prev](boost.flyweight.md) | [Next](boost.signals2-signals.md)
--- | ---
