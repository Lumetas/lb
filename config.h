/* modifier 0 means no modifier */
static int surfuseragent    = 1;  /* Append Surf version to default WebKit user agent */
static char *fulluseragent  = "Mozilla/5.0 (Macintosh; Intel Mac OS X 15_7_2) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/26.0 Safari/605.1.15"; /* Or override the whole user agent string */
static char *scriptfile     = "~/lb/js/build.js";
static char *styledir       = "~/lb/css/";
static char *certdir        = "~/lb/certificates/";
static char *cachedir       = "~/lb/cache/";
static char *cookiefile     = "~/lb/cookies.txt";
static char *bookmarkdir    = "~/lb/bookmarks/"; 
static char *historyfile    = "~/lb/history.txt";
#define DEFAULT_URL "~/lb/html/index.html";

/* Чёрно-белая тема для dmenu и утилит */
#define DWMENU_BW "-nf white -nb black -sf black -sb white"
#define NOTIFY_BW "-t 5000 -h int:value:42"

/* Webkit default features */
/* Highest priority value will be used.
 * Default parameters are priority 0
 * Per-uri parameters are priority 1
 * Command parameters are priority 2
 */
static Parameter defconfig[ParameterLast] = {
	/* parameter                    Arg value       priority */
	[AccessMicrophone]    =       { { .i = 1 },     },
	[AccessWebcam]        =       { { .i = 1 },     },
	[Certificate]         =       { { .i = 1 },     },
	[CaretBrowsing]       =       { { .i = 0 },     },
	[CookiePolicies]      =       { { .v = "@Aa" }, },
	[DarkMode]            =       { { .i = 1 },     },
	[DefaultCharset]      =       { { .v = "UTF-8" }, },
	[DiskCache]           =       { { .i = 1 },     },
	[DNSPrefetch]         =       { { .i = 0 },     },
	[Ephemeral]           =       { { .i = 0 },     },
	[FileURLsCrossAccess] =       { { .i = 0 },     },
	[FontSize]            =       { { .i = 12 },    },
	[Geolocation]         =       { { .i = 1 },     },
	[HideBackground]      =       { { .i = 0 },     },
	[Inspector]           =       { { .i = 1 },     },
	[JavaScript]          =       { { .i = 1 },     },
	[KioskMode]           =       { { .i = 0 },     },
	[LoadImages]          =       { { .i = 1 },     },
	[MediaManualPlay]     =       { { .i = 1 },     },
	[PDFJSviewer]         =       { { .i = 1 },     },
	[PreferredLanguages]  =       { { .v = (char *[]){ NULL } }, },
	[RunInFullscreen]     =       { { .i = 0 },     },
	[ScrollBars]          =       { { .i = 1 },     },
	[ShowIndicators]      =       { { .i = 1 },     },
	[SiteQuirks]          =       { { .i = 1 },     },
	[SmoothScrolling]     =       { { .i = 1 },     },
	[SpellChecking]       =       { { .i = 0 },     },
	[SpellLanguages]      =       { { .v = ((char *[]){ "en_US", NULL }) }, },
	[StrictTLS]           =       { { .i = 1 },     },
	[Style]               =       { { .i = 1 },     },
	[WebGL]               =       { { .i = 1 },     },
	[ZoomLevel]           =       { { .f = 1.0 },   },
};

static UriParameters uriparams[] = {
	{ "(://|\\.)suckless\\.org(/|$)", {
	  [JavaScript] = { { .i = 0 }, 1 },
	}, },
};

/* default window size: width, height */
static int winsize[] = { 800, 600 };

static WebKitFindOptions findopts = WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE |
                                    WEBKIT_FIND_OPTIONS_WRAP_AROUND;

#define PROMPT_GO   "Go:"
#define PROMPT_FIND "Find:"

/* SETPROP(readprop, setprop, prompt)*/
#define SETPROP(r, s, p) { \
        .v = (const char *[]){ "/bin/sh", "-c", \
             "prop=\"$(printf '%b' \"$(xprop -id $1 "r" " \
             "| sed -e 's/^"r"(UTF8_STRING) = \"\\(.*\\)\"/\\1/' " \
             "      -e 's/\\\\\\(.\\)/\\1/g')\" " \
             "| dmenu " DWMENU_BW " -p '"p"' -w $1)\" " \
             "&& xprop -id $1 -f "s" 8u -set "s" \"$prop\"", \
             "surf-setprop", winid, NULL \
        } \
}

/* DOWNLOAD(URI, referer) */
#define DOWNLOAD(u, r) { \
        .v = (const char *[]){ "st", "-e", "/bin/sh", "-c",\
             "curl -g -L -J -O -A \"$1\" -b \"$2\" -c \"$2\"" \
             " -e \"$3\" \"$4\"; read", \
             "surf-download", useragent, cookiefile, r, u, NULL \
        } \
}

#define NEW_TAB { \
    .v = (const char *[]){ "/bin/sh", "-c", \
         "surf -e $(cat ~/lb/xid) ~/lb/html/index.html", \
         NULL \
    } \
}

/* PLUMB(URI) */
/* This called when some URI which does not begin with "about:",
 * "http://" or "https://" should be opened.
 */
#define PLUMB(u) {\
        .v = (const char *[]){ "/bin/sh", "-c", \
             "xdg-open \"$0\"", u, NULL \
        } \
}

/* VIDEOPLAY(URI) */
#define VIDEOPLAY(u) {\
        .v = (const char *[]){ "/bin/sh", "-c", \
             "mpv --really-quiet \"$0\"", u, NULL \
        } \
}

/* BOOKMARK_ADD - добавить текущую страницу в закладки */
#define BOOKMARK_ADD { \
        .v = (const char *[]){ "/bin/sh", "-c", \
             "mkdir -p ~/lb/bookmarks && " \
             "name=\"$(echo '' | dmenu " DWMENU_BW " -p 'Bookmark name:' -w $1)\" && " \
             "if [ -n \"$name\" ]; then " \
             "  url=\"$(xprop -id $1 _SURF_URI | sed 's/^_SURF_URI(UTF8_STRING) = \"\\(.*\\)\"/\\1/')\" && " \
             "  echo \"$url\" > ~/lb/bookmarks/\"$name\" && " \
             "  notify-send " NOTIFY_BW " \"Bookmark added\" \"$name: $url\"; " \
             "fi", \
             "surf-bookmark-add", winid, NULL \
        } \
}

/* BOOKMARK_OPEN - выбрать и открыть закладку */
#define BOOKMARK_OPEN { \
        .v = (const char *[]){ "/bin/sh", "-c", \
             "mkdir -p ~/lb/bookmarks && " \
             "file=\"$(ls ~/lb/bookmarks/ | dmenu " DWMENU_BW " -p 'Open bookmark:' -w $1 -l 10)\" && " \
             "if [ -n \"$file\" ]; then " \
             "  url=\"$(cat ~/lb/bookmarks/\"$file\")\" && " \
             "  xprop -id $1 -f _SURF_GO 8u -set _SURF_GO \"$url\"; " \
             "fi", \
             "surf-bookmark-open", winid, NULL \
        } \
}

/* BOOKMARK_DELETE - выбрать и удалить закладку */
#define BOOKMARK_DELETE { \
        .v = (const char *[]){ "/bin/sh", "-c", \
             "mkdir -p ~/lb/bookmarks && " \
             "file=\"$(ls ~/lb/bookmarks/ | dmenu " DWMENU_BW " -p 'Delete bookmark:' -w $1 -l 10)\" && " \
             "if [ -n \"$file\" ]; then " \
             "  rm ~/lb/bookmarks/\"$file\" && " \
             "  notify-send " NOTIFY_BW " \"Bookmark deleted\" \"$file\"; " \
             "fi", \
             "surf-bookmark-delete", winid, NULL \
        } \
}

/* Упрощённый SMART_GO с детектированием URL и историей */
#define SMART_GO { \
        .v = (const char *[]){ "/bin/sh", "-c", \
             "mkdir -p ~/lb && " \
             "touch ~/lb/history.txt && " \
             "input=\"$(echo '~/lb/html/index.html' | dmenu " DWMENU_BW " -p 'Go or search:' -w $1)\"; " \
             "if [ -n \"$input\" ]; then " \
             "  url=\"$input\"; " \
             "  if echo \"$url\" | grep -q '^[a-zA-Z][a-zA-Z0-9+.-]*://'; then " \
             "    final_url=\"$url\"; " \
             "  elif echo \"$url\" | grep -qi '^localhost'; then " \
             "    final_url=\"http://$url\"; " \
             "  elif echo \"$url\" | grep -qiE '^[a-zA-Z0-9]([a-zA-Z0-9-]*[a-zA-Z0-9])?\\.(com|org|net|edu|gov|mil|io|co|uk|de|fr|ru|ua|jp|cn|br|au|in|info|biz|mobi|name|tv|me|us|ca|es|it|nl|pl|se|no|fi|dk|ch|at|be|cz|gr|hu|pt|ro|sk|tr|eu|asia|app|dev|site|online|xyz|tech|store|shop|blog|club|live|studio|news|wiki|space|work|digital|cloud|media|world|today|center|plus|expert|guru|agency|network|solutions|services|systems|company|group)(/|$|:[0-9])'; then " \
             "    final_url=\"http://$url\"; " \
             "  else " \
             "    final_url=\"https://www.google.com/search?q=$(echo \"$url\" | sed 's/ /+/g')\"; " \
             "  fi; " \
             "  echo \"$final_url\" >> ~/lb/history.txt; " \
             "  tail -1000 ~/lb/history.txt > ~/lb/history.tmp && " \
             "  mv ~/lb/history.tmp ~/lb/history.txt; " \
             "  xprop -id $1 -f _SURF_GO 8u -set _SURF_GO \"$final_url\"; " \
             "fi", \
             "surf-smart-go", winid, NULL \
        } \
}

/* Просмотр истории */
#define SHOW_HISTORY { \
        .v = (const char *[]){ "/bin/sh", "-c", \
             "mkdir -p ~/lb && " \
             "touch ~/lb/history.txt && " \
             "url=\"$(tac ~/lb/history.txt | dmenu " DWMENU_BW " -p 'History:' -w $1 -l 20)\"; " \
             "if [ -n \"$url\" ]; then " \
             "  xprop -id $1 -f _SURF_GO 8u -set _SURF_GO \"$url\"; " \
             "fi", \
             "surf-show-history", winid, NULL \
        } \
}

/* Очистить историю */
#define CLEAR_HISTORY { \
        .v = (const char *[]){ "/bin/sh", "-c", \
             "echo '' > ~/lb/history.txt && " \
             "notify-send " NOTIFY_BW " \"History cleared\" \"All history entries have been removed\"", \
             "surf-clear-history", NULL \
        } \
}

/* styles */
/*
 * The iteration will stop at the first match, beginning at the beginning of
 * the list.
 */
static SiteSpecific styles[] = {
	/* regexp               file in $styledir */
	{ ".*",                 "main.css" },
};

/* certificates */
/*
 * Provide custom certificate for urls
 */
static SiteSpecific certs[] = {
	/* regexp               file in $certdir */
	{ "://suckless\\.org/", "suckless.org.crt" },
};

#define MODKEY GDK_CONTROL_MASK

/* hotkeys */
/*
 * If you use anything else but MODKEY and GDK_SHIFT_MASK, don't forget to
 * edit the CLEANMASK() macro.
 */
static Key keys[] = {
	/* modifier              keyval          function    arg */
	{ MODKEY,                GDK_KEY_g,      spawn,      SMART_GO },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_g,      spawn,      SHOW_HISTORY },
	// { MODKEY|GDK_SHIFT_MASK, GDK_KEY_h,      spawn,      CLEAR_HISTORY },
	
	{ MODKEY,                GDK_KEY_f,      spawn,      SETPROP("_SURF_FIND", "_SURF_FIND", PROMPT_FIND) },
	{ MODKEY,                GDK_KEY_slash,  spawn,      SETPROP("_SURF_FIND", "_SURF_FIND", PROMPT_FIND) },

	{ 0,                     GDK_KEY_Escape, stop,       { 0 } },
	{ MODKEY,                GDK_KEY_x,      stop,       { 0 } },

	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_r,      reload,     { .i = 1 } },
	{ MODKEY,                GDK_KEY_r,      reload,     { .i = 0 } },

	{ MODKEY,                GDK_KEY_l,      navigate,   { .i = +1 } },
	{ MODKEY,                GDK_KEY_h,      navigate,   { .i = -1 } },

	/* vertical and horizontal scrolling, in viewport percentage */
	{ MODKEY,                GDK_KEY_j,      scrollv,    { .i = +10 } },
	{ MODKEY,                GDK_KEY_k,      scrollv,    { .i = -10 } },
	{ MODKEY,                GDK_KEY_space,  scrollv,    { .i = +50 } },
	{ MODKEY,                GDK_KEY_b,      scrollv,    { .i = -50 } },
	{ MODKEY,                GDK_KEY_i,      scrollh,    { .i = +10 } },
	{ MODKEY,                GDK_KEY_u,      scrollh,    { .i = -10 } },

	/* Закладки */
	{ MODKEY,                GDK_KEY_b,      spawn,      BOOKMARK_ADD }, /* Ctrl+B - добавить закладку */
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_b,      spawn,      BOOKMARK_OPEN }, /* Ctrl+Shift+B - открыть закладку */
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_d,      spawn,      BOOKMARK_DELETE }, /* Ctrl+Shift+D - удалить закладку */

	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_j,      zoom,       { .i = -1 } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_k,      zoom,       { .i = +1 } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_q,      zoom,       { .i = 0  } },
	{ MODKEY,                GDK_KEY_minus,  zoom,       { .i = -1 } },
	{ MODKEY,                GDK_KEY_plus,   zoom,       { .i = +1 } },

	{ MODKEY,                GDK_KEY_p,      clipboard,  { .i = 1 } },
	{ MODKEY,                GDK_KEY_c,      clipboard,  { .i = 0 } },

	{ MODKEY,                GDK_KEY_n,      find,       { .i = +1 } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_n,      find,       { .i = -1 } },

	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_p,      print,      { 0 } },
	{ MODKEY,                GDK_KEY_t,      showcert,   { 0 } },

	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_a,      togglecookiepolicy, { 0 } },
	{ 0,                     GDK_KEY_F11,    togglefullscreen, { 0 } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_o,      toggleinspector, { 0 } },

	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_c,      toggle,     { .i = CaretBrowsing } },
	// { MODKEY|GDK_SHIFT_MASK, GDK_KEY_g,      toggle,     { .i = Geolocation } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_s,      toggle,     { .i = JavaScript } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_i,      toggle,     { .i = LoadImages } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_v,      toggle,     { .i = ScrollBars } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_t,      toggle,     { .i = StrictTLS } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_m,      toggle,     { .i = Style } },
	{ MODKEY|GDK_SHIFT_MASK, GDK_KEY_d,      toggle,     { .i = DarkMode } },
};

/* button definitions */
/* target can be OnDoc, OnLink, OnImg, OnMedia, OnEdit, OnBar, OnSel, OnAny */
static Button buttons[] = {
	/* target       event mask      button  function        argument        stop event */
	{ OnLink,       0,              2,      clicknewwindow, { .i = 0 },     1 },
	{ OnLink,       MODKEY,         2,      clicknewwindow, { .i = 1 },     1 },
	{ OnLink,       MODKEY,         1,      clicknewwindow, { .i = 1 },     1 },
	{ OnAny,        0,              8,      clicknavigate,  { .i = -1 },    1 },
	{ OnAny,        0,              9,      clicknavigate,  { .i = +1 },    1 },
	{ OnMedia,      MODKEY,         1,      clickexternplayer, { 0 },       1 },
};
