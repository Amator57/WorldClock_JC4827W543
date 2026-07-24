#include "timezones_db.h"

#include <string.h>

//------------------------------------------------------------
// Europe
//------------------------------------------------------------

const TimeZoneInfo timeZones[] =
{
    // Ukraine
    { "Kyiv",         "Київ",          2,  0, TZ_DST_EUROPE },

    // Spain
    { "Valencia",     "Валенсія",      1,  0, TZ_DST_EUROPE  },
    { "Madrid",       "Мадрид",        1,  0, TZ_DST_EUROPE  },
    { "Barcelona",    "Барселона",     1,  0, TZ_DST_EUROPE  },

    // Portugal
    { "Lisbon",       "Лісабон",       0,  0, TZ_DST_EUROPE  },

    // United Kingdom
    { "London",       "Лондон",        0,  0, TZ_DST_EUROPE  },

    // Ireland
    { "Dublin",       "Дублін",        0,  0, TZ_DST_EUROPE  },

    // France
    { "Paris",        "Париж",         1,  0, TZ_DST_EUROPE  },

    // Germany
    { "Berlin",       "Берлін",        1,  0, TZ_DST_EUROPE  },

    // Italy
    { "Rome",         "Рим",           1,  0, TZ_DST_EUROPE  },

    // Austria
    { "Vienna",       "Відень",        1,  0, TZ_DST_EUROPE  },

    // Czech Republic
    { "Prague",       "Прага",         1,  0, TZ_DST_EUROPE  },

    // Poland
    { "Warsaw",       "Варшава",       1,  0, TZ_DST_EUROPE  },

    // Hungary
    { "Budapest",     "Будапешт",      1,  0, TZ_DST_EUROPE  },

    // Romania
    { "Bucharest",    "Бухарест",      2,  0, TZ_DST_EUROPE  },

    // Bulgaria
    { "Sofia",        "Софія",         2,  0, TZ_DST_EUROPE  },

    // Greece
    { "Athens",       "Афіни",         2,  0, TZ_DST_EUROPE  },

    // Serbia
    { "Belgrade",     "Белград",       1,  0, TZ_DST_EUROPE  },

    // Netherlands
    { "Amsterdam",    "Амстердам",     1,  0, TZ_DST_EUROPE  },

    // Belgium
    { "Brussels",     "Брюссель",      1,  0, TZ_DST_EUROPE  },

    // Switzerland
    { "Zurich",       "Цюрих",         1,  0, TZ_DST_EUROPE  },

    // Denmark
    { "Copenhagen",   "Копенгаген",    1,  0, TZ_DST_EUROPE  },

    // Norway
    { "Oslo",         "Осло",          1,  0, TZ_DST_EUROPE  },

    // Sweden
    { "Stockholm",    "Стокгольм",     1,  0, TZ_DST_EUROPE  },

    // Finland
    { "Helsinki",     "Гельсінкі",     2,  0, TZ_DST_EUROPE  },

    // Estonia
    { "Tallinn",      "Таллінн",       2,  0, TZ_DST_EUROPE  },

    // Latvia
    { "Riga",         "Рига",          2,  0, TZ_DST_EUROPE  },

    // Lithuania
    { "Vilnius",      "Вільнюс",       2,  0, TZ_DST_EUROPE  },

    // Luxembourg
    { "Luxembourg",   "Люксембург",    1,  0, TZ_DST_EUROPE  },

    // Turkey
    { "Istanbul",     "Стамбул",       3,  0, TZ_DST_NONE },
    //--------------------------------------------------------
    // Canada
    //--------------------------------------------------------

    { "Ottawa",          "Оттава",             -5,  0, TZ_DST_USA_CANADA  },
    { "Toronto",         "Торонто",            -5,  0, TZ_DST_USA_CANADA  },
    { "Montreal",        "Монреаль",           -5,  0, TZ_DST_USA_CANADA  },
    { "Winnipeg",        "Вінніпег",           -6,  0, TZ_DST_USA_CANADA  },
    { "Calgary",         "Калгарі",            -7,  0, TZ_DST_USA_CANADA  },
    { "Edmonton",        "Едмонтон",           -7,  0, TZ_DST_USA_CANADA  },
    { "Vancouver",       "Ванкувер",           -7,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // USA
    //--------------------------------------------------------

    { "Washington",      "Вашингтон",          -5,  0, TZ_DST_USA_CANADA  },
    { "New York",        "Нью-Йорк",           -5,  0, TZ_DST_USA_CANADA  },
    { "Boston",          "Бостон",             -5,  0, TZ_DST_USA_CANADA  },
    { "Philadelphia",    "Філадельфія",        -5,  0, TZ_DST_USA_CANADA  },
    { "Miami",           "Маямі",              -5,  0, TZ_DST_USA_CANADA  },
    { "Atlanta",         "Атланта",            -5,  0, TZ_DST_USA_CANADA  },
    { "Detroit",         "Детройт",            -5,  0, TZ_DST_USA_CANADA  },

    { "Chicago",         "Чикаго",             -6,  0, TZ_DST_USA_CANADA  },
    { "Dallas",          "Даллас",             -6,  0, TZ_DST_USA_CANADA  },
    { "Houston",         "Х'юстон",            -6,  0, TZ_DST_USA_CANADA  },
    { "Minneapolis",     "Міннеаполіс",        -6,  0, TZ_DST_USA_CANADA  },
    { "New Orleans",     "Новий Орлеан",       -6,  0, TZ_DST_USA_CANADA  },

    { "Denver",          "Денвер",             -7,  0, TZ_DST_USA_CANADA  },
    { "Salt Lake City",  "Солт-Лейк-Сіті",     -7,  0, TZ_DST_USA_CANADA  },
    { "Phoenix",         "Фінікс",             -7,  0, TZ_DST_NONE },

    { "Las Vegas",       "Лас-Вегас",          -8,  0, TZ_DST_USA_CANADA  },
    { "Los Angeles",     "Лос-Анджелес",       -8,  0, TZ_DST_USA_CANADA  },
    { "San Diego",       "Сан-Дієго",          -8,  0, TZ_DST_USA_CANADA  },
    { "San Francisco",   "Сан-Франциско",      -8,  0, TZ_DST_USA_CANADA  },
    { "Seattle",         "Сіетл",              -8,  0, TZ_DST_USA_CANADA  },

    { "Anchorage",       "Анкоридж",           -9,  0, TZ_DST_USA_CANADA  },
    { "Honolulu",        "Гонолулу",          -10,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // Mexico
    //--------------------------------------------------------

    { "Mexico City",     "Мехіко",             -6,  0, TZ_DST_NONE },
    { "Monterrey",       "Монтеррей",          -6,  0, TZ_DST_NONE },
    { "Cancun",          "Канкун",             -5,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // Central America
    //--------------------------------------------------------

    { "Guatemala City",  "Гватемала",          -6,  0, TZ_DST_NONE },
    { "San Jose",        "Сан-Хосе",           -6,  0, TZ_DST_NONE },
    { "Panama City",     "Панама",             -5,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // South America
    //--------------------------------------------------------

    { "Bogota",          "Богота",             -5,  0, TZ_DST_NONE },
    { "Lima",            "Ліма",               -5,  0, TZ_DST_NONE },
    { "Quito",           "Кіто",               -5,  0, TZ_DST_NONE },
    { "Caracas",         "Каракас",            -4,  0, TZ_DST_NONE },

    { "La Paz",          "Ла-Пас",             -4,  0, TZ_DST_NONE },
    { "Santiago",        "Сантьяго",           -4,  0, TZ_DST_NONE  },

    { "Buenos Aires",    "Буенос-Айрес",       -3,  0, TZ_DST_NONE },
    { "Montevideo",      "Монтевідео",         -3,  0, TZ_DST_NONE  },

    { "Sao Paulo",       "Сан-Паулу",          -3,  0, TZ_DST_NONE },
    { "Rio de Janeiro",  "Ріо-де-Жанейро",     -3,  0, TZ_DST_NONE },
        //--------------------------------------------------------
    // Middle East
    //--------------------------------------------------------

    { "Jerusalem",      "Єрусалим",          2,  0, TZ_DST_NONE  },
    { "Amman",          "Амман",             3,  0, TZ_DST_NONE  },
    { "Beirut",         "Бейрут",            2,  0, TZ_DST_NONE  },

    { "Riyadh",         "Ер-Ріяд",           3,  0, TZ_DST_NONE },
    { "Doha",           "Доха",              3,  0, TZ_DST_NONE },
    { "Kuwait City",    "Ель-Кувейт",        3,  0, TZ_DST_NONE },
    { "Manama",         "Манама",            3,  0, TZ_DST_NONE },
    { "Abu Dhabi",      "Абу-Дабі",          4,  0, TZ_DST_NONE },
    { "Dubai",          "Дубай",             4,  0, TZ_DST_NONE },
    { "Muscat",         "Маскат",            4,  0, TZ_DST_NONE },

    { "Tehran",         "Тегеран",           3, 30, TZ_DST_NONE },

    //--------------------------------------------------------
    // Central Asia
    //--------------------------------------------------------

    { "Baku",           "Баку",              4,  0, TZ_DST_NONE },
    { "Tbilisi",        "Тбілісі",           4,  0, TZ_DST_NONE },
    { "Yerevan",        "Єреван",            4,  0, TZ_DST_NONE },

    { "Tashkent",       "Ташкент",           5,  0, TZ_DST_NONE },
    { "Samarkand",      "Самарканд",         5,  0, TZ_DST_NONE },

    { "Almaty",         "Алмати",            5,  0, TZ_DST_NONE },
    { "Astana",         "Астана",            5,  0, TZ_DST_NONE },

    { "Bishkek",        "Бішкек",            6,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // South Asia
    //--------------------------------------------------------

    { "Karachi",        "Карачі",            5,  0, TZ_DST_NONE },
    { "Islamabad",      "Ісламабад",         5,  0, TZ_DST_NONE },

    { "Delhi",          "Делі",              5, 30, TZ_DST_NONE },
    { "Mumbai",         "Мумбаї",            5, 30, TZ_DST_NONE },
    { "Bangalore",      "Бенгалуру",         5, 30, TZ_DST_NONE },
    { "Chennai",        "Ченнаї",            5, 30, TZ_DST_NONE },
    { "Kolkata",        "Колката",           5, 30, TZ_DST_NONE },

    { "Kathmandu",      "Катманду",          5, 45, TZ_DST_NONE },

    { "Colombo",        "Коломбо",           5, 30, TZ_DST_NONE },

    { "Dhaka",          "Дакка",             6,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // Southeast Asia
    //--------------------------------------------------------

    { "Yangon",         "Янгон",             6, 30, TZ_DST_NONE },

    { "Bangkok",        "Бангкок",           7,  0, TZ_DST_NONE },
    { "Phnom Penh",     "Пномпень",          7,  0, TZ_DST_NONE },
    { "Ho Chi Minh",    "Хошимін",           7,  0, TZ_DST_NONE },
    { "Hanoi",          "Ханой",             7,  0, TZ_DST_NONE },
    { "Jakarta",        "Джакарта",          7,  0, TZ_DST_NONE },

    { "Singapore",      "Сінгапур",          8,  0, TZ_DST_NONE },
    { "Kuala Lumpur",   "Куала-Лумпур",      8,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // East Asia
    //--------------------------------------------------------

    { "Beijing",        "Пекін",             8,  0, TZ_DST_NONE },
    { "Shanghai",       "Шанхай",            8,  0, TZ_DST_NONE },
    { "Hong Kong",      "Гонконг",           8,  0, TZ_DST_NONE },
    { "Taipei",         "Тайбей",            8,  0, TZ_DST_NONE },

    { "Seoul",          "Сеул",              9,  0, TZ_DST_NONE },

    { "Tokyo",          "Токіо",             9,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // Far East
    //--------------------------------------------------------

    { "Vladivostok",    "Владивосток",      10,  0, TZ_DST_NONE },
    { "Magadan",        "Магадан",          11,  0, TZ_DST_NONE },
    { "Petropavlovsk",  "Петропавловськ",   12,  0, TZ_DST_NONE },
        //--------------------------------------------------------
    // Australia
    //--------------------------------------------------------

    { "Perth",           "Перт",              8,  0, TZ_DST_NONE },
    { "Darwin",          "Дарвін",            9, 30, TZ_DST_NONE },
    { "Adelaide",        "Аделаїда",          9, 30, TZ_DST_AUSTRALIA  },
    { "Brisbane",        "Брисбен",          10,  0, TZ_DST_NONE },
    { "Sydney",          "Сідней",           10,  0, TZ_DST_AUSTRALIA  },
    { "Canberra",        "Канберра",         10,  0, TZ_DST_AUSTRALIA  },
    { "Melbourne",       "Мельбурн",         10,  0, TZ_DST_AUSTRALIA  },
    { "Hobart",          "Гобарт",           10,  0, TZ_DST_AUSTRALIA  },

    //--------------------------------------------------------
    // New Zealand
    //--------------------------------------------------------

    { "Auckland",        "Окленд",           12,  0, TZ_DST_NEW_ZEALAND  },
    { "Wellington",      "Веллінгтон",       12,  0, TZ_DST_NEW_ZEALAND  },
    { "Christchurch",    "Крайстчерч",       12,  0, TZ_DST_NEW_ZEALAND  },

    //--------------------------------------------------------
    // Pacific
    //--------------------------------------------------------

    { "Guam",            "Гуам",             10,  0, TZ_DST_NONE },
    { "Port Moresby",    "Порт-Морсбі",      10,  0, TZ_DST_NONE },
    { "Noumea",          "Нумеа",            11,  0, TZ_DST_NONE },
    { "Suva",            "Сува",             12,  0, TZ_DST_NONE },
    { "Apia",            "Апіа",             13,  0, TZ_DST_NONE },
    { "Kiritimati",      "Кірітіматі",       14,  0, TZ_DST_NONE },

    { "Honiara",         "Хоніара",          11,  0, TZ_DST_NONE },
    { "Nukualofa",       "Нукуалофа",        13,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // Africa
    //--------------------------------------------------------

    { "Cairo",           "Каїр",              2,  0, TZ_DST_EGYPT  },
    { "Alexandria",      "Александрія",       2,  0, TZ_DST_EGYPT  },

    { "Tripoli",         "Триполі",           2,  0, TZ_DST_NONE },
    { "Tunis",           "Туніс",             1,  0, TZ_DST_NONE },
    { "Algiers",         "Алжир",             1,  0, TZ_DST_NONE },
    { "Casablanca",      "Касабланка",        1,  0, TZ_DST_NONE  },

    { "Khartoum",        "Хартум",            2,  0, TZ_DST_NONE },

    { "Addis Ababa",     "Аддис-Абеба",       3,  0, TZ_DST_NONE },
    { "Nairobi",         "Найробі",           3,  0, TZ_DST_NONE },
    { "Kampala",         "Кампала",           3,  0, TZ_DST_NONE },
    { "Dar es Salaam",   "Дар-ес-Салам",      3,  0, TZ_DST_NONE },

    { "Lagos",           "Лагос",             1,  0, TZ_DST_NONE },
    { "Abuja",           "Абуджа",            1,  0, TZ_DST_NONE },
    { "Accra",           "Аккра",             0,  0, TZ_DST_NONE },

    { "Johannesburg",    "Йоганнесбург",      2,  0, TZ_DST_NONE },
    { "Cape Town",       "Кейптаун",          2,  0, TZ_DST_NONE },

    { "Windhoek",        "Віндгук",           2,  0, TZ_DST_NONE },

    //--------------------------------------------------------
    // UTC reference
    //--------------------------------------------------------

    { "UTC",             "UTC",               0,  0, TZ_DST_NONE }
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
