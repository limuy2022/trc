/**
 * ÖĞÎÄ£¬gbk±àÂë
 */

#include <language/error.h>
#include <language/language.h>

namespace language {
namespace error {
    const char* error_map[] = { "Ãû×Ö´íÎó:", "Öµ´íÎó:",
        "Óï·¨´íÎó:", "°æ±¾´íÎó:", "ÎÄ¼ş´ò¿ª´íÎó:",
        "Ä£¿éÕÒ²»µ½´íÎó:", "²ÎÊı´íÎó:", "³ıÁã´íÎó:",
        "ÔËĞĞ´íÎó:", "¶ÏÑÔ´íÎó:", "Ë÷Òı´íÎó:", "ÄÚ´æ´íÎó:",
        "¼ü´íÎó:", "ÏµÍ³´íÎó:" };
    const char* error_from = "´íÎóÀ´×Ô";
    const char* error_in_line = "´íÎó·¢ÉúÔÚĞĞ";

    const char* nameerror = "Ãû×Ö\"%\"Ã»ÓĞ±»¶¨Òå.";
    const char* openfileerror = "ÎŞ·¨´ò¿ª?\"%\".";
    const char* versionerror
        = R"(ÎŞ·¨Ö´ĞĞËüÃÇ£¬ÒòÎªËûÃÇµÄ°æ±¾ºÅ%±Ètrc°æ±¾ºÅ%¸ß)";
    const char* memoryerror = "ÎŞ·¨´Ó²Ù×÷ÏµÍ³ÖĞÉêÇëÄÚ´æ.";
    const char* zerodiverror = "\"%\"±»Áã³ı";
    const char* modulenotfounderror = "ÎŞ·¨ÕÒµ½\"%\"Ä£¿é.";
    const char* keyerror = "¼ü\"%\"Î´¶¨Òå";
    const char* indexerror = R"(%³¬³öÁË%µÄ·¶Î§)";
    const char* valueerror = R"("%"²»ÄÜ±»×ª»»Îª"%")";

    const char* argumenterror = R"(%ĞèÒª%¸ö²ÎÊı.)";

    const char* syntaxerror_int = R"(Êı×Ö%²»ÕıÈ·.)";
    const char* syntaxerror_lexstring
        = R"(Õâ¸ö×Ö·û´®²»ÒÔ"»ò'½áÎ²)";
    const char* syntaxerror_no_expect = R"(%ÊÇ²»±»ÆÚ´ıµÄ)";
    const char* syntaxerror_expect = R"(%ÊÇ±»ÆÚ´ıµÄ.)";
    const char* syntaxerror_lexanno
        = "¶àĞĞ×¢ÊÍÓ¦µ±ÒÔ*/½áÎ²";
    const char* syntaxerror_escape_char
        = R"(×ªÒå·û%Î´¶¨Òå)";
    const char* syntaxerror = "´íÎóµÄÓï·¨";

    const char* asserterror_default = "¶ÏÑÔ";
    const char* asserterror_user = "%";

    const char* dll_open_err = "ÕÒ²»µ½dll\"%\"";

    const char* noreach = "Õâ¸öÏîÄ¿ÔËĞĞÁË²»Ó¦¸Ã±»ÔËĞĞµÄ´úÂë"
                          ".Çë½«Õâ¸öÎÊÌâ±¨¸æ¸øgithub²Ö¿â";
    const char* magic_value_error
        = "Trc:\"%s\"²»ÊÇÒ»¸öctreeÎÄ¼ş.ÒòÎªËüµÄÄ§Êı²»ÕıÈ·."
          "\n";
}
namespace help {
    const char* help_msg
        = "TrcÊÇÒ»ÃÅ»ùÓÚÕ»µÄ±à³ÌÓïÑÔ¡£Õâ¸öÏîÄ¿ÊµÏÖÁË´ó²¿·ÖÏ"
          "Ö´ú±à³ÌÓïÑÔ»ù´¡¹¦ÄÜ£¬Ìá¹©ÁËÍêÉÆµÄ¹¤¾ßÁ´£¬·Ç³£ÊÊº"
          "Ï×÷Îª¹¤×÷½Å±¾»òÕßÇ¶Èëµ½ÄãµÄÏîÄ¿ÖĞÈ¥£¬Ò²ÓĞÖúÓÚÑ§Ï"
          "°±àÒëÔ­Àí.";
}

namespace TVM {
    const char* type_change_error_msg = "²»ÄÜ×ª»»Îª";
    const char* oper_not_def_error_msg = "ÎŞ·¨Ê¹ÓÃ";
}

namespace trc {
    const char* mode_not_found = "Ä£Ê½Ã»ÓĞ±»¶¨Òå\n";
}

namespace tdb {
    const char* var = "±äÁ¿";
    const char* not_defined = "Ã»ÓĞ±»¶¨Òå";
    const char* start_tip
        = "trcµÄµ÷ÊÔÆ÷ÕıÔÚÔËĞĞ.Äã¿ÉÒÔÔÄ¶Á'Doc/"
          "use/TDB.txt'ÒÔÑ°ÕÒ°ïÖú.\n";
    const char* instruction = "Ö¸Áî";
}
}
