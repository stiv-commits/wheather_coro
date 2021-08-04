# Демонстрационный проект - Сервер погоды 

Приложение реализует следующий функционал:
* получает HTTP запрос, содержащий записанный текстом адрес
* производится запрос к сервису геокодинга (Яндекс) для получения координат,
* производится запрос к сервису OpenWeather для получения прогноза погоды,
* результат записывается в базу данных PostgreSQL или оперативную память,
* пауза на случайное число секунд до конфигурируемого параметра (к примеру, 60),
* результат возвращается как результат запроса в виде JSON
* при запросе близких координат, к тем, что были запрошены в фиксированный (конфигурируеммый) период времени, данные о погоде беруться из БД
* при повторном запросе адреса, в фиксированный (конфигурируеммый) период времени, данные о погоде беруться из БД

Приложение сделано с целью получения опыта построения программ на корутинах из с++20 стандарта. 

### Особенности реализации
Приложение асинхронное, реализуеться с помощью корутин, в результате код логики работы имеет линейный вид:
```
boost::asio::awaitable<void> BusinessLogic(Dependencies dep, CommonCfg cfg)
{
    auto adr = co_await dep.httpSession->GetRequest();
    if (adr.Empty())
    {
      co_await dep.httpSession->SendAnswerError("Cant get request");
      co_return;
    }
    
    auto coor = dep.db->GetCoorByAdrress(adr, cfg.livetime);
    if (coor.Empty())
    {
      coor = co_await dep.geoClient->GetCoordinate(adr);
      if (coor.Empty())
      {
        co_await dep.httpSession->SendAnswerError("Cant get coordinate");
        co_return;
      }
      dep.db->PutCoordinate(adr, coor);
    }
    ....
}
```
Соединения до геосервера и сервера погоды устанавливаються при старте программы. Количество соединений устанавливаеться через конфиг-файл. 

### Парметры HTTP запроса
```
localhost:5001/
 ? city=<string>
 & street=<string>
 & house=<string>
```

### Пример запроса
```
curl -G --data-urlencode "city=Moscow" --data-urlencode "street=tverskaya" --data-urlencode "house=7" localhost:5001
```

### Пример ответа
```
{"coord":{"lon":37.6098,"lat":55.7565},"weather":[{"id":601,"main":"Snow","description":"snow","icon":"13n"}],"base":"stations","main":{"temp":266.95,"feels_like":261.24,"temp_min":266.15,"temp_max":267.59,"pressure":1011,"humidity":86},"visibility":8000,"wind":{"speed":4,"deg":10},"snow":{"1h":1},"clouds":{"all":90},"dt":1610221654,"sys":{"type":1,"id":9027,"country":"RU","sunrise":1610171700,"sunset":1610198275},"timezone":10800,"id":524894,"name":"Moscow","cod":200}
```

## Конфигурация
Конфигурирование программы осуществляеться через конфиг файл. Путь к файлу указываеться через параметр запуска _inifile_
```
weather --inifile /path/to/conffile.ini
```

Параметры  файла конфигурации:
| Имя секции | Имя параметра | Назначение |
| ------ | ------ | ------ |
|common | 
|common | threads | Количество потоков на которых будет работать сервер
| common | request_pause | Пауза в секундах, которая будет происходить после ответа на запрос.
| common | radius | Радиус близких координат, в метрах
| common | livetime | Время "жизни" данных в БД, в секундах
| common | type\_storage | Тип хранилища информации, возможные значения: _in_memory_ - в памяти, _postgre_ - в БД postgre
|server | 
|server | ip | IP адрес сервера
|server | port | Порт сервера
| yan\_geo |
| yan\_geo | key | Ключ доступа для получения координат(https://yandex.ru/dev/maps/geocoder/)
| yan\_geo | url | Url сервера геокоординат
| yan\_geo | port=443 | порт сервера геокоординат
| yan\_geo | connection | Количетсво соединений до сервера геокоординат
| weather |
| weather |key | Ключ доступа для получения координат(https://openweathermap.org/current)
| weather |url | Url сервера погоды
| weather |port | порт сервера погоды
| weather |connection | Количетсво соединений до сервера погоды

## Сборка проекта

Для сборки необходим менеджер зависимостей [Conan](https://conan.io)

Последовательность сборки для Visual Studio 16:

```
mkdir build
cd build
conan install ..
cmake .. -G "Visual Studio 16"
cmake --build . --config Release
```

