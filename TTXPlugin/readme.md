# TTXPlugin

## �T�v

TTXPlugin �́ATera Term �̊g���v���O�C�� (TTX) �����X�g�A�b�v�\�����܂��B
�܂��A�\�ȏꍇ�A�g���v���O�C���𖳌��ɂ��܂��B

���g���v���O�C���𖳌����́A **�uTTXPlugin �ɑΉ����� TTX�v**  �̂݉\

TTX �́A�ʏ�C���X�g�[���Ŏg�p�L�����g�p�҂��I�����ăC���X�g�[�����܂��B
TTX �ׂ͍��������񋟂ł��邽�ߗL�p�ł����A��ɕK�v�ɂȂ�Ƃ����킯�ł͂���܂���B
���j���[�ɂ���ɕ\������܂��B

�����ŁA�g��Ȃ��Ƃ��� TTX �̋@�\����菜�������Ǝv���̂ł����A�ăC���X�g�[�����邩�A�v���O�����t�H���_����ړI�� DLL �t�@�C�����폜����ȊO�ɕ��@�͂Ȃ��A�J���҈ȊO�̐l�ɂ͕~�����������̂ƂȂ��Ă��܂��B

�{�g���́A **�uTTXPlugin �ɑΉ����� TTX�v** �Ɋւ��ẮA���j���[���� ON/OFF ���s����悤�ɂ��܂��B

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
�u---�v �͕ύX�ł��Ȃ����ڂł��B
6. �uOK�v�{�^�����N���b�N���܂��B�ݒ肵�Ȃ��ꍇ�́uCANCEL�v���N���b�N���܂��B
7. Tera Term ���ċN�����܂��B(�ċN�����Ȃ��Ɛݒ�͔��f����܂���)

## �ݒ�t�@�C��

TERATERM.INI �ݒ�t�@�C���͎g�p���܂���Bttermpro.exe �Ɠ����f�B���N�g���ɂɂ���u ttx.txt �v���g�p���܂��B

[Load] �Z�N�V������ TTX �t�@�C���̖��O�Ɛݒ�Ƃ��� �uOn�v �uOff�v �u---�v ��o�^���܂��B
�uOff�v ��ݒ肷��� TTX �𖳌��ɂ��܂��B
�ȗ����܂߂Ă��̑��̏ꍇ�� �uOn�v �Ƃ��ĔF���� TTX �L���ɂȂ�܂��B
�u---�v ��ݒ肷��Ɛݒ��ʂ��� TTX ��؂�ւ��ݒ�͂ł��Ȃ��Ȃ�܂��B

## �r���h

cmake ���g�p���ăr���h���܂��B

```bat
mkdir build
cd build
cmake -A Win32 ..
cmake --build . --config Release
```

## �J������

�{�@�\�̐����Ƃ��� **�uTTXPlugin �ɑΉ����� TTX�v** �݂̂�����ΏۂƂȂ�܂��B

TTX �� **�uTTXPlugin �ɑΉ����� TTX�v** �ɂ���ɂ́A�e TTX �\�[�X�R�[�h�� TTXIgnore() �֐��ǉ��� TTXBind() �֐��ɌĂяo������ǉ����܂��B

TTXIgnore() �֐��ǉ��F

``` C
BOOL TTXIgnore(int order, PCHAR name, DWORD version)
{
    char buf[8];
    GetPrivateProfileString("Load", INISECTION, "", buf, sizeof(buf), ".\\ttx.txt");
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

Tera Term �̃v���O�����������C���� TTX �� DLL �Ăяo���O�ɔ��ʂ���΁A ttx.txt �t�@�C�������ׂĂ� TTX ��Ώۂɂ��邱�Ƃ��ł��܂��B
