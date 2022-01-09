# ttset �p�X���Ή�

2020/1/9 tomo3136

## �T�v

`TeraTerm v5` �ȍ~�́A�����p�X������̃��C�h�L�����N�^���Ɠ����ɁA�p�X���̕������� `MAX_PATH` �����������Ȃ�A���C�h�L�����N�^�̉ϒ�������ɂȂ�B

### �ϒ���

* `TeraTerm v4` �ȑO:  
  ttset �\���̂̌Œ蒷�̃p�X��(�}���`�o�C�g������)���g�p�B(�p�X���� MAX_PATH �̕���������)

* `TeraTerm v5` �ȍ~:  
  �}���`�o�C�g������ŃA�N�Z�X����ꍇ�́A TeraTerm v4 �ȑO�Ɠ����\���̃����o�[�̃p�X�����g�p�B(�p�X���� MAX_PATH �̕���������)  
  ���C�h�L�����N�^������ŃA�N�Z�X����ꍇ�́Attset �\���̂ɐV�K�ɒǉ����ꂽ�����o�[���g�p�B

## �Ή�

�v���O�C���́A�\�[�X�R�[�h���x���� `TeraTerm v4`(�����}���`�o�C�g������) �A`TeraTerm v5`(�������C�h�L�����N�^������) �����ɑΉ����������߁A�ȉ��Ƃ���B

* `v4` �p�̏ꍇ�́A�r���h�I�v�V���� `-DTT4` ��t���ăr���h����B(cmake �Ŏw��)  
  ���w�肵�Ȃ��ꍇ�́A `v5` �p���r���h����B(���チ�C���ɂȂ�͂��Ȃ̂�)
* `ttset` �\���̂̃p�X������ւ̃A�N�Z�X�́A���̊֐����g�p����B(`ttxcommon.h` �Q��)
  * `TTXGetPath()`:  
    `ttset` �\���̂��� `uid` �ɑΉ������p�X��������擾����B  
    * `v4` �ł́A�����}���`�o�C�g������̃R�s�[���擾����B
    * `v5` �ł́A�������C�h�L�����N�^������̃R�s�[���擾����B

    �߂�l�̕����񂪖����ꍇ�́A NULL ��Ԃ��B  
    �߂�l�̕�����́A�s�v�ɂȂ����� `TTXFree()` �Ń��������������B

    ```c ttxcommon.h
      LPTSTR TTXGetPath(PTTSet ts, UINT uid);
    ```

  * `TTXSetPath()`:  
    `uid` �ɑΉ������p�X�������`ttset` �\���̂ɐݒ肷��B  
    �ݒ�ł����ꍇ�́A TRUE ��Ԃ��B�o���Ȃ��ꍇ�́A FALSE ��Ԃ��B

    ```c ttxcommon.h
      BOOL TTXSetPath(PTTSet ts, UINT uid, LPTSTR path);
    ```

* �p�X������� `uid`:  
  (`ttxcommon.h` �Q��)

  ```c ttxcommon.h
    //path string
    enum {
        ID_HOMEDIR              = 1,
        ID_SETUPFNAME           = 2,
        ID_KEYCNFNM             = 3,
        ID_LOGFN                = 4,
        ID_MACROFN              = 5,
        ID_UILANGUAGEFILE       = 6,
        ID_UILANGUAGEFILE_INI   = 7,
        ID_EXEDIR               = 8,   /* support v5 */
        ID_LOGDIR               = 9,   /* support v5 */
        ID_FILEDIR              = 10,
        ID_STRMAX,
    };
  ```
