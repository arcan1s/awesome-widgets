Code style
----------

The recommended code style is Qt one. See [this document](https://wiki.qt.io/Qt_Coding_Style)
for more details. To avoid manual labor there is automatic cmake target named
`clangformat` (see below). Some additional detail see below.

* Indent is only spaces. 4 spaces.
* It is highly recommended to name private variables with `m_` prefix (`m_foo`).
  There is no exceptions for properties.
* Avoid to create a large methods. Exception: if method contains lambda functions.
* If some method is called only once, it is recommended to use lambda functions.
  Exception is `Q_INVOKABLE` methods.
* STL containers are not recommended, use Qt ones instead.
* In other hand Qt specific variables types (`qint`, `qfloat`, etc) are not
  recommended.
* Do not repeat yourself ([DRY](https://en.wikipedia.org/wiki/Don't_repeat_yourself)).
* Headers declaration:
    * Include only those headers which are strictly necessary inside headers. Use
      forward class declaration instead. Exception is base class header declaration.
    * In a`*.cpp` file header declaration should have the following order separated
      by a new line in the alphabet order:
        1. Class header.
        2. KDE specific headers.
        3. Qt specific headers.
        4. Third party headers.
        5. Project headers.
    * Any header should have [include guard](https://en.wikipedia.org/wiki/Include_guard)
      named as `CLASSNAMECAPS_H`
* If any `#if` directive is used condition should be mentioned in `#endif`:

  ```
  #if (FOO)
  someCodeInside();
  #endif /* FOO */
  ```

* `Q_PROPERTY` macro is allowed and recommended for QObject based classes.
* Qt macros (e.g. `signals`, `slots`, `Q_OBJECT`, etc) are allowed.
* Current project standard is **C++11**.
* Do not use C-like code:
    * C-like style iteration if possible. Use `Q_FOREACH` (`foreach`) and
      `std::for_each` instead if possible. It is also recommended to use iterators.
    * C-like casts, use `const_cast`, `static_cast`, `dymanic_Cast` instead. Using
      of `reinterpret_cast` is not recommended. It is highly recommended to use
      `dynamic_Cast` with the exception catching. It is also possible to use
      `qvariant_cast` if required. Exception is class constructors, e.g.:

      ```
      char c = 'c';
      std::string s = "string";
      qDebug() << QString("some string") << QChar(c) << QString(s);
      ```

    * C-like `NULL`, use `nullptr` instead.
* It is highly recommended to avoid implicit casts.
* Abstract classes (which have at least one pure virtual method) are allowed.
* Templates are allowed and recommended. Templates usually should be described
  inside header not source code file.
* Hardcode is not recommended. But it is possible to use cmake variables to
  configure some items during build time.
* Build should not require any additional system variable declaration/changing.
* Any line should not end with space.
* Do not hesitate move public methods to private one if possible.
* Do not hesitate use `const` modifier. In other hand `volatile` modifier is not
  recommended.
* New lines rules:
    * One line after license header.
    * One line between header group declaration (see above).
    * Two lines after header declaration and before declaration at the end of a
      file.
    * One line after class and types forward declarations in headers.
    * One line before each method modifiers (`public`, `public slots`, etc).
    * Two lines between methods inside source code (`*.cpp`).
    * One line after `qCDebug()` information (see below).
    * One line inside a method to improve code reading.
* Each destructor should be virtual.
* Class constructor should have default arguments. Use `QObject *parent` property
  for QObject based classes.
* QObject based classes constructors should have explicit modifier.
* Create one file (source and header) per class.
* `else if` construction is allowed and recommended.
* 'true ? foo : bar' construction is allowed and recommended for one-line assignment.

Comments
--------

Please do not hesitate to use comments inside source code (especially in non-obvious
blocks). Comments also may use the following keywords:

* **TODO** - indicates that some new code should be implemented here later. Please
  note that usually these methods should be implemented before the next release.
* **FIXME** - some dirty hacks and/or methods which should be done better.
* **HACK** - hacks inside code which requires to avoid some restrictions and/or
  which adds additional non-obvious optimizations.

Do not use dots at the end of the comment line.

Development
-----------

* Officially the latest libraries versions should be used. In addition it is
  possible to add workarounds for all versions (usually by using preprocessor
  directives).
* Build should not contain any warning.
* Try to minimize message in Release build with logging disabled. It is highly
  recommended to fix KDE/Qt specific warning if possible
* Do not use dependency to KDE libraries if there are no any strictly necessary.
  Exceptions are KNotification and KI18n libraries.
* It is highly recommended to use submodules for third party libraries if possible.
* The main branch is **development**. Changes in this branch may be merged to the
  master one only if it is a 'stable' build.
* For experimental features development new branch `feature-foo` creation is allowed
  and recommended.
* Experimental features should be added inside `BUILD_FUTURE` definition:

  ```
  #ifdef BUILD_FUTURE
  someTestFunctionInside();
  #endif /* BUILD_FUTURE */
  ```

* Any project specific build variable should be mentioned inside `version.h` as
  well.
* Recommended compiler is `clang`.

HIG
---

The recommended HIG is [KDE one](https://techbase.kde.org/Projects/Usability/HIG).
Avoid to paint interfaces inside plugin because QML and C++ parts may have different
theming.

Licensing
---------

All files should be licensed under GPLv3, the owner of the license should be the
project (i.e. **awesome-widgets**). See **Tools** section for more details.

Logging
-------

For logging please use [QLoggingCategory](http://doc.qt.io/qt-5/qloggingcategory.html).
Available categories should be declared in `awdebug.*` files. The following log
levels should be used:

* **debug** (`qCDebug()`) - method arguments information.
* **info** (`qCInfo()`) - additional information inside methods.
* **warning** (`qCWarning()`) - not critical information, which may be caused by
  mistakes in configuration for example.
* **error** (`qCError()`) - an error which has been captured in runtime. All errors
  should have own callback methods.
* **critical** (`qCCritical()`) - a critical error. After this error program will
  be terminated.

The empty log string (e.g. `qCDebug();`) is not allowed because the method names
will be stripped by compiler with `Release` build type. To log class constructor
and destructor use `__PRETTY_FUNCTION__` macro.

Testing
-------

* Any changes should be tested by using `plasmawindowed` and `plasmashell` applications.
  (It is also possible to use `plasmaengineexplorer` and `plasmoidviewer` in addition.)
* Any test should be performed on real (not Virtual Machine) system.
* Test builds should be:
    1. `-DCMAKE_BUILD_TYPE=Debug`.
    2. `-DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON`.
    3. `-DCMAKE_BUILD_TYPE=Release`.
* It is recommended to create addition test if possible.
* Addition test functions should be declated and used only inside `BUILD_TESTING`
  definition.

Tools
-----

* For QString concatenation use `QString::arg` method.
* Any source file should have license header:

  ```
  /***************************************************************************
  *   This file is part of awesome-widgets                                  *
  *                                                                         *
  *   awesome-widgets is free software: you can redistribute it and/or      *
  *   modify it under the terms of the GNU General Public License as        *
  *   published by the Free Software Foundation, either version 3 of the    *
  *   License, or (at your option) any later version.                       *
  *                                                                         *
  *   awesome-widgets is distributed in the hope that it will be useful,    *
  *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
  *   GNU General Public License for more details.                          *
  *                                                                         *
  *   You should have received a copy of the GNU General Public License     *
  *   along with awesome-widgets. If not, see http://www.gnu.org/licenses/  *
  ***************************************************************************/
  ```

* Recommended class constructor for QObject based classes:

  ```
  FooClass::FooClass(QObject *parent, const QVariant var)
      : QObject(parent)
      , m_var(var)
  {
      qCDebug(LOG_AW) << __PRETTY_FUNCTION__;
      // some code below if any
  }
  ```

* Property usage:

  ```
        Q_PROPERTY(bool prop READ prop WRITE setProp);
  public:
        bool prop() const
        {
            return m_prop;
        };
        void setProp(const bool _prop)
        {
            // error checking if required
            m_prop = _prop
        }
  private:
        // declare with default value
        bool m_prop = false;
  ```
* Use `cppcheck` to avoid common errors in the code. To start application just
  run `make cppcheck`.
* Use `clang-format` to apply valid code format. To start application just run
  `make clangformat`.
