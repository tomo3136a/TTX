# COM �|�[�g�x���Đڑ��v���O�C�� for Tera Term

## �T�v

��ȗp�r�́A�O�����u�� USB �o�R�ł� UART �ʐM�p�ł��B

��ʓI�� USB-UART �ϊ��P�[�u���ł͔������Ȃ��̂ł����A���u�ɑg�ݍ��܂ꂽ USB I/F �� USB �P�[�u���łȂ��� UART �ʐM����ꍇ�A Tera Term �̎����Đڑ��@�\(AutoComPortReconnect) �����s�̃_�C�A���O��\�����邱�Ƃ�����܂��B

USB�P�[�u���ڑ���A�����҂��Ă��� COM �|�[�g���J���Ɩ��Ȃ��ʐM�ł��܂��B

���̂��߁A�����Đڑ��@�\�� USB �ڑ������m���Ă��� COM �|�[�g���J���܂ł̊ԂɁA��莞�Ԃ̃K�[�h���Ԃ�݂��邱�Ƃŉ������v���O�C���ł��B

�܂��A�\���I�� UART �ڑ����Đڑ��������ꍇ�A���j���[�u�t�@�C��-�ڑ��f�v���ă��j���[�u�t�@�C��-�V�����ڑ��v�E�u�ڑ���I���v������Ǝ�Ԃ������邽�߁A���j���[�Ɂu�Đڑ��v��݂��܂����B

## �Ή��o�[�W����

�ȉ��̃o�[�W�����œ�����m�F

  Tera Term v4.96, v4.98, v4.104

�ȉ��̃o�[�W�����ł͐���ɓ��삵�܂���

  Tera Term v4.105

## �g����

1. �v���O�C�����r���h���܂��B
2. Tera Term ���s�� (ttermpro.exe������t�H���_) �Ƀr���h���� TTXReconnect.dll ���R�s�[���܂��B
3. TERATERM.INI �ݒ�t�@�C����ǉ��C�����܂��B
4. Tera Term �����s���A COM �|�[�g�� UART �ʐM���܂��B
5. �Đڑ����s���Ƃ��́A���j���[[�t�@�C��-�Đڑ�] ��I�����܂��B

## �ݒ�t�@�C��

�Đڑ��҂����Ԃ��~���b�Ŏw�肵�܂��B�ȗ��� 0 �Ɠ����ł��B

��F�Đڑ��҂����� 1 �b�̏ꍇ

```ini
[TTXReconnect]
ReconnectWait=1000
```

## �J������

### �o�[�W�����Ԃ̈ʒu�����␳����TTSet�\���̕␳�}�N���ł��r���h

�R���p�C���I�v�V������ /D"_TTX_VERSION_SUPPORT" ���w�肵�܂��B

## �r���h

cmake �Ńr���h����ꍇ

```bat
mkdir build
cd build
cmake -A Win32 ..
cmake --build . --config Release
```
