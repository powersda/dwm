/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx  = 2;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static unsigned int systrayonleft = 0;   	/* 0: systray in the right corner, >0: systray on left of status text */
static unsigned int systrayspacing = 2;   /* systray spacing */
static int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static int showsystray        = 1;     /* 0 means no systray */
static unsigned int gappih    = 10;       /* horiz inner gap between windows */
static unsigned int gappiv    = 10;       /* vert inner gap between windows */
static unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static int user_bh            = 0;        /* 0 means dwm will calculate, else user selected */
static char font[]            = { "Ubuntu Nerd Font:size=12:autohint=true:antialias:true" };
static char dmenufont[]       = "Ubuntu Nerd Font:size=12:autohint=true:antialias:true";
static const char *fonts[]    = { font };
static char normbgcolor[]     = "#ff0000";
static char normbordercolor[] = "#444444";
static char normfgcolor[]     = "#00ff00";
static char selfgcolor[]      = "#ff0000";
static char selbordercolor[]  = "#005577";
static char selbgcolor[]      = "#00ff00";
static char col1[]	      = "#ffffff";
static char col2[]	      = "#ffffff";
static char col3[]	      = "#ffffff";
static char col4[]	      = "#ffffff";
static char col5[]	      = "#ffffff";
static char col6[]            = "#ffffff";
static int  pushSysTrayRight  = 25;
static int  pushSBRight       = 15;
static int swallowfloating    = 0;
static int attachBottom	      = 1;

enum { SchemeNorm, SchemeCol1, SchemeCol2, SchemeCol3, SchemeCol4,
       SchemeCol5, SchemeCol6, SchemeSel, SchemeTitle }; /* color schemes */

static  char *colors[][3]      = {
	/*               fg           bg           border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
	[SchemeCol1] = { col1,        normbgcolor, normbordercolor },
	[SchemeCol2] = { col6,        normbgcolor, normbordercolor },
	[SchemeCol3] = { col3,        normbgcolor, normbordercolor },
	[SchemeCol4] = { col4,        normbgcolor, normbordercolor },
	[SchemeCol5] = { col5,        normbgcolor, normbordercolor },
	[SchemeCol6] = { col6,        normbgcolor, normbordercolor },
	[SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
        [SchemeTitle]= { normfgcolor, normbgcolor, selbordercolor  },
};


typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
/*const char *spcmd2[] = {"st", "-n", "spfm", "-g", "144x41", "-e", "ranger", NULL };
const char *spcmd3[] = {"keepassxc", NULL };*/
static Sp scratchpads[] = {
	/* name          cmd  */
	{"spterm",      spcmd1},
/*	{"spranger",    spcmd2},
	{"keepassxc",   spcmd3},*/
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     iscentered  isfloating isterminal  noswallow  monitor */
	{ "Gimp",     NULL,	  NULL,       0,            0,          1,	   0,	       0,	     -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,          0,         0,	       0,            -1 },
	{ "St",	      NULL,	  NULL,	      0,            0,          0,         1,          0,            -1 },
	{ NULL,	     "spterm",	  NULL,       SPTAG(0),     0,          1,	   1,	       0,	     -1 },
	{ NULL,      "spfm",      NULL,	      SPTAG(1),     0,          1,	   0, 	       0,	     -1 },
	{ NULL,      "keepassxc", NULL,	      SPTAG(2),     0,          0,         0,          0,            -1 },
};

/* layout(s) */
static float mfact     = 0.50; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 0;    /* 1 means respect size hints in tiled resizals */


struct Pertag {
	unsigned int curtag, prevtag; /* current and previous tag */
	int nmasters[LENGTH(tags) + 1]; /* number of windows in master area */
	float mfacts[LENGTH(tags) + 1]; /* mfacts per tag */
	unsigned int sellts[LENGTH(tags) + 1]; /* selected layouts */
	const Layout *ltidxs[LENGTH(tags) + 1][2]; /* matrix of tags and layouts indexes  */
	int showbars[LENGTH(tags) + 1]; /* display bar for the current tag */
	int enablegaps[NUMTAGS + 1];
	unsigned int gaps[NUMTAGS + 1];
};

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, "-nhb", normbgcolor, "-nhf", col1, "-shb", selbgcolor, "-shf", col1,  NULL };
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "font",               STRING,  &font },
		{ "dmenufont",          STRING,  &dmenufont },
		{ "background",         STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "foreground",         STRING,  &normfgcolor },
		{ "foreground",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "background",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          	INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	FLOAT,   &mfact },
                { "gappih",             INTEGER, &gappih},
                { "gappiv",             INTEGER, &gappiv},
                { "gappoh",             INTEGER, &gappoh},
                { "gappov",             INTEGER, &gappov},
                { "user_bh",            INTEGER, &user_bh},
                { "systraypinning",     INTEGER, &systraypinning},
                { "systrayonleft",      INTEGER, &systrayonleft},
                { "systrayspacing",     INTEGER, &systrayspacing},
                { "systraypinningfailfirst", INTEGER, &systraypinningfailfirst},
                { "showsystray",        INTEGER, &showsystray},
                { "col1",	        STRING,  &col1},
                { "col2",        	STRING,  &col2},
                { "col3",      		STRING,  &col3},
                { "col4",	        STRING,  &col4},
                { "col5",      		STRING,  &col5},
                { "col6",	        STRING,  &col6},
		{ "pushSysTrayRight",	INTEGER, &pushSysTrayRight},
		{ "pushSBRight",	INTEGER, &pushSBRight},
		{ "swallowfloating",	INTEGER, &swallowfloating},
		{ "attachBottom",	INTEGER, &attachBottom}
                
};


static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY|Mod1Mask,              XK_h,      incrgaps,       {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_h,      incrgaps,       {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_j,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_j,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_k,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_k,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_u,      incrihgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_u,      incrihgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_i,      incrivgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_o,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,      incrohgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_p,      incrovgaps,     {.i = +1 } },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_p,      incrovgaps,     {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_l,      togglegaps,     {0} },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_l,      defaultgaps,    {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
/*	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },*/
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ControlMask,		XK_comma,  cyclelayout,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, cyclelayout,    {.i = +1 } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~SPTAGMASK } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~SPTAGMASK } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,            		XK_grave,  togglescratch,  {.ui = 0 } },
/*	{ MODKEY,            		XK_u,	   togglescratch,  {.ui = 1 } },
	{ MODKEY,            		XK_x,	   togglescratch,  {.ui = 2 } },i*/
	{ MODKEY|ShiftMask,             XK_j,      pushdown,       {0} },
	{ MODKEY|ShiftMask,             XK_k,      pushup,         {0} },
	{ MODKEY,                       XK_h,      focusmaster,    {0} },
	{ MODKEY,                       XK_f,      togglefullscr,  {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefakefullscreen, {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
        { MODKEY|ShiftMask|ControlMask, XK_q,      quit,           {0} },
	{ MODKEY|ShiftMask,             XK_r,      quit,           {1} }, 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button1,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
