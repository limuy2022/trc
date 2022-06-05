/**
 * ���ĵ����
 * ͨ���滻dll���ﵽ�������Ե�Ч��
 * ע�⣺���ļ��ı���������ģ�Ϊ����Ӧ���ģ�����ΪGBK
 */

// �������Ϣ���嶨�壬$��ʶ������

#include <language/error.h>
#include <language/language.h>

namespace language {
namespace error {
    const char* error_map[] = { "��������:", "ֵ����:",
        "�﷨����:", "�汾����:", "���ļ�����:",
        "ģ��δ���ڴ���:", "��������:", "�������:", "���д���:",
        "���Դ���:", "��������:", "�ڴ����:", "������:",
        "ϵͳ����:" };
    const char* error_from = "��������";
    const char* error_in_line = "������";

    const char* nameerror = "����\"%\"δ����.";
    const char* openfileerror = "���ܴ�\"%\".";
    const char* versionerror = R"(������������.��Ϊ%��%��)";
    const char* memoryerror = "���ܴӲ���ϵͳ�����ڴ�.";
    const char* zerodiverror = "\"%\"�����";
    const char* modulenotfounderror = "�����ҵ�ģ��\"%\".";
    const char* keyerror = "��\"%\"δ����";
    const char* indexerror = R"(%������%�ķ�Χ)";
    const char* valueerror = R"("$"���ܱ��"$")";

    const char* argumenterror = R"(%��Ҫ%������.)";

    const char* syntaxerror_int = R"(����%�ǲ���ȷ��.)";
    const char* syntaxerror_lexstring
        = R"(����ַ���Ӧ����"��'��β)";
    const char* syntaxerror_no_expect = R"(%���Ǳ��ڴ����ַ�)";
    const char* syntaxerror_expect = R"(%�Ǳ��ڴ����ַ�.)";
    const char* syntaxerror_lexanno = "ע��Ӧ����*/��β";
    const char* syntaxerror_escape_char = R"(δ����ת���%)";

    const char* asserterror_default = "����";
    const char* asserterror_user = "%";

    const char* dll_open_err = "��̬���ӿ�\"%\"����ʧ��";

    const char* noreach = "����ִ���˲�Ӧ��ִ�еĴ��룬�뽫������"
                          "������github�ֿ�";
    const char* magic_value_error
        = "Trc:\"%s\"����һ��ctree�ļ�.��Ϊ����ħ���Ǵ����"
          "\n";
}
namespace help {
    const char* help_msg
        = "Trc��һ������ջ�ı�����ԡ�����Ŀ�������������\n\
�����������㣬����֧�ṹ��ѭ���ṹ��Ȼ��ʵ���˳����۵���ʵ���˹��ڸ߾�����������ͣ�\n\
��������Լ�һЩ���õ����ú���������ʵ�����������գ�ת���ַ������ֲ���ȫ�ֱ���������\n\
�˴ӱ���ϵͳ�������������Ʊ���ϵͳ��������ģ��ĸ��ʹ�ó�����Խ���ģ�黯��̡�\n\
�����˶��ʵ�ʵĹ��ߣ�����룬���У������룬�������룬�����ļ���������ԣ������нű��ȹ���";
}

namespace TVM {
    const char* type_change_error_msg = "����ת��������";
    const char* oper_not_def_error_msg = "����ʹ��";
}

namespace trc {
    const char* mode_not_found = " ģʽû�б�����\n";
}

namespace tdb {
    const char* var = "����";
    const char* not_defined = "δ����";
    const char* start_tip
        = "trc�ĵ�������������.������Ķ�'Doc/"
          "TDB.txt'ȥ��ȡ����.\n";
    const char* instruction = "ָ��";
}
}
