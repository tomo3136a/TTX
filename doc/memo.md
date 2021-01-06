# �J������

## TTX �Ƃ�

TTX �́A Tera Term eXtension �̗��́B
�\����ێ����Ă��邽�߁A�Â��\�[�X�R�[�h�Ńr���h�����v���O�C���t�@�C���͂��̂܂܎g����炵���B���X�A�Ăяo���K�񂪕ς���Ă���A�Â����Ă����߂炵���B

Tera Term 4.70�`4.105 ���炢�܂ł͌ʂ̃o�O�͂�����̂́A���삳���邱�Ƃ��ł���B����ł��ŐV�łŃr���h����Γ��삪�]�߂�B

## C11H17N3O8.txt �Ƃ�

TTX �𐧌䂷�邽�߂̐ݒ�t�@�C���B
***C11H17N3O8*** �́A�ӂ��łŒm���� �e�g���h�g�L�V�� (tetrodotoxin, TTX) �̉��w���B�_�o�ł�


## �R�[�h�̐��`

�\�Ȍ��肫�ꂢ�ȏ�Ԃ�ۂB

�\�[�X�t�@�C���̖��̂́A��{�I�ɏ������ɓ��ꂷ��B

�G�f�B�^�� Visual Studio Code ���g�p���Ă�̂ŁA�Ƃ肠�����A Shift �{ Alt+F �Ő��`����B

## �R�[�h�̋��ʉ�

���ʃR�[�h�́Acommon/ttxcommon.h ����� common/ttxcommon.c �ɓZ�߂�B�������A�sera �serm �\�[�X�R�[�h�Ɉˑ������A�P�̂ŃR���p�C���\�ȃ\�[�X�R�[�h�Ƃ���B�e�v���O�C���\�[�X�R�[�h�ɂ����ẮAInclude �ɂ��g�p�\�ɂ���B

```c
#include "compat_w95.h"
#include "ttxcommon.h"              //<== �ǉ�
#include "resource.h"
```

## UI ��ʂ̍��ۉ�

���ۉ��͓���̂œ��ʂ̓V���v���ɃR�[�h�������Ƃ���B
UILangageFile �̑I�������{��(=2)�Ƃ��̑�(=1)�Ƃ��A���̑��̏ꍇ�͉p��\���Ƃ���B

```c
    UINT lang;
    LPSTR s;

    lang = UILang(pvar->ts->UILanguageFile);

    s = (lang == 2) ? "���{��(&J)..." : "&Japanese...";
    AppendMenu(pvar->SetupMenu, flag, ID_MENUITEM, s);
```

## �v���O�C�� ON/OFF �Ή�

�v���O�C����L���E�����𐧌䂵�����v���O�C���ɂ́ATTXBind �֐����� ***TTXIgnore()*** �֐���ǉ�����B

```c
BOOL __declspec(dllexport) PASCAL TTXBind(WORD Version, TTXExports *exports)
{
    int size = sizeof(Exports) - sizeof(exports->size);
    /* do version checking if necessary */
    /* if (Version!=TTVERSION) return FALSE; */

    //{when TTXPlugin support}              //<== �ǉ�
    if (TTXIgnore(ORDER, INISECTION, 0))    //<== �ǉ�
        return TRUE;                        //<== �ǉ�

```

## ���j���[�A�C�e���ԍ��̏d���Ή�

���j���[�A�C�e���̔ԍ��́A56000 �ȍ~�Ɋ��蓖�Ă�B
�e�v���O�C�����m�ŏd�����Ȃ��悤�ɋC��t����B�d�Ȃ��Ă��܂����Ƃ��ɑΉ��ł���悤�ɁA���j���[�A�C�e���ԍ��I�t�Z�b�g�ɑΉ�������B

***pvar->menuoffset*** �̐ݒ��ǉ�����B�܂��A���j���[�A�C�e���̔ԍ�(ID_MENUITEM �� ID_MENUITEM1 ��)�̉ӏ��� " ***+ pvar->menuoffset*** " ��ǉ�����B

```c
typedef struct
{
    PTTSet ts;
    PComVar cv;
    BOOL skip;
    int menuoffset;         // <==�ǉ�
} TInstVar;
...

static void PASCAL TTXModifyMenu(HMENU menu)
{
    HMENU submenu;
    UINT menu_id;

    pvar->menuoffset = MenuOffset(INISECTION, ID_MENUITEM, 0);  // <==�ǉ�

    submenu = GetSubMenu(menu, 0);
    menu_id = ID_MENUITEM + pvar->menuoffset;                   // <==�I�t�Z�b�g�����Z
    AppendMenu(submenu, MF_ENABLED, menu_id, "item");
    ...
}
...

static void PASCAL TTXModifyPopupMenu(HMENU menu)
{
    //���l�ɃI�t�Z�b�g�����Z
}
...

static int PASCAL TTXProcessCommand(HWND hWin, WORD cmd)
{
    UINT menu_id;

    menu_id  = cmd + pvar->menuoffset;          // <==�I�t�Z�b�g�����Z
    switch (menu_id)
    {
    case ID_MENUITEM:
        //menu pocedure...
        break;
    case ID_MENUITEM1:
        //menu pocedure...
        break;
    ...
    }
    return 0;
}
```

## �������̓��I���蓖��

��{�I�ɂ́A�֐����Ń������̊��蓖�āA�J�����s���B
***buf*** ����� ***buf_sz*** �́A���L�ȊO�Œl��ύX���Ȃ��B( ***buf*** �̉ӏ��͔C�ӂ̖��O)

�ŏ��Ƀ������̈���`����B

```c
char *buf;
int buf_sz;

buf = NULL;
buf_sz = 4100;      // <== ��F 4096�o�C�g + 4�o�C�g
```

�������̈���m�ۂ���B

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
buf = malloc(buf_sz);
if (!buf)
{
    //���������m�ۂł��Ȃ������ꍇ�A���Ɋm�ۂ��������������
    return -1;
}
```

�������̈�̎g�p���I������烁�������J������B

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
```

## ������̓��I���蓖��

��{�I�ɂ́A�������̊��蓖�ĂƓ����B��{�I�ɂ́A�֐����Ń������̊��蓖�āA�J�����s���B
***buf*** ����� ***buf_sz*** �́A���L�ȊO�Œl��ύX���Ȃ��B( ***buf*** �̉ӏ��͔C�ӂ̖��O)

�ŏ��Ƀ������̈���`����B

```c
PCHAR buf;
int buf_sz;
PCHAR p;

buf = NULL;
buf_sz = 0;
```

������𕡐�����B

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
buf_sz = strlen(src) + 2;
buf = malloc(buf_sz * sizeof(CHAR));
if (!buf)
{
    //���������m�ۂł��Ȃ������ꍇ�A���Ɋm�ۂ��������������
    return -1;
}
memcpy(buf, buf_sz, src);
```

�������ǉ�����B

```c
buf_sz += strlen(src);
p = malloc(buf_sz);
if (!p)
{
    if (buf)
        fee(buf);
    //���������m�ۂł��Ȃ������ꍇ�A���Ɋm�ۂ��������������
    return -1;
}
strcpy_s(p, buf_sz, buf);
strcat_s(p, buf_sz, src);
free(buf);
buf = p;
```

������̎g�p���I������烁�������J������B

```c
if (buf)
{
    free(buf);
    buf = NULL;
}
```

## �r���h��

�����̃r���h�����Ǘ�����̂͏o�@�\���Ȃ��̂ŁA cmake �����ɂ���B
�r���h�̎g������ Visual Studio �̍ŐV�ł��g�p����B

�������A�C���X�g�[���� cmake ����Ăяo���Ȃ��̂� Visual Studio ���J���ăr���h����B

## �v���O�C���o�[�W����

�S�̂� CMakeLists.txt �� project �� VERSION ���w�肵�ăv���_�N�g�o�[�W������ݒ肷��B
�ʂ̃v���O�C���Ƀo�[�W������t�������ꍇ�́A�v���O�C�����Ƃ� CMakeLists.txt �� project �� VERSION ���w�肵�ăt�@�C���o�[�W������ݒ肷��B�v���O�C�����Ƃ� CMakeLists.txt �� project �� VERSION ���w�肵�Ȃ������ꍇ�́A�v���_�N�g�o�[�W�������K�p�����B

�v���_�N�g�o�[�W�����̗�(�S�̂�1��)�F

``` Cmake
cmake_minimum_required(VERSION 3.14)

project(TTX VERSION 1.0.4)        # <== �v���_�N�g�o�[�W������ 1.0.4

add_subdirectory(TTXPlugin)
set_target_properties(TTXPlugin PROPERTIES FOLDER TTX)
```

�t�@�C���o�[�W�����̗�(�v���O�C�����Ǝw��\)�F

``` Cmake
cmake_minimum_required(VERSION 3.14)

project(TTXPlugin)              # <== VERSION �w�肵�Ȃ��ꍇ�́A
                                #     �v���_�N�g�o�[�W�������g�p

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")

if(USE_UNICODE_API)
  add_definitions(-DUNICODE -D_UNICODE)
endif()
```

�v���O�C�����X�g�\���ɂ́A���\�[�X�t�@�C���̓��e��\�����邽�߁A�_�C�A���O���s�v�ł����[�\�[�X�t�@�C�������p�ɂ���B

## �_�C�A���O�̒����z�u

�_�C�A���O��e�E�C���h�E�̒����ɔz�u����ɂ́A�_�C�A���O�� Procedure �֐��̏��������� WM_CREATE �ɂ����āA�_�C�A���O�̏�������A **MoveParent()** �֐������s����B

```c
static LRESULT CALLBACK SettingProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        //�_�C�A���O���̏�����
        MoveParentCenter(dlg);      // <== ������ǉ�����B
        return TRUE;
    }
    return FALSE;
}
```

## �_�C�A���O�{�b�N�X���̃R���|�[�l���g�̃��T�C�Y

�_�C�A���O�̃��T�C�Y�Ń_�C�A���O���̃R���|�[�l���g��A�����ă��T�C�Y����ɂ́AProcedure �֐��̏��������� WM_CREATE �ŃR���|�[�l���g�̈ʒu�� **GetPointRB()** �֐��ŕۑ����A���T�C�Y���� WM_SIZE �� **MovePointRB()** �֐����g���ʒu��␳����B

- GetPointRB() �R���|�[�l���g�̉E�����_�C�A���O�̉E������̈ʒu���擾
- MovePoinrRB() �R���|�[�l���g�̈ʒu���_�C�A���O�̉E����̈ʒu�Ɉړ�

```c
static LRESULT CALLBACK dlg_proc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static POINT ptMsg, ptLog, ptBtn, ptSts;    // <== �Ώۂ̈ʒu�ۑ��p

    switch (msg)
    {
    case WM_INITDIALOG:
        GetPointRB(hWnd, IDC_MSG, &ptMsg);      // <== �ʒu��ۑ�
        GetPointRB(hWnd, IDC_LOG, &ptLog);      // <== �ʒu��ۑ�
        GetPointRB(hWnd, IDOK, &ptBtn);         // <== �ʒu��ۑ�
        GetPointRB(hWnd, IDC_STATUS, &ptSts);   // <== �ʒu��ۑ�
        //���̑��̐ݒ�
        return TRUE;

    case WM_SIZE:
        MovePointRB(hWnd, IDC_MSG, &ptMsg,
                    RB_RIGHT);                      // <== �ʒu��␳
        MovePointRB(hWnd, IDC_LOG, &ptLog,
                    RB_RIGHT | RB_BOTTOM);          // <== �ʒu��␳
        MovePointRB(hWnd, IDOK, &ptBtn,
                    RB_TOP | RB_BOTTOM);            // <== �ʒu��␳
        MovePointRB(hWnd, IDC_STATUS, &ptSts,
                    RB_RIGHT | RB_TOP | RB_BOTTOM); // <== �ʒu��␳
        //���̑��̐ݒ�
        return TRUE;

```

## ���쌠

���쌠�́A�\�[�X�R�[�h��h�L�������g�̍ŏ��������

```
(C) 2020,2021 tomo3136a
```

�Ə����Ă����΂悢�B**�u(C)�v** �� **�u���J�����N�v** �� **�u����ҁv** ������Α��͕s�v�炵���B
���ƁA���쌠�҂̘A����Ƃ����ǂ����ɖ��L���Ă����B

## ���C�Z���X

��������I�[�v���\�[�X�p���C�Z���X����A�������ɗ͂Ȃ������� MIT ���C�Z���X��I�ԁB
MIT ���C�Z���X���Ɩ������āA���C�Z���X�t�@�C�����킩��ꏊ�ɒu���Ă����B
