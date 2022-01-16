# Tera Term 5 �Ή�

date:  2021/11/23

����v���O�C���� Tera Term 5 �Ή��ɂ��邽�߂̌��������������ɋL���B

## �T�v

Tera Term v.4.106 �����[�X(r9298, 2021/6/5)��A�{�i�I�� Tera Term 5 �̎������n�܂����B

Tera Term 5 �̃v���O�C���ɑ΂����v�ȋ@�\�ύX�͈ȉ��ƂȂ�B

* ���W���[�o�[�W�����A�b�v
* �����R�[�h�� Unicode �Ή�
* �ݒ�t�@�C���̕ۑ���̕ύX
* �p�X������� MAX_PATH(=260) �������̓P�p
* Windows 95 �̃T�|�[�g�I��
* ���s���ϐ�(TTset, ComVar)�̍\���ύX

## �u���W���[�o�[�W�����A�b�v�v�ɂ���

�Z�L�����e�B�ɖ���g�p�@�\�ɑ傫�ȃo�O�ł��Ȃ�����A�����ɍŐV�łɐ؂�ւ���l�͑����Ȃ��Ǝv����B\
�������܂߁ACOM �|�[�g�̂悤�ȃ��K�V�f�o�C�X����Ƃ��Ďg�p����l�́A�ŐV�łɂ���K�v���������Ȃ��B\
���̂��߁A�v���O�C���Ƃ��Ă͂��΂炭�͗����W���[�o�[�W�����ɑΉ��������B

�v���O�C���̃\�[�X�R�[�h�́A�́A�\�Ȍ��蓯���Ƃ��Av4 �p�ATera Term 5 �p�ɕʁX�Ƀr���h���邱�ƂőΉ�����B\
v4 �p�̃r���h�́A�R���p�C���̃I�v�V���� **-DTT4** ���w�肷��B

## �u�����R�[�h�� Unicode �Ή��v�ɂ���

v4 �ȑO�́A�����R�[�h�͊�{�I�Ƀo�C�g�����œ��삵�Ă����B\
Tera Term 5 �ł́A�ꕔ�������ăo�C�g�����͔p�~���A���C�h����(Unicode)�ɕύX�B

Unicode �����Ȃ��@�\�F

* �ʐM�f�[�^\
�ʐM�f�[�^�̓o�C�g��Ƃ��Ĉ������ߕύX�ΏۊO�B

* ���s���ϐ�(TTset, ComVar)\
�� �����I�ɂ̓��C�h�����ɂȂ�Ǝv����B

* �ݒ�t�@�C��(TERATERM.INI, KEYBOARD.CNF, cygterm.cfg)\
�� GetPrivateProfileStringW �֐��Ŏ����I�Ƀ��C�h�����ɕϊ����Ď擾����B\
�� GetPrivateProfileStringA �֐��Ŗ����I�Ƀo�C�g�����Ŏ擾����B

1. tchar.h �C���N���[�h
2. �R���p�C���I�v�V���� **-DUNICODE**, **-D_UNICODE** ���w��
3. ������p�̕ϐ��̌^���C��
	| v4 �ȑO | Tera Term 5 |
	|-|-|
	|char | TCHAR |
	|char * | LPTSTR |
	|const char * | LPCTSTR |
	|||
4. �Ώۂ̕�����́A **_T( )** �Ŋ���
5. �����񑀍�֐��́A tchar.h �̊֐��ɒu������
	* strxxx �� _tcsxxx (��xxx�͂��낢��Ȋ֐���)
	* atoi �� _tstoi
	* strlen �� _tsnlen
	* snprintf_s �� sntprintf_s
6. �Z�L���A�΍􂵂Ă��Ȃ�������֐��͂��̍ۃZ�L���A�΍�
7. �����̕�����T�C�Y���^�ɑΉ�\
	sizeof(buf) �� sizeof(buf)/sizeof(buf[0])\
	sizeof(buf) �� sizeof(buf)/sizeof(TCHAR)\
	p = malloc(len) �� p = (LPTSTR)malloc(len*sizeof(TCHAR))
8. �v���O�C���Ăяo���֐��̈����̌^�́A�}�N�����g�p\
�v���O�C���Ăяo���֐��̈����� const �̈������قȂ邽�߁A\
�P���u�������̓��[�j���O�ɂȂ�B���̂��߁A���Ή��̃}�N�����g�p����B
	| v4 �ȑO | Tera Term 5 | ���Ή��v���O�C�� |
	|-|-|-|
	|char * | wchar_t * | TTX_LPTSTR |
	|char * | const wchar_t * | TTX_LPCTSTR |
	|||

## �u�ݒ�t�@�C���̕ۑ���̕ύX�v�ɂ���

v4 �ł́AVirtualStore �@�\�ɂ��A�v���O�����t�@�C���t�H���_�� TeraTerm �t�H���_�̉��ɐݒ�t�@�C��������悤�Ɍ����Ă����B\
Tera Term 5 �ł� AppData/Roaming/teraterm5 �t�H���_�̉��Ɉړ������B\
���̑��̐ݒ�t�@�C���́A�C���X�g�[��������ω����Ȃ��̂ł��̑Ώۂł͂Ȃ��B\

�v���O�C���Ƃ��ẮA�傫���Ή��͕K�v�Ȃ��BRoaming�t�H���_�͊K�w���[���A\
�e�L�X�g�{�b�N�X�ŕ\������ƂƂĂ������B
��������̑΍􂪗~�����B(T.B.D.)

�e��p�X�̎擾�E�ݒ�́A�w���p�֐����g�p����B(ttxcommon.h�Q��)

| ���� | �֐� | ���� |
|-|-|-|
|�p�X�擾 | TTXGetPath(ts, id) | �ݒ�ϐ� ts ���� id �Ŏw�肵���p�X���擾����B<br/>�擾�����p�X�� TTXFree() �Ŕj������|
|�p�X�ݒ� | TTXSetPath(ts, id, s) | �ݒ�ϐ� ts ���� id �Ŏw�肵���p�X�� s ��ݒ肷��B |
|||

    //path string
    enum {
        ID_HOMEDIR              = 1,
        ID_SETUPFNAME           = 2,
        ID_KEYCNFNM             = 3,
        ID_LOGFN                = 4,
        ID_MACROFN              = 5,
        ID_UILANGUAGEFILE       = 6,
        ID_UILANGUAGEFILE_INI   = 7,
        ID_EXEDIR               = 8,
        ID_LOGDIR               = 9,
        ID_STRMAX,
    };
    LPTSTR TTXGetPath(PTTSet ts, UINT uid);
    BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR s);

    BOOL TTXFree(LPVOID *pBuf);

TTXFree() �֐��́A��������i�[�����|�C���^�ϐ��̃A�h���X�������ɓn���B\
�|�C���^�̎w����������������A�|�C���^�ϐ��� NULL �ɐݒ肷��B

## �u�p�X������� MAX_PATH(=260) �������̓P�p�v�ɂ���

v4 �ȑO�́A�X�^�b�N��� MAX_PATH �o�C�g�̔z���p�ӂ��Ďg�p���Ă����B
Tera Term 5 �ł͓��I�ɕK�v�������p�ӂ��A�g�p���I�������j������B

���I�ȕ�����̈�̎擾�́A��{�I�� **malloc()** �֐����g�p����B\
������̃^�C�v��ϊ��𔺂����Ƃ��������߁A���̃w���p�֐����g�p����B(ttxcommon.h�Q��)

### LPTSTR������̊֐�

| ���� | �֐� | ���� |
|-|-|-|
|LPTSTR �����񕡐�| _tcsdup(pszSrc) | LPTSTR ������𕡐�����B<br/>�擾����������� free() �Ŕj������|
|�o�C�g������擾| toMB(pszSrc) | LPTSTR �����񂩂�o�C�g��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|���C�h������擾| toWC(pszSrc) | LPTSTR �����񂩂烏�C�h��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|||

### ���C�h������̊֐�

| ���� | �֐� | ���� |
|-|-|-|
|���C�h�����񂩂�o�C�g������擾| WC2MB(cp, pwzSrc) | ���C�h�����񂩂�w�肵���R�[�h�y�[�W cp ��<br/>�o�C�g��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|���C�h�����񂩂�o�C�g������擾<br/>(�o�C�g�������OS�̕W��)| WC2ACP(pwzSrc) | ���C�h�����񂩂�o�C�g��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|���C�h�����񂩂�o�C�g������擾<br/>(�o�C�g�������UTF-8)| WC2UTF8(pwzSrc) | ���C�h�����񂩂�o�C�g��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|||

### �o�C�g������̊֐�

| ���� | �֐� | ���� |
|-|-|-|
|�o�C�g�����񂩂烏�C�h������擾| MB2WC(cp, pwzSrc) | �w�肵���R�[�h�y�[�W cp �̃o�C�g�����񂩂�<br/>���C�h��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|�o�C�g�����񂩂烏�C�h������擾<br/>(�o�C�g�������OS�̕W��)| ACP2WC(cp, pwzSrc) | �o�C�g�����񂩂烏�C�h��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|�o�C�g�����񂩂烏�C�h������擾<br/>(�o�C�g�������UTF-8)| UTF82WC(cp, pwzSrc) | �o�C�g�����񂩂烏�C�h��������擾����B<br/>�擾����������� TTXFree() �Ŕj������|
|||

## �uWindows 95 �̃T�|�[�g�I���v�ɂ���

Windows 95 �p�̃r���h���T�|�[�g���Ȃ��B
v4 �ł́A**DllMain()** �֐��ɁA**DoCover_IsDebuggerPresent()** �����邽�߁A
compat_w95.h ���C���N���[�h���Ă������AWindows95 �͊m�F�o���Ȃ������B\
�T�|�[�g�O�ƂȂ������߁A�폜�Bv4 �ł������Ă� Windows10 �͓��삵�Ă���B

## �u���s���ϐ�(TTset, ComVar)�̍\���ύX�v�ɂ���

�v���O�C�����猩��΁A�ǂ̃o�[�W������ Tera Term �ł������v���O�C�����g����̂��]�܂����B\
�������Ȃ���ATetra Term �{�̂��炷��΁A�@�\��ǉ��E�X�V�ɂ����s���ϐ��̍\���͕ς���Ă��Ă��܂��B\
�ł���΁A�������W���[�o�[�W�����ł̎��s���ϐ�(TTset, ComVar)�́A�����ɒǉ����Ă��������ɂ��Ăق����B

v4 ���� Tera Term 5 �ɂȂ�ɂ������āA�����ǉ�����\��������ӏ��ɗ\��̈��݂��Ă��ꂽ�B\
v4 �� Tera Term 5 �ō\���͈قȂ邪�ATera Term 5 ���ł͍\�����ۂ����ƍl������B
