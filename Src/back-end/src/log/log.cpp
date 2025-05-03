#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include "log/log.hpp"
#include "lib/lib.hpp"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_IMG(
// const char* background_image = "../include/log/backgrounds/anime_tyan.webp";
const char* background_image = "../../include/log/backgrounds/735.jpg";
)
    
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
FILE*       LogFile = nullptr;
const char* LogName = "../Log/log.html";

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeStyle     (ON_IMG(const char* wayToImage,) size_t nTabBefore);
static void makeTextClass (size_t nTabBefore);

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetHtmlColor(LogColor color);

static void fprintfInHtml                   (const char* format, ...);
static void fprintfHtml                     ();
static void fprintfBody                     ();
static void fprintfBodyWithBracket          ();
static void fprintfBodyWithBracketEnd       ();
static void fprintfHead                     ();
static void fprintfHeadEnd                  ();
static void fprintfStyle                    ();
static void fprintfStyleEnd                 ();
static void fprintfNTab                     (size_t nTab);
static void fprintfNS                       ();
static void fprintfNNS                      (size_t nNS);
ON_GRADIENT(
static void fprintfCloseBracket             ();
)
static void fprintfTextSection              ();
static void fprintfTextSectionEnd           ();
static void fprintfH1                       ();
static void fprintfH1End                    ();
// static void fprintfH2                       ();
// static void fprintfH2End                    ();
static void fprintfP                        ();
static void fprintfPEnd                     ();
static void fprintfPtext                    ();
static void fprintfPtextEnd                 ();
static void fprintfMetaWithArgs             (const char* format, ...);
static void fprintfDivWithArgs              (const char* format, ...);
static void fprintfSpanWithArgs             (const char* format, ...);
static void fprintfSpanEnd                  ();
static void fprintfBodyBefore               ();
static void fprintfBodyBeforeEnd            ();
ON_GRADIENT(
static void fprintfKeyFramesGradient        ();
static void fprintfKeyFramesGradientEnd     ();
)

static void makeBodyBefore    (                         size_t nTabBefore);

ON_IMG(
static void makeBackGroundImage      (const char* wayToImage, size_t nTabBefore);
)
OFF_IMG(
static void makeBackGround           (                        size_t nTabBefore);
)
ON_GRADIENT(
static void makeBackGroundGradient   (                        size_t nTabBefore);
)
static void makeTextSection          (                        size_t nTabBefore);
static void makeTextColor            (                        size_t nTabBefore);
static void makeP                    (                        size_t nTabBefore);
static void makeColor                (                        size_t nTabBefore);
static void makeColorP               (                        size_t nTabBefore);
static void makeH1                   (                        size_t nTabBefore);
// static void makeH2                   (                        size_t nTabBefore);


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void OpenLog()
{
    assert(LogName && "logName = nullptr");
    if (LogFile)
        EXIT(EXIT_FAILURE, "reopen log file.");

    int sys_return = system("mkdir -p ../Log/");

    if (sys_return != 0)
        EXIT(EXIT_FAILURE, "failed create log dir.");

    LogFile = fopen(LogName, "w");
    if (!LogFile)
        EXIT(EXIT_FAILURE, "failed open log file: '%s'\n", LogName);

    fprintfHtml(); fprintfNS();

    makeStyle(ON_IMG(background_image,) 1);

    makeTextClass(0);

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CloseLog()
{
    fclose(LogFile);
    LogFile = nullptr;

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogTextColor(LogColor color)
{
    assert(LogFile);

    static bool WasUsed = false;
    if (WasUsed)
    {
        LogTextColorEnd();
        WasUsed = false;
    }
    else
    {
        WasUsed = true;
    }
    
    const char* color_html = GetHtmlColor(color);

    fprintfNTab(3);
        fprintfSpanWithArgs("class=\"color %s\"", color_html); fprintfNS();
        
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogTextColorEnd()
{
    assert(LogFile);

    fprintfNTab(3); fprintfSpanEnd(); fprintfNS();

    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogAdcPrint(const char* format, ...)
{
    assert(LogFile);
    assert(format);

    va_list args;
    va_start(args, format);

    fprintfNTab(4); fprintfPtext(); vfprintf(LogFile, format, args); fprintfPtextEnd(); fprintfNS();

    fflush(LogFile);
    
    va_end(args);
    
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void LogPrint(LogColor color, const char* format, ...)
{
    assert(LogFile);

    va_list args;
    va_start(args, format);
    
    const char* color_html = GetHtmlColor(color);
    fprintfNTab(3);
    fprintfSpanWithArgs("class=\"color %s\"", color_html); fprintfNS();
    
    fprintfNTab(4); fprintfPtext(); vfprintf(LogFile, format, args); fprintfPtextEnd(); fprintfNS();
    
    fflush(LogFile);
    
    va_end(args);
    
    fprintfNTab(3);
    fprintfSpanEnd(); fprintfNS();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeStyle(ON_IMG(const char* wayToImage,) size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfHead(); fprintfNS();
    
    fprintfNTab(nTabBefore + 1);
        fprintfMetaWithArgs("name=\"viewport\" content=\"width=device-width, initial-scale=1.0\""); fprintfNS();
    
    fprintfNTab(nTabBefore + 1);
        fprintfStyle(); fprintfNS();
    
    ON_IMG
    (
            makeBackGroundImage(wayToImage, nTabBefore + 2);        
    ) 
    OFF_IMG
    (
            makeBackGround          (nTabBefore + 2);
        ON_GRADIENT
        (
            makeBackGroundGradient  (nTabBefore + 2);
        )
    )
            makeTextSection         (nTabBefore + 2);
            makeH1                  (nTabBefore + 2);
            makeP                   (nTabBefore + 2);
            makeColor               (nTabBefore + 2);
            makeColorP              (nTabBefore + 2);
            makeTextColor           (nTabBefore + 2);

            makeBodyBefore(nTabBefore + 2);

    fprintfNTab(nTabBefore + 1);
        fprintfStyleEnd(); fprintfNS();
    
    fprintfNTab(nTabBefore);
    fprintfHeadEnd(); fprintfNS();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeTextClass(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfNS(); fprintfBody(); fprintfNS();
    
    fprintfNTab(nTabBefore + 1);
        fprintfDivWithArgs("class=\"text-section\""); fprintfNS();
    }
    
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeBodyBefore(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfBodyBefore(); fprintfNS();

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("content: \"\";\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("position: fixed;\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("top: 0;\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("left: 0;\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("right: 0;\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("bottom: 0;\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("background: rgba(0, 0, 0, 0.5);\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("z-index: -1;");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("pointer-events: none;\n");


    fprintfNTab(nTabBefore);
    fprintfBodyBeforeEnd();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OFF_IMG(
static void makeBackGround(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfBodyWithBracket(); fprintfNS();
    
        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("margin: 0;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("padding: 0;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("height: 100vh;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("position: relative;\n");

        fprintfNTab(nTabBefore + 1);
        
        ON_GRADIENT
        (
            fprintfInHtml("background: linear-gradient(135deg, #000000, #000000, #6600ff, #0de612, #0de612);\n");
            // fprintfInHtml("background: linear-gradient(135deg, #ffffff, #ffffff, #ffffff, #0644f6, #0644f6, #0644f6, #ea2b0b, #ea2b0b, #ea2b0b);\n");
        )
        OFF_GRADIENT
        (
            fprintfInHtml("background-color: #000000;\n");
        )

        ON_GRADIENT(
        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("animation: gradient 15s ease infinite;\n");
        )
    
        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("background-attachment: fixed;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("background-size: 400%% 400%%;\n");


        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("font-family: Arial, sans-serif;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("color: white;\n");



    fprintfNTab(nTabBefore);
    fprintfBodyWithBracketEnd(); fprintfNNS(2);
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_GRADIENT(
static void makeBackGroundGradient(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfKeyFramesGradient(); fprintfNS();

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("0%% {\n");
            
            fprintfNTab(nTabBefore + 2);
                fprintfInHtml("background-position: 0%% 50%%;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfCloseBracket(); fprintfNS();


        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("50%% {\n");

            fprintfNTab(nTabBefore + 2);
                fprintfInHtml("background-position: 100%% 50%%;\n");

        fprintfNTab(nTabBefore + 1);
                fprintfCloseBracket(); fprintfNS();


        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("100%% {\n");
        
            fprintfNTab(nTabBefore + 2);
                fprintfInHtml("background-position: 0%% 50%%;\n");

        fprintfNTab(nTabBefore + 1);
                fprintfCloseBracket(); fprintfNS();

    fprintfNTab(nTabBefore);
    fprintfKeyFramesGradientEnd(); fprintfNNS(2);
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
ON_IMG(
static void makeBackGroundImage(const char* wayToImage, size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfBodyWithBracket(); fprintfNS();
    
        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("margin: 0;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("padding: 0;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("height: 100vh;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("background-image: url('%s');\n", wayToImage);

        fprintfNTab(nTabBefore + 1);
            OFF_IMG(
            fprintfInHtml("background-size: 400%% 400%%;\n");
            )
            ON_IMG(
            fprintfInHtml("background-size: cover;\n");
            )
        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("background-position: center;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("background-repeat: no-repeat;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("background-attachment: fixed;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("font-family: Arial, sans-serif;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("color: white;\n");

    fprintfNTab(nTabBefore);
    fprintfBodyWithBracketEnd(); fprintfNNS(2);
}
)
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeTextSection(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfTextSection(); fprintfNS();

        // fprintfNTab(nTabBefore + 1);
            // fprintfInHtml("background-color: rgba(0, 0, 0, 0.4);\n"); // затемнение секции текста

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("padding: 30px;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("border-radius: 10px;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("max-width: 800px;\n");

        fprintfNTab(nTabBefore + 1);
            fprintfInHtml("margin: 20px auto;\n");

        // fprintfNTab(nTabBefore + 1);
            // fprintfInHtml("backdrop-filter: blur(5px);\n");

    fprintfNTab(nTabBefore);
            fprintfTextSectionEnd();  fprintfNNS(2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeH1(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfH1(); fprintfNS();
    
    fprintfNTab(nTabBefore + 1); 
        fprintfInHtml("text-align: center;\n");
    fprintfNTab(nTabBefore + 1); 
        fprintfInHtml("margin-bottom: 5px;\n");

    fprintfNTab(nTabBefore);
    fprintfH1End(); fprintfNNS(2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static void makeH2(size_t nTabBefore)
// {
//     fprintfNTab(nTabBefore);
//     fprintfH2(); fprintfNS();
    
//     fprintfNTab(nTabBefore + 1); 
//         fprintfInHtml("text-align: center;\n");
//     fprintfNTab(nTabBefore + 1); 
//         fprintfInHtml("margin-bottom: 5px;\n");

//     fprintfNTab(nTabBefore);
//     fprintfH2End(); fprintfNNS(2);
// }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeP(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfP(); fprintfNS();

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("line-height: 0.5;\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("text-align: justify;\n");

    fprintfNTab(nTabBefore + 1);
        fprintfInHtml("margin: 10px 0;\n");

    fprintfNTab(nTabBefore);
    fprintfPEnd(); fprintfNNS(2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeTextColor(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfInHtml(".red_text { color: #ff4444; }\n");

    fprintfNTab(nTabBefore);
    fprintfInHtml(".green_text { color: #0bf80b; }\n");
    
    fprintfNTab(nTabBefore);
    fprintfInHtml(".pink_text { color: #f605c7; }\n");
    
    fprintfNTab(nTabBefore);
    fprintfInHtml(".yellow_text { color: #ecc40b; }\n");

    fprintfNTab(nTabBefore);
    fprintfInHtml(".blue_text { color: #0c89e8; }\n");

    fprintfNTab(nTabBefore);
    fprintfInHtml(".black_text { color: #000000; }");

    fprintfNS();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeColor(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfInHtml(".color {\n");

    fprintfNTab(nTabBefore + 1);
    fprintfInHtml("padding: 0px 10px;\n");

    fprintfNTab(nTabBefore + 1);
    fprintfInHtml("margin: 1px 0;\n");

    fprintfNTab(nTabBefore + 1);
    fprintfInHtml("line-height: 0.5;\n");

    // fprintfNTab(nTabBefore + 1);
    // fprintfInHtml("border-radius: 3px;\n");
    
    // fprintfNTab(nTabBefore + 1);
    // fprintfInHtml("border-left: 3px solid currentColor;\n");

    fprintfNTab(nTabBefore);
    fprintfInHtml("}"); fprintfNNS(2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void makeColorP(size_t nTabBefore)
{
    assert(LogFile);

    fprintfNTab(nTabBefore);
    fprintfInHtml(".color p {\n");

    fprintfNTab(nTabBefore  + 1);
    fprintfInHtml("white-space: pre;\n");

    fprintfNTab(nTabBefore + 1);
    fprintfInHtml("padding: 0;\n");

    fprintfNTab(nTabBefore + 1);
    fprintfInHtml("margin: 0;\n");

    fprintfNTab(nTabBefore + 1);
    fprintfInHtml("line-height: 1.1;\n");
    
    // fprintfNTab(nTabBefore + 1);
    // fprintfInHtml("border-left: 3px solid currentColor;\n");

    fprintfNTab(nTabBefore);
    fprintfInHtml("}"); fprintfNNS(2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfInHtml(const char* format, ...)
{
    assert(LogFile);

    va_list args;
    va_start(args, format);

    vfprintf(LogFile, format, args);
    fflush(LogFile);

    va_end(args);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfHtml()
{
    assert(LogFile);

    fprintfInHtml("<html>");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfBody()
{
    assert(LogFile);

    fprintfInHtml("<body>");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfBodyWithBracket()
{
    assert(LogFile);

    fprintfInHtml("body {");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfBodyWithBracketEnd()
{
    assert(LogFile);

    fprintfInHtml("}");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfBodyBefore()
{
    assert(LogFile);

    fprintfInHtml("body::before {");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfBodyBeforeEnd()
{
    assert(LogFile);

    fprintfInHtml("}");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfHead()
{
    assert(LogFile);

    fprintfInHtml("<head>");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfHeadEnd()
{
    assert(LogFile);

    fprintfInHtml("</head>");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfStyle()
{
    assert(LogFile);

    fprintfInHtml("<style>");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfStyleEnd()
{
    assert(LogFile);

    fprintfInHtml("</style>");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfMetaWithArgs(const char* format, ...)
{
    assert(LogFile);

    va_list args;
    va_start(args, format);

    fprintfInHtml("<meta ");

    vfprintf(LogFile, format, args);
    fflush(LogFile);

    fprintfInHtml(">");

    va_end(args);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfDivWithArgs(const char* format, ...)
{
    assert(LogFile);

    va_list args;
    va_start(args, format);

    fprintfInHtml("<div ");

    vfprintf(LogFile, format, args);
    fflush(LogFile);

    fprintfInHtml(">");

    va_end(args);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfSpanWithArgs(const char* format, ...)
{
    assert(LogFile);

    va_list args;
    va_start(args, format);

    fprintfInHtml("<span ");

    vfprintf(LogFile, format, args);
    fflush(LogFile);

    fprintfInHtml(">");

    va_end(args);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfSpanEnd()
{
    assert(LogFile);

    fprintfInHtml("</span>");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_GRADIENT(
static void fprintfKeyFramesGradient()
{
    assert(LogFile);

    fprintfInHtml("@keyframes gradient {");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfKeyFramesGradientEnd()
{
    assert(LogFile);

    fprintfInHtml("}");
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfNTab(size_t nTab)
{
    assert(LogFile);

    for (size_t i = 0; i < nTab; i++) fprintfInHtml("\t");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfNS()
{
    assert(LogFile);

    fprintfInHtml("\n");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfNNS(size_t nNS)
{
    assert(LogFile);

    for (size_t i = 0; i < nNS; i++) fprintfInHtml("\n");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_GRADIENT(
static void fprintfCloseBracket()
{
    assert(LogFile);

    fprintfInHtml(" }");
    return;
}
)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfTextSection()
{
    assert(LogFile);

    fprintfInHtml(".text-section {");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfTextSectionEnd()
{
    assert(LogFile);

    fprintfInHtml("}");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfH1()
{
    assert(LogFile);

    fprintfInHtml("h1 {");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfH1End()
{
    assert(LogFile);

    fprintfInHtml("}");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static void fprintfH2()
// {
//     fprintfInHtml("h2 {");
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static void fprintfH2End()
// {
//     fprintfInHtml("}");
// }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfP()
{
    assert(LogFile);

    fprintfInHtml("p {");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfPEnd()
{
    assert(LogFile);

    fprintfInHtml("}");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfPtext()
{
    assert(LogFile);

    fprintfInHtml("<p>");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void fprintfPtextEnd()
{
    assert(LogFile);

    fprintfInHtml("</p>");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const char* GetHtmlColor(LogColor color)
{
    switch (color)
    {
        case LogColor::Red:    return "red_text";
        case LogColor::Green:  return "green_text";
        case LogColor::Pink:   return "pink_text";
        case LogColor::Yellow: return "yellow_text";
        case LogColor::Black:  return "black_text";
        case LogColor::Blue:   return "blue_text";
        case LogColor::White:  return "";
        default:               assert(0 && "undef color in log");
    }

    return "";
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------