/**
 * ���ģ�gbk����
 */

#include <language/error.h>
#include <language/language.h>

namespace language {
namespace error {
    const char* error_map[]
        = { "���ִ���:", "ֵ����:", "�﷨����:", "�汾����:", "�ļ��򿪴���:",
              "ģ���Ҳ�������:", "��������:", "�������:", "���д���:",
              "���Դ���:", "��������:", "�ڴ����:", "������:", "ϵͳ����:" };
    const char* error_from = "��������";
    const char* error_in_line = "����������";

    const char* nameerror = "����\"%\"û�б�����.";
    const char* openfileerror = "�޷���?\"%\".";
    const char* versionerror
        = R"(�޷�ִ�����ǣ���Ϊ���ǵİ汾��%��trc�汾��%��)";
    const char* memoryerror = "�޷��Ӳ���ϵͳ�������ڴ�.";
    const char* zerodiverror = "\"%\"�����";
    const char* modulenotfounderror = "�޷��ҵ�\"%\"ģ��.";
    const char* keyerror = "��\"%\"δ����";
    const char* indexerror = R"(%������%�ķ�Χ)";
    const char* valueerror = R"("%"���ܱ�ת��Ϊ"%")";

    const char* argumenterror = R"(%��Ҫ%������.)";

    const char* syntaxerror_int = R"(����%����ȷ.)";
    const char* syntaxerror_lexstring = R"(����ַ�������"��'��β)";
    const char* syntaxerror_no_expect = R"(%�ǲ����ڴ���)";
    const char* syntaxerror_expect = R"(%�Ǳ��ڴ���.)";
    const char* syntaxerror_lexanno = "����ע��Ӧ����*/��β";
    const char* syntaxerror_escape_char = R"(ת���%δ����)";
    const char* syntaxerror = "������﷨";
    const char* syntaxerror_more_left_braces = "δƥ���'%'";
    const char* syntaxerror_less_left_braces = "δƥ���'%'";

    const char* asserterror_default = "����";
    const char* asserterror_user = "%";

    const char* dll_open_err = "�Ҳ���dll\"%\"";

    const char* noreach = "�����Ŀ�����˲�Ӧ�ñ����еĴ���"
                          ".�뽫������ⱨ���github�ֿ�";
    const char* magic_value_error
        = "Trc:\"%s\"����һ��ctree�ļ�.��Ϊ����ħ������ȷ."
          "\n";
}
namespace help {
    const char* help_msg = "Trc��һ�Ż���ջ�ı�����ԡ������Ŀʵ��"
                           "�˴󲿷��ִ�������ԵĹ��ܣ��ṩ��һ����"
                           "�ƵĹ����������ܷ��㱻Ƕ�뵽�����Ŀ�л�"
                           "����Ϊ�����ű���Ҳ���԰�����ȥѧϰ����ԭ��.";
}

namespace TVM {
    const char* type_change_error_msg = "����ת��Ϊ";
    const char* oper_not_def_error_msg = "�޷�ʹ��";
}

namespace trc {
    const char* mode_not_found = "ģʽû�б�����\n";
}

namespace tdb {
    const char* var = "����";
    const char* not_defined = "û�б�����";
    const char* start_tip = "trc�ĵ�������������.������Ķ�'Doc/"
                            "use/TDB.txt'��Ѱ�Ұ���.\n";
    const char* instruction = "ָ��";
}
}
