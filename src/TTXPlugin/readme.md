# TTXPlugin

## �T�v

TTXPlugin �́ATera Term �̊g���v���O�C�� (TTX) �����X�g�A�b�v�\�����܂��B
�܂��A�\�ȏꍇ�A�g���v���O�C���𖳌��ɂ��܂��B

���g���v���O�C���𖳌����́A�����_�ł� **�uTTXPlugin �ɑΉ����� TTX�v**  �̂݉\ \
�@TeraTerm �̃\�[�X�R�[�h�Ƀp�b�`�����Ă邱�Ƃɂ��A���ׂĂ̊g���v���O�C���ɑΉ������邱�Ƃ��ł��܂��B

TTX �́A�ʏ�C���X�g�[���Ŏg�p�L�����g�p�҂��I�����ăC���X�g�[�����܂��B
TTX �ׂ͍��������񋟂ł��邽�ߗL�p�ł����A��ɕK�v�ɂȂ�Ƃ����킯�ł͂���܂���B
���j���[�ɂ���ɕ\������܂��B

�����ŁA�g��Ȃ��Ƃ��� TTX �̋@�\����菜�������Ǝv���̂ł����A�ăC���X�g�[�����邩�A�v���O�����t�H���_����ړI�� DLL �t�@�C�����폜����ȊO�ɕ��@�͂Ȃ��A�J���҈ȊO�̐l�ɂ͕~�����������̂ƂȂ��Ă��܂��B

�{�g���v���O�C���ɂ��A���j���[����g���v���O�C���� ON/OFF ���s����悤�ɂ��܂��B

## �Ή��o�[�W����

�ȉ��̃o�[�W�����œ�����m�F

  Tera Term v4.98, v4.105

## �g����

1. �v���O�C�����r���h���܂��B
2. Tera Term ���s�� (ttermpro.exe������t�H���_) �Ƀr���h���� TTXPlugin.dll ���R�s�[���܂��B
3. Tera Term �����s���A ���j���[[�w���v-TTX] ��I�����܂��B
4. TXX �����X�g�A�b�v����܂��B
5. �L���ɂ��������ڂ́uon�v�ɁA�����ɂ��������ڂ́uoff�v�ɂ��܂��B
�ύX�͍��ڂ��_�u���N���b�N���܂��B
�u-�v �͕ύX�ł��Ȃ����ڂł��B
6. �uOK�v�{�^�����N���b�N���܂��B�ݒ肵�Ȃ��ꍇ�́uCANCEL�v���N���b�N���܂��B
7. Tera Term ���ċN�����܂��B(�ċN�����Ȃ��Ɛݒ�͔��f����܂���)

## �ݒ�t�@�C��

ttermpro.exe �Ɠ����f�B���N�g���ɂɂ��� TERATERM.INI ���g�p���܂��B

[TTXPlugin] �Z�N�V������ TTX �t�@�C���̖��O�Ɛݒ�Ƃ��� �uon�v �uoff�v �u-�v ��o�^���܂��B
�uoff�v ��ݒ肷��� TTX �𖳌��ɂ��܂��B
�u-�v ��ݒ肷��Ɛݒ��ʂ��� TTX �𖳌��ɐݒ�͂ł��Ȃ��Ȃ�܂��B
�ȗ����܂߂Ă��̑��̏ꍇ�� �uon�v �Ƃ��ĔF���� TTX �L���ɂȂ�܂��B

�܂��A�g���v���O�C����DLL�t�@�C�����Ή������Ă���K�v������܂����A
2�߂̃p�����[�^�Ƀ��j���[ID �̃I�t�Z�b�g���w��ł��܂��B

���̗�ł́A�uTTXProxy�v �uTTXResizeMenu�v �uTTXttyplay�v �uTTXttyrec�v
�̊g���v���O�C���𖳌��ɂ��Ă��܂��B
�܂��A�uTTXPlugin�v�g���v���O�C���́A�����ւ̐ݒ���֎~(�u-�v)�A�Ȃ������j���[�� MenuID �ɃI�t�Z�b�g�l 100 ��ǉ����܂��B

TERATERM.INI:

``` INI
[TTXPlugin]
TTXProxy=off
TTXResizeMenu=off
ttxssh=on
TTXttyplay=off
TTXttyrec=off
TTXPlugin=-,100
```

## �r���h

cmake ���g�p���ăr���h���܂��B

```bat
mkdir build
cd build
cmake -A Win32 ..
cmake --build . --config Release
```

### Tera Term �p�b�`

Tera Term �Ƀp�b�`�𓖂Ăăr���h���邱�Ƃɂ��A���ׂĂ̊g���v���O�C���� on/off ���s�����Ƃ��\�ɂ��܂��B

�p�b�`�F

``` C
--- C:/work/tt/4-stable/teraterm/teraterm/ttplug.c	Mon May 24 00:35:59 2021
+++ C:/work/tt/dev/teraterm/teraterm/ttplug.c	Mon May 24 06:43:54 2021
@@ -68,6 +68,18 @@
   char buf[1024];
   DWORD err;
   char uimsg[MAX_UIMSG];
+  char *p1;
+  char *p2;
+
+  p1 = strrchr(fileName, '\\');
+  if (NULL != p1) {
+    p2 = strchr(++p1, '.');
+    if (NULL != p2) {
+      strncpy_s(buf, 1024, p1, (p2 - p1)/sizeof(char));
+      GetPrivateProfileString("TTXPlugin", buf, "", buf, sizeof(buf), ts.SetupFName);
+      if (_strnicmp("off", buf, 3) == 0) return;
+    }
+  }
 
   if (NumExtensions>=MAXNUMEXTENSIONS) return;
   LibHandle[NumExtensions] = LoadLibrary(fileName);
```

## �J������

Tera Term �Ƀp�b�`�𓖂ĂȂ��ꍇ�A **�uTTXPlugin �ɑΉ����� TTX�v** �̂ݐ���Ώۂ�d�Ȃ�܂��B

TTX �� **�uTTXPlugin �ɑΉ����� TTX�v** �ɂ���ɂ́A�e TTX �\�[�X�R�[�h�� TTXIgnore() �֐��ǉ��� TTXBind() �֐��ɌĂяo������ǉ����܂��B

TTXIgnore() �֐��ǉ��F

``` C
BOOL TTXIgnore(int order, PCHAR name, DWORD version)
{
    char buf[8];
    GetPrivateProfileString("TTXPlugin", INISECTION, "", buf, sizeof(buf), ".\\TERATERM.INI");
    return (_strnicmp("off", buf, 3) == 0);
}
```

TTXBind() �֐��ɌĂяo������ǉ��F

``` C
BOOL __declspec(dllexport) PASCAL FAR TTXBind(WORD Version, TTXExports *exports)
{
    int size = sizeof(Exports) - sizeof(exports->size);
    /* do version checking if necessary */
    /* if (Version!=TTVERSION) return FALSE; */

    if (TTXIgnore(ORDER, INISECTION, 0))     //<= �ǉ�
        return TRUE;     �@�@�@�@�@�@�@�@�@�@�@//<= �ǉ�

    if (size > exports->size)
    {
        size = exports->size;
    }
    memcpy((char *)exports + sizeof(exports->size),
           (char *)&Exports + sizeof(exports->size),
           size);
    return TRUE;
}

```

�{�@�\�́A TTX �����[�h���Ȃ��킯�ł͂Ȃ��A�Ăяo���G���g����o�^���Ȃ������ł��̂ŁA���{�I�ȉ������@�ł͂���܂��񂪁A���i�̎g�p�ł͏\���Ǝv���܂��B