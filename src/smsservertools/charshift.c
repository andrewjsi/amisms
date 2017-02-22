/*
SMS Server Tools 3
Copyright (C) 2006- Keijo Kasvi
http://smstools3.kekekasvi.com/

Based on SMS Server Tools 2, http://stefanfrings.de/smstools/
SMS Server Tools version 2 and below are Copyright (C) Stefan Frings.

This program is free software unless you got it under another license directly
from the author. You can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software Foundation.
Either version 2 of the License, or (at your option) any later version.
*/

#ifdef PRINT_NATIONAL_LANGUAGE_SHIFT_TABLES
#undef DISABLE_NATIONAL_LANGUAGE_SHIFT_TABLES
#endif

#ifndef DISABLE_NATIONAL_LANGUAGE_SHIFT_TABLES

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

#include "logging.h"
#include "smsd_cfg.h"
#include "pdu.h"
#include "charset.h"
#include "extras.h"
#include "version.h"

#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
#define LANGUAGE_NAMES_COUNT 14
#else
#define LANGUAGE_NAMES_COUNT 4
#endif

char *language_names[] = {
  "basic",
  "Turkish",
  "Spanish",
  "Portuguese"
#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
  ,
  "Bengali and Assemese",
  "Gujarati",
  "Hindi",
  "Kannada",
  "Malayalam",
  "Oriya",
  "Punjabi",
  "Tamil",
  "Telugu",
  "Urdu"
#endif
};

/*
Character Set in tables is UTF-8.
*/

char *locking_shift[][128] = {

  /* Basic Character Set */
  {
    "@", "£", "$", "¥", "è", "é", "ù", "ì", "ò", "Ç", "\n", "Ø", "ø", "\r", "Å", "å", 
    "Δ", "_", "Φ", "Γ", "Λ", "Ω", "Π", "Ψ", "Σ", "Θ", "Ξ", 0, "Æ", "æ", "ß", "É", 
    " ", "!", "\"", "#", "¤", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", 
    "¡", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Ä", "Ö", "Ñ", "Ü", "§", 
    "¿", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ä", "ö", "ñ", "ü", "à"
  },

  /* Locking Shift 1 - Turkish */
  {
    "@", "£", "$", "¥", "€", "é", "ù", "ı", "ò", "Ç", "\n", "Ğ", "ğ", "\r", "Å", "å", 
    "Δ", "_", "Φ", "Γ", "Λ", "Ω", "Π", "Ψ", "Σ", "Θ", "Ξ", 0, "Ş", "ş", "ß", "É", 
    " ", "!", "\"", "#", "¤", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", 
    "İ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Ä", "Ö", "Ñ", "Ü", "§", 
    "ç", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ä", "ö", "ñ", "ü", "à"
  },

  /* Locking Shift 2 - No Locking Shift Table Defined for Spanish */
  {
  },

  /* Locking Shift 3 - Portuguese */
  {
    "@", "£", "$", "¥", "ê", "é", "ú", "í", "ó", "ç", "\n", "Ô", "ô", "\r", "Á", "á", 
    "Δ", "_", "ª", "Ç", "À", "∞", "^", "\\", "€", "Ó", "|", 0, "Â", "â", "Ê", "É", 
    " ", "!", "\"", "#", "º", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", 
    "Í", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Ã", "Õ", "Ú", "Ü", "§", 
    "~", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ã", "õ", "`", "ü", "à"
  }

#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
  ,
  /* Locking Shift 4 - Bengali and Assemese */
  {
    "ঁ", "ং", "ঃ", "অ", "আ", "ই", "ঈ", "উ", "ঊ", "ঋ", "\n", "ঌ", 0, "\r", 0, "এ", 
    "ঐ", 0, 0, "ও", "ঔ", "ক", "খ", "গ", "ঘ", "ঙ", "চ", 0, "ছ", "জ", "ঝ", "ঞ", 
    " ", "!", "ট", "ঠ", "ড", "ঢ", "ণ", "ত", ")", "(", "থ", "দ", ",", "ধ", ".", "ন", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", 0, "প", "ফ", "?", 
    "ব", "ভ", "ম", "য", "র", 0, "ল", 0, 0, 0, "শ", "ষ", "স", "হ", "়", "ঽ", 
    "া", "ি", "ী", "ু", "ূ", "ৃ", "ৄ", 0, 0, "ে", "ৈ", 0, 0, "ো", "ৌ", "্", 
    "ৎ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ৗ", "ড়", "ঢ়", "ৰ", "ৱ"
  },

  /* Locking Shift 5 - Gujarati */
  {
    "ઁ", "ં", "ઃ", "અ", "આ", "ઇ", "ઈ", "ઉ", "ઊ", "ઋ", "\n", "ઌ", "ઍ", "\r", 0, "એ", 
    "ઐ", "ઑ", 0, "ઓ", "ઔ", "ક", "ખ", "ગ", "ઘ", "ઙ", "ચ", 0, "છ", "જ", "ઝ", "ઞ", 
    " ", "!", "ટ", "ઠ", "ડ", "ઢ", "ણ", "ત", ")", "(", "થ", "દ", ",", "ધ", ".", "ન", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", 0, "પ", "ફ", "?", 
    "બ", "ભ", "મ", "ય", "ર", 0, "લ", "ળ", 0, "વ", "શ", "ષ", "સ", "હ", "઼", "ઽ", 
    "ા", "િ", "ી", "ુ", "ૂ", "ૃ", "ૄ", "ૅ", 0, "ે", "ૈ", "ૉ", 0, "ો", "ૌ", "્", 
    "ૐ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ૠ", "ૡ", "ૢ", "ૣ", "૱"
  },

  /* Locking Shift 6 - Hindi */
  {
    "ँ", "ं", "ः", "अ", "आ", "इ", "ई", "उ", "ऊ", "ऋ", "\n", "ऌ", "ऍ", "\r", "ऎ", "ए", 
    "ऐ", "ऑ", "ऒ", "ओ", "औ", "क", "ख", "ग", "घ", "ङ", "च", 0, "छ", "ज", "झ", "ञ", 
    " ", "!", "ट", "ठ", "ड", "ढ", "ण", "त", ")", "(", "थ", "द", ",", "ध", ".", "न", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "ऩ", "प", "फ", "?", 
    "ब", "भ", "म", "य", "र", "ऱ", "ल", "ळ", "ऴ", "व", "श", "ष", "स", "ह", "़", "ऽ", 
    "ा", "ि", "ी", "ु", "ू", "ृ", "ॄ", "ॅ", "ॆ", "े", "ै", "ॉ", "ॊ", "ो", "ौ", "्", 
    "ॐ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ॲ", "ॻ", "ॼ", "ॾ", "ॿ"
  },

  /* Locking Shift 7 - Kannada */
  {
    0, "ಂ", "ಃ", "ಅ", "ಆ", "ಇ", "ಈ", "ಉ", "ಊ", "ಋ", "\n", "ಌ", 0, "\r", "ಎ", "ಏ", 
    "ಐ", 0, "ಒ", "ಓ", "ಔ", "ಕ", "ಖ", "ಗ", "ಘ", "ಙ", "ಚ", 0, "ಛ", "ಜ", "ಝ", "ಞ", 
    " ", "!", "ಟ", "ಠ", "ಡ", "ಢ", "ಣ", "ತ", ")", "(", "ಥ", "ದ", ",", "ಧ", ".", "ನ", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", 0, "ಪ", "ಫ", "?", 
    "ಬ", "ಭ", "ಮ", "ಯ", "ರ", "ಱ", "ಲ", "ಳ", 0, "ವ", "ಶ", "ಷ", "ಸ", "ಹ", "಼", "ಽ", 
    "ಾ", "ಿ", "ೀ", "ು", "ೂ", "ೃ", "ೄ", 0, "ೆ", "ೇ", "ೈ", 0, "ೊ", "ೋ", "ೌ", "್", 
    "ೕ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ೖ", "ೠ", "ೡ", "ೢ", "ೣ"
  },

  /* Locking Shift 8 - Malayalam */
  {
    0, "ം", "ഃ", "അ", "ആ", "ഇ", "ഈ", "ഉ", "ഊ", "ഋ", "\n", "ഌ", 0, "\r", "എ", "ഏ", 
    "ഐ", 0, "ഒ", "ഓ", "ഔ", "ക", "ഖ", "ഗ", "ഘ", "ങ", "ച", 0, "ഛ", "ജ", "ഝ", "ഞ", 
    " ", "!", "ട", "ഠ", "ഡ", "ഢ", "ണ", "ത", ")", "(", "ഥ", "ദ", ",", "ധ", ".", "ന", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", 0, "പ", "ഫ", "?", 
    "ബ", "ഭ", "മ", "യ", "ര", "റ", "ല", "ള", "ഴ", "വ", "ശ", "ഷ", "സ", "ഹ", 0, "ഽ", 
    "ാ", "ി", "ീ", "ു", "ൂ", "ൃ", "ൄ", 0, "െ", "േ", "ൈ", 0, "ൊ", "ോ", "ൌ", "്", 
    "ൗ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ൠ", "ൡ", "ൢ", "ൣ", "൹"
  },

  /* Locking Shift 9 - Oriya */
  {
    "ଁ", "ଂ", "ଃ", "ଅ", "ଆ", "ଇ", "ଈ", "ଉ", "ଊ", "ଋ", "\n", "ଌ", 0, "\r", 0, "ଏ", 
    "ଐ", 0, 0, "ଓ", "ଔ", "କ", "ଖ", "ଗ", "ଘ", "ଙ", "ଚ", 0, "ଛ", "ଜ", "ଝ", "ଞ", 
    " ", "!", "ଟ", "ଠ", "ଡ", "ଢ", "ଣ", "ତ", ")", "(", "ଥ", "ଦ", ",", "ଧ", ".", "ନ", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", 0, "ପ", "ଫ", "?", 
    "ବ", "ଭ", "ମ", "ଯ", "ର", 0, "ଲ", "ଳ", 0, "ଵ", "ଶ", "ଷ", "ସ", "ହ", "଼", "ଽ", 
    "ା", "ି", "ୀ", "ୁ", "ୂ", "ୃ", "ୄ", 0, 0, "େ", "ୈ", 0, 0, "ୋ", "ୌ", "୍", 
    "ୖ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ୗ", "ୠ", "ୡ", "ୢ", "ୣ"
  },

  /* Locking Shift 10 - Punjabi */
  {
    "ਁ", "ਂ", "ਃ", "ਅ", "ਆ", "ਇ", "ਈ", "ਉ", "ਊ", 0, "\n", 0, 0, "\r", 0, "ਏ", 
    "ਐ", 0, 0, "ਓ", "ਔ", "ਕ", "ਖ", "ਗ", "ਘ", "ਙ", "ਚ", 0, "ਛ", "ਜ", "ਝ", "ਞ", 
    " ", "!", "ਟ", "ਠ", "ਡ", "ਢ", "ਣ", "ਤ", ")", "(", "ਥ", "ਦ", ",", "ਧ", ".", "ਨ", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", 0, "ਪ", "ਫ", "?", 
    "ਬ", "ਭ", "ਮ", "ਯ", "ਰ", 0, "ਲ", "ਲ਼", 0, "ਵ", "ਸ਼", 0, "ਸ", "ਹ", "਼", 0, 
    "ਾ", "ਿ", "ੀ", "ੁ", "ੂ", 0, 0, 0, 0, "ੇ", "ੈ", 0, 0, "ੋ", "ੌ", "੍", 
    "ੑ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ੰ", "ੱ", "ੲ", "ੳ", "ੴ"
  },

  /* Locking Shift 11 - Tamil */
  {
    0, "ஂ", "ஃ", "அ", "ஆ", "இ", "ஈ", "உ", "ஊ", 0, "\n", 0, 0, "\r", "எ", "ஏ", 
    "ஐ", 0, "ஒ", "ஓ", "ஔ", "க", 0, 0, 0, "ங", "ச", 0, 0, "ஜ", 0, "ஞ", 
    " ", "!", "ட", 0, 0, 0, "ண", "த", ")", "(", 0, 0, ",", 0, ".", "ந", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "ன", "ப", 0, "?", 
    0, 0, "ம", "ய", "ர", "ற", "ல", "ள", "ழ", "வ", "ஶ", "ஷ", "ஸ", "ஹ", 0, 0, 
    "ா", "ி", "ீ", "ு", "ூ", 0, 0, 0, "ெ", "ே", "ை", 0, "ொ", "ோ", "ௌ", "்", 
    "ௐ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ௗ", "௰", "௱", "௲", "௹"
  },

  /* Locking Shift 12 - Telugu */
  {
    "ఁ", "ం", "ః", "అ", "ఆ", "ఇ", "ఈ", "ఉ", "ఊ", "ఋ", "\n", "ఌ", 0, "\r", "ఎ", "ఏ", 
    "ఐ", 0, "ఒ", "ఓ", "ఔ", "క", "ఖ", "గ", "ఘ", "ఙ", "చ", 0, "ఛ", "జ", "ఝ", "ఞ", 
    " ", "!", "ట", "ఠ", "డ", "ఢ", "ణ", "త", ")", "(", "థ", "ద", ",", "ధ", ".", "న", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", 0, "ప", "ఫ", "?", 
    "బ", "భ", "మ", "య", "ర", "ఱ", "ల", "ళ", 0, "వ", "శ", "ష", "స", "హ", 0, "ఽ", 
    "ా", "ి", "ీ", "ు", "ూ", "ృ", "ౄ", 0, "ె", "ే", "ై", 0, "ొ", "ో", "ౌ", "్", 
    "ౕ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ౖ", "ౠ", "ౡ", "ౢ", "ౣ"
  },

  /* Locking Shift 13 - Urdu */
  {
    "ا", "آ", "ب", "ٻ", "ڀ", "پ", "ڦ", "ت", "ۂ", "ٿ", "\n", "ٹ", "ٽ", "\r", "ٺ", "ټ", 
    "ث", "ج", "ځ", "ڄ", "ڃ", "څ", "چ", "ڇ", "ح", "خ", "د", 0, "ڌ", "ڈ", "ډ", "ڊ", 
    " ", "!", "ڏ", "ڍ", "ذ", "ر", "ڑ", "ړ", ")", "(", "ڙ", "ز", ",", "ږ", ".", "ژ", 
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "ښ", "س", "ش", "?", 
    "ص", "ض", "ط", "ظ", "ع", "ف", "ق", "ک", "ڪ", "ګ", "گ", "ڳ", "ڱ", "ل", "م", "ن", 
    "ں", "ڻ", "ڼ", "و", "ۄ", "ە", "ہ", "ھ", "ء", "ی", "ې", "ے", "ٍ", "ِ", "ُ", "ٗ", 
    "ٔ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", 
    "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "ٕ", "ّ", "ٓ", "ٖ", "ٰ"
  }
#endif
};

char *single_shift[][128] = {

  /* Basic Character Set Extension */
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "\f", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, "^", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, "{", "}", 0, 0, 0, 0, 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 1 - Turkish */
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "\f", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, "^", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, "{", "}", 0, 0, 0, 0, 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", 0, 0, 0, 0, 0, 0, "Ğ", 0, "İ", 0, 0, 0, 0, 0, 0, 
    0, 0, 0, "Ş", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, "ç", 0, "€", 0, "ğ", 0, "ı", 0, 0, 0, 0, 0, 0, 
    0, 0, 0, "ş", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 2 - Spanish */
  {
    0, 0, 0, 0, 0, 0, 0, 0, 0, "ç", "\f", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, "^", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, "{", "}", 0, 0, 0, 0, 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "Á", 0, 0, 0, 0, 0, 0, 0, "Í", 0, 0, 0, 0, 0, "Ó", 
    0, 0, 0, 0, 0, "Ú", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, "á", 0, 0, 0, "€", 0, 0, 0, "í", 0, 0, 0, 0, 0, "ó", 
    0, 0, 0, 0, 0, "ú", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 3 - Portuguese */
  {
    0, 0, 0, 0, 0, "ê", 0, 0, 0, "ç", "\f", "Ô", "ô", 0, "Á", "á", 
    0, 0, "Φ", "Γ", "^", "Ω", "Π", "Ψ", "Σ", "Θ", 0, 0, 0, 0, 0, "Ê", 
    0, 0, 0, 0, 0, 0, 0, 0, "{", "}", 0, 0, 0, 0, 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "À", 0, 0, 0, 0, 0, 0, 0, "Í", 0, 0, 0, 0, 0, "Ó", 
    0, 0, 0, 0, 0, "Ú", 0, 0, 0, 0, 0, "Ã", "Õ", 0, 0, 0, 
    0, "Â", 0, 0, 0, "€", 0, 0, 0, "í", 0, 0, 0, 0, 0, "ó", 
    0, 0, 0, 0, 0, "ú", 0, 0, 0, 0, 0, "ã", "õ", 0, 0, "â"
  }

#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
  ,
  /* Single Shift 4 - Bengali and Assemese */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "০", "১", 0, "২", "৩", "৪", "৫", 
    "৬", "৭", "৮", "৯", "য়", "ৠ", "ৡ", "ৢ", "{", "}", "ৣ", "৲", "৳", "৴", "৵", "\\", 
    "৶", "৷", "৸", "৹", "৺", 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 5 - Gujarati */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "।", "॥", 0, "૦", "૧", "૨", "૩", 
    "૪", "૫", "૬", "૭", "૮", "૯", 0, 0, "{", "}", 0, 0, 0, 0, 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 6 - Hindi */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "।", "॥", 0, "०", "१", "२", "३", 
    "४", "५", "६", "७", "८", "९", "॑", "॒", "{", "}", "॓", "॔", "क़", "ख़", "ग़", "\\", 
    "ज़", "ड़", "ढ़", "फ़", "य़", "ॠ", "ॡ", "ॢ", "ॣ", "॰", "ॱ", 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 7 - Kannada */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "।", "॥", 0, "೦", "೧", "೨", "೩", 
    "೪", "೫", "೬", "೭", "೮", "೯", "ೞ", "ೱ", "{", "}", "ೲ", 0, 0, 0, 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 8 - Malayalam */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "।", "॥", 0, "൦", "൧", "൨", "൩", 
    "൪", "൫", "൬", "൭", "൮", "൯", "൰", "൱", "{", "}", "൲", "൳", "൴", "൵", "ൺ", "\\", 
    "ൻ", "ർ", "ൽ", "ൾ", "ൿ", 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 9 - Oriya */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "।", "॥", 0, "୦", "୧", "୨", "୩", 
    "୪", "୫", "୬", "୭", "୮", "୯", "ଡ଼", "ଢ଼", "{", "}", "ୟ", "୰", "ୱ", 0, 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 10 - Punjabi */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "।", "॥", 0, "੦", "੧", "੨", "੩", 
    "੪", "੫", "੬", "੭", "੮", "੯", "ਖ਼", "ਗ਼", "{", "}", "ਜ਼", "ੜ", "ਫ਼", "ੵ", 0, "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 11 - Tamil */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "।", "॥", 0, "௦", "௧", "௨", "௩", 
    "௪", "௫", "௬", "௭", "௮", "௯", "௳", "௴", "{", "}", "௵", "௶", "௷", "௸", "௺", "\\", 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 12 - Telugu */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", 0, 0, 0, "౦", "౧", "౨", "౩", 
    "౪", "౫", "౬", "౭", "౮", "౯", "ౘ", "ౙ", "{", "}", "౸", "౹", "౺", "౻", "౼", "\\", 
    "౽", "౾", "౿", 0, 0, 0, 0, 0, 0, 0, 0, 0, "[", "~", "]", 0, 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },

  /* Single Shift 13 - Urdu */
  {
    "@", "£", "$", "¥", "¿", "\"", "¤", "%", "&", "'", "\f", "*", "+", 0, "-", "/", 
    "<", "=", ">", "¡", "^", "¡", "_", "#", "*", "؀", "؁", 0, "۰", "۱", "۲", "۳", 
    "۴", "۵", "۶", "۷", "۸", "۹", "،", "؍", "{", "}", "؎", "؏", "ؐ", "ؑ", "ؒ", "\\", 
    "ؓ", "ؔ", "؛", "؟", "ـ", "ْ", "٘", "٫", "٬", "ٲ", "ٳ", "ۍ", "[", "~", "]", "۔", 
    "|", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", 
    "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, "€", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }
#endif
};

char *log_err_str = 0;

void logerror(char* format, ...)
{
  va_list argp;
  char text[2048];

  va_start(argp, format);
  vsnprintf(text, sizeof(text), format, argp);
  va_end(argp);

  if (!log_err_str)
  {
    log_err_str = (char *)malloc(strlen(text) +1);
    if (log_err_str)
      *log_err_str = 0;
  }
  else
    log_err_str = (char *)realloc((void *)log_err_str, strlen(log_err_str) + strlen(text) +1);

  if (log_err_str)
    strcat(log_err_str, text);
}

char *get_language_name
(
  int value
)
{
  if (value >= 0 && value < LANGUAGE_NAMES_COUNT)
    return language_names[value];

  return "";
}

int parse_language_setting
(
  char *value
)
{
  int i;
  char test[32];
  char *p;

  snprintf(test, sizeof(test), "%s", value);
  if ((p = strchr(test, ' ')))
    *p = '\0';

  if (!(*test))
    return -1;

  for (i = 0; i < LANGUAGE_NAMES_COUNT; i++)
    if (!strncasecmp(test, language_names[i], strlen(test)))
      return i;

#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
  if (!strncasecmp(test, "Bengali", strlen(test)) || !strncasecmp(test, "Assemese", strlen(test)))
    return 4;
#endif

  i = (int)strtol(test, NULL, 10);
  if (errno == EINVAL || i < 0 || i > LANGUAGE_NAMES_COUNT - 1)
    return -1;

  return i;
}

int select_language_shift_tables
(
  int *language,
  int *language_ext,
  int DEVICE_language,
  int DEVICE_language_ext
)
{

  // language_ext defaults to language:
  if (*language_ext < 0)
    *language_ext = *language;

  // If only language_ext is given (other than basic), language defaults to basic:
  if (*language < 0 && *language_ext > 0)
    *language = 0;

  if (*language < 0)
  {
    if (DEVICE_language_ext < 0)
      DEVICE_language_ext = DEVICE_language;

    if (DEVICE_language < 0 && DEVICE_language_ext > 0)
      DEVICE_language = 0;

    *language = DEVICE_language;
    *language_ext = DEVICE_language_ext;
  }

  // No locking shift table for Spanish:
  if (*language == 2)
    *language = 0;

  if (*language > 0 || *language_ext > 0)
    return 1;

  return 0;
}

int search_from_table(
  char *table[],
  char *test
)
{
  int i;

  for (i = 0; i < 128; i++)
    if (table[i] && !strcmp(table[i], test))
      return i;

  return -1;
}

int utf2gsm_shift
(
  char *text,
  size_t text_size,
  int *textlen,
  int *language,
  int *language_ext,
  char **notice
)
{
  int result = 0;
  char *source;
  char *ps, *pd;
  char *p, *p2;
  char *max_ps; // If UTF-8 is broken
  int bytes;
  int chars;
  char test[5];
  char tmp[64];
  int i, l;
  int found;
  int retry = 1;
  int initial_textlen = *textlen;
  int basic_ok = *language > 0;
  int basic_ext_ok = *language_ext > 0;
  int language_used = 0;
  int language_ext_used = 0;

  if (!(source = (char *)malloc(initial_textlen + 1)))
    result = -1;
  else
  {
    strncpy(source, text, initial_textlen);
    source[initial_textlen] = '\0';

    while (retry)
    {
      retry = 0;
      free(log_err_str);
      log_err_str = 0;
      memset(text, 0, text_size);
      ps = source;
      pd = text;
      max_ps = ps + initial_textlen -1;
      *textlen = 0;
      chars = 0;

      while (ps <= max_ps && *ps)
      {
        bytes = utf8bytes(ps);
        if (bytes <= 0)
        {
          memset(tmp, 0, sizeof(tmp));
          for (i = 0; ps[i] && i < 6; i++)
            sprintf(strchr(tmp, 0), " %02X", (unsigned char)ps[i]);
          tb_sprintf("ERROR: Invalid UTF-8 sequence after %i characters, starting at%s.\n", chars, tmp);
          logerror(tb + 7);
          if (notice)
            strcat_realloc(notice, tb, 0);

          result = -1;
          break;
        }

        strncpy(test, ps, bytes);
        test[bytes] = '\0';
        ps += bytes;
        chars++;
        found = 0;

        if (basic_ok && (i = search_from_table(locking_shift[0], test)) >= 0)
        {
          found = 1;
          *pd = (unsigned char)i;
          pd++;
          (*textlen)++;
        }

        if (!found && basic_ext_ok && (i = search_from_table(single_shift[0], test)) >= 0)
        {
          found = 1;
          *pd = (unsigned char)0x1B;
          pd++;
          *pd = (unsigned char)i;
          pd++;
          (*textlen) += 2;
        }

        if (!found && (i = search_from_table(locking_shift[*language], test)) >= 0)
        {
          if (basic_ok)
          {
            basic_ok = 0;
            retry = 1;
            break;
          }

          found = 1;
          *pd = (unsigned char)i;
          pd++;
          (*textlen)++;
          language_used = 1;
        }

        if (!found && (i = search_from_table(single_shift[*language_ext], test)) >= 0)
        {
          if (basic_ext_ok)
          {
            basic_ext_ok = 0;
            retry = 1;
            break;
          }

          found = 1;
          *pd = (unsigned char)0x1B;
          pd++;
          *pd = (unsigned char)i;
          pd++;
          (*textlen) += 2;
          language_ext_used = 1;
        }

        if (!found)
        {
          memset(tmp, 0, sizeof(tmp));
          for (i = 0, l = strlen(test); i < l; i++)
            sprintf(strchr(tmp, 0), "%02X ", (unsigned char)test[i]);
          tb_sprintf("NOTICE: Cannot convert %i. UTF-8 character %s(%s) to GSM, not found on language tables %i/%i.\n", chars, tmp, test, *language, *language_ext);
          logerror(tb + 8);
          if (notice)
            strcat_realloc(notice, tb, 0);
        }
      }
    }

    free(source);

    if (!language_used)
      *language = 0;
    if (!language_ext_used)
      *language_ext = 0;
  }

  if (log_err_str)
  {
    p = log_err_str;
    while (p && *p)
    {
      if ((p2 = strchr(p, '\n')))
        *p2 = 0;
      writelogfile(LOG_NOTICE, 0, "%s", p);
      p = (p2)? p2 +1 : NULL;
    }

    free(log_err_str);
    log_err_str = 0;
  }

  return result;
}

int get_language_shift
(
  char *udh,
  int *language,
  int *language_ext
)
{
  int result = 0;
  int octets, idx, id, i;

  if (language)
    *language = -1;
  if (language_ext)
    *language_ext = -1;

  if (!udh || octet2bin_check(udh) <= 0)
    return 0;

  octets = strlen(udh) /3;
  idx = 1;
  while (idx < octets)
  {
    if ((id = octet2bin_check(udh +idx *2 +idx)) < 0)
      return 0;

    if (id == 0x25 || id == 0x24)
    {
      if (++idx >= octets)
        return 0;
      if ((i = octet2bin_check(udh +idx *2 +idx)) != 0x01)
        return 0;
      if (++idx >= octets)
        return 0;
      i = octet2bin_check(udh +idx *2 +idx);
      if (i < 0 || i > 13)
        return 0;

      if (id == 0x25 && language)
        *language = i;
      else if (id == 0x24 && language_ext)
        *language_ext = i;

      result++;
      idx++;
      continue;
    }

    if (++idx >= octets)
      return 0;
    if ((i = octet2bin_check(udh +idx *2 +idx)) < 0)
      return 0;
    idx += i +1;
  }

  return result;
}

int gsm2utf8_shift
(
  char *buffer,
  size_t buffer_size,
  int userdatalength,
  int language,
  int language_ext
)
{
  char *source;
  int dest_length = 0;
  int s = 0;
  int ch;
  char *p;
  int l;

  if (language < 0)
    language = 0;
  if (language_ext < 0)
    language_ext = 0;

  if (language >= LANGUAGE_NAMES_COUNT || language_ext >= LANGUAGE_NAMES_COUNT)
  {
    writelogfile(LOG_ERR, 0, "Message is using language values which are not supported in this compilation: language:%i, language_ext:%i", language, language_ext);
    return 0;
  }

  if ((source = (char *)malloc(userdatalength)))
  {
    memcpy(source, buffer, userdatalength);
    *buffer = '\0';

    while (s < userdatalength)
    {
      p = 0;

      if ((ch = source[s]) == 0x1B)
      {
        if (++s >= userdatalength)
          break;
        ch = source[s];
        p = single_shift[language_ext][ch];

        // 0x1B is reserved for the extension to another extension table.
        // On receipt of this code, a receiving entity shall display a space
        // until another extension table is defined.
        if (!p && ch == 0x1B)
          p = locking_shift[0][0x20];
      }
      else
        p = locking_shift[language][ch];

      if (p)
      {
        l = strlen(p);
        if (dest_length + l + 1 > (int)buffer_size)
          break;
        strcat(buffer, p);
        dest_length += l;
      }

      s++;
    }

    free(source);

    return dest_length;
  }

  return 0;
}

#ifdef PRINT_NATIONAL_LANGUAGE_SHIFT_TABLES

char *utf2ucs2html
(
  char *dest,
  size_t dest_size,
  char *src
)
{
  char ucs2[2];

  *dest = 0;

  if (!utf8_to_ucs2_char(src, 0, ucs2))
    snprintf(dest, dest_size, "<br>ERROR");
  else if (ucs2[0])
    snprintf(dest, dest_size, "<br><small>%02X%02X</small>", (unsigned char)ucs2[0], (unsigned char)ucs2[1]);
  else
    snprintf(dest, dest_size, "<br><small>&nbsp;</small>");

  return dest;
}

void print_language_tables
(
void
)
{
  int table, locking, row, col, i;
  char *p;
  char ucs2html[128];

  printf(
"<!DOCTYPE html>\n"\
"<html lang='en'>\n"\
"<head>\n"\
"<meta charset='UTF-8'/>\n"
"<title>SMS Server Tools 3</title>\n");

  printf(
"<style>\n");

#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
  printf(
"@import url(http://fonts.googleapis.com/earlyaccess/notosansbengali.css);\n"\
"@import url(http://fonts.googleapis.com/earlyaccess/notosansgujarati.css);\n"\
"@import url(http://fonts.googleapis.com/earlyaccess/notosanskannada.css);\n"\
"@import url(http://fonts.googleapis.com/earlyaccess/notosansmalayalam.css);\n"\
"@import url(http://fonts.googleapis.com/earlyaccess/notosansoriya.css);\n"\
"@import url(http://fonts.googleapis.com/earlyaccess/notosanstamil.css);\n"\
"@import url(http://fonts.googleapis.com/earlyaccess/notosanstelugu.css);\n"\
"@import url(http://fonts.googleapis.com/earlyaccess/notonastaliqurdu.css);\n"\
".lang_bengali { font-family: 'Noto Sans Bengali', sans-serif; }\n"\
".lang_gujarati { font-family: 'Noto Sans Gujarati', sans-serif; }\n"\
".lang_kannada { font-family: 'Noto Sans Kannada', sans-serif; }\n"\
".lang_malayalam { font-family: 'Noto Sans Malayalam', sans-serif; }\n"\
".lang_oriya { font-family: 'Noto Sans Oriya', sans-serif; }\n"\
".lang_tamil { font-family: 'Noto Sans Tamil', sans-serif; }\n"\
".lang_telugu { font-family: 'Noto Sans Telugu', sans-serif; }\n"\
".lang_urdu { font-family: 'Noto Nastaliq Urdu', serif; }\n");
#endif

  printf(
"table.nlst,\n"\
"table.nlst td { border: 1px solid black; border-collapse: collapse; text-align: center; font-family: sans-serif; }\n"\
".nlst_text { font-family: sans-serif; }\n"\
".nlst_header { font-family: sans-serif; background-color: lightgrey; }\n"\
".nlst_notused { background-color: #F2F2F2; }\n"\
".nlst_reserved { background-color: #BDBDBD; }\n"\
"small { font-size: x-small; }\n"\
"body { margin: 5px 5px 10px; font: 10pt verdana, geneva, lucida, arial, helvetica, sans-serif; }\n"\
"h3 { background-color: #DCDCFE; }\n"\
"</style>\n</head>\n<body>\n");

  printf(
"<h2><font color=blue><a href=\"http://smstools3.kekekasvi.com\">SMS Server Tools 3</a></font></h2>\n"\
"<a href=\"index.html\">Home</a>\n"\
"<h3>National Language Shift Tables</h3>\n"\
"<!-- START -->\n");

  printf("<p class='nlst_text'>\n");
  printf("SMS Server Tools 3 version %s.<br />\n", smsd_version);
  printf("Character set is UTF-8.<br />\n");
  printf("Codes shown are UCS-2BE.<br />\n");
#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
  printf("Tables marked with *) are shown using a font from <a href='https://fonts.google.com/earlyaccess'>Google Fonts Early Access</a>.<br />\n");
#endif
  printf("</p>\n");

  for (table = 0; table < LANGUAGE_NAMES_COUNT; table++)
  {
    locking = (table == 2)? 0 : table;

    printf("<p class='nlst_text'>\n");
    printf("%s (%i)", language_names[table], table);

    p = "";
#ifndef NATIONAL_LANGUAGES_EUROPEAN_ONLY
    switch (table)
    {
      case 4: p = "bengali"; break;
      case 5: p = "gujarati"; break;
      case 7: p = "kannada"; break;
      case 8: p = "malayalam"; break;
      case 9: p = "oriya"; break;
      case 11: p = "tamil"; break;
      case 12: p = "telugu"; break;
      case 13: p = "urdu"; break;
    }
#endif

    printf("%s<br /><table class='nlst%s%s'>\n", (*p)? "&nbsp; *)" : "", (*p)? " lang_" : "", p);

    printf("<tr class='nlst_header'>\n");
    for (i = 0; i < 2; i++)
    {
      printf("<td><small>%s<br>Shift</small></td>", (!i)? "Locking" : "Single");
      for (col = 0; col <= 0x70; col += 0x10)
        printf("<td>0x%02X</td>", col);
    }
    printf("</tr>\n");

    for (row = 0; row <= 0x0F; row++)
    {
      printf("<tr><td class='nlst_header'>0x%02X</td>", row);

      for (col = 0; col <= 0x70; col += 0x10)
      {
        if ((p = locking_shift[locking][row +col]))
        {
          utf2ucs2html(ucs2html, sizeof(ucs2html), p);

          switch (*p)
          {
            case '\n': p = "<b>LF</b>"; break;
            case '\r': p = "<b>CR</b>"; break;
            case ' ': p = "<b>SP</b>"; break;
            case '<': p = "&lt;"; break;
            case '>': p = "&gt;"; break;
          }
          printf("<td>%s%s</td>", p, ucs2html);
        }
        else
          printf("<td class='nlst_%s'></td>", (row +col == 0x1B)? "reserved" : "notused");
      }

      printf("<td class='nlst_header'>0x%02X</td>", row);

      for (col = 0; col <= 0x70; col += 0x10)
      {
        if ((p = single_shift[table][row +col]))
        {
          utf2ucs2html(ucs2html, sizeof(ucs2html), p);

          switch (*p)
          {
            case '\f': p = "<b>FF</b>"; break;
            case '<': p = "&lt;"; break;
            case '>': p = "&gt;"; break;
          }
          printf("<td>%s%s</td>", p, ucs2html);
        }
        else
          printf("<td class='nlst_%s'></td>", (row +col == 0x0D || row +col == 0x1B)? "reserved" : "notused");
      }

      printf("</tr>\n");
    }

    printf("</table>\n<br />\n");
    printf("</p>\n");
  }

  printf("<hr>\n</body>\n</html>\n");
}
#endif
#endif
