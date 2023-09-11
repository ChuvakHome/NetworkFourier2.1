
Для сборки программы необходимо:

1. Откройте Терминал
2. Перейдите в директорию, где находится Makefile проекта
3. Запустите `make` или `make -jX` (X — количество логических процессоров

После сборки Вы увидите, что появились файлы NetworkFourier21 и NetworkFourier.app.

NetworkFourier21 — двоичный исполняемый файл, полезный при изменении кода программы и его отладки. Этот файл нужно запускать через скрипт `run`. Введите `./run NetworkFourier21` или `bash ./run NetworkFourier21`. При использовании первой команды, Вы можете обнаружить, что файл `run` будет неисполняемым. Для решения этой проблемы запустите `chmod a+x run`.

NetworkFourier.app — app bundle для macOS, он может быть запущен как обычная программа для macOS. При запуске этой программы может появиться следующая проблема: программа не реагирует на нажатие клавиш, при этом ввод текста в поле работает корректно. Попробуйте изменить настройки мониторинга ввода для исправления проблемы:

1. Откройте "Системные настройки"
2. Перейдите в настройки безопасности
3. Найдите пункт "Мониронинг ввода"
4. Переключите флажок с NetworkFourier

==========================================

To build program:
1. Open Terminal
2. Change current directory to the directory where project's Makefile is located
3. run `make` or `make -jX` (X is number of logical processors available on your machine) 

After building you will see, that NetworkFourier21 and NetworkFourier.app files appear. 

NetworkFourier21 is binary executable, it's useful to run it if you want to change the program and debug it. You should run the binary executable using `run` script. Type `./run NetworkFourer21` or `bash ./run NetworkFourier21` to run the binary executable file. If you use first command, perhaps, `run` script file won't be executable, to change it execute command `chmod a+x run`.

NetworkFourier.app is macOS bundle, you can run it as simple macOS program. After open it you can face with the inability to use keyboard, meanwhile textField is still react to text enter. To solve the problem: 
1. Open 'System Settings' 
2. Go to security settings
3. Find item 'Input monitoring'
4. Toggle checkbox with NetworkFourier