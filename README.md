# TTX
Tera Term Extension Library

## �T�v

Tera Term �̃v���O�C���W�ł��B

| �@�\ | �v���O�C�� | ���� |
| ---- | ---------- | ---- |
| ��M���|�[�g�\�� | [TTXReport](TTXReport) | ��M�������������͂��A�}�b�`�����s�����|�[�g�E�B���h�E�ɕ\�����܂� |
| �Đڑ� | [TTXReconnect](TTXReconnect) | �Đڑ����j���[��ǉ����܂��B�Đڑ��܂ł̑҂����Ԃ��w��ł��܂� |
| �ڑ��o�ߎ��Ԃ�\�� | [TTXDuration](TTXDuration) | �ڑ��J�n����̎��Ԃ�\�����܂� |
| TTX �v���O�C���ꗗ�\�� | [TTXPlugin](TTXPlugin) | TTX �v���O�C���̈ꗗ��\�����܂��B�\�ȏꍇ�́A�v���O�C����L��/������ݒ肵�܂� |
| ���s���؂�ւ� | [TTXEnv](TTXEnv) | ���s�������j���[����؂�ւ��\�ɂ��܂� |
| ���p�b�P�[�W�쐬 | [TTXPackage](TTXPackage) | ���s�����p�b�P�[�W�����A�ڐA�o�b�`�t�@�C�����쐬���܂� |
| �V���[�g�J�b�g�쐬 | [TTXShortcut](TTXShortcut) | ���݂̐ݒ�� Tera Term ���N������V���[�g�J�b�g���쐬���܂� |
| ���[�U�[�L�[�ݒ� | [TTXUserKey](TTXUserKey) | �t�@���N�V�����L�[�Ȃǂ����[�U�[�L�[�Ɋ��蓖�āA�L�[�����ɂĕ����񑗐M�E�}�N�����s�E���j���[�I�����s����悤�ɐݒ肵�܂� |
| TTX �v���O�C���̃~�X�}�b�`���� | [TTXEstimateMismatch](TTXEstimateMismatch) | Tera Term �̃o�[�W�����ɂ���� TTX �v���O�C���� tttset �\���̂��~�X�}�b�`���Ă��Ȃ������肷�����\�����܂� |

## ���쌠��

�{�v���O�����̓t���[�\�t�g�E�F�A�ł��BMIT���C�Z���X�̂��Ƃɔz�z����Ă��܂��B

�{�v���O�����͏��p���p��킸�����ŗ��p�ł��܂����A��҂͖{�v���O�����̎g�p�ɂ����萶�����Q����ɑ΂��Ĉ�؂̐ӔC�𕉂��܂���B

  URL: https://github.com/tomo3136a/TTX

## �g�p��̒���

���̓_�͒��ӂ��Ďg�p�肢�܂��B

* ���{�ꓙ��2�o�C�g�����̓p�X���Ƃ��Đ���ɔF�����Ȃ���������܂���B
* �����p�X���Ȃǐ���ɔF�����Ȃ���������܂���B

## �Ή��o�[�W����

  Tera Term v4.96, v4.98, v4.105

## �r���h����

�r���h����ɂ͎��̊���p�ӂ��܂��B

* **cmake**
* **Visual Studio 2019**
* **Tera Term �\�[�X�R�[�h**

�C���X�g�[�����쐬����ꍇ�́AVisual Studio �g���@�\ **�uMicrosoft Visual Studio Installer Projects�v** ���C���X�g�[�����܂��B

```txt
cmake version 3.14.0
```

```txt
-- Building for: Visual Studio 16 2019
-- Selecting Windows SDK version 10.0.17763.0 to target Windows 10.0.19041.
-- The C compiler identification is MSVC 19.27.29112.0
-- The CXX compiler identification is MSVC 19.27.29112.0
```

�u**Microsoft Visual Studio Installer Projects**�v

```txt
Works with
  Visual Studio 2017, 2019
More Info
  Version 0.9.9
  Released on 2017/2/10 5:47:29
  Last updated
  2020/9/16 2:04:23
```

## �r���h

���̏��Ńr���h���܂��B

1. �{�\�[�X�R�[�h(TTX)����肵�܂��B
2. Tera Term �\�[�X�R�[�h�t�H���_�ɁA�{�\�[�X�R�[�h��z�u���܂��B
3. �{�\�[�X�R�[�h�̃t�H���_�ɂ��� **build.bat** �����s���܂��B 
   �R�}���h���C���ōs���ꍇ�́A**build.bat** ������f�B���N�g���ňȉ������s���܂��B

    ```bat
    mkdir build
    cd build
    cmake -A Win32 ..
    cmake --build . --config Release
    ```

4. **build/Release/** �Ƀr���h�����v���O�C������������܂��B�܂��A**build/data/** �ɂ��̑��̃t�@�C������������܂��B
5. �C���X�g�[�����쐬����ꍇ�́A�{�\�[�X�R�[�h�� **Setup/Setup.sln** �� Visual Studio �ŊJ���A�C���X�g�[�����r���h���Ă��������B�r���h�����C���X�g�[��(TTXSetup-x.x.x.msi) �́A**Setup/Release/** �t�H���_�ɐ�������܂��B

## �C���X�g�[��

### �C���X�g�[���ɂ��C���X�g�[��

Tera Term ���W���̃C���X�g�[����ɃC���X�g�[���ς݂̏ꍇ�A�C���X�g�[��(TTXSetup-x.x.x.msi) ���g�p���ăC���X�g�[���ł��܂��B

1. �C���X�g�[�������s���܂��B
2. ���C�Z���X�ɓ��ӂ��܂��B
3. �C���X�g�[�����K�v�ȃR���|�[�l���g��I�����܂��B
    �s�v�ȃv���O�C���̓`�F�b�N���O���Ă��������B
4. �C���X�g�[�����I�����܂��B
5. �C���X�g�[�����J�n���܂��B

### �R�}���h���C������̃C���X�g�[��

�r���h��A�{�\�[�X�R�[�h�̃t�H���_�ɂ��� **install.bat** �����s���܂��B

**instal.bat** �́A**build/Relese/** �t�H���_�� **build/data/** �t�H���_�̃t�@�C���� Tera Term �̃t�H���_�ɃR�s�[���܂��B

