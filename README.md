Тестовый, написанный для себя, однако полноценно работающий вебсокет сервер(тестовый клиент можно запустить в браузер файл client.html, взято из https://www.websocket.org/echo.html), работающий по протоколу RFC 6455

Реализована расширяемость, вы можете задать свою стратегию наследуемую от AbstractStrategy, в примере задействована простая registrStrategy - читается строка и переводится в верхний регистр

SSL поддержка отсутствует, но вы можете самостоятельно обернуть код если хотите работать по wss, по аналогии в моем проекте https://github.com/mousemove/telegramSteamBot



![alt text](https://sun9-6.userapi.com/c850528/v850528949/1e47ec/fL02Xx9ig_M.jpg)


Используется ssl,crypto


