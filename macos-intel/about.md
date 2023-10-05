
//////////////////////////////////////////

Использованные ресурсы:
1. Библиотека SFML: [Официальный сайт](https://www.sfml-dev.org), [GitHub репозиторий](https://github.com/SFML/SFML)
2. Библиотека Native File Dialog Extended: [GitHub репозиторий](https://github.com/btzy/nativefiledialog-extended)
3. Библиотека на языке C для конвертации строк из кодировки UTF-8 в CP1251: [GitHub репозиторий](https://github.com/bravikov/utf8_to_cp1251)

//////////////////////////////////////////

Для сборки программы необходимо:

1. Открыть Терминал
2. Перейти в директорию, где находится Makefile проекта
3. Запустить `make` или `make -jX` (X - количество логических процессоров)

После сборки Вы увидите, что появились файлы NetworkFourier21 и NetworkFourier.app.

NetworkFourier21 - двоичный исполняемый файл, полезный при изменении кода программы и его отладки. Этот файл нужно запускать через скрипт `run`. Введите `./run <SFML_ROOT> NetworkFourier21` или `bash ./run NetworkFourier21`. При использовании первой команды, Вы можете обнаружить, что файл `run` будет неисполняемым. Для решения этой проблемы запустите `chmod a+x run`. <SFML_ROOT> - директория с заголовочными файлами, динамическими библиотеками и фреймворками SFML, доступны версии 2.5.1 и 2.6.0: версия 2.5.1 более дружелюбна в плане линковки; имеющаяся версия 2.6.0 более-менее стабильна в плане сборки приложений, но не исключено возникновение проблем, связанных с линковкой.

NetworkFourier.app - app bundle для macOS, он может быть запущен как обычная программа для macOS. При запуске этой программы может появиться следующая проблема: программа не реагирует на нажатие клавиш, при этом ввод текста в поле работает корректно. Попробуйте изменить настройки мониторинга ввода для исправления проблемы:

1. Откройте "Системные настройки"
2. Перейдите в настройки безопасности
3. Найдите пункт "Мониторинг ввода"
4. Переключите флажок с NetworkFourier

Если программа NetworkFourier.app уже ранее была запущена на Вашей машине и разрешили ей считывать нажатия клавиш, то при перекомпиляции новая версия программы в виде app bundle может не воспринимать нажатия клавиш. В этом случае необходимо заново разрешить программе считывать нажатия клавиш. Для этого перед запуском программы перейти в Системные настройки и удалить пункт с NetworkFourier.app из раздела "Мониторинг ввода". После открытия программы нажмите любую клавишу, macOS предложит разрешить NetworkFourier считывать нажатия клавиш. После повторного добавления NetworkFourier в соответствующий пункт настроек следует перезапустить программу. Эту процедуру нужно выполнять после каждой перекомпиляции проекта.

==========================================

//////////////////////////////////////////

Resources used:
1. SFML Library: [Official website](https://www.sfml-dev.org), [GitHub repository](https://github.com/SFML/SFML)
2. Native File Dialog Extended Library: [GitHub repository](https://github.com/btzy/nativefiledialog-extended)
3. C library for converting UTF-8 encoded strings to CP1251: [GitHub repository](https://github.com/bravikov/utf8_to_cp1251)

//////////////////////////////////////////

To build program:
1. Open Terminal
2. Change current directory to the directory where project's Makefile is located
3. run `make` or `make -jX` (X is number of logical processors available on your machine)

After building you will see, that NetworkFourier21 and NetworkFourier.app files appear.

NetworkFourier21 is binary executable, it's useful to run it if you want to change the program and debug it. You should run the binary executable using `run` script. Type `./run <SFML_ROOT> NetworkFourer21` or `bash ./run <SFML_ROOT> NetworkFourier21` to run the binary executable file. If you use first command, perhaps, `run` script file won't be executable, to change it you should execute command `chmod a+x run`. <SFML_ROOT> is a directory with header files, dynamic libraries and frameworks of SFML library, now versions 2.5.1 and 2.6.0 are available. SFML 2.5.1 is pretty linking-friendly, SFML 2.6.0 is more or less capable of linking the final program but you can still get a linking error while building the final program.

NetworkFourier.app is macOS bundle, you can run it as simple macOS program. After open it you can face with the inability to use keyboard, meanwhile textField is still react to text enter. To solve the problem:
1. Open 'System Settings'
2. Go to security settings
3. Find item 'Input monitoring'
4. Toggle checkbox with NetworkFourier

If the NetworkFourier.app program was previously running on your machine and allowed it to read keystrokes, then when recompiling, the new version of the program in the form of an app bundle may not perceive keystrokes. In this case, it is necessary to allow the program to read keystrokes again. To do this, before launching the program, go to System Settings and delete the item with NetworkFourier.app from the "Input Monitoring" section. After opening the program, press any key, macOS will prompt you to allow NetworkFourier to read keystrokes. After re-adding NetworkFourier to the corresponding settings item, restart the program. This procedure should be performed after each recompilation of the project.
