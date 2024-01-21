/*
 *      SC - spreadsheet calculator, version 3.1.
 *
 *      (c) Copyright DEMOS, 1989
 *      All rights reserved.
 *
 *      Authors: Serg I. Ryshkov, Serg V. Vakulenko
 *
 *      Mon Jul 31 17:17:13 MSK 1989
 */

/* # define DEBUG */

# ifdef MAKEHELPFILE
#    undef MESGFILE
# endif

# ifdef MAKELMESG
#    undef MESGFILE
# endif

# ifdef MAKERMESG
#    undef MESGFILE
# endif

# if defined (MAKERMESG) || defined (MAKELMESG)
#    undef MESGFILE
#    undef RUS
#    define RUS
# endif

# ifdef MESGFILE

# include <stdio.h>

# define RMESGFILE      "sc.rmsg"       /* number of lines in cache */
# define LMESGFILE      "sc.lmsg"       /* number of lines in cache */

# define POOLSIZE       16      /* number of lines in cache */
# define LINESIZE       100     /* maximum length of line */
# define MESGSIZE       160     /* maximum number of diagnostics */

# define MAPBUSY        01
# define MAPRUS         02

static char pool [POOLSIZE] [LINESIZE];

static struct {
	short mflags;
	short msgindex;
	long time;
} map [POOLSIZE];

static struct index {
	short seek;
	short poolindex;
} lindex [MESGSIZE];

static nmesg;
static long timecount;
static FILE *lfile;

# ifdef RUS
static struct index rindex [MESGSIZE];
static FILE *rfile;
# endif

extern char *strcpy (), *strcat ();

static mesginit ()
{
	char name [LINESIZE];
	register i;

	timecount = 1;
	nmesg = i = 0;
	strcat (strcpy (name, MESGFILE), LMESGFILE);
	if (! (lfile = fopen (name, "r")))
		return;
	for (;;) {
		lindex[i].seek = ftell (lfile);
		if (! fgetline (name, lfile))
			break;
		lindex[i].poolindex = -1;
		++i;
	}
	if (! i)
		return;
# ifdef RUS
	strcat (strcpy (name, MESGFILE), RMESGFILE);
	if (! (rfile = fopen (name, "r")))
		return;
	for (;;) {
		rindex[nmesg].seek = ftell (rfile);
		if (! fgetline (name, rfile))
			break;
		rindex[nmesg].poolindex = -1;
		++nmesg;
	}
	if (! nmesg)
		return;
	if (i < nmesg)
# endif
	nmesg = i;
}

static fgetline (s, f)
register char *s;
register FILE *f;
{
	/* read line from file until '\0'; return 0 if eof */
	while ((*s++ = getc (f)) != EOF)
		if (! s[-1])
			return (1);
	return (0);
}

static freeline ()
{
	register i;
	register mintime, minindex;
	register struct index *ind;

# ifdef DEBUG
	printf ("freeline called\n");
# endif
	/* find free place in pool */
	for (i=0; i<POOLSIZE; ++i)
		if (! (map[i].mflags & MAPBUSY)) {
# ifdef DEBUG
			printf ("freeline return (%d)\n", i);
# endif
			return (i);
		}
	/* pool is full; find the oldest line */
	mintime = map[0].time;
	minindex = 0;
	for (i=1; i<POOLSIZE; ++i)
		if (map[i].time < mintime)
			minindex = i;
	/* remove line from pool */
# ifdef RUS
	ind = (map[minindex].mflags & MAPRUS) ? rindex : lindex;
# else
	ind = lindex;
# endif
	ind[map[minindex].msgindex].poolindex = -1;
	map[minindex].mflags = 0;
# ifdef DEBUG
	printf ("freeline return (%d)\n", minindex);
# endif
	return (minindex);
}

# endif /* MESGFILE */

# ifdef RUS
int rusin, rusout;

# ifdef MESGFILE
char *rmesg (n)
{
	if (! nmesg)
		mesginit ();
	if (! nmesg) {
		outerr ("message init error\n");
		exit (1);
	}
	if (n<0 || n>nmesg)
		return ("");
	if (rindex[n].poolindex >= 0) {
		/* line is in cache */
		map[rindex[n].poolindex].time = ++timecount;
		return (pool[rindex[n].poolindex]);
	}
	/* get free pool index */
	rindex[n].poolindex = freeline ();
	map[rindex[n].poolindex].time = ++timecount;
	map[rindex[n].poolindex].msgindex = n;
	map[rindex[n].poolindex].mflags = MAPBUSY | MAPRUS;
	/* set file pointer on the beginning of line */
	fseek (rfile, (long) rindex[n].seek, 0);
	/* read line from file */
	fgetline (pool[rindex[n].poolindex], rfile);
	return (pool[rindex[n].poolindex]);
}
# else /* MESGFILE */
char *rmesg [] = {
/* 0  */        "Слишком длинный вход в termcap\n",
/* 1  */        "Плохой вход в termcap\n",
/* 2  */        "Бесконечный tc= цикл\n",
/* 3  */        "Таблицу нельзя увеличить",
/* 4  */        "Таблицу нельзя расширить",
/* 5  */        "Таблицу нельзя удлинить",
/* 6  */        "Первый спрятанный диапазон %s-%s.",
/* 7  */        "Введите первый столбец: ",
/* 8  */        "Введите последний столбец: ",
/* 9  */        "Первый спрятанный диапазон %d-%d.",
/* 10 */        "Введите первую строку: ",
/* 11 */        "Введите последнюю строку: ",
/* 12 */        "Нельзя спрятать последнюю строку",
/* 13 */        "Нельзя спрятать последний столбец",
/* 14 */        "Копировать область %s%d-%s%d в: ",
/* 15 */        "Заполнить область %s%d-%s%d числами, начиная с: ",
/* 16 */        "Заполнить область %s%d-%s%d числами, начиная с %s, шаг: ",
/* 17 */        "Область %s%d-%s%d заполнена числами, начиная с %s, шаг %s.",
/* 18 */        "Введите имя ячейки: ",
/* 19 */        "Записать таблицу в файл: ",
/* 20 */        "Считать файл: ",
/* 21 */        "Слить текущую таблицу и файл: ",
/* 22 */        "Печатать таблицу в файл: ",
/* 23 */        "Записать таблицу в формате tbl в файл: ",
/* 24 */        "В области %s%d-%s%d формулы заменены числами.",
/* 25 */        "Область %s%d-%s%d удалена.",
/* 26 */        "Текущая точность %d, допустимый диапазон 0-72.",
/* 27 */        "Введите точность: ",
/* 28 */        "Текущая ширина %d, допустимый диапазон 0-72.",
/* 29 */        "Введите ширину для текущего столбца: ",
/* 30 */        "Суффикс \"%s\" (по умолчанию).",
/* 31 */        "Не могу создать %s",
/* 32 */        "Файл '%s' записан.",
/* 33 */        "Не могу прочитать %s",
/* 34 */        "вызов: sc [-] [-r#] [-c#] [-a] [-b] [-l] [-m] [файл]\n",
/* 35 */        "не могу инициализировать терминал\n",
/* 36 */        "Мало памяти!",
/* 37 */        "мало памяти\n",
/* 38 */        "^Pшкола ^Cотказ ^Xэкран ^Gграфик ^Aимя ^Fформула ^Vчисло ^Tтекст ^Eредактор",
/* 39 */        "^Pшкола ^Cотказ ^Xэкран ^Gграфик",
/* 40 */        "Ответьте y(es) или n(o).",
/* 41 */        "\3Выйти из табличного процессора? \2",
/* 42 */        "\3Сохранить таблицу в рабочем файле 'workfile'? \2",
/* 43 */        "\3Сохранить файл \"%s\" ? \2",
/* 44 */        "размер %s%d",
/* 45 */        ", конец %s%d",
/* 46 */        ", метка %s%d",
/* 47 */        ", авто",
/* 48 */        ", занято %ld",
/* 49 */        ", свободно %ld",
/* 50 */        "\2формула\1 %s \3%s\2",
/* 51 */        "\2число\1 %s \3%s\2",
/* 52 */        "\2текст\1 %s \3\"%s\"\2",
/* 53 */        "\2пусто",
/* 54 */        "в\3Л\2ево в\3П\2раво \3Ц\2ентр по \3У\2молчанию \3Т\2очность",
/* 55 */        "\1^Lлево ^Rправо ^Dуничт ^Uвст ^Xэкран ^Cотказ Backspace Tab Return \3Ред\2",
/* 56 */        "Текущая высота %d, допустимый диапазон %d-%d.",
/* 57 */        "Введите новую высоту таблицы: ",
/* 58 */        "Текущая ширина %d, допустимый диапазон %d-%d.",
/* 59 */        "Введите новую ширину таблицы: ",
/* 60 */        "Ответьте д(а) или н(ет).",
/* 61 */        "Нет информации",
/* 62 */        "Не могу открыть %s",
/* 63 */        "\1\16Нажмите любую клавишу...\2\17",
/* 64 */        "\3Ф\2айл/ \3Я\2чейка/ \3С\2трока/ с\3Т\2олбец/ \3О\2бласть/ \3Д\2вижение/ \3Г\2рафик/ \3Р\2азное/ в\3Ы\2ход",
/* 65 */        "\3М\2етка \3К\2опия \3С\2тереть \3В\2ернуть \3Ф\2ормат/",
/* 66 */        "\3Ш\2ирина \3В\2ысота \3С\2ократить \3П\2еревычислить \3А\2вто \3Р\2амка \3Я\2зык \3Т\2очность",
/* 67 */        "\3Я\2чейка в\3В\2ерх в\3Н\2из в\3Л\2ево в\3П\2раво \3К\2онец",
/* 68 */        "\3М\2етка \3К\2опия \3З\2аполнить \3С\2тереть \3В\2ернуть \3Ч\2исла \3Ф\2ормат/",
/* 69 */        "\3З\2апись \3Ч\2тение \3С\2лияние \3П\2ечать \3Г\2рафик \3Т\2бл",
/* 70 */        "вст\3А\2вить \3К\2опировать с\3П\2рятать \3О\2ткрыть \3С\2тереть \3В\2ернуть \3Ч\2исла \3Ф\2ормат/",
/* 71 */        "вст\3А\2вить \3К\2опировать с\3П\2рятать \3О\2ткрыть \3С\2тереть \3В\2ернуть \3Ш\2ирина \3Ч\2исла \3Ф\2ормат/",
/* 72 */        "Ячейка/",
/* 73 */        "Разное/",
/* 74 */        "Движение/",
/* 75 */        "Файл/",
/* 76 */        "Область/",
/* 77 */        "Строка/",
/* 78 */        "сТолбец/",
/* 79 */        "вЫход",
/* 80 */        "Х-У",
/* 81 */        "Стереть",
/* 82 */        "Вернуть",
/* 83 */        "Метка",
/* 84 */        "Копия",
/* 85 */        "Язык",
/* 86 */        "Перевычислить",
/* 87 */        "Ширина",
/* 88 */        "Высота",
/* 89 */        "Сократить",
/* 90 */        "Конец",
/* 91 */        "Ячейка",
/* 92 */        "вНиз",
/* 93 */        "вВерх",
/* 94 */        "вЛево",
/* 95 */        "вПраво",
/* 96 */        "Запись",
/* 97 */        "Чтение",
/* 98 */        "Слияние",
/* 99 */        "Печать",
/* 100 */       "Тбл",
/* 101 */       "Числа",
/* 102 */       "сПрятать",
/* 103 */       "Открыть",
/* 104 */       "Заполнить",
/* 105 */       "встАвить",
/* 106 */       "Точность",
/* 107 */       "\3Т\2ип/ \3М\2етка ось\3А\2 ось\3Б\2 ось\3В\2 ось\3Г\2 \3С\2брос \3П\2ечать",
/* 108 */       "График/",
/* 109 */       "Авто",
/* 110 */       "осьА",
/* 111 */       "осьБ",
/* 112 */       "осьВ",
/* 113 */       "График/Тип/",
/* 114 */       "Сброс",
/* 115 */       "Тип '%s'",
/* 116 */       ", оси %s%d-%s%d",
/* 117 */       "осьГ",
/* 118 */       "График",
/* 119 */       "Нет данных для графика.",
/* 120 */       "Не могу создать процесс.",
/* 121 */       "Ошибка в %s.",
/* 122 */       "Не могу создать трубу.",
/* 123 */       "\3Д\2иаграмма \3Г\2рафик г\3И\2стограмма \3С\2овмещенная-гистограмма \3Х\2-У",
/* 124 */       "диаграмма",
/* 125 */       "график",
/* 126 */       "гистограмма",
/* 127 */       "совмещенная гистограмма",
/* 128 */       "Диаграмма",
/* 129 */       "График",
/* 130 */       "гИстограмма",
/* 131 */       "Сгистограмма",
/* 132 */       "Установлен тип '%s'.",
/* 133 */       "Записать график в файл: ",
/* 134 */       "Минуточку... ",
/* 135 */       "Рамка",
/* 136 */       "Печать",
/* 137 */       "Ячейка/Формат/",
/* 138 */       "Область/Формат/",
/* 139 */       "Строка/Формат/",
/* 140 */       "сТолбец/Формат/",
/* 141 */       "вЛево",
/* 142 */       "вПраво",
/* 143 */       "Центр",
/* 144 */       "Умолчание",
/* 145 */	"\3М\2етка в\3Л\2ево в\3П\2раво \3Ц\2ентр по \3У\2молчанию \3Т\2очность",
/* 146 */       "ШЭТ  Версия 3.1   (c) ДЕМОС, 1989   Москва, Новокузнецкая, 33",
};
# endif /* MESGFILE */

# if defined (MAKEHELPFILE) || !defined (NOHELP) && !defined (HELPFILE)
char *rhelp1 [] = {
"                              \3\16Передвижение\17\2\n",
"\3^D\2 или стрелка \3'вниз' \1-\2 вниз            \3^L\2 или стрелка \3'влево' \1-\2 влево\n",
"\3^U\2 или стрелка \3'вверх' \1-\2 вверх          \3^R\2 или стрелка \3'вправо' \1-\2 вправо\n",
"\3^Y\2 или клавиша \3'Home' \1-\2 страница влево  \3^K\2 или клавиша \3'PgUp' \1-\2 страница вверх\n",
"\3^Z\2 или клавиша \3'End' \1-\2 страница вправо  \3^B\2 или клавиша \3'PgDn' \1-\2 страница вниз\n",
"\3J\2ump/\3U\2p \1-\2 к первой строке               \3J\2ump/\3L\2eft \1-\2 к первому столбцу\n",
"\3J\2ump/\3D\2own \1-\2 к посл. ячейке в столбце    \3J\2ump/\3R\2ight \1-\2 к посл. ячейке в строке\n",
"\3J\2ump/\3C\2ell \1-\2 к ячейке по имени           \3J\2ump/\3E\2nd \1-\2 в конец таблицы\n",
"\n",
"                                \3\16Разное\17\2\n",
"\3^X \1-\2 перерисовка экрана                 \3^C \1-\2 прервать текущую команду\n",
"\n",
"                             \3\16Ввод значений\17\2\n",
"\3^A \1-\2 ввести имя текущей ячейки          \3^V \1-\2 ввести значение текущей ячейки\n",
"\3^F \1-\2 ввести формулу из текущей ячейки   \3^T \1-\2 ввести строку из текущей ячейки\n",
"\3^E \1-\2 войти в режим редактирования       \3^J, ^M\2 или клавиша \3'return' \1-\2 запомнить\n",
"\n",
"                            \3\16Редактирование\17\2\n",
"\3^D\2 или стрелка \3'вниз' \1-\2 удалить символ  \3^L\2 или стрелка \3'влево' \1-\2 влево\n",
"\3^U\2 или стрелка \3'вверх' \1-\2 вернуть символ \3^R\2 или стрелка \3'вправо' \1-\2 вправо\n",
"\3^H\2 или клавиша \3'delete' \1-\2 забой         \3^I\2 или клавиша \3'tab' \1-\2 в конец строки\n",
"\3^J, ^M\2 или клавиша \3'return' \1-\2 выход из режима редактирования\n",
0,
};

char *rhelp2 [] = {
"                               \3\16Операции\17\2\n",
"\3+ - * / \1-\2 математические операции       \3^ \1-\2 возведение в степень\n",
"\3~ \1-\2 квадратный корень                   \3' \1-\2 квадрат, 2' равно 4\n",
"\3++ \1-\2 сумма ячеек в области              \3-- \1-\2 среднее ячеек в области\n",
"\3** \1-\2 произведение ячеек в области       \3// \1-\2 количество ячеек в области\n",
"\3< = > <= >= \1-\2 операции отношения        \3& | ! \1-\2 логические 'и', 'или', 'не'\n",
"\3?: \1-\2 операция выбора                    \3$ \1-\2 'жесткая' ссылка на ячейку\n",
"\n",
"                               \3\16Константы\17\2\n",
"\3pi \1-\2 3.14...                            \3e \1-\2 2.71...\n",
"\n",
"                                \3\16Функции\17\2\n",
"\3exp () \1-\2 экспонента\n",
"\3int (,) \1-\2 отбрасывание дробной части    \3round (,) \1-\2 округление\n",
"\3ln () \1-\2 натуральный логарифм            \3log () \1-\2 десятичный логарифм\n",
"\3pow (,) \1-\2 возведение в степень          \3sin (), cos(), tan() \1-\2 тригонометрия\n",
"\3floor () \1-\2 макс. целое, не превосх.     \3ceil () \1-\2 мин. целое, не меньшее\n",
"\3hypot (,) \1-\2 гипотенуза                  \3fabs () \1-\2 модуль\n",
"\3asin () \1-\2 arcsin в диап. -пи/2..пи/2    \3acos () \1-\2 arccos в диап. 0..пи\n",
"\3atan () \1-\2 arctan в диап. -пи/2..пи/2    \3gamma () \1-\2 логарифм гамма-функции\n",
"\3dtr () \1-\2 из градусов в радианы          \3rtd () \1-\2 из радианов в градусы\n",
"\3max (,) \1-\2 максимум                      \3min (,) \1-\2 минимум\n",
0,
};
# endif /* MAKEHELPFILE || !NOHELP && !HELPFILE */
# endif /* RUS */

# ifdef MESGFILE
char *lmesg (n)
{
	if (! nmesg)
		mesginit ();
	if (! nmesg) {
		outerr ("message init error\n");
		exit (1);
	}
	if (n<0 || n>nmesg)
		return ("");
	if (lindex[n].poolindex >= 0) {
		/* line is in cache */
		map[lindex[n].poolindex].time = ++timecount;
		return (pool[lindex[n].poolindex]);
	}
	/* get free pool index */
	lindex[n].poolindex = freeline ();
	map[lindex[n].poolindex].time = ++timecount;
	map[lindex[n].poolindex].msgindex = n;
	map[lindex[n].poolindex].mflags = MAPBUSY;
	/* set file pointer on the beginning of line */
	fseek (lfile, (long) lindex[n].seek, 0);
	/* read line from file */
	fgetline (pool[lindex[n].poolindex], lfile);
	return (pool[lindex[n].poolindex]);
}
# else /* MESGFILE */
char *lmesg [] = {
/* 0  */        "Termcap entry too long\n",
/* 1  */        "Bad termcap entry\n",
/* 2  */        "Infinite tc= loop\n",
/* 3  */        "The table can't be any bigger",
/* 4  */        "The table can't be any wider",
/* 5  */        "The table can't be any longer",
/* 6  */        "First hidden range is %s-%s.",
/* 7  */        "Enter first column: ",
/* 8  */        "Enter last column: ",
/* 9  */        "First hidden range is %d-%d.",
/* 10 */        "Enter first row: ",
/* 11 */        "Enter last row: ",
/* 12 */        "You can't hide the last row",
/* 13 */        "You can't hide the last col",
/* 14 */        "Copy area %s%d-%s%d to: ",
/* 15 */        "Fill area %s%d-%s%d by: ",
/* 16 */        "Fill area %s%d-%s%d by %s, step: ",
/* 17 */        "Area %s%d-%s%d filled by %s, step %s.",
/* 18 */        "Enter name of cell: ",
/* 19 */        "Save table in file: ",
/* 20 */        "Load file: ",
/* 21 */        "Merge current table and file: ",
/* 22 */        "Print table to file: ",
/* 23 */        "Write table in tbl format to file: ",
/* 24 */        "Area %s%d-%s%d valueized.",
/* 25 */        "Area %s%d-%s%d deleted.",
/* 26 */        "Current precision is %d, legal range is 0-72.",
/* 27 */        "Enter precision: ",
/* 28 */        "Current width is %d, legal range is 0-72.",
/* 29 */        "Enter width for current column: ",
/* 30 */        "Suffix is \"%s\" by default.",
/* 31 */        "Can't create %s",
/* 32 */        "File '%s' written.",
/* 33 */        "Can't read %s",
/* 34 */        "use: sc [-] [-r#] [-c#] [-a] [-b] [-l] [-m] [file]\n",
/* 35 */        "cannot initialize terminal\n",
/* 36 */        "Out of memory!",
/* 37 */        "out of memory\n",
/* 38 */        "^Phelp ^Cancel ^Xclear ^Graph n^Ame ^Formula ^Value ^Text ^Edit",
/* 39 */        "^Phelp ^Cancel ^Xclear ^Graph ",
/* 40 */        "Answer y(es) or n(o).",
/* 41 */        "\3Exit from spreadsheet calculator? \2",
/* 42 */        "\3Save table in workfile? \2",
/* 43 */        "\3Save file \"%s\" ? \2",
/* 44 */        "size %s%d",
/* 45 */        ", last %s%d",
/* 46 */        ", marked %s%d",
/* 47 */        ", autocalc",
/* 48 */        ", memory used %ld",
/* 49 */        ", free %ld",
/* 50 */        "\2formula\1 %s \3%s\2",
/* 51 */        "\2value\1 %s \3%s\2",
/* 52 */        "\2text\1 %s \3\"%s\"\2",
/* 53 */        "\2empty",
/* 54 */        "\3L\2eftalign \3R\2ightalign \3C\2enter \3D\2efault \3P\2recision",
/* 55 */        "\1^Left ^Right ^Delete ^Undelete ^Xclear ^Cancel Backspace Tab Return \3Edit\2",
/* 56 */        "Current height is %d, legal range is %d-%d.",
/* 57 */        "Enter new table height: ",
/* 58 */        "Current width is %d, legal range is %d-%d.",
/* 59 */        "Enter new table width: ",
/* 60 */        "Answer д(а) or н(ет).",
/* 61 */        "No help available",
/* 62 */        "Cannot open %s",
/* 63 */        "\1\16Press any key...\2\17",
/* 64 */        "\3F\2ile/ \3C\2ell/ \3R\2ow/ c\3O\2lumn/ \3A\2rea/ \3J\2ump/ \3G\2raph/ \3U\2tility/ \3Q\2uit",
/* 65 */        "\3M\2ark \3C\2opy \3D\2elete \3U\2ndelete \3F\2ormat/",
/* 66 */        "\3W\2idth \3H\2eight \3M\2inimize \3R\2ecalc \3A\2utocalc \3B\2order \3L\2anguage \3P\2recision",
/* 67 */        "\3C\2ell \3U\2p \3D\2own \3L\2eft \3R\2ight \3E\2nd",
/* 68 */        "\3M\2ark \3C\2opy f\3I\2ll \3D\2elete \3U\2ndelete \3V\2alueize \3F\2ormat/",
/* 69 */        "\3S\2ave \3L\2oad \3M\2erge \3P\2rint pl\3O\2t \3T\2bl",
/* 70 */        "\3I\2nsert \3C\2opy \3H\2ide \3S\2how \3D\2elete \3U\2ndelete \3V\2alueize \3F\2ormat/",
/* 71 */        "\3I\2nsert \3C\2opy \3H\2ide \3S\2how \3D\2elete \3U\2ndelete \3W\2idth \3V\2alueize \3F\2ormat/",
/* 72 */        "Cell/",
/* 73 */        "Utility/",
/* 74 */        "Jump/",
/* 75 */        "File/",
/* 76 */        "Area/",
/* 77 */        "Row/",
/* 78 */        "cOlumn/",
/* 79 */        "Quit",
/* 80 */        "X-Y",
/* 81 */        "Delete",
/* 82 */        "Undelete",
/* 83 */        "Mark",
/* 84 */        "Copy",
/* 85 */        "Language",
/* 86 */        "Recalc",
/* 87 */        "Width",
/* 88 */        "Height",
/* 89 */        "Minimize",
/* 90 */        "End",
/* 91 */        "Cell",
/* 92 */        "Down",
/* 93 */        "Up",
/* 94 */        "Left",
/* 95 */        "Right",
/* 96 */        "Save",
/* 97 */        "Load",
/* 98 */        "Merge",
/* 99 */        "Print",
/* 100 */       "Tbl",
/* 101 */       "Valueize",
/* 102 */       "Hide",
/* 103 */       "Show",
/* 104 */       "fIll",
/* 105 */       "Insert",
/* 106 */       "Precision",
/* 107 */       "\3S\2hape/ \3M\2ark axis\3A\2 axis\3B\2 axis\3C\2 axis\3D\2 \3R\2eset \3H\2ardcopy",
/* 108 */       "Graph/",
/* 109 */       "Autocalc",
/* 110 */       "axisA",
/* 111 */       "axisB",
/* 112 */       "axisC",
/* 113 */       "Graph/Shape/",
/* 114 */       "Reset",
/* 115 */       "Shape is '%s'",
/* 116 */       ", axes %s%d-%s%d",
/* 117 */       "axisD",
/* 118 */       "plOt",
/* 119 */       "Nothing to plot.",
/* 120 */       "Can't fork.",
/* 121 */       "Error in %s.",
/* 122 */       "Cannot create pipe.",
/* 123 */       "\3P\2ie \3G\2raph \3B\2ar \3S\2tacked-bar \3X\2-Y",
/* 124 */       "pie",
/* 125 */       "graph",
/* 126 */       "bar",
/* 127 */       "stacked bar",
/* 128 */       "Pie",
/* 129 */       "Graph",
/* 130 */       "Bar",
/* 131 */       "Stacked-bar",
/* 132 */       "Graph shape is '%s'.",
/* 133 */       "Plot graph to file: ",
/* 134 */       "Just a moment... ",
/* 135 */       "Border",
/* 136 */       "Hardcopy",
/* 137 */       "Cell/Format/",
/* 138 */       "Area/Format/",
/* 139 */       "Row/Format/",
/* 140 */       "cOlumn/Format/",
/* 141 */       "Leftalign",
/* 142 */       "Rightalign",
/* 143 */       "Center",
/* 144 */       "Default",
/* 145 */	"\3M\2ark \3L\2eftalign \3R\2ightalign \3C\2enter \3D\2efault \3P\2recision",
/* 146 */       "SC  Version 3.1   Copyright (c) 1989 DEMOS   Moscow, Novokuznetskaya, 33",
};
# endif /* MESGFILE */

# if defined (MAKEHELPFILE) || !defined (NOHELP) && !defined (HELPFILE)
char *lhelp1 [] = {
"                                \3\16Moving\17\2\n",
"\3^D\2 or \3'down'\2 arrow \1-\2 down               \3^L\2 or \3'left'\2 arrow \1-\2 left\n",
"\3^U\2 or \3'up'\2 arrow \1-\2 up                   \3^R\2 or \3'right'\2 arrow \1-\2 right\n",
"\3^Y\2 or \3'Home'\2 key \1-\2 page left            \3^K\2 or \3'PgUp'\2 key \1-\2 page up\n",
"\3^Z\2 or \3'End'\2 key \1-\2 page right            \3^B\2 or \3'PgDn'\2 key \1-\2 page down\n",
"\3J\2ump/\3U\2p \1-\2 to first row                  \3J\2ump/\3L\2eft \1-\2 to first column\n",
"\3J\2ump/\3D\2own \1-\2 to last cell in column      \3J\2ump/\3R\2ight \1-\2 to last cell in row\n",
"\3J\2ump/\3C\2ell \1-\2 to cell by name             \3J\2ump/\3E\2nd \1-\2 to the end of table\n",
"\n",
"                               \3\16Utilities\17\2\n",
"\3^X\2 \1-\2 redraw screen                      \3^C\2 \1-\2 cancel current command\n",
"\n",
"                            \3\16Entering values\17\2\n",
"\3^A\2 \1-\2 take name of current cell          \3^V\2 \1-\2 take value of current cell\n",
"\3^F\2 \1-\2 take formula from the current cell \3^T\2 \1-\2 take text from the current cell\n",
"\3^E\2 \1-\2 enter editor                       \3^J\2, \3^M\2 or \3'return'\2 key \1-\2 accept\n",
"\n",
"                                \3\16Editing\17\2\n",
"\3^D\2 or \3'down'\2 arrow \1-\2 delete char        \3^L\2 or \3'left'\2 arrow \1-\2 left one char\n",
"\3^U\2 or \3'up'\2 arrow \1-\2 undelete char        \3^R\2 or \3'right'\2 arrow \1-\2 right one char\n",
"\3^H\2 or \3'delete'\2 key \1-\2 backspase          \3^I\2 or \3'tab'\2 key \1-\2 go to end of string\n",
"\3^J\2, \3^M\2 or \3'return'\2 key \1-\2 exit from editor\n",
0,
};

char *lhelp2 [] = {
"                              \3\16Operations\17\2\n",
"\3+ - * / \1-\2 math operations               \3^ \1-\2 raise to power\n",
"\3~ \1-\2 square root                         \3' \1-\2 square (postfix operation)\n",
"\3++ \1-\2 sum cells in region                \3-- \1-\2 average of cells in region\n",
"\3** \1-\2 multiply cells in region           \3// \1-\2 number of cells in region\n",
"\3< = > <= >= \1-\2 relational ops            \3& | ! \1-\2 'and', 'or', 'not' ops\n",
"\3?: \1-\2 conditional expression             \3$ \1-\2 'fix' reference to cell\n",
"\n",
"                               \3\16Constants\17\2\n",
"\3pi \1-\2 3.14...                            \3e \1-\2 2.71...\n",
"\n",
"                               \3\16Functions\17\2\n",
"\3exp () \1-\2 exponential function\n",
"\3int (,) \1-\2 truncate fractional part      \3round (,) \1-\2 round to specified digit\n",
"\3ln () \1-\2 natural logarithm               \3log () \1-\2 decimal logarithm\n",
"\3pow (,) \1-\2 raise to power                \3sin (), cos(), tan() \1-\2 trig functions\n",
"\3floor () \1-\2 largest int not greater than \3ceil () \1-\2 smallest int not less than\n",
"\3hypot (,) \1-\2 hypotenuza                  \3fabs () \1-\2 absolute value\n",
"\3asin () \1-\2 arc sin in range -pi/2..pi/2  \3acos () \1-\2 arc cosine in range 0..pi\n",
"\3atan () \1-\2 arc tan in range -pi/2..pi/2  \3gamma () \1-\2 log of gamma function\n",
"\3dtr () \1-\2 convert degrees to radians     \3rtd () \1-\2 convert radians to degrees\n",
"\3max (,) \1-\2 maximum of two expressions    \3min (,) \1-\2 minimum of two expressions\n",
0,
};
# endif /* MAKEHELPFILE || !NOHELP && !HELPFILE */

# ifdef MAKESUM
main ()
{
	register char *p;
	register sum;

	for (sum=0, p=lmesg[146]; *p; ++p)
		sum += *p & 0377;
	printf ("lat = %d\n", sum);
# ifdef RUS
	for (sum=0, p=rmesg[146]; *p; ++p)
		sum += *p & 0377;
	printf ("rus = %d\n", sum);
# endif
	exit (0);
}
# endif /* MAKESUM */

# ifdef MAKERMESG
# include <stdio.h>
# define MESGNUM (sizeof (rmesg) / sizeof (char *))
main ()
{
	register char *p, **q;
	register len, max;

	max = 0;
	for (q=rmesg; q<rmesg+MESGNUM; ++q) {
		if (*q) {
			for (len=0, p= *q; *p; ++p, ++len)
				putchar (*p);
			if (len > max)
				max = len;
		}
		putchar (0);
	}
	fprintf (stderr, "maximum line length is %d bytes\n", max);
	exit (0);
}
# endif

# ifdef MAKELMESG
# include <stdio.h>
# define MESGNUM (sizeof (lmesg) / sizeof (char *))
main ()
{
	register char *p, **q;
	register len, max;

	max = 0;
	for (q=lmesg; q<lmesg+MESGNUM; ++q) {
		if (*q) {
			for (len=0, p= *q; *p; ++p, ++len)
				putchar (*p);
			if (len > max)
				max = len;
		}
		putchar (0);
	}
	fprintf (stderr, "maximum line length is %d bytes\n", max);
	exit (0);
}
# endif
