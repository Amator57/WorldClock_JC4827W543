#include "timezones_db.h"

#include <string.h>

//------------------------------------------------------------
// Europe
//------------------------------------------------------------

const TimeZoneInfo timeZones[] =
{
    // Ukraine
    { "Kyiv",         "Київ",          2,  0, true  },

    // Spain
    { "Valencia",     "Валенсія",      1,  0, true  },
    { "Madrid",       "Мадрид",        1,  0, true  },
    { "Barcelona",    "Барселона",     1,  0, true  },

    // Portugal
    { "Lisbon",       "Лісабон",       0,  0, true  },

    // United Kingdom
    { "London",       "Лондон",        0,  0, true  },

    // Ireland
    { "Dublin",       "Дублін",        0,  0, true  },

    // France
    { "Paris",        "Париж",         1,  0, true  },

    // Germany
    { "Berlin",       "Берлін",        1,  0, true  },

    // Italy
    { "Rome",         "Рим",           1,  0, true  },

    // Austria
    { "Vienna",       "Відень",        1,  0, true  },

    // Czech Republic
    { "Prague",       "Прага",         1,  0, true  },

    // Poland
    { "Warsaw",       "Варшава",       1,  0, true  },

    // Hungary
    { "Budapest",     "Будапешт",      1,  0, true  },

    // Romania
    { "Bucharest",    "Бухарест",      2,  0, true  },

    // Bulgaria
    { "Sofia",        "Софія",         2,  0, true  },

    // Greece
    { "Athens",       "Афіни",         2,  0, true  },

    // Serbia
    { "Belgrade",     "Белград",       1,  0, true  },

    // Netherlands
    { "Amsterdam",    "Амстердам",     1,  0, true  },

    // Belgium
    { "Brussels",     "Брюссель",      1,  0, true  },

    // Switzerland
    { "Zurich",       "Цюрих",         1,  0, true  },

    // Denmark
    { "Copenhagen",   "Копенгаген",    1,  0, true  },

    // Norway
    { "Oslo",         "Осло",          1,  0, true  },

    // Sweden
    { "Stockholm",    "Стокгольм",     1,  0, true  },

    // Finland
    { "Helsinki",     "Гельсінкі",     2,  0, true  },

    // Estonia
    { "Tallinn",      "Таллінн",       2,  0, true  },

    // Latvia
    { "Riga",         "Рига",          2,  0, true  },

    // Lithuania
    { "Vilnius",      "Вільнюс",       2,  0, true  },

    // Luxembourg
    { "Luxembourg",   "Люксембург",    1,  0, true  },

    // Turkey
    { "Istanbul",     "Стамбул",       3,  0, false },
    //--------------------------------------------------------
    // Canada
    //--------------------------------------------------------

    { "Ottawa",          "Оттава",             -5,  0, true  },
    { "Toronto",         "Торонто",            -5,  0, true  },
    { "Montreal",        "Монреаль",           -5,  0, true  },
    { "Winnipeg",        "Вінніпег",           -6,  0, true  },
    { "Calgary",         "Калгарі",            -7,  0, true  },
    { "Edmonton",        "Едмонтон",           -7,  0, true  },
    { "Vancouver",       "Ванкувер",           -8,  0, true  },

    //--------------------------------------------------------
    // USA
    //--------------------------------------------------------

    { "Washington",      "Вашингтон",          -5,  0, true  },
    { "New York",        "Нью-Йорк",           -5,  0, true  },
    { "Boston",          "Бостон",             -5,  0, true  },
    { "Philadelphia",    "Філадельфія",        -5,  0, true  },
    { "Miami",           "Маямі",              -5,  0, true  },
    { "Atlanta",         "Атланта",            -5,  0, true  },
    { "Detroit",         "Детройт",            -5,  0, true  },

    { "Chicago",         "Чикаго",             -6,  0, true  },
    { "Dallas",          "Даллас",             -6,  0, true  },
    { "Houston",         "Х'юстон",            -6,  0, true  },
    { "Minneapolis",     "Міннеаполіс",        -6,  0, true  },
    { "New Orleans",     "Новий Орлеан",       -6,  0, true  },

    { "Denver",          "Денвер",             -7,  0, true  },
    { "Salt Lake City",  "Солт-Лейк-Сіті",     -7,  0, true  },
    { "Phoenix",         "Фінікс",             -7,  0, false },

    { "Las Vegas",       "Лас-Вегас",          -8,  0, true  },
    { "Los Angeles",     "Лос-Анджелес",       -8,  0, true  },
    { "San Diego",       "Сан-Дієго",          -8,  0, true  },
    { "San Francisco",   "Сан-Франциско",      -8,  0, true  },
    { "Seattle",         "Сіетл",              -8,  0, true  },

    { "Anchorage",       "Анкоридж",           -9,  0, true  },
    { "Honolulu",        "Гонолулу",          -10,  0, false },

    //--------------------------------------------------------
    // Mexico
    //--------------------------------------------------------

    { "Mexico City",     "Мехіко",             -6,  0, false },
    { "Monterrey",       "Монтеррей",          -6,  0, false },
    { "Cancun",          "Канкун",             -5,  0, false },

    //--------------------------------------------------------
    // Central America
    //--------------------------------------------------------

    { "Guatemala City",  "Гватемала",          -6,  0, false },
    { "San Jose",        "Сан-Хосе",           -6,  0, false },
    { "Panama City",     "Панама",             -5,  0, false },

    //--------------------------------------------------------
    // South America
    //--------------------------------------------------------

    { "Bogota",          "Богота",             -5,  0, false },
    { "Lima",            "Ліма",               -5,  0, false },
    { "Quito",           "Кіто",               -5,  0, false },
    { "Caracas",         "Каракас",            -4,  0, false },

    { "La Paz",          "Ла-Пас",             -4,  0, false },
    { "Santiago",        "Сантьяго",           -4,  0, true  },

    { "Buenos Aires",    "Буенос-Айрес",       -3,  0, false },
    { "Montevideo",      "Монтевідео",         -3,  0, true  },

    { "Sao Paulo",       "Сан-Паулу",          -3,  0, false },
    { "Rio de Janeiro",  "Ріо-де-Жанейро",     -3,  0, false },
        //--------------------------------------------------------
    // Middle East
    //--------------------------------------------------------

    { "Jerusalem",      "Єрусалим",          2,  0, true  },
    { "Amman",          "Амман",             3,  0, true  },
    { "Beirut",         "Бейрут",            2,  0, true  },

    { "Riyadh",         "Ер-Ріяд",           3,  0, false },
    { "Doha",           "Доха",              3,  0, false },
    { "Kuwait City",    "Ель-Кувейт",        3,  0, false },
    { "Manama",         "Манама",            3,  0, false },
    { "Abu Dhabi",      "Абу-Дабі",          4,  0, false },
    { "Dubai",          "Дубай",             4,  0, false },
    { "Muscat",         "Маскат",            4,  0, false },

    { "Tehran",         "Тегеран",           3, 30, false },

    //--------------------------------------------------------
    // Central Asia
    //--------------------------------------------------------

    { "Baku",           "Баку",              4,  0, false },
    { "Tbilisi",        "Тбілісі",           4,  0, false },
    { "Yerevan",        "Єреван",            4,  0, false },

    { "Tashkent",       "Ташкент",           5,  0, false },
    { "Samarkand",      "Самарканд",         5,  0, false },

    { "Almaty",         "Алмати",            5,  0, false },
    { "Astana",         "Астана",            5,  0, false },

    { "Bishkek",        "Бішкек",            6,  0, false },

    //--------------------------------------------------------
    // South Asia
    //--------------------------------------------------------

    { "Karachi",        "Карачі",            5,  0, false },
    { "Islamabad",      "Ісламабад",         5,  0, false },

    { "Delhi",          "Делі",              5, 30, false },
    { "Mumbai",         "Мумбаї",            5, 30, false },
    { "Bangalore",      "Бенгалуру",         5, 30, false },
    { "Chennai",        "Ченнаї",            5, 30, false },
    { "Kolkata",        "Колката",           5, 30, false },

    { "Kathmandu",      "Катманду",          5, 45, false },

    { "Colombo",        "Коломбо",           5, 30, false },

    { "Dhaka",          "Дакка",             6,  0, false },

    //--------------------------------------------------------
    // Southeast Asia
    //--------------------------------------------------------

    { "Yangon",         "Янгон",             6, 30, false },

    { "Bangkok",        "Бангкок",           7,  0, false },
    { "Phnom Penh",     "Пномпень",          7,  0, false },
    { "Ho Chi Minh",    "Хошимін",           7,  0, false },
    { "Hanoi",          "Ханой",             7,  0, false },
    { "Jakarta",        "Джакарта",          7,  0, false },

    { "Singapore",      "Сінгапур",          8,  0, false },
    { "Kuala Lumpur",   "Куала-Лумпур",      8,  0, false },

    //--------------------------------------------------------
    // East Asia
    //--------------------------------------------------------

    { "Beijing",        "Пекін",             8,  0, false },
    { "Shanghai",       "Шанхай",            8,  0, false },
    { "Hong Kong",      "Гонконг",           8,  0, false },
    { "Taipei",         "Тайбей",            8,  0, false },

    { "Seoul",          "Сеул",              9,  0, false },

    { "Tokyo",          "Токіо",             9,  0, false },

    //--------------------------------------------------------
    // Far East
    //--------------------------------------------------------

    { "Vladivostok",    "Владивосток",      10,  0, false },
    { "Magadan",        "Магадан",          11,  0, false },
    { "Petropavlovsk",  "Петропавловськ",   12,  0, false },
        //--------------------------------------------------------
    // Australia
    //--------------------------------------------------------

    { "Perth",           "Перт",              8,  0, false },
    { "Darwin",          "Дарвін",            9, 30, false },
    { "Adelaide",        "Аделаїда",          9, 30, true  },
    { "Brisbane",        "Брисбен",          10,  0, false },
    { "Sydney",          "Сідней",           10,  0, true  },
    { "Canberra",        "Канберра",         10,  0, true  },
    { "Melbourne",       "Мельбурн",         10,  0, true  },
    { "Hobart",          "Гобарт",           10,  0, true  },

    //--------------------------------------------------------
    // New Zealand
    //--------------------------------------------------------

    { "Auckland",        "Окленд",           12,  0, true  },
    { "Wellington",      "Веллінгтон",       12,  0, true  },
    { "Christchurch",    "Крайстчерч",       12,  0, true  },

    //--------------------------------------------------------
    // Pacific
    //--------------------------------------------------------

    { "Guam",            "Гуам",             10,  0, false },
    { "Port Moresby",    "Порт-Морсбі",      10,  0, false },
    { "Noumea",          "Нумеа",            11,  0, false },
    { "Suva",            "Сува",             12,  0, false },
    { "Apia",            "Апіа",             13,  0, false },
    { "Kiritimati",      "Кірітіматі",       14,  0, false },

    { "Honiara",         "Хоніара",          11,  0, false },
    { "Nukualofa",       "Нукуалофа",        13,  0, false },

    //--------------------------------------------------------
    // Africa
    //--------------------------------------------------------

    { "Cairo",           "Каїр",              2,  0, true  },
    { "Alexandria",      "Александрія",       2,  0, true  },

    { "Tripoli",         "Триполі",           2,  0, false },
    { "Tunis",           "Туніс",             1,  0, false },
    { "Algiers",         "Алжир",             1,  0, false },
    { "Casablanca",      "Касабланка",        1,  0, true  },

    { "Khartoum",        "Хартум",            2,  0, false },

    { "Addis Ababa",     "Аддис-Абеба",       3,  0, false },
    { "Nairobi",         "Найробі",           3,  0, false },
    { "Kampala",         "Кампала",           3,  0, false },
    { "Dar es Salaam",   "Дар-ес-Салам",      3,  0, false },

    { "Lagos",           "Лагос",             1,  0, false },
    { "Abuja",           "Абуджа",            1,  0, false },
    { "Accra",           "Аккра",             0,  0, false },

    { "Johannesburg",    "Йоганнесбург",      2,  0, false },
    { "Cape Town",       "Кейптаун",          2,  0, false },

    { "Windhoek",        "Віндгук",           2,  0, false },

    //--------------------------------------------------------
    // UTC reference
    //--------------------------------------------------------

    { "UTC",             "UTC",               0,  0, false }
};

//------------------------------------------------------------

const uint16_t timeZonesCount =
    sizeof(timeZones) / sizeof(timeZones[0]);

//------------------------------------------------------------

const TimeZoneInfo *getTimeZone(uint16_t index)
{
    if (index >= timeZonesCount)
        return nullptr;

    return &timeZones[index];
}

//------------------------------------------------------------

const TimeZoneInfo *findTimeZoneByEnglish(const char *name)
{
    for (uint16_t i = 0; i < timeZonesCount; i++)
    {
        if (strcmp(name, timeZones[i].nameEN) == 0)
            return &timeZones[i];
    }

    return nullptr;
}

//------------------------------------------------------------

const TimeZoneInfo *findTimeZoneByUkrainian(const char *name)
{
    for (uint16_t i = 0; i < timeZonesCount; i++)
    {
        if (strcmp(name, timeZones[i].nameUA) == 0)
            return &timeZones[i];
    }

    return nullptr;
}
