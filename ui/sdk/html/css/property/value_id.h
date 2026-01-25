#ifndef _UI_SDK_HTML_CSS_PROPERTY_VALUEID_H_
#define _UI_SDK_HTML_CSS_PROPERTY_VALUEID_H_

#include <string>

namespace html {

enum class CSSValueID : int {
  Invalid,

  // 固定格式， id, // "name"
  // gen_value_hash.py 脚本将读取这些字段生成Perfect Hash。

  // Wide Keywords
  Initial = 1,       // "initial"
  Inherit,           // "inherit"
  Unset,             // "unset"
  Revert,            // "revert"
  RevertLayer,       // "revert-layer"

  Auto,              // "auto"
  Contain,           // "contain"
  Cover,             // "cover"

  Top,               // "top"
  Bottom,            // "bottom"
  Left,              // "left"
  Right,             // "right"
  Center,            // "center"

  BorderBox,         // "border-box"
  PaddingBox,        // "padding-box"
  ContentBox,        // "center-box"
  Text,              // "text"

  Scroll,            // "scroll"
  Fixed,             // "fixed"
  Local,             // "local"

  None,              // "none"
  Url,               // "url"

  Repeat,            // "repeat"
  RepeatX,           // "repeat-x"
  RepeatY,           // "repeat-y"
  Space,             // "space"
  Round,             // "round"
  NoRepeat,          // "no-repeat"

  // https://www.w3.org/wiki/CSS/Properties/color/keywords
  ColorBasicStart,

  // black	#000000	0,0,0
  ColorBasicBlack = ColorBasicStart, // "black"
  // silver	#C0C0C0	192,192,192
  ColorBasicSilver, // "silver"
  // gray	#808080	128,128,128
  ColorBasicGray, // "gray"
  // white	#FFFFFF	255,255,255
  ColorBasicWhite, // "white"
  // maroon	#800000	128,0,0
  ColorBasicMaroon, // "maroon"
  // red	#FF0000	255,0,0
  ColorBasicRed, // "red"
  // purple	#800080	128,0,128
  ColorBasicPurple, // "purple"
  // fuchsia	#FF00FF	255,0,255
  ColorBasicFuchsia, // "fuchsia"
  // green	#008000	0,128,0
  ColorBasicGreen, // "green"
  // lime	#00FF00	0,255,0
  ColorBasicLime, // "lime"
  // olive	#808000	128,128,0
  ColorBasicOlive, // "olive"
  // yellow	#FFFF00	255,255,0
  ColorBasicYellow, // "yellow"
  // navy	#000080	0,0,128
  ColorBasicNavy, // "navy"
  // blue	#0000FF	0,0,255
  ColorBasicBlue, // "blue"
  // teal	#008080	0,128,128
  ColorBasicTeal, // "teal"
  // aqua	#00FFFF	0,255,255
  ColorBasicAqua, // "aqua"

  ColorBasicEnd = ColorBasicAqua,
  ColorExtendedStart,

  // aliceblue	#f0f8ff	240,248,255
  ColorExtendedAliceblue = ColorExtendedStart, // "aliceblue"
  // antiquewhite	#faebd7	250,235,21
  ColorExtendedAntiquewhite, // "antiquewhite"
  // aquamarine	#7fffd4	127,255,212
  ColorExtendedAquamarine, // "aquamarine"
  // azure	#f0ffff	240,255,255
  ColorExtendedAzure, // "azure"
  // beige	#f5f5dc	245,245,220
  ColorExtendedBeige, // "beige"
  // bisque	#ffe4c4	255,228,196
  ColorExtendedBisque, // "bisque"
  // blanchedalmond	#ffebcd	255,235,
  ColorExtendedBlanchedalmond, // "blanchedalmond"
  // blueviolet	#8a2be2	138,43,226
  ColorExtendedBlueviolet, // "blueviolet"
  // brown	#a52a2a	165,42,42
  ColorExtendedBrown, // "brown"
  // burlywood	#deb887	222,184,135
  ColorExtendedBurlywood, // "burlywood"
  // cadetblue	#5f9ea0	95,158,160
  ColorExtendedCadetblue, // "cadetblue"
  // chartreuse	#7fff00	127,255,0
  ColorExtendedChartreuse, // "chartreuse"
  // chocolate	#d2691e	210,105,30
  ColorExtendedChocolate, // "chocolate"
  // coral	#ff7f50	255,127,80
  ColorExtendedCoral, // "coral"
  // cornflowerblue	#6495ed	100,149,
  ColorExtendedCornflowerblue, // "cornflowerblue"
  // cornsilk	#fff8dc	255,248,220
  ColorExtendedCornsilk, // "cornsilk"
  // crimson	#dc143c	220,20,60
  ColorExtendedCrimson, // "crimson"
  // cyan	#00ffff	0,255,255
  ColorExtendedCyan, // "cyan"
  // darkblue	#00008b	0,0,139
  ColorExtendedDarkblue, // "darkblue"
  // darkcyan	#008b8b	0,139,139
  ColorExtendedDarkcyan, // "darkcyan"
  // darkgoldenrod	#b8860b	184,134,11
  ColorExtendedDarkgoldenrod, // "darkgoldenrod"
  // darkgray	#a9a9a9	169,169,169
  ColorExtendedDarkgray, // "darkgray"
  // darkgreen	#006400	0,100,0
  ColorExtendedDarkgreen, // "darkgreen"
  // darkgrey	#a9a9a9	169,169,169
  ColorExtendedDarkgrey, // "darkgrey"
  // darkkhaki	#bdb76b	189,183,107
  ColorExtendedDarkkhaki, // "darkkhaki"
  // darkmagenta	#8b008b	139,0,139
  ColorExtendedDarkmagenta, // "darkmagenta"
  // darkolivegreen	#556b2f	85,107,4
  ColorExtendedDarkolivegreen, // "darkolivegreen"
  // darkorange	#ff8c00	255,140,0
  ColorExtendedDarkorange, // "darkorange"
  // darkorchid	#9932cc	153,50,204
  ColorExtendedDarkorchid, // "darkorchid"
  // darkred	#8b0000	139,0,0
  ColorExtendedDarkred, // "darkred"
  // darksalmon	#e9967a	233,150,122
  ColorExtendedDarksalmon, // "darksalmon"
  // darkseagreen	#8fbc8f	143,188,14
  ColorExtendedDarkseagreen, // "darkseagreen"
  // darkslateblue	#483d8b	72,61,139
  ColorExtendedDarkslateblue, // "darkslateblue"
  // darkslategray	#2f4f4f	47,79,79
  ColorExtendedDarkslategray, // "darkslategray"
  // darkslategrey	#2f4f4f	47,79,79
  ColorExtendedDarkslategrey, // "darkslategrey"
  // darkturquoise	#00ced1	0,206,209
  ColorExtendedDarkturquoise, // "darkturquoise"
  // darkviolet	#9400d3	148,0,211
  ColorExtendedDarkviolet, // "darkviolet"
  // deeppink	#ff1493	255,20,147
  ColorExtendedDeeppink, // "deeppink"
  // deepskyblue	#00bfff	0,191,255
  ColorExtendedDeepskyblue, // "deepskyblue"
  // dimgray	#696969	105,105,105
  ColorExtendedDimgray, // "dimgray"
  // dimgrey	#696969	105,105,105
  ColorExtendedDimgrey, // "dimgrey"
  // dodgerblue	#1e90ff	30,144,255
  ColorExtendedDodgerblue, // "dodgerblue"
  // firebrick	#b22222	178,34,34
  ColorExtendedFirebrick, // "firebrick"
  // floralwhite	#fffaf0	255,250,240
  ColorExtendedFloralwhite, // "floralwhite"
  // forestgreen	#228b22	34,139,34
  ColorExtendedForestgreen, // "forestgreen"
  // gainsboro	#dcdcdc	220,220,220
  ColorExtendedGainsboro, // "gainsboro"
  // ghostwhite	#f8f8ff	248,248,255
  ColorExtendedGhostwhite, // "ghostwhite"
  // gold	#ffd700	255,215,0
  ColorExtendedGold, // "gold"
  // goldenrod	#daa520	218,165,32
  ColorExtendedGoldenrod, // "goldenrod"
  // greenyellow	#adff2f	173,255,47
  ColorExtendedGreenyellow, // "greenyellow"
  // grey	#808080	128,128,128
  ColorExtendedGrey, // "grey"
  // honeydew	#f0fff0	240,255,240
  ColorExtendedHoneydew, // "honeydew"
  // hotpink	#ff69b4	255,105,180
  ColorExtendedHotpink, // "hotpink"
  // indianred	#cd5c5c	205,92,92
  ColorExtendedIndianred, // "indianred"
  // indigo	#4b0082	75,0,130
  ColorExtendedIndigo, // "indigo"
  // ivory	#fffff0	255,255,240
  ColorExtendedIvory, // "ivory"
  // khaki	#f0e68c	240,230,140
  ColorExtendedKhaki, // "khaki"
  // lavender	#e6e6fa	230,230,250
  ColorExtendedLavender, // "lavender"
  // lavenderblush	#fff0f5	255,240,24
  ColorExtendedLavenderblush, // "lavenderblush"
  // lawngreen	#7cfc00	124,252,0
  ColorExtendedLawngreen, // "lawngreen"
  // lemonchiffon	#fffacd	255,250,20
  ColorExtendedLemonchiffon, // "lemonchiffon"
  // lightblue	#add8e6	173,216,230
  ColorExtendedLightblue, // "lightblue"
  // lightcoral	#f08080	240,128,128
  ColorExtendedLightcoral, // "lightcoral"
  // lightcyan	#e0ffff	224,255,255
  ColorExtendedLightcyan, // "lightcyan"
  // lightgoldenrodyellow	#fafad2	25
  ColorExtendedLightgoldenrodyellow, // "lightgoldenrodyellow"
  // lightgray	#d3d3d3	211,211,211
  ColorExtendedLightgray, // "lightgray"
  // lightgreen	#90ee90	144,238,144
  ColorExtendedLightgreen, // "lightgreen"
  // lightgrey	#d3d3d3	211,211,211
  ColorExtendedLightgrey, // "lightgrey"
  // lightpink	#ffb6c1	255,182,193
  ColorExtendedLightpink, // "lightpink"
  // lightsalmon	#ffa07a	255,160,122
  ColorExtendedLightsalmon, // "lightsalmon"
  // lightseagreen	#20b2aa	32,178,170
  ColorExtendedLightseagreen, // "lightseagreen"
  // lightskyblue	#87cefa	135,206,25
  ColorExtendedLightskyblue, // "lightskyblue"
  // lightslategray	#778899	119,136,
  ColorExtendedLightslategray, // "lightslategray"
  // lightslategrey	#778899	119,136,
  ColorExtendedLightslategrey, // "lightslategrey"
  // lightsteelblue	#b0c4de	176,196,
  ColorExtendedLightsteelblue, // "lightsteelblue"
  // lightyellow	#ffffe0	255,255,224
  ColorExtendedLightyellow, // "lightyellow"
  // limegreen	#32cd32	50,205,50
  ColorExtendedLimegreen, // "limegreen"
  // linen	#faf0e6	250,240,230
  ColorExtendedLinen, // "linen"
  // magenta	#ff00ff	255,0,255
  ColorExtendedMagenta, // "magenta"
  // mediumaquamarine	#66cdaa	102,20
  ColorExtendedMediumaquamarine, // "mediumaquamarine"
  // mediumblue	#0000cd	0,0,205
  ColorExtendedMediumblue, // "mediumblue"
  // mediumorchid	#ba55d3	186,85,211
  ColorExtendedMediumorchid, // "mediumorchid"
  // mediumpurple	#9370db	147,112,21
  ColorExtendedMediumpurple, // "mediumpurple"
  // mediumseagreen	#3cb371	60,179,1
  ColorExtendedMediumseagreen, // "mediumseagreen"
  // mediumslateblue	#7b68ee	123,104,
  ColorExtendedMediumslateblue, // "mediumslateblue"
  // mediumspringgreen	#00fa9a	0,250,
  ColorExtendedMediumspringgreen, // "mediumspringgreen"
  // mediumturquoise	#48d1cc	72,209,2
  ColorExtendedMediumturquoise, // "mediumturquoise"
  // mediumvioletred	#c71585	199,21,1
  ColorExtendedMediumvioletred, // "mediumvioletred"
  // midnightblue	#191970	25,25,112
  ColorExtendedMidnightblue, // "midnightblue"
  // mintcream	#f5fffa	245,255,250
  ColorExtendedMintcream, // "mintcream"
  // mistyrose	#ffe4e1	255,228,225
  ColorExtendedMistyrose, // "mistyrose"
  // moccasin	#ffe4b5	255,228,181
  ColorExtendedMoccasin, // "moccasin"
  // navajowhite	#ffdead	255,222,173
  ColorExtendedNavajowhite, // "navajowhite"
  // oldlace	#fdf5e6	253,245,230
  ColorExtendedOldlace, // "oldlace"
  // olivedrab	#6b8e23	107,142,35
  ColorExtendedOlivedrab, // "olivedrab"
  // orange	#ffa500	255,165,0
  ColorExtendedOrange, // "orange"
  // orangered	#ff4500	255,69,0
  ColorExtendedOrangered, // "orangered"
  // orchid	#da70d6	218,112,214
  ColorExtendedOrchid, // "orchid"
  // palegoldenrod	#eee8aa	238,232,17
  ColorExtendedPalegoldenrod, // "palegoldenrod"
  // palegreen	#98fb98	152,251,152
  ColorExtendedPalegreen, // "palegreen"
  // paleturquoise	#afeeee	175,238,23
  ColorExtendedPaleturquoise, // "paleturquoise"
  // palevioletred	#db7093	219,112,14
  ColorExtendedPalevioletred, // "palevioletred"
  // papayawhip	#ffefd5	255,239,213
  ColorExtendedPapayawhip, // "papayawhip"
  // peachpuff	#ffdab9	255,218,185
  ColorExtendedPeachpuff, // "peachpuff"
  // peru	#cd853f	205,133,63
  ColorExtendedPeru, // "peru"
  // pink	#ffc0cb	255,192,203
  ColorExtendedPink, // "pink"
  // plum	#dda0dd	221,160,221
  ColorExtendedPlum, // "plum"
  // powderblue	#b0e0e6	176,224,230
  ColorExtendedPowderblue, // "powderblue"
  // rosybrown	#bc8f8f	188,143,143
  ColorExtendedRosybrown, // "rosybrown"
  // royalblue	#4169e1	65,105,225
  ColorExtendedRoyalblue, // "royalblue"
  // saddlebrown	#8b4513	139,69,19
  ColorExtendedSaddlebrown, // "saddlebrown"
  // salmon	#fa8072	250,128,114
  ColorExtendedSalmon, // "salmon"
  // sandybrown	#f4a460	244,164,96
  ColorExtendedSandybrown, // "sandybrown"
  // seagreen	#2e8b57	46,139,87
  ColorExtendedSeagreen, // "seagreen"
  // seashell	#fff5ee	255,245,238
  ColorExtendedSeashell, // "seashell"
  // sienna	#a0522d	160,82,45
  ColorExtendedSienna, // "sienna"
  // skyblue	#87ceeb	135,206,235
  ColorExtendedSkyblue, // "skyblue"
  // slateblue	#6a5acd	106,90,205
  ColorExtendedSlateblue, // "slateblue"
  // slategray	#708090	112,128,144
  ColorExtendedSlategray, // "slategray"
  // slategrey	#708090	112,128,144
  ColorExtendedSlategrey, // "slategrey"
  // snow	#fffafa	255,250,250
  ColorExtendedSnow, // "snow"
  // springgreen	#00ff7f	0,255,127
  ColorExtendedSpringgreen, // "springgreen"
  // steelblue	#4682b4	70,130,180
  ColorExtendedSteelblue, // "steelblue"
  // tan	#d2b48c	210,180,140
  ColorExtendedTan, // "tan"
  // thistle	#d8bfd8	216,191,216
  ColorExtendedThistle, // "thistle"
  // tomato	#ff6347	255,99,71
  ColorExtendedTomato, // "tomato"
  // turquoise	#40e0d0	64,224,208
  ColorExtendedTurquoise, // "turquoise"
  // violet	#ee82ee	238,130,238
  ColorExtendedViolet, // "violet"
  // wheat	#f5deb3	245,222,179
  ColorExtendedWheat, // "wheat"
  // whitesmoke	#f5f5f5	245,245,245
  ColorExtendedWhitesmoke, // "whitesmoke"
  // yellowgreen	#9acd32	154,205,50
  ColorExtendedYellowgreen, // "yellowgreen"

  ColorExtendedEnd = ColorExtendedYellowgreen,
  Transparent, // "transparent"

  // Function
  ColorMix,  // "color-mix"
};

constexpr size_t MaxCSSValueNameLength = 48;

CSSValueID CSSValueNameToId(const std::u16string& name);

// perfect hash function.
CSSValueID CSSValueIDNameToIdByHash(const char* name, unsigned int len);
}

#endif