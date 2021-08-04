# ���������������� ������ - ������ ������ 

���������� ��������� ��������� ����������:
* �������� HTTP ������, ���������� ���������� ������� �����
* ������������ ������ � ������� ���������� (������) ��� ��������� ���������,
* ������������ ������ � ������� OpenWeather ��� ��������� �������� ������,
* ��������� ������������ � ���� ������ PostgreSQL ��� ����������� ������,
* ����� �� ��������� ����� ������ �� ���������������� ��������� (� �������, 60),
* ��������� ������������ ��� ��������� ������� � ���� JSON
* ��� ������� ������� ���������, � ���, ��� ���� ��������� � ������������� (����������������) ������ �������, ������ � ������ �������� �� ��
* ��� ��������� ������� ������, � ������������� (����������������) ������ �������, ������ � ������ �������� �� ��

���������� ������� � ����� ��������� ����� ���������� �������� �� ��������� �� �++20 ���������. 

### ����������� ����������
���������� �����������, ������������ � ������� �������, � ���������� ��� ������ ������ ����� �������� ���:
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
���������� �� ���������� � ������� ������ ���������������� ��� ������ ���������. ���������� ���������� ���������������� ����� ������-����. 

### �������� HTTP �������
```
localhost:5001/
 ? city=<string>
 & street=<string>
 & house=<string>
```

### ������ �������
```
curl -G --data-urlencode "city=Moscow" --data-urlencode "street=tverskaya" --data-urlencode "house=7" localhost:5001
```

### ������ ������
```
{"coord":{"lon":37.6098,"lat":55.7565},"weather":[{"id":601,"main":"Snow","description":"snow","icon":"13n"}],"base":"stations","main":{"temp":266.95,"feels_like":261.24,"temp_min":266.15,"temp_max":267.59,"pressure":1011,"humidity":86},"visibility":8000,"wind":{"speed":4,"deg":10},"snow":{"1h":1},"clouds":{"all":90},"dt":1610221654,"sys":{"type":1,"id":9027,"country":"RU","sunrise":1610171700,"sunset":1610198275},"timezone":10800,"id":524894,"name":"Moscow","cod":200}
```

## ������������
���������������� ��������� ��������������� ����� ������ ����. ���� � ����� ������������ ����� �������� ������� _inifile_
```
weather --inifile /path/to/conffile.ini
```

���������  ����� ������������:
| ��� ������ | ��� ��������� | ���������� |
| ------ | ------ | ------ |
|common | 
|common | threads | ���������� ������� �� ������� ����� �������� ������
| common | request_pause | ����� � ��������, ������� ����� ����������� ����� ������ �� ������.
| common | radius | ������ ������� ���������, � ������
| common | livetime | ����� "�����" ������ � ��, � ��������
| common | type\_storage | ��� ��������� ����������, ��������� ��������: _in_memory_ - � ������, _postgre_ - � �� postgre
|server | 
|server | ip | IP ����� �������
|server | port | ���� �������
| yan\_geo |
| yan\_geo | key | ���� ������� ��� ��������� ���������(https://yandex.ru/dev/maps/geocoder/)
| yan\_geo | url | Url ������� ������������
| yan\_geo | port=443 | ���� ������� ������������
| yan\_geo | connection | ���������� ���������� �� ������� ������������
| weather |
| weather |key | ���� ������� ��� ��������� ���������(https://openweathermap.org/current)
| weather |url | Url ������� ������
| weather |port | ���� ������� ������
| weather |connection | ���������� ���������� �� ������� ������

## ������ �������

��� ������ ��������� �������� ������������ [Conan](https://conan.io)

������������������ ������ ��� Visual Studio 16:

```
mkdir build
cd build
conan install ..
cmake .. -G "Visual Studio 16"
cmake --build . --config Release
```

