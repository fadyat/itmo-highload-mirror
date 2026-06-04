## Настройка окружения

Инструкция приведена для Ubuntu 24.04. Тем не менее, настройка под Mac OS и Windows будет мало
отличаться (под Windows вам, вероятно, понадобится WSL или Cygwin). В теории курс
кросс-платформенный - все задачи можно делать на Linux, Mac и Windows - однако
мы тестировали только под Ubuntu 24.04, так что на других платформах следует ожидать непредвиденных проблем.
В случае их возникновения, пишите в чат курса - чем сможем, поможем :)

### Регистрация в системе

1. Зарегистрируйтесь в [тестовой системе](https://app.manytask.org). Секретный код: `CargoComrades`.
1. Сгенерируйте ssh ключ, если у вас его еще нет.

	```
	ssh-keygen -N "" -f ~/.ssh/id_ed25519 -t ed25519
	```

1. Добавьте свой публичный ключ id_ed25519.pub (`cat ~/.ssh/id_ed25519.pub`) в https://gitlab.manytask.org/-/user_settings/ssh_keys
1. Проверьте, что ssh ключ работает. Выполните команду `ssh git@gitlab.manytask.org`. Вы должны увидеть такое приветствие:

	```
	$ ssh git@gitlab.manytask.org
	PTY allocation request failed on channel 0
	Welcome to GitLab, Miron Fedorov!
	Connection to gitlab.manytask.org closed.
	```

### Настройка репозитория

1. Склонируйте репозиторий с задачами.

	```
	git clone git@gitlab.manytask.org:rust/public-2025-fall.git shad-rust
	```

   Команда `git clone` создаст директорию `shad-rust` и запишет туда все файлы из этого репозитория.
1. Каждую неделю после занятий вам надо будет обновлять репозиторий, чтобы у вас появились условия
   новых задач:

	```
	git pull --rebase
	```

1. Для отправки решения на сервер, необходимо, чтобы у вас были заданы имя и email в git:

	```
	git config --global user.name "Miron Fedorov"
	git config --global user.email miron@fedorov.ru
	```

1. Откройте страницу своего репозитория в браузере: для этого нужно перейти по ссылке `My Repo` на [странице с задачами](https://app.manytask.org/rust-2025-fall).
1. Скопируйте ссылку, которая появляется при нажатии синей кнопки Clone -> Clone with SSH.
1. Запустите из директории репозитория команду:

	```
	git remote add student $ADDRESS
	```

   `$ADDRESS` нужно вставить из прошлого шага.

### Настройка окружения

1. Установите Rust, следуя [официальному руководству](https://www.rust-lang.org/tools/install).
1. Проверьте, что у вас работают команды `cargo` и `rustup`:

	```
	$ cargo --version
	cargo 1.89.0 (c24e10642 2025-06-23)
	$ rustup --version
	rustup 1.28.2 (e4f3ad6f8 2025-04-28)
	```

1. Настройте свой редактор:
	* Если вы используете [VS Code](https://code.visualstudio.com), установите следующие расширения:
		* [rust-analyzer](https://marketplace.visualstudio.com/items?itemName=matklad.rust-analyzer)
		* [CodeLLDB](https://marketplace.visualstudio.com/items?itemName=vadimcn.vscode-lldb) (опционально, для работы дебаггера)
	* Если вы используете другой редактор:
		1. Установите `rust-analyzer`:
			```
			rustup component add rust-analyzer
			```
		1. Настройте свой редактор согласно [инструкции](https://rust-analyzer.github.io/book/other_editors.html)

### Отправка решения

Чтобы проверить работоспособность окружения, решите первую тестовую задачу:

1. Откройте `task/add/src/lib.rs`. Убедитесь, что у вас работают базовые вещи: подсветка ошибок компиляции, автокомплит, go to definition.
1. Находясь в директории `add`, запустите локальные тесты командой `cargo xtask check`. Убедитесь, что они падают.
1. Напишите правильную реализацию функции `add` в `task/add/src/lib.rs`.
1. Снова запустите локальные тесты. Убедитесь, что они проходят.
1. Закомитьте изменения:

    ```
	git add .
	git commit -m 'Solve task: add'  # сообщение может быть произвольным
    ```

1. Отправьте своё решение на сервер командой `cargo xtask submit`. Ваш сабмит должен появиться по ссылке `My Submits` на [app.manytask.org/rust-2025-fall](https://app.manytask.org/rust-2025-fall).
После успешного прохождения тестов вам должно начислиться 1 балл в
[таблице с баллами](https://app.manytask.org/rust-2025-fall/database).

Если на каком-то этапе у вас возникли проблемы - пишите в чат курса.
